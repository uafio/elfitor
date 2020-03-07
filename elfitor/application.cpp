#include "imgui.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>

#ifdef APIENTRY
#undef APIENTRY
#endif
#define STB_IMAGE_IMPLEMENTATION
#include <Windows.h>
#include <commdlg.h>

#include "elfreader/ELFReader.h"
#include "imelf.h"
#include "imgui_memory_editor.h"


std::vector< ELFReader* > elfs;
MemoryEditor medit;
CTX ctx;

void ToolTip( const char* desc )
{
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos( ImGui::GetFontSize() * 35.0f );
    ImGui::TextUnformatted( desc );
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
}

void HelpMarker( const char* desc )
{
    ImGui::TextDisabled( "(?)" );
    if ( ImGui::IsItemHovered() ) {
        ToolTip( desc );
    }
}


void MenuItemAbout( void )
{
    puts( "Pressed About" );
}

void MenuItemOpen( void )
{
    OPENFILENAMEA ofile = { 0 };
    char fpath[_MAX_PATH] = { 0 };

    ofile.lStructSize = sizeof( ofile );
    ofile.hwndOwner = GetActiveWindow();
    ofile.lpstrFile = fpath;
    ofile.nMaxFile = sizeof( fpath );

    if ( GetOpenFileNameA( &ofile ) ) {
        FileFactory* factory = new FileFactory( fpath );
        ELFReader* newelf = factory->build();
        printf( "newelf: %p\n", newelf );
        printf( "base: %p\n", newelf->get_elf_header() );
        printf( "is_valid: %d\n", newelf->is_valid() );
        if ( newelf && newelf->is_valid() ) {
            //Context* ctx = newfile->get_context();
            //State* state = State::instance();
            //state->update( ctx );
            elfs.push_back( newelf );
            ctx.elf = newelf;
            ctx.display.hdr = ctx.display.ehdr;
            ctx.display.idx = 0;
        }
    }
}

void MenuItemSave( void )
{
    printf( "Pressed Save\n" );
    if ( ctx.elf == nullptr )
        return;

    ctx.elf->save( ctx.elf->filepath() );
}

void MenuItemSaveAs( void )
{
    puts( "Pressed Save As" );
    if ( ctx.elf == nullptr )
        return;

    OPENFILENAMEA ofile = { 0 };
    char szFile[_MAX_PATH] = { 0 };
    ofile.lStructSize = sizeof( ofile );
    ofile.hwndOwner = GetActiveWindow();
    ofile.lpstrFile = szFile;
    ofile.nMaxFile = sizeof( szFile );

    if ( GetSaveFileNameA( &ofile ) ) {
        ctx.elf->save( szFile );
    }
}

void MenuItemClose( void )
{
    if ( ctx.elf ) {
        for ( auto elf = elfs.begin(); elf != elfs.end(); elf++ ) {
            if ( *elf == ctx.elf ) {
                elfs.erase( elf );
                break;
            }
        }
        ctx = { 0 };
    }
}

void ElfHeaderNavigation64( ELFReader* elf )
{
    ImGuiTreeNodeFlags flags = 0;
    flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
    flags |= ImGuiTreeNodeFlags_Leaf;

    if ( ctx.elf == elf && ctx.display.hdr == ctx.display.ehdr ) {
        ImGui::TreeNodeEx( "ELF Header Leaf", flags | ImGuiTreeNodeFlags_Selected, "ELF Header" );
    } else {
        ImGui::TreeNodeEx( "ELF Header Leaf", flags, "ELF Header" );
    }
    if ( ImGui::IsItemClicked() ) {
        ctx.elf = elf;
        ctx.display.hdr = ctx.display.ehdr;
    }

    if ( ctx.elf == elf && ctx.display.hdr == ctx.display.phdr ) {
        ImGui::TreeNodeEx( elf->get_prog_header( 0 ), flags | ImGuiTreeNodeFlags_Selected, "Program Headers" );
    } else {
        ImGui::TreeNodeEx( elf->get_prog_header( 0 ), flags, "Program Headers" );
    }
    if ( ImGui::IsItemClicked() ) {
        ctx.elf = elf;
        ctx.display.hdr = ctx.display.phdr;
    }

    if ( ctx.elf == elf && ctx.display.hdr == ctx.display.shdr ) {
        ImGui::TreeNodeEx( elf->get_section_header( 0 ), flags | ImGuiTreeNodeFlags_Selected, "Section Headers" );
    } else {
        ImGui::TreeNodeEx( elf->get_section_header( 0 ), flags, "Section Headers" );
    }
    if ( ImGui::IsItemClicked() ) {
        ctx.elf = elf;
        ctx.display.hdr = ctx.display.shdr;
    }
}

void ElfHeaderNavigation32( ELFReader* elf )
{
}

void ShowNagivationWindow( void )
{
    ImGuiWindowFlags flags = 0;
    flags |= ImGuiWindowFlags_NoTitleBar;
    flags |= ImGuiWindowFlags_NoMove;
    flags |= ImGuiWindowFlags_NoResize;
    flags |= ImGuiWindowFlags_NoCollapse;
    flags |= ImGuiWindowFlags_NoNav;

    ImGui::Begin( "Navigation", NULL, flags );

    if ( elfs.size() ) {
        for ( auto elf = elfs.begin(); elf != elfs.end(); ) {
            char label[_MAX_PATH];
            snprintf( label, sizeof( label ), "%s##%zd", ( *elf )->filename(), std::distance( elfs.begin(), elf ) );

            bool close = true;
            if ( ImGui::CollapsingHeader( label, &close, ImGuiTreeNodeFlags_DefaultOpen ) ) {
                if ( ( *elf )->is_32bit() ) {
                    ElfHeaderNavigation32( *elf );
                } else {
                    ElfHeaderNavigation64( *elf );
                }
            }
            if ( close == false ) {
                if ( *elf == ctx.elf ) {
                    ctx = { 0 };
                }
                delete *elf;
                elfs.erase( elf );
            } else {
                ++elf;
            }
        }
    }

    ImGui::End();
}


void ShowMainWindow( void )
{
    ImGuiWindowFlags wflags = 0;
    wflags |= ImGuiWindowFlags_NoMove;
    wflags |= ImGuiWindowFlags_NoResize;
    wflags |= ImGuiWindowFlags_NoCollapse;

    ImGui::Begin( "MainWindow", NULL, wflags );
    if ( ctx.elf == nullptr ) {
        ImGui::Text( "ELF Editor" );
    } else if ( ctx.display.hdr == ctx.display.ehdr ) {
        Imelf::Ehdr::Draw( ctx.elf );
    } else if ( ctx.display.hdr == ctx.display.phdr ) {
        Imelf::Phdr::Draw( ctx.elf );
    } else if ( ctx.display.hdr == ctx.display.shdr ) {
        Imelf::Shdr::Draw( ctx.elf );
    } else {
        ImGui::Text( "ELF Header for %s", ctx.elf->filepath() );
    }

    ImGui::End();
}

void ShowHexEditorWindow( void )
{
    if ( ctx.elf == nullptr ) {
        return;
    }
    if ( ctx.display.hdr == ctx.display.ehdr ) {
        medit.DrawWindow( "medit", ctx.elf->get_elf_header(), ctx.elf->get_file_size() );        
    } else if ( ctx.display.hdr == ctx.display.phdr ) {
        Elf64_Phdr* phdr = reinterpret_cast< Elf64_Phdr* >( ctx.elf->get_prog_header( ctx.display.idx ) );
        if ( phdr->p_offset + phdr->p_filesz < ctx.elf->get_file_size() ) {
            medit.DrawWindow( "medit", ctx.elf->rva2va( phdr->p_offset ), phdr->p_memsz, phdr->p_offset );
        }
    } else if ( ctx.display.hdr == ctx.display.shdr ) {
        Elf64_Shdr* shdr = reinterpret_cast< Elf64_Shdr* >( ctx.elf->get_section_header( ctx.display.idx ) );
        if ( shdr->sh_offset + shdr->sh_size < ctx.elf->get_file_size() ) {
            medit.DrawWindow( "medit", ctx.elf->rva2va( shdr->sh_offset ), shdr->sh_size, shdr->sh_offset );
        }
	} else {
		// should not reach
        assert( false );
	}
}

void ShowApplicationWindow( GLFWwindow* window )
{
    if ( ImGui::BeginMainMenuBar() ) {
        if ( ImGui::BeginMenu( "File" ) ) {
            if ( ImGui::MenuItem( "Open" ) ) {
                MenuItemOpen();
            } else if ( ImGui::MenuItem( "Save" ) ) {
                MenuItemSave();
            } else if ( ImGui::MenuItem( "Save As" ) ) {
                MenuItemSaveAs();
            } else if ( ImGui::MenuItem( "Close" ) ) {
                MenuItemClose();
            } else if ( ImGui::MenuItem( "Exit" ) ) {
                glfwSetWindowShouldClose( window, true );
            }

            ImGui::EndMenu();
        }
        if ( ImGui::BeginMenu( "Settings" ) ) {
            ImGui::EndMenu();
        }
        if ( ImGui::MenuItem( "About" ) ) {
            MenuItemAbout();
        }

        ImGui::EndMainMenuBar();
    }

    ShowNagivationWindow();
    ShowMainWindow();
    ShowHexEditorWindow();
    //ImGui::ShowMetricsWindow();
}
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


<<<<<<< Updated upstream
std::vector< File* > files;
MemoryEditor medit;
CTX ctx;

=======
>>>>>>> Stashed changes
void Tooltip( const char* desc )
{
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos( ImGui::GetFontSize() * 35.0f );
    ImGui::TextUnformatted( desc );
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
}

void HoverTooltip( const char* desc )
{
    if ( ImGui::IsItemHovered() ) {
        Tooltip( desc );
    }
}

<<<<<<< Updated upstream
void HelpMarker( const char* desc )
{
    ImGui::TextDisabled( "(?)" );
    HoverTooltip( desc );
}

=======
void HoverTooltip( const char* desc )
{
    if ( ImGui::IsItemHovered() ) {
        Tooltip( desc );
    }
}
>>>>>>> Stashed changes

void MenuItemAbout( void )
{
    puts( "Pressed About" );
}

void MenuItemOpen( void )
{
    OPENFILENAMEA ofile = { 0 };
    char szFile[_MAX_PATH] = { 0 };

    ofile.lStructSize = sizeof( ofile );
    ofile.hwndOwner = GetActiveWindow();
    ofile.lpstrFile = szFile;
    ofile.nMaxFile = sizeof( szFile );

    if ( GetOpenFileNameA( &ofile ) ) {
<<<<<<< Updated upstream
        ELFReader* newelf = new ELFReader( szFile );
        if ( newelf->is_valid() ) {
            elfs.push_back( newelf );
            ctx.elf = newelf;
            ctx.display.hdr = ctx.display.ehdr;
            ctx.display.idx = 0;
=======
        FileFactory* factory = new FileFactory( fpath );
        File* newfile = factory->build();
        if ( newfile ) {
            State::instance()->add( newfile );
>>>>>>> Stashed changes
        }
    }
}

void MenuItemSave( void )
{
    printf( "Pressed Save\n" );
    if ( State::instance()->get_ctx_file() == nullptr )
        return;

    auto state = State::instance();
    state->get_ctx_file()->save( state->get_ctx_file()->filepath() );
}

void MenuItemSaveAs( void )
{
    puts( "Pressed Save As" );
    if ( State::instance()->get_ctx_file() == nullptr )
        return;

    OPENFILENAMEA ofile = { 0 };
    char szFile[_MAX_PATH] = { 0 };
    ofile.lStructSize = sizeof( ofile );
    ofile.hwndOwner = GetActiveWindow();
    ofile.lpstrFile = szFile;
    ofile.nMaxFile = sizeof( szFile );

    if ( GetSaveFileNameA( &ofile ) ) {
        State::instance()->get_ctx_file()->save( szFile );
    }
}

void MenuItemClose( void )
{
    State::instance()->close();
}

/*
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
        //assert( false );
	}
}
*/

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

    State::instance()->ShowNavigation();
    State::instance()->ShowMainWindow();
    //ShowHexEditorWindow();
    //ImGui::ShowMetricsWindow();
}
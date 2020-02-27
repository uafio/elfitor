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
#include "stb_image.h"

std::vector< ELFReader* > elfs;

struct _CTX {
    ELFReader* elf;
    struct {
        enum {
            ehdr = 1,
            phdr,
            shdr,
        } hdr;
        int idx;
    } display;
} ctx;

static void HelpMarker( const char* desc )
{
    ImGui::TextDisabled( "(?)" );
    if ( ImGui::IsItemHovered() ) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos( ImGui::GetFontSize() * 35.0f );
        ImGui::TextUnformatted( desc );
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

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
        ELFReader* newelf = new ELFReader( szFile );
        if ( newelf->is_valid() ) {
            elfs.push_back( newelf );
            ctx.elf = newelf;
            ctx.display.hdr = ctx.display.ehdr;
            ctx.display.idx = 0;
            newelf->print_elf_header();
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

void MainWindowElfHeader64( void )
{
    Elf64_Ehdr* ehdr = reinterpret_cast< Elf64_Ehdr* >( ctx.elf->get_elf_header() );

    static ImGuiTableFlags flags = ImGuiTableFlags_SizingPolicyFixedX | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;
    ImGui::BeginTable( "elf header 32", 6, flags, ImVec2( 0, ImGui::GetTextLineHeightWithSpacing() * 1 ) );
    ImGui::TableSetupColumn( "Member", 0, 420.0 );
    ImGui::TableSetupColumn( "Offset", 0, 200.0 );
    ImGui::TableSetupColumn( "Size", 0, 150.0 );
    ImGui::TableSetupColumn( "Value", 0, 240.0 );
    ImGui::TableSetupColumn( "Meaning", 0, 300.0 );
    ImGui::TableSetupColumn( "", ImGuiTableColumnFlags_WidthStretch );
    ImGui::TableAutoHeaders();

    // ==================================================================================================================== //

    ImGui::TableNextRow();
    ImGui::Text( "e_ident[EI_MAG]" );
    ImGui::TableNextCell();
    ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_ident ) );
    ImGui::TableNextCell();
    ImGui::Text( "%02x", sizeof( ehdr->e_ident ) );
    ImGui::TableNextCell();
    ImGui::InputScalar( "ei_ident_input", ImGuiDataType_U32, ehdr->e_ident, NULL, NULL, "%08X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
    ImGui::TableNextCell();
    if ( *(uint32_t*)&ehdr->e_ident[0] == 'FLE\x7f' ) {
        ImGui::Text( "%#02x %c%c%c", ehdr->e_ident[0], ehdr->e_ident[1], ehdr->e_ident[2], ehdr->e_ident[3] );
    } else {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        ImGui::Text( "%#02x %c%c%c", ehdr->e_ident[0], ehdr->e_ident[1], ehdr->e_ident[2], ehdr->e_ident[3] );
        ImGui::PopStyleColor();
    }
    ImGui::TableNextCell();
    HelpMarker( "0x7F followed by ELF(45 4c 46) in ASCII; these four bytes constitute the magic number." );

    // ==================================================================================================================== //

    ImGui::TableNextRow();
    ImGui::Text( "e_ident[EI_CLASS]" );
    ImGui::TableNextCell();
    ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_class ) );
    ImGui::TableNextCell();
    ImGui::Text( "%02x", sizeof( ehdr->e_class ) );
    ImGui::TableNextCell();
    if ( ehdr->e_class == 1 || ehdr->e_class == 2 ) {
        ImGui::InputScalar( "ei_class_input", ImGuiDataType_U8, &ehdr->e_class, NULL, NULL, "%02X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
    } else {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        ImGui::InputScalar( "ei_class_input", ImGuiDataType_U8, &ehdr->e_class, NULL, NULL, "%02X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
        ImGui::PopStyleColor();
    }
    ImGui::TableNextCell();
    const char* e_class[] = { "", "32-bit", "64-bit" };
    const char* e_class_cur = e_class[0];
    if ( ehdr->e_class == 1 || ehdr->e_class == 2 ) {
        e_class_cur = e_class[ehdr->e_class];
    }
    if ( ImGui::BeginCombo( "e_class_combo", e_class_cur, ImGuiComboFlags_None ) ) {
        for ( int n = 1; n < _countof( e_class ); n++ ) {
            bool is_selected = ( e_class_cur == e_class[n] );
            if ( ImGui::Selectable( e_class[n], is_selected ) ) {
                e_class_cur = e_class[n];
                ehdr->e_class = n;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::TableNextCell();
    HelpMarker( "This byte is set to either 1 or 2 to signify 32-bit or 64-bit format, respectively." );

    // ==================================================================================================================== //

    ImGui::TableNextRow();
    ImGui::Text( "e_ident[EI_DATA]" );
    ImGui::TableNextCell();
    ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_data ) );
    ImGui::TableNextCell();
    ImGui::Text( "%02x", sizeof( ehdr->e_data ) );
    ImGui::TableNextCell();
    if ( ehdr->e_data > 2 ) {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        ImGui::InputScalar( "ei_data_input", ImGuiDataType_U8, &ehdr->e_data, NULL, NULL, "%02X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
        ImGui::PopStyleColor();
    } else {
        ImGui::InputScalar( "ei_data_input", ImGuiDataType_U8, &ehdr->e_data, NULL, NULL, "%02X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
    }
    ImGui::TableNextCell();
    const char* e_data[] = { "", "little-endian", "big-endian" };
    const char* e_data_cur = e_data[0];
    if ( ehdr->e_data == 1 || ehdr->e_data == 2 ) {
        e_data_cur = e_data[ehdr->e_data];
    }
    if ( ImGui::BeginCombo( "e_data_combo", e_data_cur, ImGuiComboFlags_None ) ) {
        for ( int n = 1; n < _countof( e_data ); n++ ) {
            bool is_selected = ( e_data_cur == e_data[n] );
            if ( ImGui::Selectable( e_data[n], is_selected ) ) {
                e_data_cur = e_data[n];
                ehdr->e_data = n;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::TableNextCell();
    HelpMarker( "This byte is set to either 1 or 2 to signify little or big endianness, respectively. This affects interpretation of multi-byte fields starting with offset 0x10." );

    // ==================================================================================================================== //

    ImGui::TableNextRow();
    ImGui::Text( "e_ident[EI_VERSION]" );
    ImGui::TableNextCell();
    ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_ver ) );
    ImGui::TableNextCell();
    ImGui::Text( "%02x", sizeof( ehdr->e_ver ) );
    ImGui::TableNextCell();
    if ( ehdr->e_ver == 1 ) {
        ImGui::InputScalar( "ei_version_input", ImGuiDataType_U8, &ehdr->e_ver, NULL, NULL, "%02X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );    
    } else {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        ImGui::InputScalar( "ei_version_input", ImGuiDataType_U8, &ehdr->e_ver, NULL, NULL, "%02X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
        ImGui::PopStyleColor();    
    }
    ImGui::TableNextCell();
    ImGui::TableNextCell();    
    HelpMarker( "Set to 1 for the original and current version of ELF." );

    // ==================================================================================================================== //

    ImGui::TableNextRow();
    ImGui::Text( "e_ident[EI_OSABI]" );
    ImGui::TableNextCell();
    ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_osabi ) );
    ImGui::TableNextCell();
    ImGui::Text( "%02x", sizeof( ehdr->e_osabi ) );
    ImGui::TableNextCell();
    const char* e_osabi[] = { "System V", "HP-UX", "NetBSD", "Linux", "GNU Hurd", "", "Solaris", "AIX", "IRIX", "FreeBSD", "Tru64", "Novell Modesto", "OpenBSD", "OpenVMS", "NonStop Kernel", "AROS", "Fenix OS", "Cloud ABI", "" };
    if ( ehdr->e_osabi >= _countof( e_osabi ) - 1 || ehdr->e_osabi == 5 ) {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        ImGui::InputScalar( "ei_osabi_input", ImGuiDataType_U8, &ehdr->e_osabi, NULL, NULL, "%02X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
        ImGui::PopStyleColor();
    } else {
        ImGui::InputScalar( "ei_osabi_input", ImGuiDataType_U8, &ehdr->e_osabi, NULL, NULL, "%02X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );    
    }
    ImGui::TableNextCell();
    const char* e_osabi_cur = e_osabi[_countof(e_osabi)];
    if ( ehdr->e_osabi < _countof( e_osabi ) && ehdr->e_osabi != 5 ) {
        e_osabi_cur = e_osabi[ehdr->e_osabi];
    }
    if ( ImGui::BeginCombo( "e_osabi_combo", e_osabi_cur, ImGuiComboFlags_None ) ) {
        for ( int n = 0; n < _countof( e_osabi ) - 1; n++ ) {
            if ( n == 5 ) {
                continue;
            }
            bool is_selected = ( e_osabi_cur == e_osabi[n] );
            if ( ImGui::Selectable( e_osabi[n], is_selected ) ) {
                e_osabi_cur = e_osabi[n];
                ehdr->e_osabi = n;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::TableNextCell();
    HelpMarker( "Identifies the target operating system ABI. It is often set to 0 regardless of the target platform." );

    // ==================================================================================================================== //

    ImGui::TableNextRow();
    ImGui::Text( "e_ident[EI_ABIVERSION]" );
    ImGui::TableNextCell();
    ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_abiversion ) );
    ImGui::TableNextCell();
    ImGui::Text( "%02x", sizeof( ehdr->e_abiversion ) );
    ImGui::TableNextCell();
    ImGui::InputScalar( "ei_abiver_input", ImGuiDataType_U8, &ehdr->e_abiversion, NULL, NULL, "%02X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
    ImGui::TableNextCell();
    ImGui::TableNextCell();
    HelpMarker("Further specifies the ABI version. Its interpretation depends on the target ABI. "
        "Linux kernel (after at least 2.6) has no definition of it, so it is ignored for statically-linked executables.");

    // ==================================================================================================================== //

    ImGui::TableNextRow();
    ImGui::Text( "e_type" );
    ImGui::TableNextCell();
    ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_type ) );
    ImGui::TableNextCell();
    ImGui::Text( "%02x", sizeof( ehdr->e_type ) );
    ImGui::TableNextCell();
    if ( ehdr->e_type < 5 || ehdr->e_type == 0xfe00 || ehdr->e_type == 0xfeff || ehdr->e_type == 0xff00 || ehdr->e_type == 0xffff ) {
        ImGui::InputScalar( "e_type_input", ImGuiDataType_U16, &ehdr->e_type, NULL, NULL, "%04X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
    } else {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        ImGui::InputScalar( "e_type_input", ImGuiDataType_U16, &ehdr->e_type, NULL, NULL, "%04X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
        ImGui::PopStyleColor();
    }
    
    ImGui::TableNextCell();

    const char* e_type[] = { "ET_NONE", "ET_REL", "ET_EXEC", "ET_DYN", "ET_CORE", "ET_LOOS", "ET_HIOS", "ET_LOPROC", "ET_HIPROC", "" };
    const char* e_type_cur = e_type[_countof(e_type) - 1];
    if ( ehdr->e_type < 5 || ehdr->e_type == 0xfe00 || ehdr->e_type == 0xfeff || ehdr->e_type == 0xff00 || ehdr->e_type == 0xffff ) {
    }

    if ( ImGui::BeginCombo( "e_type_combo", e_type_cur, ImGuiComboFlags_None ) ) {
        for ( int n = 0; n < _countof( e_type ); n++ ) {
            bool is_selected = ( e_type_cur == e_type[n] );
            if ( ImGui::Selectable( e_type[n], is_selected ) ) {
                e_type_cur = e_type[n];
                if ( n < 5 ) {
                    ehdr->e_type = n;
                } else if ( n == 5 ) {
                    ehdr->e_type = 0xfe00;
                } else if ( n == 6 ) {
                    ehdr->e_type = 0xfeff;
                } else if ( n == 7 ) {
                    ehdr->e_type = 0xff00;
                } else if ( n == 8 ) {
                    ehdr->e_type = 0xffff;
                }
            }
        }
        ImGui::EndCombo();
    }

    ImGui::TableNextCell();
    HelpMarker( "Identifies object file type." );

    // ==================================================================================================================== //

    ImGui::TableNextRow();
    ImGui::Text( "e_machine" );
    ImGui::TableNextCell();
    ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_machine ) );
    ImGui::TableNextCell();
    ImGui::Text( "%02x", sizeof( ehdr->e_machine ) );
    ImGui::TableNextCell();
    auto machine = ehdr->e_machine;
    if ( !machine || machine == 2 || machine == 3 || machine == 8 || machine == 0x14 || machine == 0x16 || machine == 0x28 || machine == 0x2a || machine == 0x32 || machine == 0x3e || machine == 0xb7 || machine == 0xf3 ) {
        const char* e_machine[] = { "NONE", "SPARC", "x86", "MIPS", "PowerPC", "S390", "ARM", "SuperH", "IA-64", "amd64", "AArch64", "RISC-V" };
        static const char* e_machine_cur = nullptr;

        switch ( machine ) {
            case 0:
                e_machine_cur = e_machine[0];
                break;
            case 2:
                e_machine_cur = e_machine[1];
                break;
            case 3:
                e_machine_cur = e_machine[2];
                break;
            case 8:
                e_machine_cur = e_machine[3];
                break;
            case 0x14:
                e_machine_cur = e_machine[4];
                break;
            case 0x16:
                e_machine_cur = e_machine[5];
                break;
            case 0x28:
                e_machine_cur = e_machine[6];
                break;
            case 0x2a:
                e_machine_cur = e_machine[7];
                break;
            case 0x32:
                e_machine_cur = e_machine[8];
                break;
            case 0x3e:
                e_machine_cur = e_machine[9];
                break;
            case 0xb7:
                e_machine_cur = e_machine[10];
                break;
            case 0xf3:
                e_machine_cur = e_machine[11];
                break;
        }

        if ( ImGui::BeginCombo( "e_machine_combo", e_machine_cur, ImGuiComboFlags_None ) ) {
            for ( int n = 0; n < _countof( e_machine ); n++ ) {
                bool is_selected = ( e_machine_cur == e_machine[n] );
                if ( ImGui::Selectable( e_machine[n], is_selected ) ) {
                    e_machine_cur = e_machine[n];
                    switch ( n ) {
                        case 0:
                            ehdr->e_machine = 0;
                            break;
                        case 1:
                            ehdr->e_machine = 2;
                            break;
                        case 2:
                            ehdr->e_machine = 3;
                            break;
                        case 3:
                            ehdr->e_machine = 8;
                            break;
                        case 4:
                            ehdr->e_machine = 0x14;
                            break;
                        case 5:
                            ehdr->e_machine = 0x16;
                            break;
                        case 6:
                            ehdr->e_machine = 0x28;
                            break;
                        case 7:
                            ehdr->e_machine = 0x2a;
                            break;
                        case 8:
                            ehdr->e_machine = 0x32;
                            break;
                        case 9:
                            ehdr->e_machine = 0x3e;
                            break;
                        case 10:
                            ehdr->e_machine = 0xb7;
                            break;
                        case 11:
                            ehdr->e_machine = 0xf3;
                            break;
                    }
                }
                if ( is_selected ) {
                    ImGui::SetItemDefaultFocus(); // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
                }
            }
            ImGui::EndCombo();
        }

    } else {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        ImGui::InputScalar( "e_machine_input", ImGuiDataType_U16, &ehdr->e_machine, NULL, NULL, "%04X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
        ImGui::PopStyleColor();
    }

    ImGui::TableNextCell();
    HelpMarker( "Specifies target instruction set architecture." );

    // ==================================================================================================================== //

    ImGui::TableNextRow();
    ImGui::Text( "e_version" );
    ImGui::TableNextCell();
    ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_version ) );
    ImGui::TableNextCell();
    ImGui::Text( "%02x", sizeof( ehdr->e_version ) );
    ImGui::TableNextCell();
    if ( ehdr->e_version == 1 ) {
        ImGui::InputScalar( "e_version_input", ImGuiDataType_U32, &ehdr->e_version, NULL, NULL, "%08X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
    } else {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        ImGui::InputScalar( "e_version_input", ImGuiDataType_U32, &ehdr->e_version, NULL, NULL, "%08X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
        ImGui::PopStyleColor();    
    }
    ImGui::TableNextCell();
    HelpMarker( "Set to 1 for the original version of ELF." );

    // ==================================================================================================================== //

    ImGui::TableNextRow();
    ImGui::Text( "e_entry" );
    ImGui::TableNextCell();
    ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_entry ) );
    ImGui::TableNextCell();
    ImGui::Text( "%02x", sizeof( ehdr->e_entry ) );
    ImGui::TableNextCell();
    ImGui::Text( "%#llx", ehdr->e_entry );
    ImGui::TableNextCell();

    // ==================================================================================================================== //

    ImGui::TableNextRow();
    ImGui::Text( "e_phoff" );
    ImGui::TableNextCell();
    ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_phoff ) );
    ImGui::TableNextCell();
    ImGui::Text( "%02x", sizeof( ehdr->e_phoff ) );
    ImGui::TableNextCell();
    ImGui::Text( "%#llx", ehdr->e_phoff );
    ImGui::TableNextCell();

    // ==================================================================================================================== //

    ImGui::TableNextRow();
    ImGui::Text( "e_shoff" );
    ImGui::TableNextCell();
    ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_shoff ) );
    ImGui::TableNextCell();
    ImGui::Text( "%02x", sizeof( ehdr->e_shoff ) );
    ImGui::TableNextCell();
    ImGui::Text( "%#llx", ehdr->e_shoff );
    ImGui::TableNextCell();

    // ==================================================================================================================== //

    ImGui::TableNextRow();
    ImGui::Text( "e_flags" );
    ImGui::TableNextCell();
    ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_flags ) );
    ImGui::TableNextCell();
    ImGui::Text( "%02x", sizeof( ehdr->e_flags ) );
    ImGui::TableNextCell();
    ImGui::Text( "%#x", ehdr->e_flags );
    ImGui::TableNextCell();

    // ==================================================================================================================== //

    ImGui::TableNextRow();
    ImGui::Text( "e_ehsize" );
    ImGui::TableNextCell();
    ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_ehsize ) );
    ImGui::TableNextCell();
    ImGui::Text( "%02x", sizeof( ehdr->e_ehsize ) );
    ImGui::TableNextCell();
    ImGui::Text( "%#x", ehdr->e_ehsize );
    ImGui::TableNextCell();

    // ==================================================================================================================== //

    ImGui::TableNextRow();
    ImGui::Text( "e_phentsize" );
    ImGui::TableNextCell();
    ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_phentsize ) );
    ImGui::TableNextCell();
    ImGui::Text( "%02x", sizeof( ehdr->e_phentsize ) );
    ImGui::TableNextCell();
    ImGui::Text( "%#x", ehdr->e_phentsize );
    ImGui::TableNextCell();

    // ==================================================================================================================== //

    ImGui::TableNextRow();
    ImGui::Text( "e_phnum" );
    ImGui::TableNextCell();
    ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_phnum ) );
    ImGui::TableNextCell();
    ImGui::Text( "%02x", sizeof( ehdr->e_phnum ) );
    ImGui::TableNextCell();
    ImGui::Text( "%d", ehdr->e_phnum );
    ImGui::TableNextCell();

    // ==================================================================================================================== //

    ImGui::TableNextRow();
    ImGui::Text( "e_shentsize" );
    ImGui::TableNextCell();
    ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_shentsize ) );
    ImGui::TableNextCell();
    ImGui::Text( "%02x", sizeof( ehdr->e_shentsize ) );
    ImGui::TableNextCell();
    ImGui::Text( "%#x", ehdr->e_shentsize );
    ImGui::TableNextCell();

    // ==================================================================================================================== //

    ImGui::TableNextRow();
    ImGui::Text( "e_shnum" );
    ImGui::TableNextCell();
    ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_shnum ) );
    ImGui::TableNextCell();
    ImGui::Text( "%02x", sizeof( ehdr->e_shnum ) );
    ImGui::TableNextCell();
    ImGui::Text( "%d", ehdr->e_shnum );
    ImGui::TableNextCell();

    // ==================================================================================================================== //

    ImGui::TableNextRow();
    ImGui::Text( "e_shstrndx" );
    ImGui::TableNextCell();
    ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_shstrndx ) );
    ImGui::TableNextCell();
    ImGui::Text( "%02x", sizeof( ehdr->e_shstrndx ) );
    ImGui::TableNextCell();
    ImGui::Text( "%d", ehdr->e_shstrndx );
    ImGui::TableNextCell();

    // ==================================================================================================================== //

    ImGui::EndTable();
}

void MainWindowProgHeader64( void )
{
    ImGui::Text( "Program Header %d", ctx.display.idx );
}

void MainWindowSectionHeader64( void )
{
    ImGui::Text( "Section Header %d, %s", ctx.display.idx, ctx.elf->get_section_name( ctx.display.idx ) );
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
        MainWindowElfHeader64();
    } else if ( ctx.display.hdr == ctx.display.phdr ) {
        MainWindowProgHeader64();
    } else if ( ctx.display.hdr == ctx.display.shdr ) {
        MainWindowSectionHeader64();
    } else {
        ImGui::Text( "ELF Header for %s", ctx.elf->filepath() );
    }

    ImGui::End();
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
                elfs.clear();
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
    ImGui::ShowMetricsWindow();
}
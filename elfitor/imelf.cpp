#include "imgui.h"
#include "imelf.h"
#include "elfreader/ELFReader.h"

#pragma warning( disable : 4200 )
template void Imelf::Ehdr::Draw< Elf32_Ehdr* >( Elf32_Ehdr* );
template void Imelf::Phdr::Draw< Elf32* >( Elf32* );
template void Imelf::Shdr::Draw< Elf32* >( Elf32* );
template void Imelf::Ehdr::Draw< Elf64_Ehdr* >( Elf64_Ehdr* );
template void Imelf::Phdr::Draw< Elf64* >( Elf64* );
template void Imelf::Shdr::Draw< Elf64* >( Elf64* );

typedef struct _ComboBoxMap {
    const int count;
    const struct {
        const uint32_t key;
        const char* const val;
    } kv[0];


    inline const char* get_val( uint32_t key ) const
    {
        for ( int i = 0; i < count; i++ ) {
            if ( key == kv[i].key ) {
                return kv[i].val;
            }
        }
        return nullptr;
    }
} ComboBoxMap;


void Tooltip( const char* desc )
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
        Tooltip( desc );
    }
}

void HoverTooltip( const char* desc )
{
    if ( ImGui::IsItemHovered() ) {
        Tooltip( desc );
    }
}


const ComboBoxMap EhdrMachineMap = {
    16,
    {
        0, "NONE", 
        1, "AT&T WE 32100", 
        2, "SPARC", 
        3, "x86", 
        4, "Motorola 68000", 
        5, "Motorola 88000", 
        7, "Intel 80860", 
        8, "MIPS", 
        0x14, "PowerPC", 
        0x16, "S390", 
        0x28, "ARM", 
        0x2a, "SuperH", 
        0x32, "IA-64", 
        0x3e, "amd64", 
        0xb7, "AArch64", 
        0xf3, "RISC-V"
    }
};

const ComboBoxMap EhdrClassMap = {
    2,
    { 
        1, "32-bit",
        2, "64-bit"
    }
};

const ComboBoxMap EhdrDataMap = {
    2,
    {
        1, "little-endian", 
        2, "big-endian"
    }
};

const ComboBoxMap EhdrOsabiMap = {
    17,
    {
        0, "System V",
        1, "HP-UX",
        2, "NetBSD",
        3, "Linux",
        4, "GNU Hurd",
        6, "Solaris",
        7, "AIX",
        8, "IRIX",
        9, "FreeBSD",
        0xa, "Tru64",
        0xb, "Novell Modesto",
        0xc, "OpenBSD",
        0xd, "OpenVMS",
        0xe, "NonStop Kernel",
        0xf, "AROS",
        0x10, "Fenix OS",
        0x11, "Cloud ABI",
    }
};

const ComboBoxMap EhdrTypeMap = {
    9,
    {
        0, "ET_NONE", 
        1, "ET_REL", 
        2, "ET_EXEC", 
        3, "ET_DYN", 
        4, "ET_CORE", 
        0xfe00, "ET_LOOS", 
        0xfeff, "ET_HIOS", 
        0xff00, "ET_LOPROC", 
        0xffff, "ET_HIPROC"
    },
};

const ComboBoxMap PhdrTypeMap = {
    18,
    {
        0, "PT_NULL", 
        1, "PT_LOAD", 
        2, "PT_DYNAMIC", 
        3, "PT_INTERP", 
        4, "PT_NOTE", 
        5, "PT_SHLIB", 
        6, "PT_PHDR", 
        7, "PT_TLS", 
        8, "PT_NUM", 
        0x60000000, "PT_LOOS", 
        0x6474e550, "PT_GNU_EH_FRAME",
        0x6474e551, "PT_GNU_STACK",
        0x6474e552, "PT_GNU_RELRO",
        0x6ffffffa, "PT_SUNWSTACK", 
        0x6ffffffb, "PT_SUNWBSS",
        0x6fffffff, "PT_HISUNW",
        0x70000000, "PT_LOPROC",
        0x7fffffff, "PT_HIPROC"
    },
};

namespace Imelf
{
    template< typename T >
    void ComboBox( T& dest, const ComboBoxMap& map )
    {
        const char* preview = "";
        for ( int i = 0; i < map.count; i++ ) {
            if ( dest == map.kv[i].key ) {
                preview = map.kv[i].val;
                break;
            }
        }

        if ( ImGui::BeginCombo( map.kv[0].val, preview, ImGuiComboFlags_None ) ) {
            for ( int i = 0; i < map.count; i++ ) {
                bool is_selected = ( preview == map.kv[i].val );
                if ( ImGui::Selectable( map.kv[i].val, is_selected ) ) {
                    dest = map.kv[i].key;
                    preview = map.kv[i].val;
                }
            }
            ImGui::EndCombo();
        }
    }

    template< typename T >
    void InputHex( const char* label, T& dest, bool valid = true )
    {
        ImGuiDataType flags = ImGuiDataType_U8;
        switch ( sizeof( ( (T*)0 )[0] ) ) {
            case 1:
                flags = ImGuiDataType_U8;
                break;
            case 2:
                flags = ImGuiDataType_U16;
                break;
            case 4:
                flags = ImGuiDataType_U32;
                break;
            case 8:
                flags = ImGuiDataType_U64;
                break;
        }

        if ( valid ) {
            ImGui::InputScalar( label, flags, &dest, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
        } else {
            ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
            ImGui::InputScalar( label, flags, &dest, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::PopStyleColor();
        }
    }

    namespace Ehdr
    {
        template< typename T >
        void Ident( T ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_ident[EI_MAG]" );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_ident ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_ident ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "ei_ident_input", ImGuiDataType_U32, ehdr->e_ident, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
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
        }

        template< typename T >
        void Class( T ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_ident[EI_CLASS]" );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_ident[EI_CLASS] ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_ident[EI_CLASS] ) );
            ImGui::TableNextCell();
            if ( EhdrClassMap.get_val( ehdr->e_ident[EI_CLASS] ) ) {
                ImGui::InputScalar( "ei_class_input", ImGuiDataType_U8, &ehdr->e_ident[EI_CLASS], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            } else {
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
                ImGui::InputScalar( "ei_class_input", ImGuiDataType_U8, &ehdr->e_ident[EI_CLASS], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
                ImGui::PopStyleColor();
            }
            ImGui::TableNextCell();
            Imelf::ComboBox( ehdr->e_ident[EI_CLASS], EhdrClassMap );
            ImGui::TableNextCell();
            HelpMarker( "This byte is set to either 1 or 2 to signify 32-bit or 64-bit format, respectively." );
        }

        template< typename T >
        void Data( T ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_ident[EI_DATA]" );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_ident[EI_DATA] ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_ident[EI_DATA] ) );
            ImGui::TableNextCell();
            if ( EhdrDataMap.get_val( ehdr->e_ident[EI_DATA] ) ) {
                ImGui::InputScalar( "ei_data_input", ImGuiDataType_U8, &ehdr->e_ident[EI_DATA], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            } else {
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
                ImGui::InputScalar( "ei_data_input", ImGuiDataType_U8, &ehdr->e_ident[EI_DATA], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
                ImGui::PopStyleColor();
            }
            ImGui::TableNextCell();
            Imelf::ComboBox( ehdr->e_ident[EI_DATA], EhdrDataMap );
            ImGui::TableNextCell();
            HelpMarker( "This byte is set to either 1 or 2 to signify little or big endianness, respectively. This affects interpretation of multi-byte fields starting with offset 0x10." );
        }

        template< typename T >
        void IdentVersion( T ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_ident[EI_VERSION]" );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_ident[EI_VERSION] ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_ident[EI_VERSION] ) );
            ImGui::TableNextCell();
            if ( ehdr->e_ident[EI_VERSION] == 1 ) {
                ImGui::InputScalar( "ei_version_input", ImGuiDataType_U8, &ehdr->e_ident[EI_VERSION], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            } else {
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
                ImGui::InputScalar( "ei_version_input", ImGuiDataType_U8, &ehdr->e_ident[EI_VERSION], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
                ImGui::PopStyleColor();
            }
            ImGui::TableNextCell();
            ImGui::TableNextCell();
            HelpMarker( "Set to 1 for the original and current version of ELF." );
        }

        template< typename T >
        void Osabi( T ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_ident[EI_OSABI]" );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_ident[EI_OSABI] ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_ident[EI_OSABI] ) );
            ImGui::TableNextCell();
            if ( EhdrOsabiMap.get_val( ehdr->e_ident[EI_OSABI] ) ) {
                ImGui::InputScalar( "ei_osabi_input", ImGuiDataType_U8, &ehdr->e_ident[EI_OSABI], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            } else {
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
                ImGui::InputScalar( "ei_osabi_input", ImGuiDataType_U8, &ehdr->e_ident[EI_OSABI], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
                ImGui::PopStyleColor();
            }
            ImGui::TableNextCell();
            Imelf::ComboBox( ehdr->e_ident[EI_OSABI], EhdrOsabiMap );
            ImGui::TableNextCell();
            HelpMarker( "Identifies the target operating system ABI. It is often set to 0 regardless of the target platform." );
        }

        template< typename T >
        void Abi( T ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_ident[EI_ABIVERSION]" );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_ident[EI_ABIVERSION] ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_ident[EI_ABIVERSION] ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "ei_abiver_input", ImGuiDataType_U8, &ehdr->e_ident[EI_ABIVERSION], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
            HelpMarker( "Further specifies the ABI version. Its interpretation depends on the target ABI. "
                        "Linux kernel (after at least 2.6) has no definition of it, so it is ignored for statically-linked executables." );
        }

        template< typename T >
        void Type( T ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_type" );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_type ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_type ) );
            ImGui::TableNextCell();
            if ( EhdrTypeMap.get_val( ehdr->e_type ) ) {
                ImGui::InputScalar( "e_type_input", ImGuiDataType_U16, &ehdr->e_type, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            } else {
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
                ImGui::InputScalar( "e_type_input", ImGuiDataType_U16, &ehdr->e_type, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
                ImGui::PopStyleColor();
            }

            ImGui::TableNextCell();
            Imelf::ComboBox( ehdr->e_type, EhdrTypeMap );
            ImGui::TableNextCell();
            HelpMarker( "Identifies object file type." );
        }

        template< typename T >
        void Machine( T ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_machine" );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_machine ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_machine ) );
            ImGui::TableNextCell();
            if ( EhdrMachineMap.get_val( ehdr->e_machine ) ) {
                ImGui::InputScalar( "e_machine_input", ImGuiDataType_U16, &ehdr->e_machine, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            } else {
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
                ImGui::InputScalar( "e_machine_input", ImGuiDataType_U16, &ehdr->e_machine, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
                ImGui::PopStyleColor();
            }
            ImGui::TableNextCell();
            Imelf::ComboBox( ehdr->e_machine, EhdrMachineMap );
            ImGui::TableNextCell();
            HelpMarker( "Specifies target instruction set architecture." );
        }

        template< typename T >
        void Version( T ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_version" );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_version ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_version ) );
            ImGui::TableNextCell();
            if ( ehdr->e_version == 1 ) {
                ImGui::InputScalar( "e_version_input", ImGuiDataType_U32, &ehdr->e_version, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            } else {
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
                ImGui::InputScalar( "e_version_input", ImGuiDataType_U32, &ehdr->e_version, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
                ImGui::PopStyleColor();
            }
            ImGui::TableNextCell();
            ImGui::TableNextCell();
            HelpMarker( "Set to 1 for the original version of ELF." );
        }

        template< typename T >
        void Entry( T ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_entry" );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_entry ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_entry ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "e_entry_input", ImGuiDataType_U64, &ehdr->e_entry, NULL, NULL, "%llX", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
            HelpMarker( "This is the memory address of the entry point from where the process starts executing." );
        }

        template< typename T >
        void Phoff( T ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_phoff" );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_phoff ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_phoff ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "e_phoff_input", ImGuiDataType_U64, &ehdr->e_phoff, NULL, NULL, "%llX", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
            HelpMarker( "Points to the start of the program header table. It usually follows the file header immediately." );
        }

        template< typename T >
        void Shoff( T ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_shoff" );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_shoff ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_shoff ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "e_shoff_input", ImGuiDataType_U64, &ehdr->e_shoff, NULL, NULL, "%llX", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
            HelpMarker( "Points to the start of the section header table." );
        }

        template< typename T >
        void Flags( T ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_flags" );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_flags ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_flags ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "e_flags_input", ImGuiDataType_U32, &ehdr->e_flags, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
            HelpMarker( "Interpretation of this field depends on the target architecture." );
        }

        template< typename T >
        void Ehsize( T ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_ehsize" );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_ehsize ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_ehsize ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "e_ehsize_input", ImGuiDataType_U16, &ehdr->e_ehsize, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
            HelpMarker( "Contains the size of this header, normally 64 Bytes for 64-bit and 52 Bytes for 32-bit format." );
        }

        template< typename T >
        void Phentsize( T ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_phentsize" );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_phentsize ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_phentsize ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "e_phentsize_input", ImGuiDataType_U16, &ehdr->e_phentsize, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
            HelpMarker( "Contains the size of a program header table entry." );
        }

        template< typename T >
        void Phnum( T ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_phnum" );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_phnum ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_phnum ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "e_phnum_input", ImGuiDataType_U16, &ehdr->e_phnum, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
            HelpMarker( "Contains the number of entries in the program header table." );
        }

        template< typename T >
        void Shentsize( T ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_shentsize" );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_shentsize ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_shentsize ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "e_shentsize_input", ImGuiDataType_U16, &ehdr->e_shentsize, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
            HelpMarker( "Contains the size of a section header table entry." );
        }

        template< typename T >
        void Shnum( T ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_shnum" );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_shnum ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_shnum ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "e_shnum_input", ImGuiDataType_U16, &ehdr->e_shnum, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
            HelpMarker( "Contains the number of entries in the section header table." );
        }

        template< typename T >
        void Shstrndx( T ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_shstrndx" );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( Elf64_Ehdr, e_shstrndx ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_shstrndx ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "e_shstrndx_input", ImGuiDataType_U16, &ehdr->e_shstrndx, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
            HelpMarker( "Contains index of the section header table entry that contains the section names." );
        }

        template< typename T >
        void Draw( T ehdr )
        {
            ImGuiTableFlags flags = ImGuiTableFlags_SizingPolicyFixedX | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;
            ImGui::BeginTable( "elf header", 6, flags, ImVec2( 0, ImGui::GetTextLineHeightWithSpacing() * 1 ) );
            ImGui::TableSetupColumn( "Member", 0, 220.0 );
            ImGui::TableSetupColumn( "Offset", 0, 100.0 );
            ImGui::TableSetupColumn( "Size", 0, 100.0 );
            ImGui::TableSetupColumn( "Value", 0, 150.0 );
            ImGui::TableSetupColumn( "Meaning", 0, 200.0 );
            ImGui::TableSetupColumn( "", ImGuiTableColumnFlags_WidthStretch );
            ImGui::TableAutoHeaders();

            Ident( ehdr );
            Class( ehdr );
            Data( ehdr );
            IdentVersion( ehdr );
            Osabi( ehdr );
            Abi( ehdr );
            Type( ehdr );
            Machine( ehdr );
            Version( ehdr );
            Entry( ehdr );
            Phoff( ehdr );
            Shoff( ehdr );
            Flags( ehdr );
            Ehsize( ehdr );
            Phentsize( ehdr );
            Phnum( ehdr );
            Shentsize( ehdr );
            Shnum( ehdr );
            Shstrndx( ehdr );

            ImGui::EndTable();
        }
    }

    namespace Phdr
    {
        template< typename T >
        void Flags( T& perm )
        {
            bool read, write, exec;
            read = true ? perm & 4 : false;
            write = true ? perm & 2 : false;
            exec = true ? perm & 1 : false;

            if ( ImGui::Checkbox( "##read", &read ) ) {
                perm ^= 4;
            }
            ImGui::SameLine();
            if ( ImGui::Checkbox( "##write", &write ) ) {
                perm ^= 2;
            }
            ImGui::SameLine();
            if ( ImGui::Checkbox( "##exec", &exec ) ) {
                perm ^= 1;
            }
        }

        template< typename T >
        void TableRow( T phdr )
        {
            ImGui::TableNextRow();
            Imelf::ComboBox( phdr->p_type, PhdrTypeMap );

            ImGui::TableNextCell();
            Imelf::Phdr::Flags( phdr->p_flags );

            ImGui::TableNextCell();
            ImGui::InputScalar( "##offset", ImGuiDataType_U64, &phdr->p_offset, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );

            ImGui::TableNextCell();
            ImGui::InputScalar( "##vaddr", ImGuiDataType_U64, &phdr->p_vaddr, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );

            ImGui::TableNextCell();
            ImGui::InputScalar( "##paddr", ImGuiDataType_U64, &phdr->p_paddr, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );

            ImGui::TableNextCell();
            ImGui::InputScalar( "##filesz", ImGuiDataType_U64, &phdr->p_filesz, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );

            ImGui::TableNextCell();
            ImGui::InputScalar( "##memsz", ImGuiDataType_U64, &phdr->p_memsz, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );

            ImGui::TableNextCell();
            ImGui::InputScalar( "##memsz", ImGuiDataType_U64, &phdr->p_align, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
        }

        template< typename T >
        void InterpretData( T phdr )
        {
            if ( phdr->p_type == 2 ) {
                // PT_DYNAMIC

            } else {
                //mViewer.HexViewer( phdr, sizeof( *phdr ) );
            }
        }

        template< typename T >
        void Draw( T elf )
        {
            ImGuiTableFlags flags = ImGuiTableFlags_SizingPolicyFixedX | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;
            ImGui::BeginTable( "elf program", 8, flags, ImVec2( 0, ImGui::GetTextLineHeightWithSpacing() * 1 ) );
            ImGui::TableSetupColumn( "Type", 0, 170.0 );
            ImGui::TableSetupColumn( "R W X", 0, 100.0 );
            ImGui::TableSetupColumn( "File Offset", 0, 150.0 );
            ImGui::TableSetupColumn( "Virtual Address", 0, 200.0 );
            ImGui::TableSetupColumn( "Physical Address", 0, 200.0 );
            ImGui::TableSetupColumn( "Segment File Size", 0, 200.0 );
            ImGui::TableSetupColumn( "Segment Virtual Size", 0, 220.0 );
            ImGui::TableSetupColumn( "Alignment", ImGuiTableColumnFlags_WidthStretch );

            const char* descs[] = { "Identifies the type of the segment.", "Segment-dependent flags (RWX).", "Offset of the segment in the file image.", "Virtual address of the segment in memory.", "On systems where physical address is relevant, reserved for segment's physical address.", "Size in bytes of the segment in the file image. May be 0.", "Size in bytes of the segment in memory. May be 0.", "0 and 1 specify no alignment. Otherwise should be a positive, integral power of 2, with p_vaddr equating p_offset modulus p_align." };

            ImGui::TableNextRow( ImGuiTableRowFlags_Headers );
            for ( int i = 0; i < _countof(descs); i++ ) {
                ImGui::TableSetColumnIndex( i );
                const char* column_name = ImGui::TableGetColumnName( i ); // Retrieve name passed to TableSetupColumn()
                ImGui::PushID( i );
                ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 0, 0 ) );
                HelpMarker( descs[i] );
                ImGui::PopStyleVar();
                ImGui::SameLine( 0.0f, ImGui::GetStyle().ItemInnerSpacing.x );
                ImGui::TableHeader( column_name );
                ImGui::PopID();
            }


            auto ehdr = elf->get_elf_header();
            for ( int i = 0; i < ehdr->e_phnum; i++ ) {
                auto phdr = elf->get_prog_header( i );
                ImGui::PushID( i );
                TableRow( phdr );
                ImGui::PopID();
            }
            ImGui::EndTable();

            ImGui::Separator();
            ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 10.0f );

            auto ctx = elf->get_ctx();

            ImGuiTabBarFlags tflags = ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_TabListPopupButton;
            if ( ImGui::BeginTabBar( "PhdrDataTabs", flags ) ) {
                for ( int i = 0; i < ehdr->e_phnum; i++ ) {
                    auto phdr = elf->get_prog_header( i );
                    ImGui::PushID( i );
                    auto type = PhdrTypeMap.get_val( phdr->p_type );
                    type = type ? type : "";
                    if ( ImGui::BeginTabItem( type, nullptr, 0 ) ) {
                        ctx.idx = i;
                        InterpretData( phdr );
                        ImGui::EndTabItem();
                    }
                    ImGui::PopID();
                }
                ImGui::EndTabBar();
            }

        }
    }
    
    namespace Shdr
    {
		template<typename O, typename T>
        void TableRow( O elf, T shdr, int idx )
        {
            ImGui::TableNextRow();
            ImGui::Text( "%s", elf->get_section_name( idx ) );

            ImGui::TableNextCell();
            InputHex( "sh_type", shdr->sh_type );

			ImGui::TableNextCell();
            InputHex( "sh_flags", shdr->sh_flags );

			ImGui::TableNextCell();
            InputHex( "sh_addr", shdr->sh_addr );

            ImGui::TableNextCell();
            InputHex( "sh_offset", shdr->sh_offset );

            ImGui::TableNextCell();
            InputHex( "sh_size", shdr->sh_size );

            ImGui::TableNextCell();
            InputHex( "sh_link", shdr->sh_link );

            ImGui::TableNextCell();
            InputHex( "sh_info", shdr->sh_info );

            ImGui::TableNextCell();
            InputHex( "sh_addralign", shdr->sh_addralign );

            ImGui::TableNextCell();
            InputHex( "sh_entsize", shdr->sh_entsize );
        }

        template< typename T >
        void Draw( T elf )
        {
            ImGuiTableFlags flags = ImGuiTableFlags_SizingPolicyFixedX | ImGuiTableFlags_NoHostExtendY | ImGuiTableFlags_Scroll | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;
            ImGui::BeginTable( "elf sections", 10, flags, ImVec2( 0,  250.0f ) );
            ImGui::TableSetupColumn( "Name", 0, 170.0 );
            ImGui::TableSetupColumn( "Type", 0, 100.0 );
            ImGui::TableSetupColumn( "Flags", 0, 100.0 );
            ImGui::TableSetupColumn( "Virtual Address", 0, 180.0 );
            ImGui::TableSetupColumn( "File Offset", 0, 150.0 );
            ImGui::TableSetupColumn( "File Size", 0, 130.0 );
            ImGui::TableSetupColumn( "Link", 0, 90.0 );
            ImGui::TableSetupColumn( "Info", 0, 90.0 );
            ImGui::TableSetupColumn( "Alignment", 0, 130.0 );
            ImGui::TableSetupColumn( "Header Size", ImGuiTableColumnFlags_WidthStretch );

            const char* descs[] = { 
				"An offset to a string in the .shstrtab section that represents the name of this section",
				"Identifies the type of this header.",
				"Identifies the attributes of the section.",
				"Virtual address of the section in memory, for sections that are loaded.",
				"Offset of the section in the file image.",
				"Size in bytes of the section in the file image. May be 0.",
				"Contains the section index of an associated section. This field is used for several purposes, depending on the type of section.",
				"Contains extra information about the section. This field is used for several purposes, depending on the type of section.",
				"Contains the required alignment of the section. This field must be a power of two.",
				"Contains the size, in bytes, of each entry, for sections that contain fixed-size entries. Otherwise, this field contains zero.",
			};

            ImGui::TableNextRow( ImGuiTableRowFlags_Headers );
            for ( int i = 0; i < _countof(descs); i++ ) {
                ImGui::TableSetColumnIndex( i );
                const char* column_name = ImGui::TableGetColumnName( i ); // Retrieve name passed to TableSetupColumn()
                ImGui::PushID( i );
                ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 0, 0 ) );
                HelpMarker( descs[i] );
                ImGui::PopStyleVar();
                ImGui::SameLine( 0.0f, ImGui::GetStyle().ItemInnerSpacing.x );
                ImGui::TableHeader( column_name );
                ImGui::PopID();
            }
            
			auto ehdr = elf->get_elf_header();
            for ( int i = 0; i < ehdr->e_shnum; i++ ) {
                auto shdr = elf->get_section_header( i );
                ImGui::PushID( i );
                TableRow( elf, shdr, i );
                ImGui::PopID();
            }

			ImGui::EndTable();
            ImGui::Separator();
            ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 10.0f );

            auto ctx = elf->get_ctx();

            ImGuiTabBarFlags tflags = ImGuiTabBarFlags_FittingPolicyResizeDown;
            if ( ImGui::BeginTabBar( "ShdrDataTabs", tflags ) ) {
                for ( int i = 0; i < ehdr->e_shnum; i++ ) {
                    auto shdr = elf->get_section_header( i );
                    ImGui::PushID( i );
                    const char* type = elf->get_section_name( i );
                    type = type ? type : "";
                    ImGuiTabItemFlags iflags = ImGuiTabItemFlags_NoCloseWithMiddleMouseButton;
                    if ( ImGui::BeginTabItem( type, nullptr, iflags ) ) {
                        ctx.idx = i;
                        //mViewer.HexViewer( shdr, sizeof( *shdr ) );
                        ImGui::EndTabItem();
                    }
                    ImGui::PopID();
                }
                ImGui::EndTabBar();
            }
		}
    }
}


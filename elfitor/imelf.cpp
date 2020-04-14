#include "imgui.h"
#include "imelf.h"
#include "elfreader/ELFReader.h"

template void Imelf::Ehdr::Draw< Elf32_Ehdr* >( Elf32_Ehdr* );
template void Imelf::Ehdr::Draw< Elf64_Ehdr* >( Elf64_Ehdr* );

template void Imelf::Phdr::Draw< Elf32* >( Elf32* );
template void Imelf::Phdr::Draw< Elf64* >( Elf64* );

template void Imelf::Shdr::Draw< Elf64* >( Elf64* );
template void Imelf::Shdr::Draw< Elf32* >( Elf32* );


void HelpMarker( const char* desc )
{
    ImGui::TextDisabled( "(?)" );
    Tooltip( desc );
}

void Tooltip( const char* desc )
{
    if ( ImGui::IsItemHovered() ) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos( ImGui::GetFontSize() * 35.0f );
        ImGui::TextUnformatted( desc );
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void FocusTooltip( const char* desc )
{
    if ( ImGui::IsItemFocused() ) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos( ImGui::GetFontSize() * 35.0f );
        ImGui::TextUnformatted( desc );
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}


const ComboBoxMap EhdrClassMap = {
    2,
    { 
        ELFCLASS32, "32-bit", "",
        ELFCLASS64, "64-bit", ""
    }
};

const ComboBoxMap EhdrDataMap = {
    2,
    {
        ELFDATA2LSB, "little-endian", "", 
        ELFDATA2MSB, "big-endian", ""
    }
};

const ComboBoxMap EhdrOsabiMap = {
    20,
    {
        ELFOSABI_SYSV, "System V", "", 
        ELFOSABI_HPUX, "HP-UX", "", 
        ELFOSABI_NETBSD, "NetBSD", "",
        ELFOSABI_LINUX, "Linux", "", 
        4, "GNU Hurd", "", 
        ELFOSABI_SOLARIS, "Solaris", "", 
        ELFOSABI_AIX, "AIX", "", 
        ELFOSABI_IRIX, "IRIX", "", 
        ELFOSABI_FREEBSD, "FreeBSD", "", 
        ELFOSABI_TRU64, "Tru64", "", 
        ELFOSABI_MODESTO, "Novell Modesto", "",
        ELFOSABI_OPENBSD, "OpenBSD", "",
        0xd, "OpenVMS", "",
        0xe, "NonStop Kernel", "",
        0xf, "AROS", "",
        0x10, "Fenix OS", "",
        0x11, "Cloud ABI", "",
        ELFOSABI_ARM_AEABI, "ARM EABI", "",
        ELFOSABI_ARM, "ARM", "",
        ELFOSABI_STANDALONE, "Embedded Application", ""
    }
};

const ComboBoxMap EhdrTypeMap = {
    9,
    {
        ET_NONE, "ET_NONE", "",
        ET_REL, "ET_REL", "",
        ET_EXEC, "ET_EXEC", "",
        ET_DYN, "ET_DYN", "",
        ET_CORE, "ET_CORE", "",
        ET_LOOS, "ET_LOOS", "", 
        ET_HIOS, "ET_HIOS", "",
        ET_LOPROC, "ET_LOPROC", "",
        ET_HIPROC, "ET_HIPROC", "",
    },
};

const ComboBoxMap EhdrMachineMap = {
    178,
    {
        EM_NONE, "EM_NONE", "",
        EM_M32, "EM_M32", "",
        EM_SPARC, "EM_SPARC", "",
        EM_386, "EM_386", "",
        EM_68K, "EM_68K", "",
        EM_88K, "EM_88K", "",
        EM_IAMCU, "EM_IAMCU", "",
        EM_860, "EM_860", "",
        EM_MIPS, "EM_MIPS", "",
        EM_S370, "EM_S370", "",
        EM_MIPS_RS3_LE, "EM_MIPS_RS3_LE", "",
        EM_PARISC, "EM_PARISC", "",
        EM_VPP500, "EM_VPP500", "",
        EM_SPARC32PLUS, "EM_SPARC32PLUS", "",
        EM_960, "EM_960", "",
        EM_PPC, "EM_PPC", "",
        EM_PPC64, "EM_PPC64", "",
        EM_S390, "EM_S390", "",
        EM_SPU, "EM_SPU", "",
        EM_V800, "EM_V800", "",
        EM_FR20, "EM_FR20", "",
        EM_RH32, "EM_RH32", "",
        EM_RCE, "EM_RCE", "",
        EM_ARM, "EM_ARM", "",
        EM_FAKE_ALPHA, "EM_FAKE_ALPHA", "",
        EM_SH, "EM_SH", "",
        EM_SPARCV9, "EM_SPARCV9", "",
        EM_TRICORE, "EM_TRICORE", "",
        EM_ARC, "EM_ARC", "",
        EM_H8_300, "EM_H8_300", "",
        EM_H8_300H, "EM_H8_300H", "",
        EM_H8S, "EM_H8S", "",
        EM_H8_500, "EM_H8_500", "",
        EM_IA_64, "EM_IA_64", "",
        EM_MIPS_X, "EM_MIPS_X", "",
        EM_COLDFIRE, "EM_COLDFIRE", "",
        EM_68HC12, "EM_68HC12", "",
        EM_MMA, "EM_MMA", "",
        EM_PCP, "EM_PCP", "",
        EM_NCPU, "EM_NCPU", "",
        EM_NDR1, "EM_NDR1", "",
        EM_STARCORE, "EM_STARCORE", "",
        EM_ME16, "EM_ME16", "",
        EM_ST100, "EM_ST100", "",
        EM_TINYJ, "EM_TINYJ", "",
        EM_X86_64, "EM_X86_64", "",
        EM_PDSP, "EM_PDSP", "",
        EM_PDP10, "EM_PDP10", "",
        EM_PDP11, "EM_PDP11", "",
        EM_FX66, "EM_FX66", "",
        EM_ST9PLUS, "EM_ST9PLUS", "",
        EM_ST7, "EM_ST7", "",
        EM_68HC16, "EM_68HC16", "",
        EM_68HC11, "EM_68HC11", "",
        EM_68HC08, "EM_68HC08", "",
        EM_68HC05, "EM_68HC05", "",
        EM_SVX, "EM_SVX", "",
        EM_ST19, "EM_ST19", "",
        EM_VAX, "EM_VAX", "",
        EM_CRIS, "EM_CRIS", "",
        EM_JAVELIN, "EM_JAVELIN", "",
        EM_FIREPATH, "EM_FIREPATH", "",
        EM_ZSP, "EM_ZSP", "",
        EM_MMIX, "EM_MMIX", "",
        EM_HUANY, "EM_HUANY", "",
        EM_PRISM, "EM_PRISM", "",
        EM_AVR, "EM_AVR", "",
        EM_FR30, "EM_FR30", "",
        EM_D10V, "EM_D10V", "",
        EM_D30V, "EM_D30V", "",
        EM_V850, "EM_V850", "",
        EM_M32R, "EM_M32R", "",
        EM_MN10300, "EM_MN10300", "",
        EM_MN10200, "EM_MN10200", "",
        EM_PJ, "EM_PJ", "",
        EM_OPENRISC, "EM_OPENRISC", "",
        EM_ARC_COMPACT, "EM_ARC_COMPACT", "",
        EM_XTENSA, "EM_XTENSA", "",
        EM_VIDEOCORE, "EM_VIDEOCORE", "",
        EM_TMM_GPP, "EM_TMM_GPP", "",
        EM_NS32K, "EM_NS32K", "",
        EM_TPC, "EM_TPC", "",
        EM_SNP1K, "EM_SNP1K", "",
        EM_ST200, "EM_ST200", "",
        EM_IP2K, "EM_IP2K", "",
        EM_MAX, "EM_MAX", "",
        EM_CR, "EM_CR", "",
        EM_F2MC16, "EM_F2MC16", "",
        EM_MSP430, "EM_MSP430", "",
        EM_BLACKFIN, "EM_BLACKFIN", "",
        EM_SE_C33, "EM_SE_C33", "",
        EM_SEP, "EM_SEP", "",
        EM_ARCA, "EM_ARCA", "",
        EM_UNICORE, "EM_UNICORE", "",
        EM_EXCESS, "EM_EXCESS", "",
        EM_DXP, "EM_DXP", "",
        EM_ALTERA_NIOS2, "EM_ALTERA_NIOS2", "",
        EM_CRX, "EM_CRX", "",
        EM_XGATE, "EM_XGATE", "",
        EM_C166, "EM_C166", "",
        EM_M16C, "EM_M16C", "",
        EM_DSPIC30F, "EM_DSPIC30F", "",
        EM_CE, "EM_CE", "",
        EM_M32C, "EM_M32C", "",
        EM_TSK3000, "EM_TSK3000", "",
        EM_RS08, "EM_RS08", "",
        EM_SHARC, "EM_SHARC", "",
        EM_ECOG2, "EM_ECOG2", "",
        EM_SCORE7, "EM_SCORE7", "",
        EM_DSP24, "EM_DSP24", "",
        EM_VIDEOCORE3, "EM_VIDEOCORE3", "",
        EM_LATTICEMICO32, "EM_LATTICEMICO32", "",
        EM_SE_C17, "EM_SE_C17", "",
        EM_TI_C6000, "EM_TI_C6000", "",
        EM_TI_C2000, "EM_TI_C2000", "",
        EM_TI_C5500, "EM_TI_C5500", "",
        EM_TI_ARP32, "EM_TI_ARP32", "",
        EM_TI_PRU, "EM_TI_PRU", "",
        EM_MMDSP_PLUS, "EM_MMDSP_PLUS", "",
        EM_CYPRESS_M8C, "EM_CYPRESS_M8C", "",
        EM_R32C, "EM_R32C", "",
        EM_TRIMEDIA, "EM_TRIMEDIA", "",
        EM_QDSP6, "EM_QDSP6", "",
        EM_8051, "EM_8051", "",
        EM_STXP7X, "EM_STXP7X", "",
        EM_NDS32, "EM_NDS32", "",
        EM_ECOG1X, "EM_ECOG1X", "",
        EM_MAXQ30, "EM_MAXQ30", "",
        EM_XIMO16, "EM_XIMO16", "",
        EM_MANIK, "EM_MANIK", "",
        EM_CRAYNV2, "EM_CRAYNV2", "",
        EM_RX, "EM_RX", "",
        EM_METAG, "EM_METAG", "",
        EM_MCST_ELBRUS, "EM_MCST_ELBRUS", "",
        EM_ECOG16, "EM_ECOG16", "",
        EM_CR16, "EM_CR16", "",
        EM_ETPU, "EM_ETPU", "",
        EM_SLE9X, "EM_SLE9X", "",
        EM_L10M, "EM_L10M", "",
        EM_K10M, "EM_K10M", "",
        EM_AARCH64, "EM_AARCH64", "",
        EM_AVR32, "EM_AVR32", "",
        EM_STM8, "EM_STM8", "",
        EM_TILE64, "EM_TILE64", "",
        EM_TILEPRO, "EM_TILEPRO", "",
        EM_MICROBLAZE, "EM_MICROBLAZE", "",
        EM_CUDA, "EM_CUDA", "",
        EM_TILEGX, "EM_TILEGX", "",
        EM_CLOUDSHIELD, "EM_CLOUDSHIELD", "",
        EM_COREA_1ST, "EM_COREA_1ST", "",
        EM_COREA_2ND, "EM_COREA_2ND", "",
        EM_ARC_COMPACT2, "EM_ARC_COMPACT2", "",
        EM_OPEN8, "EM_OPEN8", "",
        EM_RL78, "EM_RL78", "",
        EM_VIDEOCORE5, "EM_VIDEOCORE5", "",
        EM_78KOR, "EM_78KOR", "",
        EM_56800EX, "EM_56800EX", "",
        EM_BA1, "EM_BA1", "",
        EM_BA2, "EM_BA2", "",
        EM_XCORE, "EM_XCORE", "",
        EM_MCHP_PIC, "EM_MCHP_PIC", "",
        EM_KM32, "EM_KM32", "",
        EM_KMX32, "EM_KMX32", "",
        EM_EMX16, "EM_EMX16", "",
        EM_EMX8, "EM_EMX8", "",
        EM_KVARC, "EM_KVARC", "",
        EM_CDP, "EM_CDP", "",
        EM_COGE, "EM_COGE", "",
        EM_COOL, "EM_COOL", "",
        EM_NORC, "EM_NORC", "",
        EM_CSR_KALIMBA, "EM_CSR_KALIMBA", "",
        EM_Z80, "EM_Z80", "",
        EM_VISIUM, "EM_VISIUM", "",
        EM_FT32, "EM_FT32", "",
        EM_MOXIE, "EM_MOXIE", "",
        EM_AMDGPU, "EM_AMDGPU", "",
        EM_RISCV, "EM_RISCV", "",
        EM_BPF, "EM_BPF", "",
    }
};

const ComboBoxMap PhdrTypeMap = {
    18,
    {
        PT_NULL, "PT_NULL", "",
        PT_LOAD, "PT_LOAD", "",
        PT_DYNAMIC, "PT_DYNAMIC", "",
        PT_INTERP, "PT_INTERP", "",
        PT_NOTE, "PT_NOTE", "",
        PT_SHLIB, "PT_SHLIB", "",
        PT_PHDR, "PT_PHDR", "",
        PT_TLS, "PT_TLS", "",
        PT_NUM, "PT_NUM", "",
        PT_LOOS, "PT_LOOS", "",
        PT_GNU_EH_FRAME, "PT_GNU_EH_FRAME", "",
        PT_GNU_STACK, "PT_GNU_STACK", "",
        PT_GNU_RELRO, "PT_GNU_RELRO", "",
        PT_SUNWBSS, "PT_SUNWBSS", "",
        PT_SUNWSTACK, "PT_SUNWSTACK", "",
        PT_HISUNW, "PT_HISUNW", "",
        PT_LOPROC, "PT_LOPROC", "",
        PT_HIPROC, "PT_HIPROC", ""
    },
};

const ComboBoxMap PhdrTypeFlags = {
    7,
    {
        PF_X, "  X", "",
        PF_W, " W ", "",
        PF_W | PF_X, " WX", "",
        PF_R, "R  ", "",
        PF_R | PF_X, "R X", "",
        PF_R | PF_X, "RW ", "",
        PF_R | PF_W | PF_X, "RWX", "",
    }
};

const ComboBoxMap ShdrTypeMap = {
    33,
    { 
        SHT_NULL, "SHT_NULL", "Section header table entry unused",
        SHT_PROGBITS, "SHT_PROGBITS", "Program data",
        SHT_SYMTAB, "SHT_SYMTAB", "Symbol table",
        SHT_STRTAB, "SHT_STRTAB", "String table",
        SHT_RELA, "SHT_RELA", "Relocation entries with addends",
        SHT_HASH, "SHT_HASH", "Symbol hash table",
        SHT_DYNAMIC, "SHT_DYNAMIC", "Dynamic linking information",
        SHT_NOTE, "SHT_NOTE", "Notes",
        SHT_NOBITS, "SHT_NOBITS", "Program space with no data (bss)",
        SHT_REL, "SHT_REL", "Relocation entries, no addends",
        SHT_SHLIB, "SHT_SHLIB", "Reserved",
        SHT_DYNSYM, "SHT_DYNSYM", "Dynamic linker symbol table",
        SHT_INIT_ARRAY, "SHT_INIT_ARRAY", "Array of constructors",
        SHT_FINI_ARRAY, "SHT_FINI_ARRAY", "Array of destructors",
        SHT_PREINIT_ARRAY, "SHT_PREINIT_ARRAY", "Array of pre-constructors",
        SHT_GROUP, "SHT_GROUP", "Section group",
        SHT_SYMTAB_SHNDX, "SHT_SYMTAB_SHNDX", "Extended section indeces",
        SHT_NUM, "SHT_NUM", "Number of defined types",
        SHT_LOOS, "SHT_LOOS", "Start OS-specific",
        SHT_GNU_ATTRIBUTES, "SHT_GNU_ATTRIBUTES", "Object attributes",
        SHT_GNU_HASH, "SHT_GNU_HASH", "GNU-style hash table",
        SHT_GNU_LIBLIST, "SHT_GNU_LIBLIST", "Prelink library list",
        SHT_CHECKSUM, "SHT_CHECKSUM", "Checksum for DSO content",
        SHT_SUNW_move, "SHT_SUNW_move", "Sun-specific low bound",
        SHT_SUNW_COMDAT, "SHT_SUNW_COMDAT", "",
        SHT_SUNW_syminfo, "SHT_SUNW_syminfo", "",
        SHT_GNU_verdef, "SHT_GNU_verdef", "Version definition section",
        SHT_GNU_verneed, "SHT_GNU_verneed", "Version needs section",
        SHT_GNU_versym, "SHT_GNU_versym", "Version symbol table",
        SHT_LOPROC, "SHT_LOPROC", "Start of processor-specific",
        SHT_HIPROC, "SHT_HIPROC", "End of processor-specific",
        SHT_LOUSER, "SHT_LOUSER", "Start of application-specific",
        SHT_HIUSER, "SHT_HIUSER", "End of application-specific",
    }
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

    void* AlignUp( void* ptr, uint32_t alignment )
    {
        uint32_t rem = (uintptr_t)ptr % alignment;
        if ( rem )
            return reinterpret_cast< void* >( (uintptr_t)ptr + alignment - rem );
        return ptr;
    }

    template< typename T >
    void DrawDynamic( T* dyn )
    {
        ImGuiTableFlags flags = ImGuiTableFlags_SizingPolicyFixedX | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;
        ImGui::BeginTable( ".dynamic", 2, flags, ImVec2( 0, ImGui::GetTextLineHeightWithSpacing() * 1 ) );
        ImGui::TableSetupColumn( "Tag", 0, 220.0 );
        ImGui::TableSetupColumn( "Value", ImGuiTableColumnFlags_WidthStretch );
        ImGui::TableAutoHeaders();

        while ( dyn->d_tag ) {
            ImGui::TableNextRow();
            ImGui::Text( "%#llx", dyn->d_tag );
            ImGui::TableNextCell();
            ImGui::Text( "%#llx", dyn->d_un.d_val );
            dyn++;
        }

        ImGui::EndTable();
    }

    namespace Ehdr
    {
        template< typename T >
        void Ident( T* ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_ident[EI_MAG]" );
            Tooltip( "0x7F followed by ELF(45 4c 46) in ASCII; these four bytes constitute the magic number." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( T, e_ident ));
            ImGui::TableNextCell();
            ImGui::Text( "%02x", 4 );
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
        }

        template< typename T >
        void Class( T* ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_ident[EI_CLASS]" );
            Tooltip( "This byte is set to either 1 or 2 to signify 32-bit or 64-bit format, respectively." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( T, e_ident[EI_CLASS] ) );
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
        }

        template< typename T >
        void Data( T* ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_ident[EI_DATA]" );
            Tooltip( "This byte is set to either 1 or 2 to signify little or big endianness, respectively. This affects interpretation of multi-byte fields starting with offset 0x10." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof(T, e_ident[EI_DATA]) );
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
        }

        template< typename T >
        void IdentVersion( T* ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_ident[EI_VERSION]" );
            Tooltip( "Set to 1 for the original and current version of ELF." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( T, e_ident[EI_VERSION] ) );
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
        }

        template< typename T >
        void Osabi( T* ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_ident[EI_OSABI]" );
            Tooltip( "Identifies the target operating system ABI. It is often set to 0 regardless of the target platform." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( T, e_ident[EI_OSABI] ) );
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
        }

        template< typename T >
        void Abi( T* ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_ident[EI_ABIVERSION]" );
            Tooltip( "Further specifies the ABI version. Its interpretation depends on the target ABI. "
                        "Linux kernel (after at least 2.6) has no definition of it, so it is ignored for statically-linked executables." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( T, e_ident[EI_ABIVERSION] ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_ident[EI_ABIVERSION] ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "ei_abiver_input", ImGuiDataType_U8, &ehdr->e_ident[EI_ABIVERSION], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
        }

        template< typename T >
        void Type( T* ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_type" );
            Tooltip( "Identifies object file type." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( T, e_type ) );
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
        }

        template< typename T >
        void Machine( T* ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_machine" );
            Tooltip( "Specifies target instruction set architecture." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( T, e_machine ) );
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
        }

        template< typename T >
        void Version( T* ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_version" );
            Tooltip( "Set to 1 for the original version of ELF." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( T, e_version ) );
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
        }

        template< typename T >
        void Entry( T* ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_entry" );
            Tooltip( "This is the memory address of the entry point from where the process starts executing." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( T, e_entry ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_entry ) );
            ImGui::TableNextCell();
            InputHex( "ehdr_e_entry", ehdr->e_entry );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
        }

        template< typename T >
        void Phoff( T* ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_phoff" );
            Tooltip( "Points to the start of the program header table. It usually follows the file header immediately." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( T, e_phoff ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_phoff ) );
            ImGui::TableNextCell();
            InputHex( "ehdr_e_phoff", ehdr->e_phoff );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
        }

        template< typename T >
        void Shoff( T* ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_shoff" );
            Tooltip( "Points to the start of the section header table." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( T, e_shoff ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_shoff ) );
            ImGui::TableNextCell();
            InputHex( "ehdr_e_shoff", ehdr->e_shoff );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
        }

        template< typename T >
        void Flags( T* ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_flags" );
            Tooltip( "Interpretation of this field depends on the target architecture." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( T, e_flags ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_flags ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "e_flags_input", ImGuiDataType_U32, &ehdr->e_flags, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
        }

        template< typename T >
        void Ehsize( T* ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_ehsize" );
            Tooltip( "Contains the size of this header, normally 64 Bytes for 64-bit and 52 Bytes for 32-bit format." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( T, e_ehsize ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_ehsize ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "e_ehsize_input", ImGuiDataType_U16, &ehdr->e_ehsize, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
        }

        template< typename T >
        void Phentsize( T* ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_phentsize" );
            Tooltip( "Contains the size of a program header table entry." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( T, e_phentsize ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_phentsize ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "e_phentsize_input", ImGuiDataType_U16, &ehdr->e_phentsize, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
        }

        template< typename T >
        void Phnum( T* ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_phnum" );
            Tooltip( "Contains the number of entries in the program header table." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( T, e_phnum ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_phnum ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "e_phnum_input", ImGuiDataType_U16, &ehdr->e_phnum, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
        }

        template< typename T >
        void Shentsize( T* ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_shentsize" );
            Tooltip( "Contains the size of a section header table entry." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( T, e_shentsize ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_shentsize ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "e_shentsize_input", ImGuiDataType_U16, &ehdr->e_shentsize, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
        }

        template< typename T >
        void Shnum( T* ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_shnum" );
            Tooltip( "Contains the number of entries in the section header table." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( T, e_shnum ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_shnum ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "e_shnum_input", ImGuiDataType_U16, &ehdr->e_shnum, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
        }

        template< typename T >
        void Shstrndx( T* ehdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "e_shstrndx" );
            Tooltip( "Contains index of the section header table entry that contains the section names." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", offsetof( T, e_shstrndx ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( ehdr->e_shstrndx ) );
            ImGui::TableNextCell();
            ImGui::InputScalar( "e_shstrndx_input", ImGuiDataType_U16, &ehdr->e_shstrndx, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase );
            ImGui::TableNextCell();
            ImGui::TableNextCell();
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
        void Perms( T& perm )
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
        void TableRow( T* phdr )
        {
            ImGui::TableNextRow();
            Imelf::ComboBox( phdr->p_type, PhdrTypeMap );

            ImGui::TableNextCell();
            Imelf::Phdr::Perms( phdr->p_flags );

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

        template< typename O, typename T >
        void Type( O* elf, T* phdr )
        {
            ImGui::TableNextRow();        
            ImGui::Text( "p_type" );
            Tooltip( "Identifies the type of the segment." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", elf->va2rva( &phdr->p_type ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( phdr->p_type ) );
            ImGui::TableNextCell();
            InputHex( "phdr_p_type", phdr->p_type );
            ImGui::TableNextCell();
            ComboBox( phdr->p_type, PhdrTypeMap );
        }

        template< typename O, typename T>
        void Flags( O* elf, T* phdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "p_flags" );
            Tooltip( "Segment-dependent flags." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", elf->va2rva( &phdr->p_flags ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( phdr->p_flags ) );
            ImGui::TableNextCell();
            InputHex( "phdr_p_flags", phdr->p_flags );
            ImGui::TableNextCell();
            ImGui::Text( "%s", PhdrTypeFlags.get_val( phdr->p_flags ) );
        }

        template< typename O, typename T>
        void Offset( O* elf, T* phdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "p_offset" );
            Tooltip( "Offset of the segment in the file image." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", elf->va2rva( &phdr->p_offset ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( phdr->p_offset ) );
            ImGui::TableNextCell();
            InputHex( "phdr_p_offset", phdr->p_offset, phdr->p_offset < elf->get_file_size() );            
        }

        template< typename O, typename T >
        void Vaddr( O* elf, T* phdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "p_vaddr" );
            Tooltip( "Virtual address of the segment in memory." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", elf->va2rva( &phdr->p_vaddr ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( phdr->p_vaddr ) );
            ImGui::TableNextCell();
            InputHex( "phdr_p_vaddr", phdr->p_vaddr );
        }

        template<typename O, typename T>
        void Paddr( O* elf, T* phdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "p_paddr" );
            Tooltip( "On systems where physical address is relevant, reserved for segment's physical address." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", elf->va2rva( &phdr->p_paddr ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( phdr->p_paddr ) );
            ImGui::TableNextCell();
            InputHex( "phdr_p_paddr", phdr->p_paddr );        
        }

        template<typename O, typename T>
        void Filesz( O* elf, T* phdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "p_filesz" );
            Tooltip( "Size in bytes of the segment in the file image." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", elf->va2rva( &phdr->p_filesz ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( phdr->p_filesz ) );
            ImGui::TableNextCell();
            InputHex( "phdr_p_filesz", phdr->p_filesz );
        }

        template< typename O, typename T >
        void Memsz( O* elf, T* phdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "p_memsz" );
            Tooltip( "Size in bytes of the segment in memory." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", elf->va2rva( &phdr->p_memsz ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( phdr->p_memsz ) );
            ImGui::TableNextCell();
            InputHex( "phdr_p_memsz", phdr->p_memsz );
        }

        template< typename O, typename T >
        void Align( O* elf, T* phdr )
        {
            ImGui::TableNextRow();
            ImGui::Text( "p_align" );
            Tooltip( "This member holds the value to which the segments are\n"
                "aligned in memory and in the file.  Loadable process seg-\n"
                "ments must have congruent values for p_vaddr and p_offset,\n"
                "modulo the page size.  Values of zero and one mean no\n"
                "alignment is required.  Otherwise, p_align should be a pos-\n"
                "itive, integral power of two, and p_vaddr should equal\n"
                "p_offset, modulo p_align." );
            ImGui::TableNextCell();
            ImGui::Text( "%08x", elf->va2rva( &phdr->p_align ) );
            ImGui::TableNextCell();
            ImGui::Text( "%02x", sizeof( phdr->p_align ) );
            ImGui::TableNextCell();
            InputHex( "phdr_p_align", phdr->p_align );
        }

        template< typename T >
        void DrawPhdr( T* elf )
        {
            ImGuiTableFlags flags = ImGuiTableFlags_SizingPolicyFixedX | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;
            ImGui::BeginTable( "program header", 6, flags, ImVec2( 0, ImGui::GetTextLineHeightWithSpacing() * 1 ) );
            ImGui::TableSetupColumn( "Member", 0, 220.0 );
            ImGui::TableSetupColumn( "Offset", 0, 100.0 );
            ImGui::TableSetupColumn( "Size", 0, 100.0 );
            ImGui::TableSetupColumn( "Value", 0, 150.0 );
            ImGui::TableSetupColumn( "Meaning", 0, 200.0 );
            ImGui::TableSetupColumn( "", ImGuiTableColumnFlags_WidthStretch );
            ImGui::TableAutoHeaders();

            auto ehdr = elf->get_elf_header();
            for ( int i = 0; i < ehdr->e_phnum; i++ ) {
                auto phdr = elf->get_prog_header( i );
                ImGui::PushID( i );
                if ( i ) {
                    ImGui::TableNextRow();
                    ImGui::Text( "" );
                }

                Type( elf, phdr );
                if ( elf->get_type() == elf64_t )
                    Flags( elf, phdr );
                Offset( elf, phdr );
                Vaddr( elf, phdr );
                Paddr( elf, phdr );
                Filesz( elf, phdr );
                Memsz( elf, phdr );
                if ( elf->get_type() == elf32_t )
                    Flags( elf, phdr );
                Align( elf, phdr );

                ImGui::PopID();
            }
            ImGui::EndTable();        
        }

        template< typename T >
        void DrawPhdrNote( T* elf, size_t start, size_t size )
        {
            ImGui::NewLine();
            ImGuiTableFlags flags = ImGuiTableFlags_SizingPolicyFixedX | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;
            ImGui::BeginTable( "PT_NOTE", 5, flags, ImVec2( 0, ImGui::GetTextLineHeightWithSpacing() * 1 ) );
            ImGui::TableSetupColumn( "namesc", 0, 100.0 );
            ImGui::TableSetupColumn( "descsz", 0, 100.0 );
            ImGui::TableSetupColumn( "type", 0, 100.0 );
            ImGui::TableSetupColumn( "name", 0, 150.0 );
            ImGui::TableSetupColumn( "desc", ImGuiTableColumnFlags_WidthStretch );
            ImGui::TableAutoHeaders();

            size_t offset = 0;
            int id = 0;
            do {
                Elf64_Nhdr* note = reinterpret_cast< Elf64_Nhdr* >( elf->rva2va( start + offset ) );
                char* name = (char*)note + sizeof( *note );
                unsigned char* desc = reinterpret_cast< unsigned char* >( AlignUp( name + note->n_namesz, 4 ) );
                offset += ((uintptr_t)AlignUp(desc + note->n_descsz, 4) - (uintptr_t)note);

                ImGui::PushID( id );
                ImGui::TableNextRow();
                ImGui::Text( "%X", note->n_namesz );
                ImGui::TableNextCell();
                ImGui::Text( "%X", note->n_descsz );
                ImGui::TableNextCell();
                ImGui::Text( "%X", note->n_type );
                ImGui::TableNextCell();
                ImGui::Text( "%s", name );
                ImGui::TableNextCell();

                char buf[256];
                char* cur = buf;
                for ( uint32_t i = 0; i < note->n_descsz; i++ ) {
                    sprintf( cur, "%02X ", desc[i] );
                    cur += 2;
                    if ( cur >= buf + sizeof( buf ) ) {
                        break;
                    }
                }
                ImGui::Text( "%s", buf );
                ImGui::PopID();
                ++id;
            } while ( offset < size );

            ImGui::EndTable();
            ImGui::NewLine();
        }

        template < typename T, typename H >
        void DrawPhdrType( T* elf, H* phdr )
        {
            ImGuiTableFlags flags = ImGuiTableFlags_SizingPolicyFixedX | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;
            ImGui::BeginTable( "program header", 6, flags, ImVec2( 0, ImGui::GetTextLineHeightWithSpacing() * 1 ) );
            ImGui::TableSetupColumn( "Member", 0, 220.0 );
            ImGui::TableSetupColumn( "Offset", 0, 100.0 );
            ImGui::TableSetupColumn( "Size", 0, 100.0 );
            ImGui::TableSetupColumn( "Value", 0, 150.0 );
            ImGui::TableSetupColumn( "Meaning", 0, 200.0 );
            ImGui::TableSetupColumn( "", ImGuiTableColumnFlags_WidthStretch );
            ImGui::TableAutoHeaders();

            Type( elf, phdr );
            if ( elf->get_type() == elf64_t )
                Flags( elf, phdr );
            Offset( elf, phdr );
            Vaddr( elf, phdr );
            Paddr( elf, phdr );
            Filesz( elf, phdr );
            Memsz( elf, phdr );
            if ( elf->get_type() == elf32_t )
                Flags( elf, phdr );
            Align( elf, phdr );

            ImGui::EndTable();

            const char* title = PhdrTypeMap.get_val( phdr->p_type );
            size_t offset = phdr->p_offset < elf->get_file_size() ? phdr->p_offset : 0;
            size_t size = ( offset + phdr->p_filesz < elf->get_file_size() ) ? phdr->p_filesz : ( elf->get_file_size() - offset );

            switch ( phdr->p_type ) {
                case PT_NULL:
                    break;
                case PT_LOAD:
                    break;
                case PT_DYNAMIC:
                    break;
                case PT_INTERP:
                    break;
                case PT_NOTE:
                    DrawPhdrNote( elf, offset, size );
                    break;
                case PT_SHLIB:
                case PT_PHDR:
                case PT_TLS:
                case PT_NUM:
                case PT_LOOS:
                case PT_GNU_EH_FRAME:
                case PT_GNU_STACK:
                case PT_GNU_RELRO:
                case PT_LOSUNW:
                case PT_HISUNW:
                case PT_LOPROC:
                case PT_HIPROC:
                default:
                    break;
            }
            elf->mViewer.DrawChildWindow( title, elf->rva2va( offset ), size, offset );
        }

        template< typename T >
        void Draw( T elf )
        {
            ElfCTX& ctx = elf->get_ctx();
            if ( ctx.idx == 0 ) {
                DrawPhdr( elf );
            } else {
                DrawPhdrType( elf, elf->get_prog_header( ctx.idx - 1) );
            }
        }
    }
    
    namespace Shdr
    {
		template<typename O, typename T>
        void TableRow( O* elf, T* shdr, int idx )
        {
            ImGui::TableNextRow();
            ImGui::Text( "%s", elf->get_section_name( idx ) );

            ImGui::TableNextCell();
            ComboBox( shdr->sh_type, ShdrTypeMap );

			ImGui::TableNextCell();
            char cflags[16];
            sprintf_s( cflags, sizeof( cflags ), "% 8X", (uint32_t)shdr->sh_flags );
            static uint32_t flags;
            if ( ImGui::Button( cflags ) ) {
                flags = (uint32_t)shdr->sh_flags;
                ImGui::OpenPopup( "Section Header Flags" );
            }
            bool close = true;
            if ( ImGui::BeginPopupModal( "Section Header Flags", &close, ImGuiWindowFlags_AlwaysAutoResize ) ) {
                ImGui::CheckboxFlags( "Writable", &flags, SHF_WRITE );
                ImGui::CheckboxFlags( "Occupies memory", &flags, SHF_ALLOC );
                ImGui::CheckboxFlags( "Executable", &flags, SHF_EXECINSTR );
                ImGui::CheckboxFlags( "Merge", &flags, SHF_MERGE );
                ImGui::CheckboxFlags( "Contains nul-terminated strings", &flags, SHF_STRINGS );
                ImGui::CheckboxFlags( "sh_info field holds a section header table index", &flags, SHF_INFO_LINK );
                ImGui::CheckboxFlags( "Preserve order after combining", &flags, SHF_LINK_ORDER );
                ImGui::CheckboxFlags( "Non-standard OS specific handling", &flags, SHF_OS_NONCONFORMING );
                ImGui::CheckboxFlags( "Section is member of a group", &flags, SHF_GROUP );
                ImGui::CheckboxFlags( "Section hold thread-local data", &flags, SHF_TLS );
                ImGui::CheckboxFlags( "Compressed data", &flags, SHF_COMPRESSED );
                ImGui::CheckboxFlags( "OS-specific", &flags, SHF_MASKOS );
                ImGui::CheckboxFlags( "Processor-specific", &flags, SHF_MASKPROC );
                ImGui::CheckboxFlags( "Special ordering requirement", &flags, SHF_ORDERED );
                ImGui::CheckboxFlags( "Excluded", &flags, SHF_EXCLUDE );
                bool save = ImGui::SmallButton( "Save" );
                ImGui::SameLine();
                bool cancel = ImGui::SmallButton( "Cancel" );
                if ( save ) {
                    shdr->sh_flags = flags;
                    ImGui::CloseCurrentPopup();
                } else if ( cancel ) {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

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
        void DrawShr( T* elf )
        {
            ImGuiTableFlags flags = ImGuiTableFlags_SizingPolicyFixedX | ImGuiTableFlags_NoHostExtendY | ImGuiTableFlags_Scroll | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;
            ImGui::BeginTable( "elf sections", 10, flags );
            ImGui::TableSetupColumn( "Name", 0, 170.0 );
            ImGui::TableSetupColumn( "Type", 0, 150.0 );
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
            for ( int i = 0; i < _countof( descs ); i++ ) {
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
        }

        template< typename T, typename H>
        void DrawStrTab( T* elf, H* shdr )
        {
            assert( shdr->sh_type == SHT_STRTAB );

            ImGuiTableFlags flags = ImGuiTableFlags_SizingPolicyFixedX | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;
            ImGui::BeginTable( "SHT_STRTAB", 3, flags, ImVec2( 0, ImGui::GetTextLineHeightWithSpacing() * 15 ) );
            ImGui::TableSetupColumn( "File Offset", 0, 100.0 );
            ImGui::TableSetupColumn( "Index", 0, 100.0 );
            ImGui::TableSetupColumn( "String", ImGuiTableColumnFlags_WidthStretch );
            ImGui::TableAutoHeaders();

            const char* string = reinterpret_cast< char* >( elf->rva2va( shdr->sh_offset ) );
            size_t index = 1;

            while ( index < shdr->sh_size && string[index] ) {

                ImGui::TableNextRow();
                ImGui::Text( "%x", shdr->sh_offset + index );

                ImGui::TableNextCell();
                ImGui::Text( "%x", index );

                ImGui::TableNextCell();
                ImGui::Text( "%s", &string[index] );

                index += strlen( &string[index++] );
            }

            ImGui::EndTable();
        }

        template< typename T, typename H>
        void DrawShrType( T* elf, H* shdr )
        {
            const char* title = elf->get_section_name( elf->get_ctx().idx - 1 );
            size_t offset = shdr->sh_offset;
            size_t size = shdr->sh_size;

            switch ( shdr->sh_type ) {
                case SHT_NULL:
                    break;
                case SHT_PROGBITS:
                    break;
                case SHT_SYMTAB:
                    break;
                case SHT_STRTAB:
                    return DrawStrTab( elf, shdr );
                case SHT_RELA:
                    break;
                case SHT_HASH:
                    break;
                case SHT_DYNAMIC:
                    return DrawDynamic( elf->get_dyn() );
                case SHT_NOTE:
                    break;
                case SHT_NOBITS:
                    return;
                case SHT_REL:
                case SHT_SHLIB:
                case SHT_DYNSYM:
                case SHT_INIT_ARRAY:
                case SHT_FINI_ARRAY:
                case SHT_PREINIT_ARRAY:
                case SHT_GROUP:
                case SHT_SYMTAB_SHNDX:
                case SHT_NUM:
                case SHT_LOOS:
                case SHT_GNU_ATTRIBUTES:
                case SHT_GNU_HASH:
                case SHT_GNU_LIBLIST:
                case SHT_CHECKSUM:
                case SHT_SUNW_move:
                case SHT_SUNW_COMDAT:
                case SHT_SUNW_syminfo:
                case SHT_GNU_verdef:
                case SHT_GNU_verneed:
                case SHT_GNU_versym:
                case SHT_LOPROC:
                case SHT_HIPROC:
                case SHT_LOUSER:
                case SHT_HIUSER:
                    break;
            }
            elf->mViewer.DrawChildWindow( title, elf->rva2va( offset ), size, offset );
        }

        template< typename T >
        void Draw( T elf )
        {
            ElfCTX& ctx = elf->get_ctx();
            if ( ctx.idx == 0 ) {
                DrawShr( elf );
            } else {
                DrawShrType( elf, elf->get_section_header( ctx.idx - 1 ) );
            }

		}
    }
}


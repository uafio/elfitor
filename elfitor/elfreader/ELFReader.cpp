#include "imgui.h"
#include <fstream>
#include <errno.h>
#include <filesystem>
#include "../imelf.h"
#include "ELFReader.h"

State* State::state;

template< typename T >
int list_prog_headers( T* elf )
{
    auto ehdr = elf->get_elf_header();
    auto ctx = elf->get_ctx();
    int result = 0;
    for ( int i = 1; i <= ehdr->e_phnum; i++ ) {
        auto phdr = elf->get_prog_header( i - 1 );
        const char* p_type = PhdrTypeMap.get_val( phdr->p_type );
        if ( ctx.idx == i ) {
            ImGui::TreeNodeEx( p_type, ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Selected, p_type );
        } else {
            ImGui::TreeNodeEx( p_type, ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf, p_type );
        }
        if ( ImGui::IsItemClicked() ) {
            result = i;
        }
    }
    return result;
}

template< typename T >
int list_section_headers( T* elf )
{
    auto ehdr = elf->get_elf_header();
    auto ctx = elf->get_ctx();
    int result = 0;
    for ( int i = 1; i <= ehdr->e_shnum; i++ ) {
        const char* s_name = elf->get_section_name( i - 1 );
        if ( ctx.idx == i ) {
            ImGui::TreeNodeEx( s_name, ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Selected, s_name );
        } else {
            ImGui::TreeNodeEx( s_name, ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf, s_name );
        }
        if ( ImGui::IsItemClicked() ) {
            result = i;
        }
    }
    return result;
}


// ==========================================================================================================

State* State::instance( void )
{
    if ( state == nullptr ) {
        state = new State();
    }
    return state;
}

State::State( void )
{
    files.reserve( 20 );
    ctx_file = nullptr;
}

File* State::get_ctx_file( void )
{
    return ctx_file;
}

void State::add( File* newfile )
{
    files.push_back( newfile );
    ctx_file = newfile;
}

void State::close( void )
{
    if ( ctx_file ) {
        remove( ctx_file );
        ctx_file = nullptr;
    }
}

void State::remove( File* file )
{
    for ( int i = 0; i < files.size(); i++ ) {
        if ( files[i] == file ) {
            if ( files[i] == ctx_file ) {
                ctx_file = nullptr;
            }
            delete files[i];
            files.erase( files.begin() + i );
            break;
        }
    }
}

void State::draw( void )
{
	// Draw the Main Window
    int index = -1;
    ImGui::Begin( "MainWindow" );

    if ( !files.empty() ) {
        ImGuiTabBarFlags tflags = ImGuiTabBarFlags_FittingPolicyResizeDown;
        tflags |= ImGuiTabBarFlags_NoTooltip;
        if ( ImGui::BeginTabBar( "FileTabs", tflags ) ) {
            for ( int i = 0; i < files.size(); i++ ) {
                auto cur = files[i];
                ImGuiTabItemFlags iflags = ImGuiTabItemFlags_NoCloseWithMiddleMouseButton;
                bool closed = true;
                ImGui::PushID( i );

                bool tab = ImGui::BeginTabItem( cur->filename(), &closed, iflags );
                Tooltip( cur->filepath() );
                if ( tab ) {
                    ctx_file = cur;
                    cur->show_main();
                    ImGui::EndTabItem();
                }

                ImGui::PopID();

                if ( !closed )
                    index = i;
            }
            ImGui::EndTabBar();
        }
    }

    ImGui::End();


	// Draw other optional windows
	if ( ctx_file )
        ctx_file->other_windows();


	// Draw navigational left panel
    ImGui::Begin( "Navigation" );
    if ( ctx_file ) {
        ctx_file->show_nav();
    }
    ImGui::End();
    

	// The end
    if ( index >= 0 )
        remove( files[index] );

}

// ==========================================================================================================

void* File::get_base( void )
{
    return base;
}

File::File( void )
{
    size = 0;
    base = nullptr;
    fname = nullptr;
    fpath = nullptr;
}

size_t File::get_file_size( void )
{
    return size;
}

const char* File::filename( void )
{
    return const_cast< const char* >( fname );
}

const char* File::filepath( void )
{
    return const_cast< const char* >( fpath );
}

void* File::rva2va( size_t rva )
{
    return (char*)base + rva;
}

uint64_t File::va2rva( void* va )
{
    return (char*)va - (char*)base;
}

bool File::save( const char* filename )
{
    std::fstream out( fname, std::fstream::binary | std::fstream::out );
    if ( out.is_open() ) {
        out.write( (char*)base, size );
        return true;
    }
    std::perror( __FUNCTION__ );
    return false;
}

File::~File( void )
{
    if ( base ) {
        free( base );
        base = nullptr;
    }

    if ( fpath ) {
        free( fpath );
        fpath = nullptr;
    }
}

// ==========================================================================================================

FileFactory::FileFactory( const char* filename )
{
    base = nullptr;
    fpath = nullptr;
    fname = nullptr;
    size = 0;

    std::fstream infile( filename, std::fstream::in | std::fstream::binary | std::fstream::ate );
    if ( infile.is_open() == false ) {
        std::perror( __FUNCTION__ );
        return;
    }

    // TODO: Create logic for headers-only classes, optional for huge files
    // Best to pass a builder class to Factory
    size = infile.tellg();
    if ( size == 0 ) {
        std::cout << "[-] Empty file." << std::endl;
        return;
    }

    base = malloc( size );
    if ( base == nullptr ) {
        std::perror( __FUNCTION__ );
        return;
    }

    fpath = (char*)malloc( strlen( filename ) + 1 );
    if ( fpath == nullptr ) {
        free( base );
        std::perror( __FUNCTION__ );
        return;
    }

    strcpy( (char*)fpath, filename );
    fname = strrchr( fpath, (char)std::experimental::filesystem::path::preferred_separator );
    if ( fname )
        fname++;

    infile.seekg( std::fstream::beg );
    infile.read( (char*)base, size );
    infile.close();

    type = get_type();
    if ( !is_supported() ) {
        free( base );
        free( (void*)fpath );
        base = nullptr, size = 0;
        std::cout << "[-] Unsupported File." << std::endl;
        return;
    }
}

File_t FileFactory::get_type( void )
{
    if ( base ) {
        if ( *(uint32_t*)base == *(uint32_t*)ELFMAG ) {
            Elf32_Ehdr* ehdr = reinterpret_cast< Elf32_Ehdr* >( base );
            if ( ehdr->e_ident[EI_CLASS] == ELFCLASS32 ) {
                return elf32_t;
            } else if ( ehdr->e_ident[EI_CLASS] == ELFCLASS64 ) {
                return elf64_t;
            }
        }
        // TODO: other supported classes
    }
    return unknown_t;
}

bool FileFactory::is_supported( void )
{
    return get_type() != unknown_t;
}

void* FileFactory::get_base( void )
{
    return base;
}

size_t FileFactory::get_file_size( void )
{
    return size;
}

char* FileFactory::filename( void )
{
    return fname;
}

char* FileFactory::filepath( void )
{
    return fpath;
}

File* FileFactory::build( void )
{
    switch ( type ) {
        case elf32_t:
            return new Elf32( this );
            break;
        case elf64_t:
            return new Elf64( this );
            break;
        default:
            return nullptr;
    }
}

// ==========================================================================================================

Elf32::Elf32( FileFactory* factory )
{
    base = factory->get_base();
    size = factory->get_file_size();
    fpath = factory->filepath();
    fname = factory->filename();
    ctx.hdr = ctx.ehdr;
    ctx.idx = 0;
}

File_t Elf32::get_type( void )
{
    return elf32_t;
}

File_t Elf32::get_type_group( void )
{
    return elf_t;
}

Elf32_Ehdr* Elf32::get_elf_header( void )
{
    return reinterpret_cast< Elf32_Ehdr* >( base );
}

Elf32_Phdr* Elf32::get_prog_header( int index )
{
    auto ehdr = get_elf_header();
    if ( index < ehdr->e_phnum ) {
        auto phdr = reinterpret_cast< Elf32_Phdr* >( (char*)base + ehdr->e_phoff );
        return &phdr[index];
    }
    return nullptr;
}

Elf32_Shdr* Elf32::get_section_header( int index )
{
    auto ehdr = get_elf_header();
    if ( index < ehdr->e_shnum ) {
        Elf32_Shdr* shdr = reinterpret_cast< Elf32_Shdr* >( (char*)base + ehdr->e_shoff );
        return shdr + index;
    }
    return nullptr;
}

char* Elf32::get_section_name( int index )
{
    char* result = nullptr;
    auto ehdr = get_elf_header();
    auto shdr = get_section_header( index );
    auto shstrtab = get_section_header( ehdr->e_shstrndx );
    char* strtab = (char*)base + shstrtab->sh_offset;
    if ( shdr && shstrtab ) {
        if ( shdr->sh_name == 0 ) {
            result = (char*)shstrtab;
        } else {
            result = (char*)strtab + shdr->sh_name;
        }
    }
    return result;
}

void Elf32::show_nav( void )
{
    const char* leafs[] = { 
        "ELF Header",
        "Program Headers",
        "Section Headers" 
    };

    for ( int i = 0; i < _countof( leafs ); i++ ) {
        if ( i == ctx.hdr ) {
            ImGui::TreeNodeEx( leafs[i], ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Selected, (char*)leafs[i] );
        } else {
            ImGui::TreeNodeEx( leafs[i], ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf, (char*)leafs[i] );
        }
        if ( ImGui::IsItemClicked() ) {
            ctx.hdr = static_cast<ElfCTX::_hdr>( i );
            ctx.idx = 0;
        }
        if ( i == ctx.phdr ) {
            ImGui::Indent();
            int idx = list_prog_headers( this );
            if ( idx ) {
                ctx.idx = idx;
                ctx.hdr = ctx.phdr;
            }
            ImGui::Unindent();
        }
    }

}

ElfCTX& Elf32::get_ctx( void )
{
    return ctx;
}

void Elf32::show_main( void )
{
    switch ( ctx.hdr ) {
        case ElfCTX::ehdr:
            Imelf::Ehdr::Draw( get_elf_header() );
            break;
        case ElfCTX::phdr:
            Imelf::Phdr::Draw( this );
            break;
        case ElfCTX::shdr:
            Imelf::Shdr::Draw( this );
            break;
        default:
            assert( false );
    }
}

Elf32_Dyn* Elf32::get_dyn( void )
{
    for ( int i = 0; i < get_elf_header()->e_phnum; i++ ) {
        auto phdr = get_prog_header( i );
        if ( phdr->p_type == PT_DYNAMIC ) {
            return (Elf32_Dyn*)rva2va( phdr->p_offset );
        }
    }
    return nullptr;
}


Elf32_Rela* Elf32::get_rela( Elf32_Shdr* shdr, int index )
{
    assert( shdr->sh_type == SHT_RELA );
    return reinterpret_cast< Elf32_Rela* >( (char*)base + shdr->sh_offset ) + index;
}


char* Elf32::get_section_name( Elf32_Shdr* section )
{
    char* result = nullptr;
    auto ehdr = get_elf_header();
    auto shstrtab = get_section_header( ehdr->e_shstrndx );
    char* strtab = (char*)base + shstrtab->sh_offset;
    if ( section && shstrtab ) {
        if ( section->sh_name == 0 ) {
            result = (char*)shstrtab;
        } else {
            result = (char*)strtab + section->sh_name;
        }
    }
    return result;
}


Elf32_Shdr* Elf32::va2section( size_t va )
{
    for ( int i = 0; i < get_elf_header()->e_shnum; i++ ) {
        Elf32_Shdr* shdr = get_section_header( i );
        if ( va >= shdr->sh_addr && va < shdr->sh_addr + shdr->sh_size ) {
            return shdr;
        }
    }

    return nullptr;
}


Elf32_Rel* Elf32::get_rel( Elf32_Shdr* shdr, int index )
{
    assert( shdr->sh_type == SHT_RELA );
    return reinterpret_cast< Elf32_Rel* >( (char*)base + shdr->sh_offset ) + index;
}


Elf32_Sym* Elf32::get_sym( Elf32_Shdr* shdr, int index )
{
    assert( shdr->sh_type == SHT_SYMTAB );
    return reinterpret_cast< Elf32_Sym* >( (char*)base + shdr->sh_offset ) + index;
}

char* Elf32::get_strtab( void )
{
    for ( int i = 0; i < get_elf_header()->e_shnum; i++ ) {
        auto section = get_section_header( i );
        if ( section->sh_type == SHT_STRTAB && !strcmp( get_section_name( i ), ".strtab" ) ) {
            return (char*)base + section->sh_offset;
        }
    }
    return nullptr;
}

// ==========================================================================================================


Elf64::Elf64( FileFactory* factory )
{
    base = factory->get_base();
    size = factory->get_file_size();
    fpath = factory->filepath();
    fname = factory->filename();
    ctx.hdr = ctx.ehdr;
    ctx.idx = 0;
}

File_t Elf64::get_type( void )
{
    return elf64_t;
}

File_t Elf64::get_type_group( void )
{
    return elf_t;
}

Elf64_Ehdr* Elf64::get_elf_header( void )
{
    return reinterpret_cast< Elf64_Ehdr* >( base );
}

Elf64_Phdr* Elf64::get_prog_header( int index )
{
    auto ehdr = get_elf_header();
    if ( index < ehdr->e_phnum ) {
        auto phdr = reinterpret_cast< Elf64_Phdr* >( (char*)base + ehdr->e_phoff );
        return &phdr[index];
    }
    return nullptr;
}

Elf64_Shdr* Elf64::get_section_header( int index )
{
    auto ehdr = get_elf_header();
    if ( index < ehdr->e_shnum ) {
        Elf64_Shdr* shdr = reinterpret_cast< Elf64_Shdr* >( (char*)base + ehdr->e_shoff );
        return shdr + index;
    }
    return nullptr;
}

char* Elf64::get_section_name( int index )
{
    char* result = nullptr;
    auto ehdr = get_elf_header();
    auto shdr = get_section_header( index );
    auto shstrtab = get_section_header( ehdr->e_shstrndx );
    char* strtab = (char*)base + shstrtab->sh_offset;
    if ( shdr && shstrtab ) {
        if ( shdr->sh_name == 0 ) {
            result = (char*)shstrtab;
        } else {
            result = (char*)strtab + shdr->sh_name;
        }
    }
    return result;
}

void Elf64::show_nav( void )
{
    const char* leafs[] = { 
        "ELF Header",
        "Program Headers",
        "Section Headers" 
    };

    for ( int i = 0; i < _countof( leafs ); i++ ) {
        if ( ctx.hdr == i ) {
            ImGui::TreeNodeEx( leafs[i], ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Selected, (char*)leafs[i] );
        } else {
            ImGui::TreeNodeEx( leafs[i], ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf, (char*)leafs[i] );
        }
        if ( ImGui::IsItemClicked() ) {
            ctx.hdr = static_cast< ElfCTX::_hdr >( i );
            ctx.idx = 0;
        }
        if ( i == ctx.hdr && ctx.hdr == ctx.phdr ) {
            ImGui::Indent();
            int idx = list_prog_headers( this );
            if ( idx ) {
                ctx.idx = idx;
            }
            ImGui::Unindent();
        }
        if ( i == ctx.hdr && ctx.hdr == ctx.shdr ) {
            ImGui::Indent();
            int idx = list_section_headers( this );
            if ( idx ) {
                ctx.idx = idx;
            }
            ImGui::Unindent();        
        }
    }
}

void Elf64::show_main( void )
{
    switch ( ctx.hdr ) {
        case ElfCTX::ehdr:
            Imelf::Ehdr::Draw( get_elf_header() );
            break;
        case ElfCTX::phdr:
            Imelf::Phdr::Draw( this );
            break;
        case ElfCTX::shdr:
            Imelf::Shdr::Draw( this );
            break;
        default:
            assert( false );
    }
}

ElfCTX& Elf64::get_ctx( void )
{
    return ctx;
}

Elf64_Dyn* Elf64::get_dyn( void )
{
    for ( int i = 0; i < this->get_elf_header()->e_phnum; i++ ) {
        auto phdr = get_prog_header( i );
        if ( phdr->p_type == PT_DYNAMIC ) {
            return (Elf64_Dyn*)rva2va( phdr->p_offset );
        }
    }
    return nullptr;
}


Elf64_Rela* Elf64::get_rela( Elf64_Shdr* shdr, int index )
{
    assert( shdr->sh_type == SHT_RELA );
    return reinterpret_cast< Elf64_Rela* >( (char*)base + shdr->sh_offset ) + index;
}


char* Elf64::get_section_name( Elf64_Shdr* section )
{
    char* result = nullptr;
    auto ehdr = get_elf_header();
    auto shstrtab = get_section_header( ehdr->e_shstrndx );
    char* strtab = (char*)base + shstrtab->sh_offset;
    if ( section && shstrtab ) {
        if ( section->sh_name == 0 ) {
            result = (char*)shstrtab;
        } else {
            result = (char*)strtab + section->sh_name;
        }
    }
    return result;
}


Elf64_Shdr* Elf64::va2section( size_t va )
{
    for ( int i = 0; i < get_elf_header()->e_shnum; i++ ) {
        Elf64_Shdr* shdr = get_section_header( i );
        if ( va >= shdr->sh_addr && va < shdr->sh_addr + shdr->sh_size ) {
            return shdr;
        }
    }

    return nullptr;
}


Elf64_Rel* Elf64::get_rel( Elf64_Shdr* shdr, int index )
{
    assert( shdr->sh_type == SHT_RELA );
    return reinterpret_cast< Elf64_Rel* >( (char*)base + shdr->sh_offset ) + index;
}


Elf64_Sym* Elf64::get_sym( Elf64_Shdr* shdr, int index )
{
    assert( shdr->sh_type == SHT_SYMTAB );
    return reinterpret_cast< Elf64_Sym* >( (char*)base + shdr->sh_offset ) + index;
}


char* Elf64::get_strtab( void )
{
    for ( int i = 0; i < get_elf_header()->e_shnum; i++ ) {
        auto section = get_section_header( i );
        if ( section->sh_type == SHT_STRTAB && !strcmp( get_section_name( i ), ".strtab" ) ) {
            return (char*)base + section->sh_offset;
        }
    }
    return nullptr;
}

// ==========================================================================================================

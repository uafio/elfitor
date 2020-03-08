#include "imgui.h"
#include <fstream>
#include <errno.h>
#include <filesystem>
<<<<<<< Updated upstream
#include "elf.h"
#include "ELFReader.h"

extern std::vector< File* > files;


File::File( void )
{
    base = nullptr;
    path = nullptr;
    fname = nullptr;
    size = 0;
}

File::File( const char* filename )
{
    base = nullptr;
    size = 0;

    std::fstream file( filename, std::fstream::in | std::fstream::binary | std::fstream::ate );
    if ( file.is_open() == false ) {
        std::perror( __FUNCTION__ );
        return;
    }

    size = file.tellg();
    if ( size == 0 ) {
        std::cout << "[-] Empty file." << std::endl;
        return;
    }
=======
#include "../imelf.h"
#include "ELFReader.h"

State* State::state;

// ==========================================================================================================
>>>>>>> Stashed changes

State* State::instance( void )
{
    if ( state == nullptr ) {
        state = new State();
    }
    return state;
}

<<<<<<< Updated upstream
    strcpy( const_cast< char* >( path ), filename );
    fname = strrchr( path, (char)std::experimental::filesystem::path::preferred_separator );
    if ( fname ) {
        fname++;
    }

    file.seekg( std::fstream::beg );
    file.read( (char*)base, size );
    file.close();

    if ( !is_valid() ) {
        free( base );
        free( (void*)path );
        base = nullptr, size = 0;
        std::cout << "[-] Invalid ELF file." << std::endl;
        return;
    }
}

File::~File( void )
{
    if ( base ) {
        free( base );
=======
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
    update( newfile );
}

void State::update( File* newfile )
{
    ctx_file = newfile;
}

void State::close( void )
{
    if ( ctx_file ) {
        remove( ctx_file );
        ctx_file = nullptr;
>>>>>>> Stashed changes
    }
    if ( path ) {
        free( (void*)path );
    }
    base = nullptr;
    path = nullptr;
}

<<<<<<< Updated upstream
size_t File::get_file_size( void )
=======
void State::remove( File* file )
>>>>>>> Stashed changes
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

<<<<<<< Updated upstream
bool File::is_valid( void )
{
    if ( base )
        return *(int*)base == *ELFMAG;
    return false;
}

bool File::save( const char* fname )
=======
void State::ShowNavigation( void )
{
    ImGuiWindowFlags flags = 0;
    flags |= ImGuiWindowFlags_NoTitleBar;
    flags |= ImGuiWindowFlags_NoMove;
    flags |= ImGuiWindowFlags_NoResize;
    flags |= ImGuiWindowFlags_NoCollapse;
    flags |= ImGuiWindowFlags_NoNav;

    int delete_idx = -1;

    ImGui::Begin( "Navigation", NULL, flags );
    for ( int i = 0; i < files.size(); i++ ) {
        char label[_MAX_PATH];
        snprintf( label, sizeof( label ), "%s##%d", files[i]->filename(), i );

        bool close = true;
        if ( ImGui::CollapsingHeader( label, &close, ImGuiTreeNodeFlags_DefaultOpen ) ) {
            ctx_file->show_nav( this );
        }
        if ( close == false ) {
            delete_idx = i;
            break;
        }
    }
    ImGui::End();

    if ( delete_idx >= 0 ) {
        remove( files[delete_idx] );
    }
}

void State::ShowMainWindow( void )
{
    if ( ctx_file ) {
        return ctx_file->show_main();
    }

    ImGuiWindowFlags wflags = 0;
    wflags |= ImGuiWindowFlags_NoMove;
    wflags |= ImGuiWindowFlags_NoResize;
    wflags |= ImGuiWindowFlags_NoCollapse;

    ImGui::Begin( "MainWindow", NULL, wflags );
    ImGui::Text( "ELF Editor" );
    ImGui::End();
}

// ==========================================================================================================

void* File::rva2va( uint64_t rva )
{
    return (char*)base + rva;
}

void* File::get_base( void )
>>>>>>> Stashed changes
{
    return base;
}

<<<<<<< Updated upstream
const char* File::filename( void )
=======
File::File( void )
>>>>>>> Stashed changes
{
    size = 0;
    base = nullptr;
    fname = nullptr;
    fpath = nullptr;
}

<<<<<<< Updated upstream
const char* File::filepath( void )
=======
size_t File::get_file_size( void )
>>>>>>> Stashed changes
{
    return size;
}

<<<<<<< Updated upstream
void* File::rva2va( uint64_t rva )
{
    return (char*)base + rva;
}

void* File::get_base( void )
{
    return base;
}

file_t File::get_type( void )
{
    if ( *(int*)base == *ELFMAG ) {
        if ( reinterpret_cast< Elf32_Ehdr* >( base )->e_ident[EI_CLASS] == ELFCLASS32 ) {
            return elf32;
        } else {
            return elf64;
        }
    }
    return unknown;
}




Elf32::Elf32( File& file )
{
    size = file.get_file_size();
    base = malloc( size );
    memcpy( base, file.get_base(), size );
    path = (const char*)malloc( strlen( file.filepath() ) + 1 );
    strcpy( (char*)path, file.filepath() );
    fname = strrchr( path, (char)std::experimental::filesystem::path::preferred_separator );
    if ( fname ) {
        fname++;
    }
}

Elf32_Ehdr* Elf32::get_elf_header( void )
{
    return reinterpret_cast< Elf32_Ehdr* >( base );
}

bool Elf32::is_32bit( void )
=======
const char* File::filename( void )
{
    return const_cast< const char* >( fname );
}

const char* File::filepath( void )
{
    return const_cast< const char* >( fpath );
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
>>>>>>> Stashed changes
{
    return reinterpret_cast< Elf32_Ehdr* >( base )->e_ident[EI_CLASS] == ELFCLASS32;
}

Elf32_Phdr* Elf32::get_prog_header( int index )
{
    Elf32_Phdr* result = nullptr;
    Elf32_Ehdr* ehdr = get_elf_header();
    if ( index < ehdr->e_phnum ) {
        Elf32_Phdr* phdr = reinterpret_cast< Elf32_Phdr* >( (char*)base + ehdr->e_phoff );
        result = &phdr[index];
    }
    return result;
}

<<<<<<< Updated upstream
Elf32_Shdr* Elf32::get_section_header( int index )
{
    Elf32_Shdr* result = nullptr;
    Elf32_Ehdr* ehdr = get_elf_header();
    if ( index < ehdr->e_shnum ) {
        Elf32_Shdr* shdr = reinterpret_cast< Elf32_Shdr* >( (char*)base + ehdr->e_shoff );
        result = shdr + index;
=======
    // TODO: Create logic for headers-only classes, optional for huge files
    // Best to pass a builder class to Factory
    size = infile.tellg();
    if ( size == 0 ) {
        std::cout << "[-] Empty file." << std::endl;
        return;
>>>>>>> Stashed changes
    }
    return result;
}

<<<<<<< Updated upstream
char* Elf32::get_section_name( int index )
{
    char* result = nullptr;
    Elf32_Ehdr* ehdr = get_elf_header();
    Elf32_Shdr* shdr = get_section_header( index );
    Elf32_Shdr* shstrtab = get_section_header( ehdr->e_shstrndx );
    char* strtab = (char*)rva2va( shstrtab->sh_offset );
    if ( shdr && shstrtab ) {
        if ( shdr->sh_name == 0 ) {
            result = (char*)shstrtab;
        } else {
            result = (char*)strtab + shdr->sh_name;
        }
=======
    base = malloc( size );
    if ( base == nullptr ) {
        std::perror( __FUNCTION__ );
        return;
>>>>>>> Stashed changes
    }
    return result;
}

file_t Elf32::get_type( void )
{
    return elf32;
}



<<<<<<< Updated upstream

=======
    type = get_type();
    if ( !is_supported() ) {
        free( base );
        free( (void*)fpath );
        base = nullptr, size = 0;
        std::cout << "[-] Unsupported File." << std::endl;
        return;
    }
}
>>>>>>> Stashed changes

file_t Elf64::get_type( void )
{
    return elf64;
}

Elf64_Ehdr* Elf64::get_elf_header( void )
{
    return reinterpret_cast< Elf64_Ehdr* >( base );
}

bool Elf64::is_32bit( void )
{
    return reinterpret_cast< Elf64_Ehdr* >( base )->e_ident[EI_CLASS] == 1;
}

Elf64_Phdr* Elf64::get_prog_header( int index )
{
    Elf64_Phdr* result = nullptr;
    Elf64_Ehdr* ehdr = get_elf_header();
    if ( index < ehdr->e_phnum ) {
        Elf64_Phdr* phdr = reinterpret_cast< Elf64_Phdr* >( (char*)base + ehdr->e_phoff );
        result = &phdr[index];
    }
    return result;
}

Elf64_Shdr* Elf64::get_section_header( int index )
{
    Elf64_Shdr* result = nullptr;
    Elf64_Ehdr* ehdr = get_elf_header();
    if ( index < ehdr->e_shnum ) {
        Elf64_Shdr* shdr = reinterpret_cast< Elf64_Shdr* >( (char*)base + ehdr->e_shoff );
        result = shdr + index;
    }

    return result;
}

<<<<<<< Updated upstream
char* Elf64::get_section_name( int index )
{
    char* result = nullptr;
    Elf64_Ehdr* ehdr = get_elf_header();
    Elf64_Shdr* shdr = get_section_header( index );
    Elf64_Shdr* shstrtab = get_section_header( ehdr->e_shstrndx );
    char* strtab = (char*)rva2va( shstrtab->sh_offset );
    if ( shdr && shstrtab ) {
        if ( shdr->sh_name == 0 ) {
            result = (char*)shstrtab;
        } else {
            result = (char*)strtab + shdr->sh_name;
        }
    }
    return result;
}
=======
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

void Elf32::show_nav( State* state )
{
    ImGuiTreeNodeFlags flags = 0;
    flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
    flags |= ImGuiTreeNodeFlags_Leaf;

    /*
    const char* leafs[] = { 
        "ELF Header",
        "Program Headers",
        "Section Headers" 
    };

    if ( state->get_ctx_file() == this ) {
        for ( int i = 0; i < _countof( leafs ); i++ ) {
            if ( i == ctx.hdr ) {
                ImGui::TreeNodeEx( leafs, flags | ImGuiTreeNodeFlags_Selected, (char*)leafs );                
            } else {
                ImGui::TreeNodeEx( leafs, flags, (char*)leafs );
            }
        }
    } else {
        for ( int i = 0; i < _countof( leafs ); i++ ) {
            ImGui::TreeNodeEx( leafs, flags, (char*)leafs );
        }
    }
    */

    auto ctx_elf = state->get_ctx_file();

    if ( ctx_elf == this && ctx.hdr == ctx.ehdr ) {
        ImGui::TreeNodeEx( "ELF Header Leaf", flags | ImGuiTreeNodeFlags_Selected, "ELF Header" );
    } else {
        ImGui::TreeNodeEx( "ELF Header Leaf", flags, "ELF Header" );
    }
    if ( ImGui::IsItemClicked() ) {
        state->update( this );
        ctx.hdr = ctx.ehdr;
    }

    if ( ctx_elf == this && ctx.hdr == ctx.phdr ) {
        ImGui::TreeNodeEx( get_prog_header( 0 ), flags | ImGuiTreeNodeFlags_Selected, "Program Headers" );
    } else {
        ImGui::TreeNodeEx( get_prog_header( 0 ), flags, "Program Headers" );
    }
    if ( ImGui::IsItemClicked() ) {
        state->update( this );
        ctx.hdr = ctx.phdr;
    }

    if ( ctx_elf == this && ctx.hdr == ctx.shdr ) {
        ImGui::TreeNodeEx( get_section_header( 0 ), flags | ImGuiTreeNodeFlags_Selected, "Section Headers" );
    } else {
        ImGui::TreeNodeEx( get_section_header( 0 ), flags, "Section Headers" );
    }
    if ( ImGui::IsItemClicked() ) {
        state->update( this );
        ctx.hdr = ctx.shdr;
    }
}

void Elf32::show_main( void )
{
    ImGuiWindowFlags wflags = 0;
    wflags |= ImGuiWindowFlags_NoMove;
    wflags |= ImGuiWindowFlags_NoResize;
    wflags |= ImGuiWindowFlags_NoCollapse;

    ImGui::Begin( "MainWindow", NULL, wflags );

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
            ImGui::Text( "MainWindow %s", fpath );
    }

    ImGui::End();
}

ElfCTX& Elf32::get_ctx( void )
{
    return ctx;
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

void Elf64::show_nav( State* state )
{
    ImGuiTreeNodeFlags flags = 0;
    flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
    flags |= ImGuiTreeNodeFlags_Leaf;

    /*
    const char* leafs[] = { 
        "ELF Header",
        "Program Headers",
        "Section Headers" 
    };

    if ( state->get_ctx_file() == this ) {
        for ( int i = 0; i < _countof( leafs ); i++ ) {
            if ( i == ctx.hdr ) {
                ImGui::TreeNodeEx( leafs, flags | ImGuiTreeNodeFlags_Selected, (char*)leafs );                
            } else {
                ImGui::TreeNodeEx( leafs, flags, (char*)leafs );
            }
        }
    } else {
        for ( int i = 0; i < _countof( leafs ); i++ ) {
            ImGui::TreeNodeEx( leafs, flags, (char*)leafs );
        }
    }
    */

    auto ctx_elf = state->get_ctx_file();

    if ( ctx_elf == this && ctx.hdr == ctx.ehdr ) {
        ImGui::TreeNodeEx( "ELF Header Leaf", flags | ImGuiTreeNodeFlags_Selected, "ELF Header" );
    } else {
        ImGui::TreeNodeEx( "ELF Header Leaf", flags, "ELF Header" );
    }
    if ( ImGui::IsItemClicked() ) {
        state->update( this );
        ctx.hdr = ctx.ehdr;
    }

    if ( ctx_elf == this && ctx.hdr == ctx.phdr ) {
        ImGui::TreeNodeEx( get_prog_header( 0 ), flags | ImGuiTreeNodeFlags_Selected, "Program Headers" );
    } else {
        ImGui::TreeNodeEx( get_prog_header( 0 ), flags, "Program Headers" );
    }
    if ( ImGui::IsItemClicked() ) {
        state->update( this );
        ctx.hdr = ctx.phdr;
    }

    if ( ctx_elf == this && ctx.hdr == ctx.shdr ) {
        ImGui::TreeNodeEx( get_section_header( 0 ), flags | ImGuiTreeNodeFlags_Selected, "Section Headers" );
    } else {
        ImGui::TreeNodeEx( get_section_header( 0 ), flags, "Section Headers" );
    }
    if ( ImGui::IsItemClicked() ) {
        state->update( this );
        ctx.hdr = ctx.shdr;
    }
}

void Elf64::show_main( void )
{
    ImGuiWindowFlags wflags = 0;
    wflags |= ImGuiWindowFlags_NoMove;
    wflags |= ImGuiWindowFlags_NoResize;
    wflags |= ImGuiWindowFlags_NoCollapse;

    ImGui::Begin( "MainWindow", NULL, wflags );

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
            ImGui::Text( "MainWindow %s", fpath );
    }

    ImGui::End();
}

ElfCTX& Elf64::get_ctx( void )
{
    return ctx;
}

// ==========================================================================================================

Elf::Elf( FileFactory* factory )
{
    base = factory->get_base();
    size = factory->get_file_size();
    fpath = factory->filepath();
    fname = factory->filename();
    ctx.hdr = ctx.ehdr;
    ctx.idx = 0;
}

ElfCTX& Elf::get_ctx( void )
{
    return ctx;
}

File_t Elf::get_type( void )
{
    return reinterpret_cast< Elf32_Ehdr* >( base )->e_ident[EI_CLASS] == ELFCLASS32 ? elf32_t : elf64_t;
}

File_t Elf::get_type_group( void )
{
    return elf_t;
}

template<typename T>
T Elf::get_elf_header( void )
{
    return reinterpret_cast< T >( base );
}
>>>>>>> Stashed changes

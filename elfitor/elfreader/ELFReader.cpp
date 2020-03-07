#include <fstream>
#include <errno.h>
#include <filesystem>
#include "ELFReader.h"

extern std::vector< ELFReader* > elfs;

/*
ELFReader::ELFReader( const char* filename )
{
    base = nullptr;
    std::fstream elf( filename, std::fstream::in | std::fstream::binary | std::fstream::ate );
    if ( elf.is_open() == false ) {
        std::perror( __FUNCTION__ );
        return;
    }

    size = elf.tellg();
    if ( size == 0 ) {
        std::cout << "[-] Empty file." << std::endl;
        return;
    }

    base = malloc( size );
    path = (char*)malloc( strlen( filename ) + 1 );
    if ( base == nullptr || path == nullptr ) {
        std::perror( __FUNCTION__ );
        return;
    }

    strcpy( path, filename );
    fname = strrchr( path, (char)std::experimental::filesystem::path::preferred_separator );
    if ( fname )
        fname++;

    elf.seekg( std::fstream::beg );
    elf.read( (char*)base, size );
    elf.close();

    if ( !is_valid() ) {
        free( base );
        base = nullptr, size = 0;
        std::cout << "[-] Invalid ELF file." << std::endl;
        return;
    }
}
*/

ELFReader::ELFReader( FileFactory* factory )
{
    base = factory->get_base();
    size = factory->get_file_size();
    path = factory->filepath();
    fname = factory->filename();
}

ELFReader::~ELFReader( void )
{
    if ( base ) {
        free( base );
        base = nullptr;
    }
}

size_t ELFReader::get_file_size( void )
{
    return size;
}

void* ELFReader::get_elf_header( void )
{
    return base;
}

bool ELFReader::is_valid( void )
{
    if ( base )
        return *(int*)base == 'FLE\x7f';
    return false;
}

bool ELFReader::is_32bit( void )
{
    return reinterpret_cast< Elf32_Ehdr* >( base )->e_ident[EI_CLASS] == ELFCLASS32;
}

bool ELFReader::save( const char* fname )
{
    std::fstream out( fname, std::fstream::binary | std::fstream::out );
    if ( out.is_open() ) {
        out.write( (char*)base, size );
        return true;
    }
    std::perror( __FUNCTION__ );
    return false;
}

char* ELFReader::filename( void )
{
    return fname;
}

char* ELFReader::filepath( void )
{
    return path;
}

void* ELFReader::get_prog_header( int index )
{
    void* result = nullptr;

    if ( this->is_32bit() ) {
        Elf32_Ehdr* ehdr = reinterpret_cast< Elf32_Ehdr* >( get_elf_header() );
        if ( index < ehdr->e_phnum ) {
            Elf32_Phdr* phdr = reinterpret_cast< Elf32_Phdr* >( (char*)base + ehdr->e_phoff );
            result = &phdr[index];
        }
    } else {
        Elf64_Ehdr* ehdr = reinterpret_cast< Elf64_Ehdr* >( get_elf_header() );
        if ( index < ehdr->e_phnum ) {
            Elf64_Phdr* phdr = reinterpret_cast< Elf64_Phdr* >( (char*)base + ehdr->e_phoff );
            result = &phdr[index];
        }
    }
    return result;
}

void* ELFReader::get_section_header( int index )
{
    void* result = nullptr;
    if ( is_32bit() ) {
        Elf32_Ehdr* ehdr = reinterpret_cast< Elf32_Ehdr* >( get_elf_header() );
        if ( index < ehdr->e_shnum ) {
            Elf32_Shdr* shdr = reinterpret_cast< Elf32_Shdr* >( (char*)base + ehdr->e_shoff );
            result = shdr + index;
        }
    } else {
        Elf64_Ehdr* ehdr = reinterpret_cast< Elf64_Ehdr* >( get_elf_header() );
        if ( index < ehdr->e_shnum ) {
            Elf64_Shdr* shdr = reinterpret_cast< Elf64_Shdr* >( (char*)base + ehdr->e_shoff );
            result = shdr + index;
        }
    }
    return result;
}

void* ELFReader::rva2va( uint64_t rva )
{
    return (char*)base + rva;
}

char* ELFReader::get_section_name( int index )
{
    char* result = nullptr;
    if ( is_32bit() ) {
        Elf32_Ehdr* ehdr = reinterpret_cast< Elf32_Ehdr* >( get_elf_header() );
        Elf32_Shdr* shdr = reinterpret_cast< Elf32_Shdr* >( get_section_header( index ) );
        Elf32_Shdr* shstrtab = reinterpret_cast< Elf32_Shdr* >( get_section_header( ehdr->e_shstrndx ) );
        char* strtab = (char*)base + shstrtab->sh_offset;
        if ( shdr && shstrtab ) {
            if ( shdr->sh_name == 0 ) {
                result = (char*)shstrtab;
            } else {
                result = (char*)strtab + shdr->sh_name;
            }
        }
    } else {
        Elf64_Ehdr* ehdr = reinterpret_cast< Elf64_Ehdr* >( get_elf_header() );
        Elf64_Shdr* shdr = reinterpret_cast< Elf64_Shdr* >( get_section_header( index ) );
        Elf64_Shdr* shstrtab = reinterpret_cast< Elf64_Shdr* >( get_section_header( ehdr->e_shstrndx ) );
        char* strtab = (char*)base + shstrtab->sh_offset;
        if ( shdr && shstrtab ) {
            if ( shdr->sh_name == 0 ) {
                result = (char*)shstrtab;
            } else {
                result = (char*)strtab + shdr->sh_name;
            }
        }
    }
    return result;
}

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

ELFReader* FileFactory::build( void )
{
    switch ( type ) {
        case elf32_t:
            return new ELFReader( this );
            break;
        case elf64_t:
            return new ELFReader( this );
            break;
        default:
            return nullptr;
    }
}
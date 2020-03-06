#include <fstream>
#include <errno.h>
#include <filesystem>
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

    base = malloc( size );
    path = (char*)malloc( strlen( filename ) + 1 );
    if ( base == nullptr || path == nullptr ) {
        std::perror( __FUNCTION__ );
        return;
    }

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
    }
    if ( path ) {
        free( (void*)path );
    }
    base = nullptr;
    path = nullptr;
}

size_t File::get_file_size( void )
{
    return size;
}

bool File::is_valid( void )
{
    if ( base )
        return *(int*)base == *ELFMAG;
    return false;
}

bool File::save( const char* fname )
{
    std::fstream out( fname, std::fstream::binary | std::fstream::out );
    if ( out.is_open() ) {
        out.write( (char*)base, size );
        return true;
    }
    std::perror( __FUNCTION__ );
    return false;
}

const char* File::filename( void )
{
    return fname;
}

const char* File::filepath( void )
{
    return path;
}

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

Elf32_Shdr* Elf32::get_section_header( int index )
{
    Elf32_Shdr* result = nullptr;
    Elf32_Ehdr* ehdr = get_elf_header();
    if ( index < ehdr->e_shnum ) {
        Elf32_Shdr* shdr = reinterpret_cast< Elf32_Shdr* >( (char*)base + ehdr->e_shoff );
        result = shdr + index;
    }
    return result;
}

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
    }
    return result;
}

file_t Elf32::get_type( void )
{
    return elf32;
}





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

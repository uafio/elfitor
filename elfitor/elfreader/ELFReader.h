#pragma once
#include <iostream>
#include <fstream>
#include "elf.h"

enum file_t {
    unknown = 0,
    elf32,
    elf64,
};

class File
{
protected:
    void* base;
    size_t size;
    const char* path;
    const char* fname;

public:
    File( void );
    File( const char* fname );
    ~File( void );
    virtual bool is_32bit( void ) = 0;
    virtual char* get_section_name( int index ) = 0;
    virtual file_t get_type( void );
    virtual bool save( const char* fname );
    void* get_base( void );
    size_t get_file_size( void );
    const char* filename( void );
    const char* filepath( void );
    bool is_valid( void );
    void* rva2va( uint64_t rva );
};

class Elf32 : public File
{
public:
    Elf32( File& file );
    Elf32_Ehdr* get_elf_header( void );
    Elf32_Phdr* get_prog_header( int index );
    Elf32_Shdr* get_section_header( int index );
    virtual char* get_section_name( int index );
    virtual file_t get_type( void );
    virtual bool is_32bit( void );
};

class Elf64 : public File
{
public:
    Elf64( File& file );
    Elf64_Ehdr* get_elf_header( void );
    Elf64_Phdr* get_prog_header( int index );
    Elf64_Shdr* get_section_header( int index );
    virtual char* get_section_name( int index );
    virtual file_t get_type( void );
    virtual bool is_32bit( void );
};

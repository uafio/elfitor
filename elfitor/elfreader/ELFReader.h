#pragma once
#include <iostream>
#include <fstream>
#include "elf.h"
#include "../imgui_memory_editor.h"

typedef enum {
    unknown_t = 0,
    elf32_t,
    elf64_t,
    elf_t,
} File_t;



class FileFactory
{
private:
    void* base;
    size_t size;
    char* fname;
    char* fpath;
    File_t type;
    bool is_supported( void );

public:
    FileFactory( const char* fname );
    void* get_base( void );
    size_t get_file_size( void );
    char* filename( void );
    char* filepath( void );
    File_t get_type( void );
    virtual class ELFReader* build( void );
};

/*
class File
{
private:
    MemoryEditor mViewer;

public:
    File( const FileFactory& factory );
    ~File( void );
    void* get_base( void );
    size_t get_file_size( void );
    const char* filename( void );
    const char* filepath( void );
    virtual void show_nav( State& state ) = 0;
    virtual void show_main( State& state ) = 0;
    virtual File_t get_type( void ) = 0;
    virtual File_t get_type_group( void ) = 0;
    virtual bool save( const char* fname ) = 0;
};
*/


class ELFReader
{
private:
    void* base;
    size_t size;
    char* path;
    char* fname;

public:
    //ELFReader( const char* fname );
    ELFReader( FileFactory* );
    ~ELFReader( void );
    bool is_valid( void );
    bool is_32bit( void );
    size_t get_file_size( void );
    bool save( const char* fname );
    char* filename( void );
    char* filepath( void );
    void* get_elf_header( void );
    void* get_prog_header( int index );
    void* get_section_header( int index );
    char* get_section_name( int index );
    void* rva2va( uint64_t rva );
};

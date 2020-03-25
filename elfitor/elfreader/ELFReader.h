#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include "elf.h"
#include "../imgui_memory_editor.h"

typedef enum {
    unknown_t = 0,
    elf32_t,
    elf64_t,
    elf_t,
} File_t;

typedef struct {
    enum _hdr {
        ehdr = 0,
        phdr,
        shdr
    } hdr;
    int idx;
} ElfCTX;

class State;
class File;
class Elf32;
class Elf64;
class FileFactory;

class State
{
private:
    State( void );
    static State* state;
    void remove( File* file );

protected:
    std::vector< File* > files;
    File* ctx_file;

public:
    static State* instance( void );
    void add( File* newfile );
    File* get_ctx_file( void );
    void close( void );
    void draw( void );
};

class FileFactory
{
private:
    void* base;
    size_t size;
    char* fname;
    char* fpath;
    File_t type;
    bool is_supported( void );
    File_t get_type( void );

public:
    FileFactory( const char* fname );
    void* get_base( void );
    size_t get_file_size( void );
    char* filename( void );
    char* filepath( void );
    virtual File* build( void );
};

class File
{
protected:
    void* base;
    size_t size;
    char* fname;
    char* fpath;

public:
    // Hex Editor
    MemoryEditor mViewer;

    // Construct the base object
    File( void );

    // Free all buffers
    ~File( void );

    // Pointer to base
    void* get_base( void );

    // Size of file
    size_t get_file_size( void );

    // Pointer to filename from filepath
    const char* filename( void );

    // Full path to filename
    const char* filepath( void );

    // Relative Virtual Address to Virtual Address
    void* rva2va( size_t rva );

    uint64_t va2rva( void* va );

    // Saves File on disk
    virtual bool save( const char* fname );

    // File type with architecture
    virtual File_t get_type( void ) = 0;

    // File type
    virtual File_t get_type_group( void ) = 0;

    // Navigational list view for supported main windows
    virtual void show_nav( void ) = 0;

    // Code to execute in the main window view
    virtual void show_main( void ) = 0;

    // Optional code of additional windows
    virtual void other_windows( void )
    {};
};

class Elf32 : public File
{
private:
    ElfCTX ctx;

public:
    Elf32( FileFactory* factory );
    virtual File_t get_type( void );
    virtual File_t get_type_group( void );
    virtual void show_nav( void );
    virtual void show_main( void );
    Elf32_Ehdr* get_elf_header( void );
    Elf32_Phdr* get_prog_header( int index );
    Elf32_Shdr* get_section_header( int index );
    char* get_section_name( int index );
    ElfCTX& get_ctx( void );
    Elf32_Dyn* get_dyn( void );
};

class Elf64 : public File
{
private:
    ElfCTX ctx;

public:
    Elf64( FileFactory* factory );
    virtual File_t get_type( void );
    virtual File_t get_type_group( void );
    virtual void show_nav( void );
    virtual void show_main( void );
    Elf64_Ehdr* get_elf_header( void );
    Elf64_Phdr* get_prog_header( int index );
    Elf64_Shdr* get_section_header( int index );
    char* get_section_name( int index );
    ElfCTX& get_ctx( void );
    Elf64_Dyn* get_dyn( void );
};

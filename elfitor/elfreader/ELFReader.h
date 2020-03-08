#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include "elf.h"

<<<<<<< Updated upstream
enum file_t {
    unknown = 0,
    elf32,
    elf64,
=======
typedef enum {
    unknown_t = 0,
    elf32_t,
    elf64_t,
    elf_t,
} File_t;

typedef struct {
    enum {
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
    void update( File* newfile );
    File* get_ctx_file( void );
    void close( void );
    void ShowNavigation( void );
    void ShowMainWindow( void );
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
>>>>>>> Stashed changes
};

class File
{
protected:
    void* base;
    size_t size;
<<<<<<< Updated upstream
    const char* path;
    const char* fname;

public:
    File( void );
    File( const char* fname );
=======
    char* fname;
    char* fpath;
    MemoryEditor mViewer;

public:
    File( void );
>>>>>>> Stashed changes
    ~File( void );
    virtual bool is_32bit( void ) = 0;
    virtual char* get_section_name( int index ) = 0;
    virtual file_t get_type( void );
    virtual bool save( const char* fname );
    void* get_base( void );
    size_t get_file_size( void );
    const char* filename( void );
    const char* filepath( void );
<<<<<<< Updated upstream
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
=======
    void* rva2va( size_t rva );
    virtual bool save( const char* fname );
    virtual File_t get_type( void ) = 0;
    virtual File_t get_type_group( void ) = 0;
    virtual void show_nav( State* state ) = 0;
    virtual void show_main( void ) = 0;
};

class Elf32 : public File
{
private:
    ElfCTX ctx;

public:
    Elf32( FileFactory* factory );
    virtual File_t get_type( void );
    virtual File_t get_type_group( void );
    virtual void show_nav( State* state );
    virtual void show_main( void );
    Elf32_Ehdr* get_elf_header( void );
    Elf32_Phdr* get_prog_header( int index );
    Elf32_Shdr* get_section_header( int index );
    char* get_section_name( int index );
    ElfCTX& get_ctx( void );
};

class Elf64 : public File
{
private:
    ElfCTX ctx;
>>>>>>> Stashed changes

class Elf64 : public File
{
public:
<<<<<<< Updated upstream
    Elf64( File& file );
    Elf64_Ehdr* get_elf_header( void );
    Elf64_Phdr* get_prog_header( int index );
    Elf64_Shdr* get_section_header( int index );
    virtual char* get_section_name( int index );
    virtual file_t get_type( void );
    virtual bool is_32bit( void );
=======
    Elf64( FileFactory* factory );
    virtual File_t get_type( void );
    virtual File_t get_type_group( void );
    virtual void show_nav( State* state );
    virtual void show_main( void );
    Elf64_Ehdr* get_elf_header( void );
    Elf64_Phdr* get_prog_header( int index );
    Elf64_Shdr* get_section_header( int index );
    char* get_section_name( int index );
    ElfCTX& get_ctx( void );
>>>>>>> Stashed changes
};

class Elf : public File
{
private:
    ElfCTX ctx;

public:
    Elf( FileFactory* factory );
    ElfCTX& get_ctx( void );
    virtual File_t get_type( void );
    virtual File_t get_type_group( void );
    //virtual void show_nav( State* state );
    //virtual void show_main( State* state );
    template< typename T >
    T get_elf_header( void );
    //template< typename T >
    //T get_prog_header( int index );
    //template< typename T >
    //T get_section_header( void );
    //char* get_section_name( void );
};
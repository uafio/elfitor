#pragma once
#include <iostream>
#include <fstream>
#include "elf.h"

class ELFReader {
private:
    void* base;
    size_t size;
    char* path;
    char* fname;

public:
    ELFReader(const char* fname);
    ~ELFReader(void);
    bool is_valid(void);
    bool is_32bit(void);
    void print_elf_header(void);
    void print_section_header(void* shdr);
    size_t get_file_size(void);
    bool save(const char* fname);
    char* filename(void);
    char* filepath(void);
    void* get_elf_header(void);
    void* get_prog_header(int index);
    void* get_section_header(int index);
    char* get_section_name(int index);
    void* rva2va( uint64_t rva );
};

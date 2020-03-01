#pragma once
#include <iostream>
#include <fstream>

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
    void* rva2va(uint64_t rva);
};

typedef struct {
    unsigned char   e_ident[4];
    uint8_t         e_class;
    uint8_t         e_data;
    uint8_t         e_ver;
    uint8_t         e_osabi;
    uint8_t         e_abiversion;
    uint8_t         e_pad[7];
    uint16_t        e_type;
    uint16_t        e_machine;
    uint32_t        e_version;
    uint32_t        e_entry;
    uint32_t        e_phoff;
    uint32_t        e_shoff;
    uint32_t        e_flags;
    uint16_t        e_ehsize;
    uint16_t        e_phentsize;
    uint16_t        e_phnum;
    uint16_t        e_shentsize;
    uint16_t        e_shnum;
    uint16_t        e_shstrndx;
} Elf32_Ehdr;
static_assert(sizeof(Elf32_Ehdr) == 0x34, "[!] Invalid Elf32_Ehdr size");

typedef struct {
    unsigned char   e_ident[4];
    uint8_t         e_class;
    uint8_t         e_data;
    uint8_t         e_ver;
    uint8_t         e_osabi;
    uint8_t         e_abiversion;
    uint8_t         e_pad[7];
    uint16_t        e_type;
    uint16_t        e_machine;
    uint32_t        e_version;
    uint64_t        e_entry;
    uint64_t        e_phoff;
    uint64_t        e_shoff;
    uint32_t        e_flags;
    uint16_t        e_ehsize;
    uint16_t        e_phentsize;
    uint16_t        e_phnum;        // number of Program Headers
    uint16_t        e_shentsize;
    uint16_t        e_shnum;
    uint16_t        e_shstrndx;
} Elf64_Ehdr;
static_assert(sizeof(Elf64_Ehdr) == 0x40, "[!] Invalid Elf64_Ehdr size");

typedef struct {
    uint32_t   p_type;
    uint32_t   p_offset;
    uint32_t   p_vaddr;
    uint32_t   p_paddr;
    uint32_t   p_filesz;
    uint32_t   p_memsz;
    uint32_t   p_flags;
    uint32_t   p_align;
} Elf32_Phdr;
static_assert(sizeof(Elf32_Phdr) == 0x20, "[!] Invalid Elf32_Phdr size");

typedef struct {
    uint32_t   p_type;
    uint32_t   p_flags;
    uint64_t   p_offset;
    uint64_t   p_vaddr;
    uint64_t   p_paddr;
    uint64_t   p_filesz;
    uint64_t   p_memsz;
    uint64_t   p_align;
} Elf64_Phdr;
static_assert(sizeof(Elf64_Phdr) == 0x38, "[!] Invalid Elf64_Phdr size");

typedef struct {
    uint32_t   sh_name;
    uint32_t   sh_type;
    uint32_t   sh_flags;
    uint32_t   sh_addr;
    uint32_t   sh_offset;
    uint32_t   sh_size;
    uint32_t   sh_link;
    uint32_t   sh_info;
    uint32_t   sh_addralign;
    uint32_t   sh_entsize;
} Elf32_Shdr;
static_assert(sizeof(Elf32_Shdr) == 0x28, "[!] Invalid Elf32_Shdr size");

typedef struct {
    uint32_t   sh_name;
    uint32_t   sh_type;
    uint64_t   sh_flags;
    uint64_t   sh_addr;
    uint64_t   sh_offset;
    uint64_t   sh_size;
    uint32_t   sh_link;
    uint32_t   sh_info;
    uint64_t   sh_addralign;
    uint64_t   sh_entsize;
} Elf64_Shdr;
static_assert(sizeof(Elf64_Shdr) == 0x40, "[!] Invalid Elf64_Shdr size");



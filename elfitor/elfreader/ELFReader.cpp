#include <fstream>
#include <errno.h>
#include <filesystem>
#include "ELFReader.h"

extern std::vector<ELFReader*> elfs;

ELFReader::ELFReader(const char* filename) {
    base = nullptr;
    std::fstream elf(filename, std::fstream::in | std::fstream::binary | std::fstream::ate);
    if (elf.is_open() == false) {
        std::perror(__FUNCTION__);
        return;
    }

    size = elf.tellg();
    if (size == 0) {
        std::cout << "[-] Empty file." << std::endl;
        return;
    }

    base = malloc(size);
    path = (char*)malloc(strlen(filename) + 1);
    if (base == nullptr || path == nullptr) {
        std::perror(__FUNCTION__);
        return;
    }
    
    strcpy(path, filename);
    fname = strrchr(path, (char)std::experimental::filesystem::path::preferred_separator);
    if (fname) fname++;

    elf.seekg(std::fstream::beg);
    elf.read((char*)base, size);
    elf.close();

    if (!is_valid()) {
        free(base);
        base = nullptr, size = 0;
        std::cout << "[-] Invalid ELF file." << std::endl;
        return;
    }
}

ELFReader::~ELFReader(void) {
    if (base) {
        free(base);
        base = nullptr;
    }
}

size_t ELFReader::get_file_size(void) {
    return size;
}

void* ELFReader::get_elf_header(void) {
    return base;
}


bool ELFReader::is_valid(void) {
    if (base)
        return *(int*)base == 'FLE\x7f';
    return false;
}

bool ELFReader::is_32bit(void) {
    return reinterpret_cast<Elf32_Ehdr*>(base)->e_class == 1;
}

void ELFReader::print_elf_header(void) {
    if (base == nullptr)
        return;
    
    union {
        Elf32_Ehdr* hdr32;
        Elf64_Ehdr* hdr64;
    } hdr;
    hdr.hdr32 = (Elf32_Ehdr*)base;

    printf("EI_MAG0      : %02x%c%c%c\n", hdr.hdr32->e_ident[0], hdr.hdr32->e_ident[1], hdr.hdr32->e_ident[2], hdr.hdr32->e_ident[3]);
    printf("EI_CLASS     : %d\n", hdr.hdr32->e_class);
    printf("EI_DATA      : %d\n", hdr.hdr32->e_data);
    printf("EI_VERSION   : %d\n", hdr.hdr32->e_ver);
    printf("EI_OSABI     : %d\n", hdr.hdr32->e_osabi);
    printf("EI_ABIVERSION: %d\n", hdr.hdr32->e_abiversion);
    printf("e_type       : %#hx\n", hdr.hdr32->e_type);
    printf("e_machine    : %#hx\n", hdr.hdr32->e_machine);
    printf("e_version    : %d\n", hdr.hdr32->e_version);
    if (is_32bit()) {
        printf("e_entry      : %#x\n", hdr.hdr32->e_entry);
        printf("e_phoff      : %#x\n", hdr.hdr32->e_phoff);
        printf("e_shoff      : %#x\n", hdr.hdr32->e_shoff);
        printf("e_flags      : %#x\n", hdr.hdr32->e_flags);
        printf("e_ehsize     : %#hx\n", hdr.hdr32->e_ehsize);
        printf("e_phentsize  : %#hx\n", hdr.hdr32->e_phentsize);
        printf("e_phnum      : %d\n", hdr.hdr32->e_phnum);
        printf("e_shentsize  : %#hx\n", hdr.hdr32->e_shentsize);
        printf("e_shnum      : %#hx\n", hdr.hdr32->e_shnum);
        printf("e_shstrndx   : %#hx\n", hdr.hdr32->e_shstrndx);
    }
    else {
        printf("e_entry      : %#llx\n", hdr.hdr64->e_entry);
        printf("e_phoff      : %#llx\n", hdr.hdr64->e_phoff);
        printf("e_shoff      : %#llx\n", hdr.hdr64->e_shoff);
        printf("e_flags      : %#x\n", hdr.hdr64->e_flags);
        printf("e_ehsize     : %#hx\n", hdr.hdr64->e_ehsize);
        printf("e_phentsize  : %#hx\n", hdr.hdr64->e_phentsize);
        printf("e_phnum      : %d\n", hdr.hdr64->e_phnum);
        printf("e_shentsize  : %#hx\n", hdr.hdr64->e_shentsize);
        printf("e_shnum      : %#hx\n", hdr.hdr64->e_shnum);
        printf("e_shstrndx   : %#hx\n", hdr.hdr64->e_shstrndx);
    }
}

void ELFReader::print_section_header(void* shdr) {
    if (is_32bit()) {
        Elf32_Shdr* s = (Elf32_Shdr*)shdr;
    }
    else {
        Elf64_Shdr* s = (Elf64_Shdr*)shdr;
        printf("sh_name: %#x\n", s->sh_name);
        printf("sh_type: %#x\n", s->sh_type);
        printf("sh_flags: %#llx\n", s->sh_flags);
        printf("sh_addr: %#llx\n", s->sh_addr);
        printf("sh_offset: %#llx\n", s->sh_offset);
        printf("sh_size: %#llx\n", s->sh_size);
        printf("sh_link: %#x\n", s->sh_link);
        printf("sh_info: %#x\n", s->sh_info);
        printf("sh_addralign: %#llx\n", s->sh_addralign);
        printf("sh_entsize: %#llx\n", s->sh_entsize);
    }
}


bool ELFReader::save(const char* fname) {
    std::fstream out(fname, std::fstream::binary | std::fstream::out);
    if (out.is_open()) {
        out.write((char*)base, size);
        return true;
    }
    std::perror(__FUNCTION__);
    return false;
}

char* ELFReader::filename(void) {
    return fname;
}

char* ELFReader::filepath(void) {
    return path;
}

void* ELFReader::get_prog_header(int index) {
    void* result = nullptr;

    if (this->is_32bit()) {
        Elf32_Ehdr* ehdr = reinterpret_cast<Elf32_Ehdr*>(get_elf_header());
        if (index < ehdr->e_phnum) {
            Elf32_Phdr* phdr = reinterpret_cast<Elf32_Phdr*>((char*)base + ehdr->e_phoff);
            result = &phdr[index];
        }
    }
    else {
        Elf64_Ehdr* ehdr = reinterpret_cast<Elf64_Ehdr*>(get_elf_header());
        if (index < ehdr->e_phnum) {
            Elf64_Phdr* phdr = reinterpret_cast<Elf64_Phdr*>((char*)base + ehdr->e_phoff);
            result = &phdr[index];
        }
    }
    return result;
}


void* ELFReader::get_section_header(int index) {
    void* result = nullptr;
    if (is_32bit()) {
        Elf32_Ehdr* ehdr = reinterpret_cast<Elf32_Ehdr*>(get_elf_header());
        if (index < ehdr->e_shnum) {
            Elf32_Shdr* shdr = reinterpret_cast<Elf32_Shdr*>((char*)base + ehdr->e_shoff);
            result = shdr + index;
        }
    }
    else {
        Elf64_Ehdr* ehdr = reinterpret_cast<Elf64_Ehdr*>(get_elf_header());
        if (index < ehdr->e_shnum) {
            Elf64_Shdr* shdr = reinterpret_cast<Elf64_Shdr*>((char*)base + ehdr->e_shoff);
            result = shdr + index;
        }
    }
    return result;
}


void* ELFReader::rva2va(uint64_t rva) {
    return (char*)base + rva;
}

char* ELFReader::get_section_name(int index) {
    char* result = nullptr;
    if (is_32bit()) {
        Elf32_Ehdr* ehdr = reinterpret_cast<Elf32_Ehdr*>(get_elf_header());
        Elf32_Shdr* shdr = reinterpret_cast<Elf32_Shdr*>(get_section_header(index));
        Elf32_Shdr* shstrtab = reinterpret_cast<Elf32_Shdr*>(get_section_header(ehdr->e_shstrndx));
        char* strtab = (char*)base + shstrtab->sh_offset;
        if (shdr && shstrtab) {
            if (shdr->sh_name == 0) {
                result = (char*)shstrtab;
            }
            else {
                result = (char*)strtab + shdr->sh_name;
            }
        }
    }
    else {
        Elf64_Ehdr* ehdr = reinterpret_cast<Elf64_Ehdr*>(get_elf_header());
        Elf64_Shdr* shdr = reinterpret_cast<Elf64_Shdr*>(get_section_header(index));
        Elf64_Shdr* shstrtab = reinterpret_cast<Elf64_Shdr*>(get_section_header(ehdr->e_shstrndx));
        char* strtab = (char*)base + shstrtab->sh_offset;
        if (shdr && shstrtab) {
            if (shdr->sh_name == 0) {
                result = (char*)shstrtab;
            }
            else {
                result = (char*)strtab + shdr->sh_name;
            }
        }
    }
    return result;
}
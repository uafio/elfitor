#pragma once
#include "elfreader/ELFReader.h"

typedef struct _CTX {
    ELFReader* elf;
    struct {
        enum {
            ehdr = 1,
            phdr,
            shdr,
        } hdr;
        int idx;
    } display;
} CTX;


namespace Imelf
{
    namespace Ehdr
    {
        void Draw( ELFReader* elf );
    }
    namespace Phdr
    {
        void Draw( ELFReader* elf );
    }
    namespace Shdr
    {
		void Draw( ELFReader* elf );
    }
}

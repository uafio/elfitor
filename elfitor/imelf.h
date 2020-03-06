#pragma once
#include "elfreader/ELFReader.h"

typedef struct _CTX {
    File* file;
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
        void Draw( File* elf );
    }
    namespace Phdr
    {
        void Draw( File* elf );
    }
    namespace Shdr
    {
		void Draw( File* elf );
    }
}

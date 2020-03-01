#pragma once
#include "elfreader/ELFReader.h"

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
    }
}
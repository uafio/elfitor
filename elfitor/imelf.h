#pragma once
#include "elfreader/ELFReader.h"

<<<<<<< Updated upstream
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


=======
>>>>>>> Stashed changes
namespace Imelf
{
    namespace Ehdr
    {
<<<<<<< Updated upstream
        void Draw( File* elf );
    }
    namespace Phdr
    {
        void Draw( File* elf );
    }
    namespace Shdr
    {
		void Draw( File* elf );
=======
        template< typename T >
        void Draw( T ehdr );
    }
    namespace Phdr
    {
        template< typename T >
        void Draw( T elf );
    }
    namespace Shdr
    {
        template< typename T >
        void Draw( T elf );
>>>>>>> Stashed changes
    }
}

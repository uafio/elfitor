#pragma once
#include "elfreader/ELFReader.h"

void HelpMarker( const char* desc );
void Tooltip( const char* desc );

namespace Imelf
{
    namespace Ehdr
    {
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
    }
}

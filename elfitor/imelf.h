#pragma once
#include "elfreader/ELFReader.h"

void HelpMarker( const char* desc );
void Tooltip( const char* desc );
void HoverTooltip( const char* desc );

namespace Imelf
{
    namespace Ehdr
    {
        template< typename T >
        void Draw( T );
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

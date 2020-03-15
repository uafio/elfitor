#pragma once
#include "elfreader/ELFReader.h"

#pragma warning( disable : 4200 )
typedef struct _ComboBoxMap {
    const int count;
    const struct {
        const uint32_t key;
        const char* const val;
        const char* const desc;
    } kv[0];

    inline const char* get_desc( uint32_t key ) const
    {
        for ( int i = 0; i < count; i++ ) {
            if ( key == kv[i].key ) {
                return kv[i].desc;
            }
        }
        return nullptr;
    }

    inline const char* get_val( uint32_t key ) const
    {
        for ( int i = 0; i < count; i++ ) {
            if ( key == kv[i].key ) {
                return kv[i].val;
            }
        }
        return nullptr;
    }
} ComboBoxMap;


void HelpMarker( const char* desc );
void Tooltip( const char* desc );
void FocusTooltip( const char* desc );
extern const ComboBoxMap PhdrTypeMap;


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

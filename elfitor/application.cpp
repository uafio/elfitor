#include "imgui.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>

#ifdef APIENTRY
#undef APIENTRY
#endif
#define STB_IMAGE_IMPLEMENTATION
#include <Windows.h>
#include <commdlg.h>

#include "elfreader/ELFReader.h"
#include "imelf.h"


void MenuItemAbout( void )
{
    puts( "Pressed About" );
}

void MenuItemOpen( void )
{
    OPENFILENAMEA ofile = { 0 };
    char fpath[_MAX_PATH] = { 0 };

    ofile.lStructSize = sizeof( ofile );
    ofile.hwndOwner = GetActiveWindow();
    ofile.lpstrFile = fpath;
    ofile.nMaxFile = sizeof( fpath );

    if ( GetOpenFileNameA( &ofile ) ) {
        FileFactory* factory = new FileFactory( fpath );
        File* newfile = factory->build();
        if ( newfile ) {
            State::instance()->add( newfile );
        }
    }
}

void MenuItemSave( void )
{
    printf( "Pressed Save\n" );
    auto state = State::instance();
    if ( state->get_ctx_file() == nullptr )
        return;

    state->get_ctx_file()->save( state->get_ctx_file()->filepath() );
}

void MenuItemSaveAs( void )
{
    puts( "Pressed Save As" );
    if ( State::instance()->get_ctx_file() == nullptr )
        return;

    OPENFILENAMEA ofile = { 0 };
    char szFile[_MAX_PATH] = { 0 };
    ofile.lStructSize = sizeof( ofile );
    ofile.hwndOwner = GetActiveWindow();
    ofile.lpstrFile = szFile;
    ofile.nMaxFile = sizeof( szFile );

    if ( GetSaveFileNameA( &ofile ) ) {
        State::instance()->get_ctx_file()->save( szFile );
    }
}

void MenuItemClose( void )
{
    State::instance()->close();
}

void ShowApplicationWindow( GLFWwindow* window )
{
    if ( ImGui::BeginMainMenuBar() ) {
        if ( ImGui::BeginMenu( "File" ) ) {
            if ( ImGui::MenuItem( "Open" ) ) {
                MenuItemOpen();
            } else if ( ImGui::MenuItem( "Save" ) ) {
                MenuItemSave();
            } else if ( ImGui::MenuItem( "Save As" ) ) {
                MenuItemSaveAs();
            } else if ( ImGui::MenuItem( "Close" ) ) {
                MenuItemClose();
            } else if ( ImGui::MenuItem( "Exit" ) ) {
                glfwSetWindowShouldClose( window, true );
            }

            ImGui::EndMenu();
        }
        if ( ImGui::BeginMenu( "Settings" ) ) {
            ImGui::EndMenu();
        }
        if ( ImGui::MenuItem( "About" ) ) {
            MenuItemAbout();
        }

        ImGui::EndMainMenuBar();
    }

    State::instance()->draw();
}
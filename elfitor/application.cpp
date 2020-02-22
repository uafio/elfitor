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
#include "stb_image.h"


std::vector<ELFReader*> elfs;

struct _CTX {
    ELFReader* elf;
    struct {
        enum {
            ehdr = 1,
            phdr,
            shdr,
        } hdr;
        int idx;
    } display;
} ctx;

void MenuItemAbout(void) {
    puts("Pressed About");
}

void MenuItemOpen(void) {

    OPENFILENAMEA ofile = { 0 };
    char szFile[_MAX_PATH] = { 0 };

    ofile.lStructSize = sizeof(ofile);
    ofile.hwndOwner = GetActiveWindow();
    ofile.lpstrFile = szFile;
    ofile.nMaxFile = sizeof(szFile);

    if (GetOpenFileNameA(&ofile)) {

        ELFReader* newelf = new ELFReader(szFile);
        if (newelf->is_valid()) {
            elfs.push_back(newelf);
            ctx.elf = newelf;
            ctx.display.hdr = ctx.display.ehdr;
            ctx.display.idx = 0;
            newelf->print_elf_header();
        }
    }
}


void MenuItemSave(void) {
    printf("Pressed Save\n");
    if (ctx.elf == nullptr) return;

    ctx.elf->save(ctx.elf->filepath());
}


void MenuItemSaveAs(void) {
    puts("Pressed Save As");
    if (ctx.elf == nullptr) return;

    OPENFILENAMEA ofile = { 0 };
    char szFile[_MAX_PATH] = { 0 };
    ofile.lStructSize = sizeof(ofile);
    ofile.hwndOwner = GetActiveWindow();
    ofile.lpstrFile = szFile;
    ofile.nMaxFile = sizeof(szFile);

    if (GetSaveFileNameA(&ofile)) {
        ctx.elf->save(szFile);
    }
}

void MenuItemClose(void) {
    if (ctx.elf) {
        for (auto elf = elfs.begin(); elf != elfs.end(); elf++) {
            if (*elf == ctx.elf) {
                elfs.erase(elf);
                break;
            }
        }
        ctx = { 0 };
    }
}

void ElfHeaderNavigation64(ELFReader* elf) {
    ImGuiTreeNodeFlags flags = 0;
    flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
    flags |= ImGuiTreeNodeFlags_Leaf;

    if (ctx.elf == elf && ctx.display.hdr == ctx.display.ehdr)
        ImGui::TreeNodeEx("ELF Header Leaf", flags | ImGuiTreeNodeFlags_Selected, "ELF Header");
    else
        ImGui::TreeNodeEx("ELF Header Leaf", flags, "ELF Header");
    if (ImGui::IsItemClicked()) {
        ctx.elf = elf;
        ctx.display.hdr = ctx.display.ehdr;
    }

    Elf64_Ehdr* e_hdr = (Elf64_Ehdr*)elf->get_elf_header();

    ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());

    if (ImGui::TreeNode(elf->get_prog_header(0), "Program Headers")) {
        for (int i = 0; i < e_hdr->e_phnum; i++) {
            if (ctx.elf == elf && ctx.display.hdr == ctx.display.phdr && ctx.display.idx == i) {
                ImGui::TreeNodeEx((void*)(intptr_t)i, flags | ImGuiTreeNodeFlags_Selected, "Program Header #%d", i + 1);
            }
            else {
                ImGui::TreeNodeEx((void*)(intptr_t)i, flags, "Program Header #%d", i + 1);
            }
            if (ImGui::IsItemClicked()) {
                ctx.elf = elf;
                ctx.display.hdr = ctx.display.phdr;
                ctx.display.idx = i;
            }
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode(elf->get_section_header(0), "Section Headers")) {
        for (int i = 0; i < e_hdr->e_shnum; i++) {
            Elf64_Shdr* shdr = (Elf64_Shdr*)elf->get_section_header(i);
            if (ctx.elf == elf && ctx.display.hdr == ctx.display.shdr && ctx.display.idx == i) {
                if (shdr->sh_type == 0 || shdr->sh_name == 0) {
                    ImGui::TreeNodeEx((void*)(intptr_t)i, flags | ImGuiTreeNodeFlags_Selected, "NULL");
                }
                else {
                    ImGui::TreeNodeEx((void*)(intptr_t)i, flags | ImGuiTreeNodeFlags_Selected, elf->get_section_name(i));
                }
            }
            else {
                if (shdr->sh_type == 0 || shdr->sh_name == 0) {
                    ImGui::TreeNodeEx((void*)(intptr_t)i, flags, "NULL");
                }
                else {
                    ImGui::TreeNodeEx((void*)(intptr_t)i, flags, elf->get_section_name(i));
                }
            }
            if (ImGui::IsItemClicked()) {
                ctx.elf = elf;
                ctx.display.hdr = ctx.display.shdr;
                ctx.display.idx = i;
            }
        }
        ImGui::TreePop();
    }

    ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());

}

void ElfHeaderNavigation32(ELFReader* elf) {

}

void ShowNagivationWindow(void) {
    ImGuiWindowFlags flags = 0;
    flags |= ImGuiWindowFlags_NoTitleBar;
    flags |= ImGuiWindowFlags_NoMove;
    flags |= ImGuiWindowFlags_NoResize;
    flags |= ImGuiWindowFlags_NoCollapse;
    flags |= ImGuiWindowFlags_NoNav;

    ImGui::Begin("Navigation", NULL, flags);

    if (elfs.size()) {
        for (auto elf = elfs.begin(); elf != elfs.end(); ) {

            char label[_MAX_PATH];
            snprintf(label, sizeof(label), "%s##%zd", (*elf)->filename(), std::distance(elfs.begin(), elf));

            bool close = true;
            if (ImGui::CollapsingHeader(label, &close, ImGuiTreeNodeFlags_DefaultOpen)) {
                if ((*elf)->is_32bit()) {
                    ElfHeaderNavigation32(*elf);
                }
                else {
                    ElfHeaderNavigation64(*elf);
                }
            }
            if (close == false) {
                if (*elf == ctx.elf) {
                    ctx = { 0 };
                }
                delete *elf;
                elfs.erase(elf);
            }
            else {
                ++elf;
            }
        }
    }

    ImGui::End();
}

void MainWindowElfHeader64(void) {
    Elf64_Ehdr* ehdr = reinterpret_cast<Elf64_Ehdr*>( ctx.elf->get_elf_header() );

    static ImGuiTableFlags flags = ImGuiTableFlags_SizingPolicyFixedX | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;
    ImGui::BeginTable("elf header 32", 5, flags, ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 1));
    ImGui::TableSetupColumn("Member", 0, 340.0);
    ImGui::TableSetupColumn("Offset", 0, 200.0);
    ImGui::TableSetupColumn("Size", 0, 150.0);
    ImGui::TableSetupColumn("Value", 0, 200.0);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableAutoHeaders();

    ImGui::TableNextRow();
    ImGui::Text("e_ident[EI_MAG]"); ImGui::TableNextCell();
    ImGui::Text("%08x", offsetof(Elf64_Ehdr, e_ident)); ImGui::TableNextCell();
    ImGui::Text("%02x", sizeof(ehdr->e_ident)); ImGui::TableNextCell();
    ImGui::Text("%08x", *(uint32_t*)ehdr->e_ident); ImGui::TableNextCell();

    ImGui::TableNextRow();
    ImGui::Text("e_ident[EI_CLASS]"); ImGui::TableNextCell();
    ImGui::Text("%08x", offsetof(Elf64_Ehdr, e_class)); ImGui::TableNextCell();
    ImGui::Text("%02x", sizeof(ehdr->e_class)); ImGui::TableNextCell();
    ImGui::Text("%d", ehdr->e_class); ImGui::TableNextCell();

    ImGui::TableNextRow();
    ImGui::Text("e_ident[EI_DATA]"); ImGui::TableNextCell();
    ImGui::Text("%08x", offsetof(Elf64_Ehdr, e_class)); ImGui::TableNextCell();
    ImGui::Text("%02x", sizeof(ehdr->e_class)); ImGui::TableNextCell();
    ImGui::Text("%d", ehdr->e_class); ImGui::TableNextCell();

    ImGui::EndTable();

}

void MainWindowProgHeader64(void) {
    ImGui::Text("Program Header %d", ctx.display.idx);
}

void MainWindowSectionHeader64(void) {
    ImGui::Text("Section Header %d, %s", ctx.display.idx, ctx.elf->get_section_name(ctx.display.idx));
}

void ShowMainWindow(void) {
    ImGuiWindowFlags wflags = 0;
    wflags |= ImGuiWindowFlags_NoMove;
    wflags |= ImGuiWindowFlags_NoResize;
    wflags |= ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("MainWindow", NULL, wflags);
    if (ctx.elf == nullptr) {
        ImGui::Text("ELF Editor");
    }
    else if (ctx.display.hdr == ctx.display.ehdr) {
        MainWindowElfHeader64();
    }
    else if (ctx.display.hdr == ctx.display.phdr) {
        MainWindowProgHeader64();
    }
    else if (ctx.display.hdr == ctx.display.shdr) {
        MainWindowSectionHeader64();
    }
    else {
        ImGui::Text("ELF Header for %s", ctx.elf->filepath());
    }

    ImGui::End();
}

void ShowApplicationWindow(GLFWwindow* window) {

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open")) {
                MenuItemOpen();
            }
            else if (ImGui::MenuItem("Save")) {
                MenuItemSave();
            }
            else if (ImGui::MenuItem("Save As")) {
                MenuItemSaveAs();
            }
            else if (ImGui::MenuItem("Close")) {
                MenuItemClose();
            }
            else if (ImGui::MenuItem("Exit")) {
                elfs.clear();
                glfwSetWindowShouldClose(window, true);
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings"))
        {
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("About")) {
            MenuItemAbout();
        }


        ImGui::EndMainMenuBar();
    }

    ShowNagivationWindow();
    ShowMainWindow();

}
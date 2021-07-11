#pragma once

#include <ui/views/view.hpp>

#include <memory>
#include <string>
#include <thread>

#include <board/board_test.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace vc::ui {

    class ViewPCB : public View {
    public:
       explicit ViewPCB(pcb::Board &board) : View("PCB"), board(board) { }

        void drawContent() override {
            auto drawList = ImGui::GetWindowDrawList();
            auto windowPos = ImGui::GetWindowPos();
            auto windowSize = ImGui::GetWindowSize();

            board.setPosition(windowPos + (windowSize - board.getDimensions()) / 2);
            board.draw(drawList);
        }

    private:
        pcb::Board &board;
        std::string console = "Console: ";
    };

}
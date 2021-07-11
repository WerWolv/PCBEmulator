#pragma once

#include <ui/views/view.hpp>

#include <chrono>
#include <memory>
#include <thread>
#include <future>

#include <board/board_test.hpp>

namespace vc::ui {

    class ViewControl : public View {
    public:
        explicit ViewControl(pcb::Board &board) : View("Control"), board(board) {

        }

        ~ViewControl() override {
            this->board.powerDown();
            this->boardThread.join();
        }

        void drawContent() override {
            ImGui::Disabled([this] {
                if (ImGui::Button("Power up PCB")) {
                    if (!this->boardRunning) {
                        this->boardRunning = true;
                        this->boardThread = std::thread([this] {
                            this->board.powerUp();
                            this->boardRunning = false;
                        });
                    }
                }
            }, this->boardRunning);

            ImGui::Disabled([this] {
                if (ImGui::Button("Unplug PCB")) {
                    this->board.powerDown();
                }
            }, !this->boardRunning);

            if (this->boardRunning) {
                ImGui::TextSpinner("PCB running...");
            } else if (this->boardThread.joinable()) {
                this->boardThread.join();
            }
        }

    private:
        pcb::Board &board;
        std::thread boardThread;
        bool boardRunning = false;
    };

}
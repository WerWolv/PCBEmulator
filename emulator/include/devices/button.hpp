#pragma once

#include <board/track.hpp>

namespace vc::dev {

    class Button : public Device, public pcb::Connectable {
    public:
        explicit Button(ImVec2 pos) {
            this->setPosition(pos);
            this->setSize({ 31, 31 });
        }

        auto get(std::string_view name) {
            return this->getTrack(name);
        }

        void tick() override {
            for (auto &trackName : this->getConnectedTrackNames()) {
                auto track = this->getTrack(trackName);
                track->setValue(this->pressed);
            }
        }

        bool needsUpdate() override { return true; }
        void reset() override {
            pressed = false;
        }

        void draw(ImVec2 start, ImDrawList *drawList) override {
            this->pressed = ImGui::IsMouseHoveringRect(start + getPosition(), start + getPosition() + getSize()) && ImGui::IsMouseDown(ImGuiMouseButton_Left);

            drawList->AddRectFilled(start + getPosition(), start + getPosition() + getSize(), ImColor(0xA0, 0xA0, 0xA0, 0xFF));
            drawList->AddCircleFilled(start + getPosition() + getSize() / 2,  9, this->pressed ? ImColor(0x80, 0x20, 0x20, 0xFF) : ImColor(0xA0, 0x20, 0x20, 0xFF));
        }

    private:
        bool pressed = false;
    };

}
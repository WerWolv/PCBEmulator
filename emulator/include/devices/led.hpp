#pragma once

#include <board/track.hpp>

namespace vc::dev {

    class LED : public Device, public pcb::Connectable {
    public:
        explicit LED(ImVec2 pos) {
            this->setPosition(pos);
            this->setSize({ 20, 10 });
        }

        auto get(std::string_view name) {
            return this->getTrack(name);
        }

        void tick() override {
            for (auto &trackName : this->getConnectedTrackNames()) {
                auto track = this->getTrack(trackName);

                if (track->hasValue())
                    glowing = track->getValue().value();
            }
        }

        bool needsUpdate() override { return true; }
        void reset() override {
            glowing = false;
        }

        void draw(ImVec2 start, ImDrawList *drawList) override {
            drawList->AddRectFilled(start + getPosition(), start + getPosition() + getSize(), ImColor(0xA0, 0xA0, 0xA0, 0xFF));
            drawList->AddRectFilled(start + getPosition() + ImVec2(5, 0), start + getPosition() + getSize() - ImVec2(5, 0), glowing ? ImColor(0xA0, 0x10, 0x10, 0xFF) : ImColor(0x30, 0x10, 0x10, 0xFF));
        }

    private:
        bool glowing = false;
    };

}
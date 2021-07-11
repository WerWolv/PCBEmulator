#pragma once

#include <board/track.hpp>

namespace vc::dev {

    class PinHeader : public Device, public pcb::Connectable {
    public:
        explicit PinHeader(ImVec2 pos) {
            this->setPosition(pos);
        }

        auto get(std::string_view name) {
            return this->getTrack(name);
        }

        void tick() override { }
        bool needsUpdate() override { return this->dataAvailable(); }
        void reset() override {
            this->receivedData.clear();
        }

        void draw(ImVec2 start, ImDrawList *drawList) override {
            this->numPins = this->getConnectedTrackNames().size();
            this->setSize({ 19.0F * this->numPins, 19.0F });

            drawList->AddRectFilled(start + getPosition(), start + getPosition() + getSize(), ImColor(0x10, 0x10, 0x10, 0xFF));

            for (auto i = 0; i < this->numPins; i++) {
                drawList->AddCircleFilled(ImVec2(start + getPosition() + ImVec2(9 + 19 * i, 10)), 4, ImColor(0xB0, 0xB0, 0xC0, 0xFF));
            }

            for (auto &trackName : this->getConnectedTrackNames()) {
                auto c = this->get(trackName)->getValue();
                if (c.has_value())
                    receivedData[std::string(trackName)] += (char)*c;
            }

            if (ImGui::IsMouseHoveringRect(start + getPosition(), start + getPosition() + getSize())) {
                ImGui::BeginTooltip();
                ImGui::TextUnformatted("Connected Tracks");
                ImGui::Separator();
                for (auto &trackName : this->getConnectedTrackNames()) {
                    ImGui::Text("%s: %s", trackName.data(), receivedData[std::string(trackName)].c_str());
                }
                ImGui::EndTooltip();
            }
        }

    private:
        u32 numPins = 1;
        std::map<std::string, std::string> receivedData;
    };

}
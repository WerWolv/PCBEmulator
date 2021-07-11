#pragma once

#include <concepts>
#include <list>
#include <map>
#include <string>
#include <string_view>

#include <devices/device.hpp>
#include <board/track.hpp>

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace vc::pcb {

    class Board {
    public:
        explicit Board(std::string_view name, ImVec2 size) : boardName(name), dimensions(size) { }
        virtual ~Board() {
            this->powerDown();

            for (auto &device : this->devices)
                delete device;

            for (auto &[name, track] : this->tracks)
                delete track;
        }

        void powerUp() {
            this->hasPower = true;

            for (auto &device : this->devices)
                device->reset();

            bool doneWork;
            do {
                doneWork = false;
                for (auto &device : this->devices) {
                    if (device->needsUpdate()) {
                        device->tick();
                        doneWork = true;
                    }
                }
            } while (doneWork && this->hasPower);
        }

        void powerDown() {
            this->hasPower = false;
        }

        [[nodiscard]]
        std::string_view getName() const {
            return this->boardName;
        }

        virtual void draw(ImDrawList *drawList) {
            drawList->AddRectFilled(getPosition(), getPosition() + getDimensions(), ImColor(0x09, 0x91, 0x32, 0xFF));

            for (auto &[name, track] : this->tracks) {
                auto [from, to] = track->getEndpoints();

                auto startPos = getPosition() + from->getPosition() + from->getSize() / 2;
                auto endPos = getPosition() + to->getPosition() + to->getSize() / 2;
                auto middlePos = startPos.x - getPosition().x > startPos.y - getPosition().y ? ImVec2(startPos.x, endPos.y) : ImVec2(endPos.x, startPos.y);

                drawList->AddLine(startPos, middlePos, ImColor(0x19, 0xC1, 0x62, 0xFF), 3);
                drawList->AddLine(middlePos, endPos, ImColor(0x19, 0xC1, 0x62, 0xFF), 3);
            }
                
                
            for (auto &device : this->devices) {
                if (auto connectable = dynamic_cast<Connectable*>(device); connectable != nullptr) {
                    connectable->draw(getPosition(), drawList);
                }
            }
        }

        [[nodiscard]]
        ImVec2 getPosition() const {
            return this->position;
        }

        void setPosition(ImVec2 pos) {
            this->position = pos;
        }

        [[nodiscard]]
        ImVec2 getDimensions() const {
            return this->dimensions;
        }

    protected:
        template<std::derived_from<dev::Device> T, typename ...Args>
        auto& createDevice(Args&&... args) {
            auto device = new T(std::forward<Args>(args)...);
            this->devices.push_back(device);

            return *device;
        }

        void createTrack(Direction direction, const std::string &name, pcb::Connectable &from, pcb::Connectable &to, bool buffered = false) {
            if (this->tracks.contains(name)) return;

            auto track = new Track(direction, buffered, &from, &to);
            this->tracks.insert({ name, track });

            from.linkTrack(name, track);
            to.linkTrack(name, track);
        }

    private:
        bool hasPower = false;
        std::string boardName;
        std::list<dev::Device*> devices;
        std::map<std::string, pcb::Track*> tracks;

        ImVec2 position;
        ImVec2 dimensions;
    };

}
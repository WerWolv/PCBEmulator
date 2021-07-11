#pragma once

#include <risc.hpp>

#include <queue>
#include <optional>
#include <map>
#include <mutex>

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace vc::pcb {

    class Connectable;

    enum class Direction {
        MISO,
        MOSI
    };

    class Track {
    public:
        Track(Direction direction, bool buffered, Connectable *from, Connectable *to) : direction(direction), buffered(buffered), from(from), to(to) {

        }

        [[nodiscard]]
        std::optional<u8> getValue() {
            std::scoped_lock lk(this->modifyMutex);

            if (this->buffered) {
                if (this->receivedData.empty())
                    return { };
                auto item = this->receivedData.front();
                this->receivedData.pop();
                return item;
            } else {
                if (!this->value.has_value())
                    return { };
                auto item = this->value.value();
                this->value.reset();
                return item;
            }
        }

        [[nodiscard]]
        bool hasValue() {
            std::scoped_lock lock(this->modifyMutex);

            if (this->buffered)
                return !this->receivedData.empty();
            else
                return this->value.has_value();
        }

        void setValue(u8 value) {
            std::scoped_lock lock(this->modifyMutex);

            if (this->buffered)
                this->receivedData.emplace(value);
            else
                this->value = value;
        }

        [[nodiscard]]
        std::pair<Connectable*, Connectable*> getEndpoints() const {
            return { from, to };
        }

        [[nodiscard]]
        Direction getDirection() const {
            return this->direction;
        }

    private:
        Direction direction;
        bool buffered;
        std::mutex modifyMutex;
        std::optional<u8> value;
        std::queue<u8> receivedData;

        Connectable *from, *to;
    };

    class Connectable {
    public:
        friend class Board;
    protected:
        auto getTrack(std::string_view name) {
            return this->connectedTracks[std::string(name)];
        }

        void linkTrack(const std::string &name, pcb::Track *track) {
            this->connectedTracks.insert({ name, track });
        }

        [[nodiscard]]
        bool dataAvailable() {
            for (auto &[name, track] : this->connectedTracks)
                if (track->hasValue())
                    return true;

            return false;
        }

        auto getConnectedTrackNames() {
            std::vector<std::string_view> result;

            for (auto &[name, track] : this->connectedTracks)
                result.push_back(name);

            return result;
        }


        virtual void draw(ImVec2 start, ImDrawList *drawList) {
            drawList->AddRectFilled(start + position, start + position + size, ImColor(0x10, 0x10, 0x10, 0xFF));
        }

    public:
        [[nodiscard]]
        ImVec2 getPosition() const {
            return this->position;
        }

        void setPosition(ImVec2 pos) {
            this->position = pos;
        }

        [[nodiscard]]
        ImVec2 getSize() const {
            return this->size;
        }

        void setSize(ImVec2 size) {
            this->size = size;
        }

    private:
        std::map<std::string, pcb::Track*> connectedTracks;

        ImVec2 position;
        ImVec2 size;
    };

}
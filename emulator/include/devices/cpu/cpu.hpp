#pragma once

#include <devices/device.hpp>
#include <devices/cpu/core/core.hpp>
#include <devices/cpu/core/io_pin.hpp>

#include <array>

#include <utils.hpp>

namespace vc::dev {

    class CPUDevice : public vc::dev::Device, public pcb::Connectable {
    public:
        explicit CPUDevice(u32 numCores, ImVec2 pos) {
            for (u32 i = 0; i < numCores; i++)
                this->cores.emplace_back(addressSpace);

            this->setPosition(pos);
            this->setSize({ 100, 100 });

            for (auto &mmio : this->addressSpace.getDevices()) {
                if (auto connectable = dynamic_cast<pcb::Connectable*>(mmio); connectable != nullptr) {
                    connectable->setPosition(this->getPosition());
                    connectable->setSize(this->getSize());
                }
            }
        }

        ~CPUDevice() override = default;

        void tick() override {
            for (auto &core : this->cores) {
                try {
                    core.execute();
                } catch (cpu::AccessFaultException &e) {
                    core.halt("Access Fault exception thrown!");
                } catch (cpu::UnalignedAccessException &e) {
                    core.halt("Unaligned Access exception thrown!");
                } catch (...) {
                    core.halt("Unknown exception thrown!");
                }
            }

            for (auto &[trackName, pinNumber] : this->pinToTrackConnections) {
                auto &pin = this->pins[pinNumber];
                auto track = this->getTrack(trackName);

                if (track->getDirection() == pcb::Direction::MOSI && pin->hasValue()) {
                    track->setValue(pin->getValue().value());
                }

                if (track->getDirection() == pcb::Direction::MISO && track->hasValue()) {
                    pin->setValue(track->getValue().value());
                }
            }
        }

        bool needsUpdate() override {
            for (auto &core : this->cores) {
                if (!core.isHalted())
                    return true;
            }

            return false;
        }

        void reset() override {
            for (auto &core : this->cores)
                core.reset();
        }

        auto& getAddressSpace() {
            return this->addressSpace;
        }

        void draw(ImVec2 start, ImDrawList *drawList) override {
            drawList->AddRectFilled(start + getPosition(), start + getPosition() + getSize(), ImColor(0x10, 0x10, 0x10, 0xFF));
            drawList->AddText(start + getPosition() + ImVec2(10, 10), ImColor(0xFFFFFFFF), fmt::format("RISC-V\n {} Core", this->cores.size()).c_str());

            if (ImGui::IsMouseHoveringRect(start + getPosition(), start + getPosition() + getSize())) {
                ImGui::BeginTooltip();
                if (this->needsUpdate())
                    ImGui::TextSpinner("Running...");
                else
                    ImGui::TextUnformatted("Halted");

                ImGui::Separator();

                for (auto &device : this->getAddressSpace().getDevices()) {
                    ImGui::TextUnformatted(fmt::format("{}: 0x{:016X} - 0x{:016X}", device->getName(), device->getBase(), device->getEnd()).c_str());
                }

                ImGui::EndTooltip();
            }
        }

        void attachToPin(u32 pinNumber, cpu::IOPin &pin) {
            this->pins.insert({ pinNumber, &pin });
        }

        void attachPinToTrack(u32 pinNumber, std::string_view trackName) {
            this->pinToTrackConnections[std::string(trackName)] = pinNumber;
        }

    private:
        cpu::AddressSpace addressSpace;
        std::vector<cpu::Core> cores;
        std::map<u32, cpu::IOPin*> pins;
        std::map<std::string, u32> pinToTrackConnections;
    };

}
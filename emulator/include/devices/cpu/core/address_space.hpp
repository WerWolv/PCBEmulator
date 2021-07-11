#pragma once

#include <risc.hpp>
#include <set>
#include <devices/cpu/core/mmio/device.hpp>
#include <utils.hpp>
#include <elf.hpp>

namespace vc::dev::cpu {

    struct AccessFaultException : public std::exception { };
    struct UnalignedAccessException : public std::exception { };

    struct byte_tag  {};
    struct hword_tag {};
    struct word_tag  {};
    struct dword_tag {};

    class AddressSpace {
    public:
        void addDevice(mmio::MMIODevice &device) {
            for (const auto mappedDevice : this->devices) {
                if (device.getBase() >= mappedDevice->getBase() && device.getEnd() <= mappedDevice->getEnd() || mappedDevice->getBase() >= device.getBase() && mappedDevice->getEnd() <= device.getEnd())
                    log::fatal("Tried to map device to occupied address range");
            }

            this->devices.insert(&device);
        }

        auto& operator()(u64 address, byte_tag) {
            auto device = findDevice(address, 1);
            if (device == nullptr) {
                log::error("Invalid memory access at {:#x}", address);
                throw AccessFaultException();
            }

            return device->byte(address - device->getBase());
        }

        auto& operator()(u64 address, hword_tag) {
            auto device = findDevice(address, 2);
            if (device == nullptr) {
                log::error("Invalid memory access at {:#x}", address);
                throw AccessFaultException();
            }
            return device->halfWord(address - device->getBase());
        }

        auto& operator()(u64 address, word_tag) {
            auto device = findDevice(address, 4);
            if (device == nullptr) {
                log::error("Invalid memory access at {:#x}", address);
                throw AccessFaultException();
            }
            return device->word(address - device->getBase());
        }

        auto& operator()(u64 address, dword_tag) {
            auto device = findDevice(address, 8);
            if (device == nullptr) {
                log::error("Invalid memory access at {:#x}", address);
                throw AccessFaultException();
            }
            return device->doubleWord(address - device->getBase());
        }

        void tickDevices() {
            for (auto &device : this->devices)
                device->doTick();
        }

        bool loadELF(std::string_view path) {
            std::vector<u8> buffer;

            {
                FILE *file = fopen(path.data(), "rb");
                if (file == nullptr) return false;
                ON_SCOPE_EXIT { fclose(file); };

                fseek(file, 0, SEEK_END);
                size_t size = ftell(file);
                rewind(file);

                buffer.resize(size, 0x00);
                fread(buffer.data(), buffer.size(), 1, file);
            }

            {
                elf64_hdr elfHeader = { 0 };
                std::memcpy(&elfHeader, buffer.data() + 0, sizeof(elf64_hdr));
                std::vector<elf64_phdr> programHeader(elfHeader.e_phnum - 1, { 0 });
                std::memcpy(programHeader.data(), buffer.data() + elfHeader.e_phoff, elfHeader.e_phentsize * programHeader.size());

                for (const auto &pheader : programHeader) {
                    for (u32 offset = 0; offset < pheader.p_filesz; offset++)
                        this->operator()(pheader.p_paddr + offset, byte_tag{}) = buffer[pheader.p_offset + offset];
                    log::info("Mapped section to {:#x}:{:#x}", pheader.p_paddr, pheader.p_paddr + pheader.p_memsz);
                }
            }

            return true;
        }

        [[nodiscard]]
        auto& getDevices() {
            return this->devices;
        }
    private:
        [[nodiscard]]
        mmio::MMIODevice* findDevice(u64 address, u8 accessSize) const {
            auto device = std::find_if(devices.begin(), devices.end(), [&](mmio::MMIODevice *curr){
                return address >= curr->getBase() && address <= curr->getEnd() - accessSize;
            });

            if (device == devices.end()) return nullptr;

            return *device;
        }

        std::set<mmio::MMIODevice*> devices;
    };

}
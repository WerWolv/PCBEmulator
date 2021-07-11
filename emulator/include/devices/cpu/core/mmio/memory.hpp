#pragma once

#include <devices/cpu/core/mmio/device.hpp>

#include <numeric>

namespace vc::dev::cpu::mmio {

    class Memory : public MMIODevice {
    public:
        Memory(u64 base, size_t size) : MMIODevice("Internal Memory", base, size) {
            this->data.resize(size);
        }

        [[nodiscard]]
        u8& byte(u64 offset) noexcept override {
            return this->data[offset];
        }

        [[nodiscard]]
        u16& halfWord(u64 offset) noexcept override {
            return *reinterpret_cast<u16*>(&this->data[offset]);
        }

        [[nodiscard]]
        u32& word(u64 offset) noexcept override {
            return *reinterpret_cast<u32*>(&this->data[offset]);
        }

        [[nodiscard]]
        u64& doubleWord(u64 offset) noexcept override {
            return *reinterpret_cast<u64*>(&this->data[offset]);
        }

    private:
        std::vector<u8> data;
    };

}
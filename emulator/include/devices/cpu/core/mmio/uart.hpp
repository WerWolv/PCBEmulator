#pragma once

#include <devices/cpu/core/mmio/device.hpp>

#include <numeric>

namespace vc::dev::cpu::mmio {

    class UART : public MMIODevice {
    public:
        UART(u64 base) : MMIODevice("UART", base, sizeof(registers)) {

        }

        [[nodiscard]]
        u8& byte(u64 offset) noexcept override {
            this->valueChanged = true;
            return *(reinterpret_cast<u8*>(&this->registers) + offset);
        }

        [[nodiscard]]
        u16& halfWord(u64 offset) noexcept override {
            this->valueChanged = true;
            return *reinterpret_cast<u16*>((reinterpret_cast<u8*>(&this->registers) + offset));
        }

        [[nodiscard]]
        u32& word(u64 offset) noexcept override {
            this->valueChanged = true;
            return *reinterpret_cast<u32*>((reinterpret_cast<u8*>(&this->registers) + offset));
        }

        [[nodiscard]]
        u64& doubleWord(u64 offset) noexcept override {
            this->valueChanged = true;
            return *reinterpret_cast<u64*>((reinterpret_cast<u8*>(&this->registers) + offset));
        }

        cpu::IOPin txPin;

    private:

        void tick() noexcept override {
            txPin.setValue(static_cast<u8>(registers.TX));
            registers.TX = 0x00;
            this->valueChanged = false;
        }

        bool needsUpdate() noexcept override {
            return this->valueChanged;
        }

        struct {
            u32 CR;
            u32 TX;
            u32 RX;
        } registers;

        bool valueChanged = false;
    };

}
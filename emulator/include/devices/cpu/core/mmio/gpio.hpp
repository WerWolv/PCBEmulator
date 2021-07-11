#pragma once

#include <devices/cpu/core/mmio/device.hpp>

#include <numeric>

namespace vc::dev::cpu::mmio {

    class GPIO : public MMIODevice {
    public:
        GPIO(u64 base) : MMIODevice("GPIO", base, sizeof(registers)) {
            registers = { 0 };
        }

        [[nodiscard]]
        u8& byte(u64 offset) noexcept override {
            return *(reinterpret_cast<u8*>(&this->registers) + offset);
        }

        [[nodiscard]]
        u16& halfWord(u64 offset) noexcept override {
            return *reinterpret_cast<u16*>((reinterpret_cast<u8*>(&this->registers) + offset));
        }

        [[nodiscard]]
        u32& word(u64 offset) noexcept override {
            return *reinterpret_cast<u32*>((reinterpret_cast<u8*>(&this->registers) + offset));
        }

        [[nodiscard]]
        u64& doubleWord(u64 offset) noexcept override {
            return *reinterpret_cast<u64*>((reinterpret_cast<u8*>(&this->registers) + offset));
        }

        std::array<cpu::IOPin, 8> gpioPins;

    private:

        void tick() noexcept override {
            u32 offset = 0;
            registers.IN = 0x00;
            for (auto &pin : gpioPins) {
                if (registers.CR & (0b01 << offset)) /* Output */{
                    pin.setValue((static_cast<u8>(registers.OUT) & (0b01 << offset)) == 0x00 ? 0 : 1);
                } else /* Input */ {
                    if (pin.hasValue())
                        registers.IN |= (pin.getValue().value() == 0x00 ? 0 : 1) << offset;
                }

                offset++;
            }
        }

        bool needsUpdate() noexcept override {
            return true;
        }

        struct {
            u32 CR;
            u32 IN;
            u32 OUT;
        } registers;
    };

}
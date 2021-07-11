#pragma once

#include <devices/cpu/core/io_pin.hpp>

#include <map>

namespace vc::dev::cpu::mmio {

    class MMIODevice {
    public:
        MMIODevice(std::string_view name, u64 base, size_t size) : name(name), base(base), size(size) { }

        constexpr auto operator<=>(const MMIODevice &other) const {
            return this->base <=> other.base;
        }

        [[nodiscard]]
        constexpr auto getBase() const noexcept {
            return this->base;
        }

        [[nodiscard]]
        constexpr auto getEnd() const noexcept {
            return this->base + this->size - 1;
        }

        [[nodiscard]]
        constexpr auto getSize() const noexcept {
            return this->size;
        }

        virtual void doTick() noexcept final {
            if (needsUpdate()) {
                this->tick();
            }
        }

        [[nodiscard]]
        virtual u8& byte(u64 offset) noexcept = 0;

        [[nodiscard]]
        virtual u16& halfWord(u64 offset) noexcept = 0;

        [[nodiscard]]
        virtual u32& word(u64 offset) noexcept = 0;

        [[nodiscard]]
        virtual u64& doubleWord(u64 offset) noexcept = 0;

        virtual bool needsUpdate() noexcept { return false; }

        [[nodiscard]]
        std::string_view getName() const {
           return this->name;
        }
    protected:
        virtual void tick() noexcept { }

    private:
        std::string name;
        u64 base;
        u64 size;
    };

}
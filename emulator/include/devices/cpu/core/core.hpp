#pragma once

#include <risc.hpp>
#include <devices/cpu/core/instructions.hpp>
#include <devices/cpu/core/registers.hpp>
#include <devices/cpu/core/address_space.hpp>

#include <thread>
#include <chrono>

namespace vc::dev::cpu {

    class Core {
    public:
        explicit Core(AddressSpace &addressSpace) : addressSpace(addressSpace) { }

        void execute();

        [[nodiscard]]
        bool isHalted() const { return halted; }

        void reset() {
            this->regs.pc = 0x00;
            for (u8 r = 1; r < 32; r++)
                this->regs.x[r] = 0x00;
            this->halted = false;
        }

        void halt(std::string_view message = "", auto ... params) {
            if (!message.empty())
                log::fatal(message, params...);

            log::fatal("Halted CPU Core at {:#x}", regs.pc);
            using namespace std::literals::chrono_literals;
            std::this_thread::sleep_for(200ms);
            this->halted = true;
        }

    private:
        constexpr void executeCompressedInstruction(const CompressedInstruction &instr);
        constexpr void executeInstruction(const Instruction &instr);

        constexpr void executeOPInstruction(const Instruction &instr);
        constexpr void executeOPIMMInstruction(const Instruction &instr);
        constexpr void executeOPIMM32Instruction(const Instruction &instr);
        constexpr void executeBRANCHInstruction(const Instruction &instr);
        constexpr void executeLOADInstruction(const Instruction &instr);
        constexpr void executeSTOREInstruction(const Instruction &instr);

        constexpr void executeC0Instruction(const CompressedInstruction &instr);
        constexpr void executeC1Instruction(const CompressedInstruction &instr);
        constexpr void executeC2Instruction(const CompressedInstruction &instr);

        u64 nextPC;
        bool halted = true;
        AddressSpace &addressSpace;
        Registers regs;
    };

}
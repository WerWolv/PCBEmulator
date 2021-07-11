#include <devices/cpu/core/core.hpp>
#include <utils.hpp>

#define INSTRUCTION(category, type, ...) { .category = { .type = { __VA_ARGS__ } } }

#define INSTR_LOG(fmt, ...) log::debug("({:#x}) " fmt, regs.pc, __VA_ARGS__)

namespace vc::dev::cpu {

    [[nodiscard]]
    constexpr u8 getOpcode(const u8 &address) {
        return address & 0b0111'1111;
    }

    void Core::execute() {
        if (this->halted) return;

        auto opcode = getOpcode(this->addressSpace(regs.pc, byte_tag()));

        /* Check if instruction is compressed */
        if ((opcode & 0b11) != 0b11) {
            const auto &instr = reinterpret_cast<CompressedInstruction&>(this->addressSpace(regs.pc, hword_tag()));

            this->nextPC = regs.pc + CompressedInstructionSize;
            executeCompressedInstruction(instr);
        } else {
            const auto &instr = reinterpret_cast<Instruction&>(this->addressSpace(regs.pc, word_tag()));

            this->nextPC = regs.pc + InstructionSize;
            executeInstruction(instr);
        }

        addressSpace.tickDevices();

        regs.pc = this->nextPC;
    }

    constexpr void Core::executeInstruction(const Instruction &instr) {
        switch (instr.getOpcode()) {
            case Opcode::OP_IMM:
                executeOPIMMInstruction(instr);
                break;
            case Opcode::OP_IMM32:
                executeOPIMM32Instruction(instr);
                break;
            case Opcode::STORE:
                executeSTOREInstruction(instr);
                break;
            case Opcode::LOAD:
                executeLOADInstruction(instr);
                break;
            case Opcode::AUIPC:
            {
                auto &i = instr.Base.U;
                INSTR_LOG("AUIPC x{}, #{:#x}", i.rd, i.getImmediate());
                regs.x[i.rd] = regs.pc + i.getImmediate();
                break;
            }
            case Opcode::JAL:
            {
                auto &i = instr.Immediate.J;
                INSTR_LOG("JAL #{:#x}", regs.pc + (util::signExtend<20, i64>(i.getImmediate()) * 2));

                auto link = this->nextPC;
                this->nextPC = regs.pc + util::signExtend<20, i64>(i.getImmediate()) * 2;
                regs.x[i.rd] = link;

                break;
            }
            case Opcode::JALR:
            {
                auto &i = instr.Base.I;
                INSTR_LOG("JALR x{}, x{}, #{:#x}", i.rd, i.rs1, util::signExtend<12, i64>(i.getImmediate()));

                auto link = this->nextPC;
                this->nextPC = (util::signExtend<12, i64>(i.getImmediate()) + regs.x[i.rs1]) & u64(~0b1);
                regs.x[i.rd] = link;

                break;
            }
            case Opcode::BRANCH:
                executeBRANCHInstruction(instr);
                break;
            case Opcode::LUI:
            {
                auto &i = instr.Base.U;
                INSTR_LOG("LUI x{}, #{:#x}", i.rd, util::signExtend<20, i64>(i.getImmediate()));
                regs.x[i.rd] = util::signExtend<12, u64>(i.getImmediate());
                break;
            }

            default: this->halt("Invalid instruction {:x}", instr.getOpcode());
        }
    }

    constexpr void Core::executeOPInstruction(const Instruction &instr) {
        const auto &i = instr.Base.R;

        #define IS_FUNC(instruction, type) (instruction.funct3 == instr_t(OPFunc3::type) && instruction.funct7 == instr_t(OPFunc7::type))

        if (IS_FUNC(i, ADD)) {
            regs.x[i.rd] = regs.x[i.rs1] + regs.x[i.rs2];
        } else {
            this->halt("Invalid OP function {:x} {:x}", i.funct3, i.funct7);
        }

        #undef IS_FUNC
    }

    constexpr void Core::executeOPIMMInstruction(const Instruction &instr) {
        const auto &i = instr.Base.I;

        switch (static_cast<OPIMMFunc>(instr.getFunction3())) {
            case OPIMMFunc::XORI:
            {
                INSTR_LOG("XORI x{}, x{}, #{:#x}", i.rd, i.rs1, util::signExtend<12, i64>(i.getImmediate()));
                regs.x[i.rd] = regs.x[i.rs1] ^ util::signExtend<12, i64>(i.getImmediate());
                break;
            }
            case OPIMMFunc::ORI:
            {
                INSTR_LOG("ORI x{}, x{}, #{:#x}", i.rd, i.rs1, util::signExtend<12, i64>(i.getImmediate()));
                regs.x[i.rd] = regs.x[i.rs1] | util::signExtend<12, i64>(i.getImmediate());
                break;
            }
            case OPIMMFunc::ADDI:
            {
                INSTR_LOG("ADDI x{}, x{}, #{:#x}", i.rd, i.rs1, util::signExtend<12, i64>(i.getImmediate()));
                regs.x[i.rd] = regs.x[i.rs1] + util::signExtend<12, i64>(i.getImmediate());
                break;
            }
            case OPIMMFunc::ANDI:
            {
                INSTR_LOG("ANDI x{}, x{}, #{:#x}", i.rd, i.rs1, util::signExtend<12, i64>(i.getImmediate()));
                regs.x[i.rd] = regs.x[i.rs1] & util::signExtend<12, i64>(i.getImmediate());
                break;
            }
            default: this->halt("Invalid OPIMM function {:x}", instr.getFunction3());
        }
    }

    constexpr void Core::executeOPIMM32Instruction(const Instruction &instr) {
        const auto &i = instr.Base.I;

        switch (static_cast<OPIMM32Func>(instr.getFunction3())) {
            case OPIMM32Func::ADDIW:
            {
                INSTR_LOG("ADDIW x{}, x{}, #{:#x}", i.rd, i.rs1, util::signExtend<12, i32>(i.getImmediate()));
                regs.x[i.rd] = util::signExtend<32, i64>((util::signExtend<12, i32>(i.getImmediate()) + regs.x[i.rs1]) & 0xFFFF'FFFF);
                break;
            }
            default: this->halt("Invalid OPIMM32 function {:x}", instr.getFunction3());
        }
    }

    constexpr void Core::executeBRANCHInstruction(const Instruction &instr) {
        const auto &i = instr.Immediate.B;

        switch (static_cast<BRANCHFunc>(instr.getFunction3())) {
            case BRANCHFunc::BEQ:
            {
                INSTR_LOG("BEQ x{}, x{}, #{:#x}", i.rs1, i.rs2, regs.pc + (util::signExtend<20, i64>(i.getImmediate()) * 2));
                if (regs.x[i.rs1] == regs.x[i.rs2]) {
                    this->nextPC = regs.pc + util::signExtend<20, i64>(i.getImmediate()) * 2;
                }

                break;
            }
            case BRANCHFunc::BNE:
            {
                INSTR_LOG("BNE x{}, x{}, #{:#x}", i.rs1, i.rs2, regs.pc + (util::signExtend<20, i64>(i.getImmediate()) * 2));
                if (regs.x[i.rs1] != regs.x[i.rs2]) {
                    this->nextPC = regs.pc + util::signExtend<20, i64>(i.getImmediate()) * 2;
                }

                break;
            }
            default: this->halt("Invalid BRANCH function {:x}", instr.getFunction3());
        }
    }

    constexpr void Core::executeLOADInstruction(const Instruction &instr) {
        const auto &i = instr.Base.I;

        switch (static_cast<LOADFunc>(instr.getFunction3())) {
            case LOADFunc::LB:
            {
                INSTR_LOG("LB x{}, #{:#x}(x{})", i.rd, util::signExtend<12, i32>(i.getImmediate()), i.rs1);
                regs.x[i.rd] = addressSpace(regs.x[i.rs1] + util::signExtend<12, i32>(i.getImmediate()), byte_tag{});
                break;
            }
            case LOADFunc::LD:
            {
                INSTR_LOG("LD x{}, #{:#x}(x{})", i.rd, util::signExtend<12, i32>(i.getImmediate()), i.rs1);
                regs.x[i.rd] = addressSpace(regs.x[i.rs1] + util::signExtend<12, i32>(i.getImmediate()), dword_tag{});
                break;
            }
            case LOADFunc::LBU:
            {
                INSTR_LOG("LBU x{}, #{:#x}(x{})", i.rd, i.getImmediate(), i.rs1);
                regs.x[i.rd] = addressSpace(regs.x[i.rs1] + i.getImmediate(), byte_tag{});
                break;
            }
            default: this->halt("Invalid LOAD function {:x}", instr.getFunction3());
        }
    }

    constexpr void Core::executeSTOREInstruction(const Instruction &instr) {
        const auto &i = instr.Base.S;

        switch (static_cast<STOREFunc>(instr.getFunction3())) {
            case STOREFunc::SB:
            {
                INSTR_LOG("SB x{}, #{:#x}(x{})", i.rs2, util::signExtend<12, i32>(i.getImmediate()), i.rs1);
                addressSpace(util::signExtend<12, i64>(i.getImmediate()) + regs.x[i.rs1], byte_tag{}) = regs.x[i.rs2];
                break;
            }
            case STOREFunc::SH:
            {
                INSTR_LOG("SH x{}, #{:#x}(x{})", i.rs2, util::signExtend<12, i32>(i.getImmediate()), i.rs1);
                addressSpace(util::signExtend<12, i64>(i.getImmediate()) + regs.x[i.rs1], hword_tag{}) = regs.x[i.rs2];
                break;
            }
            case STOREFunc::SW:
            {
                INSTR_LOG("SW x{}, #{:#x}(x{})", i.rs2, util::signExtend<12, i32>(i.getImmediate()), i.rs1);
                addressSpace(util::signExtend<12, i64>(i.getImmediate()) + regs.x[i.rs1], word_tag{}) = regs.x[i.rs2];
                break;
            }
            case STOREFunc::SD:
            {
                INSTR_LOG("SD x{}, #{:#x}(x{})", i.rs2, util::signExtend<12, i32>(i.getImmediate()), i.rs1);
                addressSpace(util::signExtend<12, i64>(i.getImmediate()) + regs.x[i.rs1], dword_tag{}) = regs.x[i.rs2];
                break;
            }
            default: this->halt("Invalid STORE function {:x}", instr.getFunction3());
        }
    }


    /* Compressed instructions */

    constexpr void Core::executeCompressedInstruction(const CompressedInstruction &instr) {
        switch (instr.getOpcode()) {
            case CompressedOpcode::C0:
                executeC0Instruction(instr);
                break;
            case CompressedOpcode::C1:
                executeC1Instruction(instr);
                break;
            case CompressedOpcode::C2:
                executeC2Instruction(instr);
                break;
            default: this->halt("Unknown compressed opcode {:x}!", instr.getOpcode());
        }
    }

    constexpr void Core::executeC0Instruction(const CompressedInstruction &instr) {
        Instruction expanded = { 0 };
        switch (static_cast<C0Funct>(instr.getFunction3())) {
            case C0Funct::C_ADDI4SPN:
            {
                auto &i = instr.CIW;

                if (i.imm == 0)
                    this->halt("Illegal instruction at {:#x}!", regs.pc);

                expanded = INSTRUCTION(Base, I, .opcode = instr_t(Opcode::OP_IMM), .rd = instr_t(i.rd + 8), .funct3 = instr_t(OPIMMFunc::ADDI), .rs1 = 2);
                expanded.Base.I.setImmediate(i.imm / 4);
                break;
            }
            default: this->halt("Invalid C0 function {:x}", instr.getFunction3());
        }

        executeInstruction(expanded);
    }

    constexpr void Core::executeC1Instruction(const CompressedInstruction &instr) {
        Instruction expanded = { 0 };
        switch (static_cast<C1Funct>(instr.getFunction3())) {
            case C1Funct::C_ADDI:
            {
                auto &i = instr.CI;
                expanded = INSTRUCTION(Base, I, .opcode = instr_t(Opcode::OP_IMM), .rd = i.rd, .funct3 = instr_t(OPIMMFunc::ADDI), .rs1 = i.rd);
                expanded.Base.I.setImmediate(util::signExtend<6, i32>((i.imm3 << 5) | (i.imm2 << 3) | (i.imm1)));
                break;
            }
            case C1Funct::C_ADDIW:
            {
                auto &i = instr.CI;
                expanded = INSTRUCTION(Base, I, .opcode = instr_t(Opcode::OP_IMM32), .rd = i.rd, .funct3 = instr_t(OPIMM32Func::ADDIW), .rs1 = i.rd);
                expanded.Base.I.setImmediate(util::signExtend<6, i32>((i.imm3 << 5) | (i.imm2 << 3) | (i.imm1)));
                break;
            }
            case C1Funct::C_LI:
            {
                auto &i = instr.CI;
                expanded = INSTRUCTION(Base, I, .opcode = instr_t(Opcode::OP_IMM), .rd = i.rd, .funct3 = instr_t(OPIMMFunc::ADDI), .rs1 = 0);
                expanded.Base.I.setImmediate(util::signExtend<6, u32>((i.imm3 << 5) | (i.imm2 << 3) | (i.imm1)));
                break;
            }
            case C1Funct::C_LUI:
            {
                auto &i = instr.CI;
                expanded = INSTRUCTION(Base, I, .opcode = instr_t(Opcode::OP_IMM), .rd = i.rd, .funct3 = instr_t(OPIMMFunc::ADDI), .rs1 = i.rd);
                expanded.Base.I.setImmediate(util::signExtend<9, i32>((i.imm3 << 9) | ((i.imm1 >> 1) << 7) | ((i.imm2 & 0b01) << 6) | ((i.imm1 & 0b001) << 5) | (((i.imm1 & 0b010) >> 1) << 4)));
                break;
            }
            case C1Funct::C_ANDI:
            {
                auto &i = instr.CI;
                expanded = INSTRUCTION(Base, I, .opcode = instr_t(Opcode::OP_IMM), .rd = i.rd, .funct3 = instr_t(OPIMMFunc::ANDI), .rs1 = i.rd);
                expanded.Base.I.setImmediate(util::signExtend<6, i32>((i.imm3 << 5) | (i.imm2 << 3) | (i.imm1)));
                break;
            }
            default: this->halt("Invalid C1 function {:x}", instr.getFunction3());
        }
        executeInstruction(expanded);
    }

    constexpr void Core::executeC2Instruction(const CompressedInstruction &instr) {
        Instruction expanded = { 0 };
        switch (static_cast<C2Funct>(instr.getFunction3())) {
            case C2Funct::C_JUMP:
            {
                auto &i = instr.CR;

                if (i.rd != 0 && i.funct4 == 0b1000 && i.rs2 != 0) /* C.MV */ {
                    expanded = INSTRUCTION(Base, R, .opcode = instr_t(Opcode::LOAD), .rd = i.rd, .funct3 = instr_t(OPFunc3::ADD), .rs1 = 0, .rs2 = i.rs2, .funct7 = instr_t(OPFunc7::ADD));
                } else if (i.rd != 0 && i.funct4 == 0b1000 && i.rs2 == 0) /* C.JR */ {
                    INSTR_LOG("C.JR x{}, x{}, #{:#x}", 0, i.rd, 0);
                    this->nextPC = regs.x[i.rd];
                    return;
                } else {
                    this->halt("Invalid C2 C_JUMP function {:x}", instr.getFunction4());
                }

                break;
            }
            case C2Funct::C_LDSP:
            {
                auto &i = instr.CI;
                expanded = INSTRUCTION(Base, I, .opcode = instr_t(Opcode::LOAD), .rd = i.rd, .funct3 = instr_t(LOADFunc::LD), .rs1 = 2);
                expanded.Base.I.setImmediate((i.imm1 << 6) | (i.imm3 << 5) | (i.imm2 << 3));
                break;
            }
            case C2Funct::C_SDSP:
            {
                auto &i = instr.CSS;
                expanded = INSTRUCTION(Base, S, .opcode = instr_t(Opcode::STORE), .funct3 = instr_t(STOREFunc::SD), .rs1 = 2, .rs2 = i.rs2);
                expanded.Base.S.setImmediate(i.imm);
                break;
            }
            default: this->halt("Invalid C2 function {:x}", instr.getFunction3());
        }

        executeInstruction(expanded);
    }

}
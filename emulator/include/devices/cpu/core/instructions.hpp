#pragma once

#include <risc.hpp>

#define INSTRUCTION_FORMAT(name, ...) struct { __VA_ARGS__ } name; static_assert(sizeof(name) == InstructionSize, "Instruction Format " #name " is invalid!")
#define COMPRESSED_INSTRUCTION_FORMAT(name, ...) struct { __VA_ARGS__ } name; static_assert(sizeof(name) == CompressedInstructionSize, "Compressed instruction Format " #name " is invalid!")

namespace vc::dev::cpu {

    using instr_t = u32;
    using comp_instr_t = u16;

    constexpr static inline size_t InstructionSize = sizeof(instr_t);
    constexpr static inline size_t CompressedInstructionSize = sizeof(comp_instr_t);

    enum class Opcode : u8 {
        LUI             = 0b0110111,
        AUIPC           = 0b0010111,
        JAL             = 0b1101111,
        JALR            = 0b1100111,
        BRANCH          = 0b1100011,
        LOAD            = 0b0000011,
        STORE           = 0b0100011,
        OP_IMM          = 0b0010011,
        OP_IMM32        = 0b0011011,
        MISC_MEM        = 0b0001111,
        SYSTEM          = 0b1110011,
        OP              = 0b0100011,
        OP_32           = 0b0111011,
        AMO             = 0b0101111,
        LOAD_FP         = 0b0000111,
        STORE_FP        = 0b0100111,
        FMADD           = 0b1000011,
        FMSUB           = 0b1000111,
        FNMADD          = 0b1001111,
        FNMSUB          = 0b1001011,
        OP_FP           = 0b1010011,
    };

    enum class CompressedOpcode : u8 {
        C0              = 0b00,
        C1              = 0b01,
        C2              = 0b10,
    };

    enum class C0Funct : u8 {
        C_ADDI4SPN      = 0b000
    };

    enum class C1Funct : u8 {
        C_ADDI          = 0b000,
        C_ADDIW         = 0b001,
        C_LI            = 0b010,
        C_LUI           = 0b011,
        C_ANDI          = 0b100
    };

    enum class C2Funct : u8 {
        C_JUMP          = 0b100,
        C_LDSP          = 0b011,
        C_SDSP          = 0b111
    };

    enum class OPFunc3 : u8 {
        ADD             = 0b000
    };

    enum class OPFunc7 : u8 {
        ADD             = 0b0000000
    };

    enum class OPIMMFunc : u8 {
        ADDI            = 0b000,
        XORI            = 0b100,
        ORI             = 0b110,
        ANDI            = 0b111
    };

    enum class OPIMM32Func : u8 {
        ADDIW           = 0b000
    };

    enum class STOREFunc : u8 {
        SB              = 0b000,
        SH              = 0b001,
        SW              = 0b010,
        SD              = 0b011,
    };

    enum class LOADFunc : u8 {
        LB              = 0b000,
        LD              = 0b011,
        LBU             = 0b100
    };

    enum class BRANCHFunc : u8 {
        BEQ             = 0b000,
        BNE             = 0b001
    };

    union Instruction {

        union {
            INSTRUCTION_FORMAT(R,
                instr_t opcode    : 7;
                instr_t rd        : 5;
                instr_t funct3    : 3;
                instr_t rs1       : 5;
                instr_t rs2       : 5;
                instr_t funct7    : 7;
            );

            INSTRUCTION_FORMAT(I,
                instr_t opcode    : 7;
                instr_t rd        : 5;
                instr_t funct3    : 3;
                instr_t rs1       : 5;
                instr_t imm0_11   : 12;

                constexpr u32 getImmediate() const { return this->imm0_11; }
                constexpr void setImmediate(u32 value) { this->imm0_11 = value; }
            );

            INSTRUCTION_FORMAT(S,
                instr_t opcode    : 7;
                instr_t imm0_4    : 5;
                instr_t funct3    : 3;
                instr_t rs1       : 5;
                instr_t rs2       : 5;
                instr_t imm5_11   : 7;

                constexpr u32 getImmediate() const { return (this->imm5_11 << 5) | this->imm0_4; }
                constexpr void setImmediate(u32 value) { this->imm0_4 = value & 0b11111; this->imm5_11 = value >> 5; }
            );

            INSTRUCTION_FORMAT(U,
                instr_t opcode    : 7;
                instr_t rd        : 5;
                instr_t imm12_31  : 20;

                constexpr u32 getImmediate() const { return this->imm12_31 << 12; }
                constexpr void setImmediate(u32 value) { this->imm12_31 = value >> 12; }
            );

        } Base;

        union {
            INSTRUCTION_FORMAT(R,
                instr_t opcode    : 7;
                instr_t rd        : 5;
                instr_t funct3    : 3;
                instr_t rs1       : 5;
                instr_t rs2       : 5;
                instr_t funct7    : 7;
            );

            INSTRUCTION_FORMAT(I,
                instr_t opcode    : 7;
                instr_t rd        : 5;
                instr_t funct3    : 3;
                instr_t rs1       : 5;
                instr_t imm0_11   : 12;

                constexpr u32 getImmediate() const { return this->imm0_11; }
            );

            INSTRUCTION_FORMAT(S,
                instr_t opcode    : 7;
                instr_t imm0_4    : 5;
                instr_t funct3    : 3;
                instr_t rs1       : 5;
                instr_t rs2       : 5;
                instr_t imm5_11   : 7;

                constexpr u32 getImmediate() const { return (this->imm5_11 << 5) | this->imm0_4; }
            );

            INSTRUCTION_FORMAT(B,
                instr_t opcode    : 7;
                instr_t imm11     : 1;
                instr_t imm1_4    : 4;
                instr_t funct3    : 3;
                instr_t rs1       : 5;
                instr_t rs2       : 5;
                instr_t imm5_10   : 6;
                instr_t imm12     : 1;

                constexpr u32 getImmediate() const { return ((this->imm12 << 12) | (this->imm11 << 11) | (this->imm5_10 << 5) | (this->imm1_4 << 1)) >> 1; }
            );

            INSTRUCTION_FORMAT(U,
                instr_t opcode    : 7;
                instr_t rd        : 5;
                instr_t imm12_31  : 20;

                constexpr u32 getImmediate() const { return this->imm12_31 << 12; }
            );

            INSTRUCTION_FORMAT(J,
                instr_t opcode    : 7;
                instr_t rd        : 5;
                instr_t imm12_19  : 8;
                instr_t imm11     : 1;
                instr_t imm1_10   : 10;
                instr_t imm20     : 1;

                constexpr u32 getImmediate() const { return ((this->imm20 << 20) | (this->imm12_19 << 12) | (this->imm11 << 11) | (this->imm1_10 << 1)) >> 1; }
            );
        } Immediate;

        [[nodiscard]]
        constexpr Opcode getOpcode() const {
            return static_cast<Opcode>(Base.R.opcode);
        }

        [[nodiscard]]
        constexpr u8 getFunction3() const {
            return Base.R.funct3;
        }
    };
    static_assert(sizeof(Instruction) == InstructionSize, "Instruction union invalid!");

    union CompressedInstruction {

        COMPRESSED_INSTRUCTION_FORMAT(CR,
            comp_instr_t opcode    : 2;
            comp_instr_t rs2       : 5;
            comp_instr_t rd        : 5;
            comp_instr_t funct4    : 4;
        );

        COMPRESSED_INSTRUCTION_FORMAT(CI,
            comp_instr_t opcode    : 2;
            comp_instr_t imm1      : 3;
            comp_instr_t imm2      : 2;
            comp_instr_t rd        : 5;
            comp_instr_t imm3      : 1;
            comp_instr_t funct3    : 3;
        );

        COMPRESSED_INSTRUCTION_FORMAT(CSS,
            comp_instr_t opcode    : 2;
            comp_instr_t rs2       : 5;
            comp_instr_t imm       : 6;
            comp_instr_t funct3    : 3;
        );

        COMPRESSED_INSTRUCTION_FORMAT(CIW,
            comp_instr_t opcode    : 2;
            comp_instr_t rd        : 3;
            comp_instr_t imm       : 8;
            comp_instr_t funct3    : 3;
        );

        COMPRESSED_INSTRUCTION_FORMAT(CL,
            comp_instr_t opcode    : 2;
            comp_instr_t rd        : 3;
            comp_instr_t imm1      : 2;
            comp_instr_t rs1       : 3;
            comp_instr_t imm2      : 3;
            comp_instr_t funct3    : 3;
        );

        COMPRESSED_INSTRUCTION_FORMAT(CS,
            comp_instr_t opcode    : 2;
            comp_instr_t rs2       : 3;
            comp_instr_t imm1      : 2;
            comp_instr_t rs1       : 3;
            comp_instr_t imm2      : 3;
            comp_instr_t funct3    : 3;
        );

        COMPRESSED_INSTRUCTION_FORMAT(CB,
            comp_instr_t opcode    : 2;
            comp_instr_t offset1   : 5;
            comp_instr_t rs1       : 3;
            comp_instr_t offset2   : 3;
            comp_instr_t funct3    : 3;
        );

        COMPRESSED_INSTRUCTION_FORMAT(CJ,
            comp_instr_t opcode    : 2;
            comp_instr_t target    : 11;
            comp_instr_t funct3    : 3;
        );

        [[nodiscard]]
        constexpr CompressedOpcode getOpcode() const {
            return static_cast<CompressedOpcode>(CJ.opcode);
        }

        [[nodiscard]]
        constexpr u8 getFunction3() const {
            return CJ.funct3;
        }

        [[nodiscard]]
        constexpr u8 getFunction4() const {
            return CR.funct4;
        }

    };
}
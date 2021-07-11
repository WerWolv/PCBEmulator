#pragma once

namespace vc::dev::cpu {

    struct Registers {

        struct Register {
            constexpr virtual Register& operator=(u64) = 0;
            constexpr virtual operator u64() = 0;
        };

        struct ZeroRegister : public Register {
            constexpr ZeroRegister& operator=(u64) override { return *this; }
            constexpr operator u64() override { return 0; }
        };

        struct GPRegister : public Register {
            constexpr GPRegister& operator=(u64 v) override { this->value = v; return *this; }
            constexpr operator u64() override { return this->value; }
        private:
            u64 value = 0;
        };

        struct {

            constexpr Register& operator[](u8 index) {
                if (index == 0) {
                    return zeroRegister;
                } else if (index <= 31) {
                    return gpRegisters[index];
                } else {
                    __builtin_unreachable();
                }
            }

        private:
            ZeroRegister zeroRegister;
            GPRegister gpRegisters[32];
        } x;

        GPRegister &zero    = static_cast<GPRegister&>(x[0]);
        GPRegister &ra      = static_cast<GPRegister&>(x[1]);
        GPRegister &sp      = static_cast<GPRegister&>(x[2]);
        GPRegister &gp      = static_cast<GPRegister&>(x[3]);
        GPRegister &tp      = static_cast<GPRegister&>(x[4]);
        GPRegister &t0      = static_cast<GPRegister&>(x[5]);
        GPRegister &t1      = static_cast<GPRegister&>(x[6]);
        GPRegister &t2      = static_cast<GPRegister&>(x[7]);
        GPRegister &fp      = static_cast<GPRegister&>(x[8]);
        GPRegister &s0      = static_cast<GPRegister&>(x[8]);
        GPRegister &s1      = static_cast<GPRegister&>(x[9]);
        GPRegister &a0      = static_cast<GPRegister&>(x[10]);
        GPRegister &a1      = static_cast<GPRegister&>(x[11]);
        GPRegister &a2      = static_cast<GPRegister&>(x[12]);
        GPRegister &a3      = static_cast<GPRegister&>(x[13]);
        GPRegister &a4      = static_cast<GPRegister&>(x[14]);
        GPRegister &a5      = static_cast<GPRegister&>(x[15]);
        GPRegister &a6      = static_cast<GPRegister&>(x[16]);
        GPRegister &a7      = static_cast<GPRegister&>(x[17]);
        GPRegister &s2      = static_cast<GPRegister&>(x[18]);
        GPRegister &s3      = static_cast<GPRegister&>(x[19]);
        GPRegister &s4      = static_cast<GPRegister&>(x[20]);
        GPRegister &s5      = static_cast<GPRegister&>(x[21]);
        GPRegister &s6      = static_cast<GPRegister&>(x[22]);
        GPRegister &s7      = static_cast<GPRegister&>(x[23]);
        GPRegister &s8      = static_cast<GPRegister&>(x[24]);
        GPRegister &s9      = static_cast<GPRegister&>(x[25]);
        GPRegister &s10     = static_cast<GPRegister&>(x[26]);
        GPRegister &s11     = static_cast<GPRegister&>(x[27]);
        GPRegister &t3      = static_cast<GPRegister&>(x[28]);
        GPRegister &t4      = static_cast<GPRegister&>(x[29]);
        GPRegister &t5      = static_cast<GPRegister&>(x[30]);
        GPRegister &t6      = static_cast<GPRegister&>(x[31]);

        u64 pc = 0;
    };

}
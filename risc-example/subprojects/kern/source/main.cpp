#include <types.hpp>

void print(const char* string) {
    char *UARTA_TX = reinterpret_cast<char*>(0x5000'0004);

    for (const char* s = string; *s != 0x00; s++)
        *UARTA_TX = *s;
}

int main() {
    print("Hello RISC-V!\n");

    volatile u8 *GPIOA_CR = reinterpret_cast<volatile u8*>(0x6000'0000);
    volatile u8 *GPIOA_IN = reinterpret_cast<volatile u8*>(0x6000'0004);
    volatile u8 *GPIOA_OUT = reinterpret_cast<volatile u8*>(0x6000'0008);

    *GPIOA_CR = 0b10;
    while (true) {
        if (*GPIOA_IN == 0b01)
            *GPIOA_OUT = 0b10;
        else
            *GPIOA_OUT = 0b00;
    }
}

[[gnu::section(".crt0")]]
[[gnu::naked]]
extern "C" void _start() {
    asm("lui sp, 0x10020");
    asm("tail main");
} 
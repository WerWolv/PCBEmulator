# PCB Emulator

A simple C++20 PCB Emulator

## Components

- Multi-core RISC-V controller
- Button
- LED
- Pin Header

## Example

The following GIF shows the interaction between a button, and LED and a UART Pin Header with a RISC-V CPU on a custom PCB.
![](https://i.imgur.com/Jm9cIdq.gif)

The RISC-V controller is currently running the following code.
```cpp
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
```

The controller used on the TestBoard PCB has the following memory mappings: 
- `1 MiB Flash Memory` at `0x0000'0000`
- `2 MiB RAM Memory` at `0x1000'0000`
- `UART-A` at `0x5000'0000`
- `GPIO-A` at `0x6000'0000`

On-Board peripherals are connected to the following controller Pins:
- `UART-A TX` on Pin 0
- `Button-A` on Pin 1
- `LED-A` on Pin 2
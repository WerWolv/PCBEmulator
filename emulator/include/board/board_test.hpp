#pragma once

#include <board/board.hpp>

#include <devices/cpu/cpu.hpp>
#include <devices/cpu/core/mmio/memory.hpp>
#include <devices/cpu/core/mmio/uart.hpp>
#include <devices/cpu/core/mmio/gpio.hpp>

#include <devices/pin_header.hpp>
#include <devices/button.hpp>
#include <devices/led.hpp>

namespace vc::pcb {

    class TestBoard : public Board {
    public:
        TestBoard() : Board("Test Board", { 500, 300 }),
        cpu(createDevice<dev::CPUDevice>(1, ImVec2{ 50, 50 })),
        uartHeader(createDevice<dev::PinHeader>(ImVec2{ 200, 250 })),
        buttonA(createDevice<dev::Button>(ImVec2({ 300, 250 }))),
        ledA(createDevice<dev::LED>(ImVec2({ 100, 200 }))),

        cpuFlash(0x0000'0000, 1_MiB),
        cpuRam(0x1000'0000, 2_MiB),
        cpuUartA(0x5000'0000),
        cpuGpioA(0x6000'0000) {
            auto &cpuAddressSpace = cpu.getAddressSpace();

            cpu.attachToPin(0, cpuUartA.txPin);
            cpu.attachToPin(1, cpuGpioA.gpioPins[0]);
            cpu.attachToPin(2, cpuGpioA.gpioPins[1]);

            cpuAddressSpace.addDevice(cpuFlash);
            cpuAddressSpace.addDevice(cpuRam);
            cpuAddressSpace.addDevice(cpuUartA);
            cpuAddressSpace.addDevice(cpuGpioA);

            cpuAddressSpace.loadELF("kernel.elf");

            this->createTrack(Direction::MOSI, "uarta_tx", cpu, uartHeader, true);
            this->createTrack(Direction::MISO, "buttona", cpu, buttonA);
            this->createTrack(Direction::MOSI, "leda", cpu, ledA);
            cpu.attachPinToTrack(0, "uarta_tx");
            cpu.attachPinToTrack(1, "buttona");
            cpu.attachPinToTrack(2, "leda");
        }

        dev::cpu::mmio::Memory cpuFlash;
        dev::cpu::mmio::Memory cpuRam;
        dev::cpu::mmio::UART cpuUartA;
        dev::cpu::mmio::GPIO cpuGpioA;

        dev::CPUDevice &cpu;
        dev::PinHeader &uartHeader;
        dev::Button &buttonA;
        dev::LED &ledA;
    };

}
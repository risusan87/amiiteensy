# AMIITeensy
Teensy 4.0 project which uses RFID-RC522 for Amiibo I/O purposes.</br>
Code written in bare-metal C.

## Compiling
Install GNU toolchain. i.MX RT family chip requires arm-none-eabi-gcc for compile.</br>
https://developer.arm.com/downloads/-/gnu-rm

Install teensy_loader_cli. Repository is available at:</br>
https://github.com/PaulStoffregen/teensy_loader_cli

Connect teensy 4.0 board via USB cable and simply run `make flash`.

## References/Credits
Low-level specification is referred from IMXRT1060 manual.</br>
https://www.pjrc.com/teensy/IMXRT1060RM_rev3_annotations.pdf

Bare-metal implementation example is referred from repository by Raphael Stäbler.</br>
https://github.com/blazer82/baremetal-blinky.teensy/tree/master?tab=readme-ov-file

Linker and startups are referred from repository by core/teensy4.</br>
https://github.com/PaulStoffregen/cores

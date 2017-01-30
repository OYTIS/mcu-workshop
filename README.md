# MCU workshop

## Prerequisites
For this workshop you will need a Linux machine with GNU make installed. Additionally you will need [Linaro toolchain](https://releases.linaro.org/components/toolchain/gcc-linaro/5.3-2016.05/gcc-linaro-5.3-2016.05.tar.xz), [OpenOCD](https://github.com/ntfreak/openocd) and a text editor of your choise.

## Contents
* docs - technical reference manual for MCU and schematics.
* boot - OpenOCD configuration file and a script for gdb.
* code - the project itself.

## Task
You will need to expand main.c template so that one can see all three LEDs blinking. There are several issues to think about:
* What registers are used to control GPIO?
* What is processor clock frequency? How fast can we turn LEDs on and off so that blinking is visible?
* Is GPIO module usable after startup or it needs to be enabled? How can it be enabled?


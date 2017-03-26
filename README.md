# MCU workshop

Here you can find the training materials for the workshop on writing firmware for microcontrollers from scratch. The [workshop](https://www.meetup.com/Embedded-Systems-Meetup-Berlin/events/237214090/) itself took place on 21.03.2017 in EndoCode office in Berlin, but the materials presented here are complete, so you can use it even if you were not attending to it. The target platform for this workshop was [CC3200 LaunchPad](http://www.ti.com/tool/cc3200-launchxl), but reader is encouraged to use any plaform available to him/her (preferrably, but not necessarilly Cortex-M based).

## Prerequisites
For this workshop you will need a Linux machine with GNU make installed. Additionally you will need [Linaro toolchain](https://releases.linaro.org/components/toolchain/binaries/5.3-2016.05/) or [GNU ARM Embedded Toolchain from launchpad.net](https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q3-update), [OpenOCD](https://github.com/ntfreak/openocd) and a text editor of your choise.

It is assumed that you already know the C language and can work with command line. Codecademy has a nice tutorial on the [command line](https://www.codecademy.com/learn/learn-the-command-line) if it is not the case, and a list of training materials on C can be found [here](http://www.iso-9899.info/wiki/Main_Page).

## Contents
* slides - slides and examples from the talk.
* docs - technical reference manual for MCU and schematics.
* boot - OpenOCD configuration file and a script for gdb.
* code - template project for CC3200.

## Quickstart
1. Read through the [slides](slides/blinking.pdf), visiting [examples](slides/examples.md) whenever you see a red "Example" label in the bootom left of a slide.
2. Try to complete the [task](#task) for CC3200 or some other Cortex-M board. In the latter case you will need to set up the template project yourself.
  - First install the toolchain and add the 'bin' directory of the toolchain to your PATH (```export PATH=${PATH}:/path/to/toolchain/bin```).
  - Go to 'code' directory and run 'make'. It should build the template project without giving any errors. 
  - On some platforms gcc/ld binaries may be called differently (i.e. 'arm-none-eabi-gcc' instead of 'arm-eabi-gcc'), you will need to modify the Makefile accordingly in this case.
  - Once you're done with the task, you will want to upload the firmware to the MCU to test it. For CC3200 the best way to do it is to run ```arm-eabi-gdb -x gdbinit ../code/blink.elf``` from the 'boot' directory, provided you have OpenOCD installed and PATH set up properly. Linux kernel might need some help to recognize the on-board usb-jtag, see [this tutorial](https://hackpad.com/Using-the-CC3200-Launchpad-Under-Linux-Rrol11xo7NQ) for the details.
3. Annotated solution for CC3200 can be seen in [code](code) directory on "cc3200" branch.

## Task
You will need to expand main.c template so that one can see all three LEDs blinking. There are several issues to think about:
* What registers are used to control GPIO?
* What is processor clock frequency? How fast can we turn LEDs on and off so that blinking is visible?
* Is GPIO module usable after startup or it needs to be enabled? How can it be enabled?


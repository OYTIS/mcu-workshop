# Example 1: compiler
Go to the *code* directory and run

```
make
make list
```

If your environment is set up correctly (see Prerequsites section in [README](../README.md)) you should get the object files in *code/obj* and listings in *code/lst*. The latter are not used to build the final program, it is just a human-readable version of compiler's output to help the programmer. Let's run

```
objdump -h obj/startup_gcc.o
```

to see the list of sections in the generated file. We can see the list of sections together wuth their attributes. VMA and LMA columns stand for Virtual Memory Address and Loadable Memory Address, which are addresses in memory during runtime and in persistent memory respectively. Now we can see that both addresses are set zero, because it's linker's job to arrange sections in memory.

Open *src/startup_gcc.c* and *lst/startup_gcc.lst* in a text editor of your choice. In the listing we can see pseudo-operations, starting with dot ('.') and real generated code. The latter has two hexadecimal numbers on the left: first is an offset in the section where it is located, second is the actual code generated. Take your time and compare C code and generated assembly for ResetISR and NmiSR. On the bottom you can also see the list of defined and undefined symbols. Among the latter are symbols defined in your linker script (see [example 2](example-2-linker)) and **main** that is defined by programmer in *src/main.c*.

# Example 2: linker
Run

```
objdump -h blink.elf
```

You can see the section list again, now with addresses assigned. So how linker assigns the addresses? Let's take a look at blink.ld.

You can see MEMORY block that decribes available physical memory on the device. We're only using SRAM here, but in most cases we will also need to add ROM, in our case it would be

```
ROM (rx) : ORIGIN = 0x00000000, LENGTH = 0x0008000
```

that is, readable and executable memory, locaded at 0x00000000 in the memory space and 0x00080000 bytes long (see **Memory model** section at p.40 of [the MCU manual](../docs/swru367c.pdf)).

Another important block is SECTIONS. Here we actually arrange sections of our object file in the memories, described in MEMORY block. Let's go through .text section description line-by-line.

    .text :
    {

names the section of *output* file.

      _text = .;

dot ('.') means current position in memory. Every time we add some data to the output section '.' advances by the size of these data. This lines means, "define symbol '\_text' and assign it to the currect position in memory". Such definitions are extremely useful for writable sections, namely *data* and *bss*. They help startup scripts find location it should initialize. You can return to ResetISR in *src/startup_gcc.c* to see how it works.

        KEEP(*(.intvecs))

Asterisk ('\*') is a wildcard, meaning 'every file' in the position before the opening parenthesis. '.intvecs' in the parentheses is the name of a section, so '\*(.intvecs)' means 'place sections named .intvecs from all the object files here'. Finally KEEP modificator forbids the linker to delete the section for the sake of optimization if it doesn't find any links to it from other sections. Naturally, interrupt vectors are here for the hardware, not for the software, so there may well be no references to it from other parts of the source code.

        *(.text*)
        *(.rodata*)
        *(.ARM.extab* .gnu.linkonce.armextab.*)

Three other kinds of sections from the object files constitute executable's text section: .text (or more precisely, all sections whose names begin with .text, notice the wildcard), .rodata (read-only data, constants), .ARM.extab (helper data for exception unwinding) and .gnu.linkonce.armextab (another name for .ARM.extab, google "Vague linkage" if you're curious).

        . = ALIGN(8);

Makes current position (the dot, '.') aligned by 8 bytes.

        _etext = .;

Another symbol marking the end of .text section

    } > SRAM

Finally, the text section is placed into SRAM, i.e. RW memory. Normally, when we have ROM memory we will place our .text there, so that it persists when we switch power off and on, but for debugging purposes we use SRAM and keep wha ROM untouched.

# Example 3: processor manual
Download [Cortex-M0 generic user guide](http://infocenter.arm.com/help/topic/com.arm.doc.dui0497a/DUI0497A_cortex_m0_r0p0_generic_ug.pdf) from ARM infocenter.
Paragraph 2.2 contains a generic memory map (p. 28) for all Cortex-M0 devices. More specific memory map you will find in a manual for specific MCU.
Paragraph 2.3 describes exceptions and interrupts. You will need to read this through if you want to use interrupts on Cortex-M0 devices.
Chapter 3 describes the instruction set architecture (ISA) of Cortex-M0. Take a look at it if you want to read or write assembly code.
In chapter 4 you will find description of internal peripherals in Cortex-M0. They are quite important, you will want to get acquainted with these if you continue working with Cortex-M0.

# Example 4: MCU manual
Open [the MCU manual](../docs/swru367c.pdf). In the table of contents you can see overview, information about the processor (note that it Cortex-M4 not Cortex-M0), and all the other sections describe various peripherals. Processor section can not always be found in documentation on MCU, but this vendor decided to collect all information useful for a programmer in one document. Here we can find a detailed memory map for this MCU (p. 40). Scan through the UART description starting on p. 131. You can find block diagram, functional description ("how it works") and, most importantly, list of registers. Registers are programmer's interface to the on-chip hardware. For example, writing to register UARTDR (p. 140), located at offset 0x000 from address 0x4000C000 will cause UART0 to start transmitting data.

# Example 5: Schematics
Open [the board schematics](../docs/CC3200-LAUNCHXL_SCH_REV4P1-C.pdf). On the first page you can see the processor. We can see that we have 40MHz crystal available as our main external clock and 32768 KHz crystal as our real time clock (hence this weird frequency, it basically means that we have a 2^15 oscillations, which is a "nice" number for computers, every millisecond, so measuring milliseconds, seconds etc. is extremely convenient).
Let's find out how we can control LEDs. You can see three controllable LEDs, D5, D6 and D7 in the lower right section of p. 4. There are two uncontrollable as well: D4 is on whenever the power is on, D1 indicates the RESET switch. Current through LEDs D5-7 is switched by transistors controlled by lines CC_GPIO_11, CC_GPIO_10 and CC_GPIO_09 connected directly to respective GPIO pins of the MCU. So once we know (from MCU manual) how to control GPIO 09-11, we can blink our LEDs.

# Example 6: registers
Assume that we want to send data to serial channel. Let's imagine we've performed all necessary initialization and everything that is left is give the actual byte to transmit to the hardware. Go to [MCU manual](../docs/swru367c.pdf) again and open page 40: you will find in the memory map that first UART is located at address 0x4000C000. Having studied UART section in the manual we will find that to transmit a byte we should write it to UARTDR register (p. 140) located at offset 0x000 (p. 138). We could write the function that does this as following:

```
void send_byte(uint8_t data) {
	*((volatile uint32_t*)(0x4000C000 + 0x000)) = data;
}
```

In the inmost paretheses we have the address of UART0DR, to the left is cast operator: this address should be interpreted as a pointer to 32-bit variable (because, as we can see on p. 140 UARTDR is 32 bits wide). Its content is volatile, i.e. accesses to it should never be optimized. Why is it important? Consider the following code:

```
send_byte(0x55);
send_byte(0x55);
```

What we want to do here is to transmit 0x55 over the serial line twice, which is perfectly legal. But without 'volatile' specifier smart enough compiler may decide that there is no reason to write the same value to the same variable twice and will 'optimize' the second send_byte out.

# Example 7: headers
Take a look at *code/inc*. CC3200 doesn't provide a single header, which would have been to massive. Otherwise every module has its own header. There are two common headers as well:
  * hw_types.h contains HWREG macros which is a wrapper to a register address similar to what we've implemented in [example 6](example-6-registers). HWREGH and HWREGB are similar macros for 16-bit and 8-bit registers. The other three macros allow for accessing individual bits in registers through bit-band memory (see section 2.2.3.1 on p.41 of MCU manual if you're feeling curious).
  * hw_memmap.h contains base addresses for all hardware modules. Register definitions in module-related headers (e.g. hw_gpio.h, hw_uart.h erc.) are offsets to base addresses defined here.

# Example 8: startup
Return to *code/src/startup_gcc.c*. It contains vector table (section 2.2.4.4 on p. 46) definition starting from line 94. It is forcedly put to section ".intvecs", which, as we can see in *code/blink.ld* is placed in the beginning of .text section. If the text section was placed in ROM, where it usually belongs, starting from address 0x00000000, interrupt vectors would be located directly at address 0x00000000, where the hardware expects it to be. Since we're working in SRAM, we need to make some manual preparations after loading the code (see *boot/gdbinit*, where program counter and stack pointer are set manually. If you want to use interrups, you should also expand ResetISR procedure to set VTABLE (p. 79) register properly.

Go to line 214, where **ResetISR** is defined. It is located at offset 4 from the beginning of vector table and is called whenever the hardware starts up (or whenever we're loading the code with gdbinit in our case), so the very first initialization code is located here. This ResetISR implementation is pretty simple, it just copies the initialization data to RAM (which totally makes sense if we start up from ROM after power-on) and sets to zero all unitialized global variables.

Other functions found in *startup_gcc.c* are core fault interrupts, which just put processor into infinite loop and **_sbrk** function used by **malloc** from standard library.

# Example 9: drivers, provided by vendor
Since our goal is to learn how to write firmware from scratch, drivers from vendor are not included in the repo. If feeling curious, please download [CC3200 SDK](http://www.ti.com/tool/cc3200sdk) from the TI website and explore *driverlib* directory inside the installed package.

# Example A: makefile
Open *code/Makefile*. Makefile defines a set of **rules** each of which has a form of

```
target: prerequisites
	recipe
```

where **target** is a name of a file (like "blink.elf", "obj/main.o" etc.) or just a word (like "clean" or "list"), **prerequisites** are targets/files that should be ready before make will start working on **target** and **recipe** is shell-code describing how to make **target** once all **prerequisites** are fulfilled. As you can see, apart from **rules** you can also define **variables** which can be substituted in any part of a **rule**.

Here we can see that in order to build "blink.elf" (TARGET) we need to have all object files in place and then link them all with arm-eabi-ld. Object files in turn require source files and are build with arm-eabi-gcc.

To use the make file, go to the directory where it's located and run 'make <target>' (i.e. make all; make clean etc.). Running 'make' without any argument will make the first target ('all' in our case) build.

Of course it is not intended to be a tutorial on GNU make, to get more info, see [official documentation](https://www.gnu.org/software/make/manual/) of [google](https://google.com) for tutorials.


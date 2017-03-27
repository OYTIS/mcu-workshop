/* Defines locations in memory of all subsystems */
#include "hw_memmap.h"

/* Defines important macros, HWREG (see below for details)*/
#include "hw_types.h"

/* Registers of GPIO subsystem */
#include "hw_gpio.h"

/* Reset-clock management for peripherals, needed for clock-gating (see 
 *   initialization below).*/
#include "hw_apps_rcm.h"

/* Open core protocol subsystem. Not much info about it is given in the MCU
 *   docs, but GPIO_PAD_CONFIG_n registeres, used during initialization
 *   are described it this header.*/
#include "hw_ocp_shared.h"

/* Integer types of known withds: (u)int8_t, (u)int16_t, (u)int32_t are defined
 *   here. Unlike standard types: char, int etc., which sizes are platform-
 *   dependent, these are guaranteed to be exactly 8, 16 and 32 bit wide
 *   respectively. Nearly every embedded project will use these types, and
 *   today they are often preferred over standard ones in application
 *   programming as well.*/
#include <stdint.h>

/* Here we define a set of registers that are used by other program. You can
 *   find definition of HWREG macro in "hw_types.h". Basically HWREG(addr)
 *   means "volatile variable located at address addr". Every address here is
 *   composed of subsystem base address defined in "hw_memmap.h" and register
 *   offset defined in respective subsystem header. */
#define GPIO1_DIR HWREG(GPIOA1_BASE | GPIO_O_GPIO_DIR)
#define RCM_GPIO1CLKEN HWREG(ARCM_BASE | APPS_RCM_O_GPIO_B_CLK_GATING)
#define GPIO_PAD_CONFIG_09 HWREG(OCP_SHARED_BASE | OCP_SHARED_O_GPIO_PAD_CONFIG_9)
#define GPIO_PAD_CONFIG_10 HWREG(OCP_SHARED_BASE | OCP_SHARED_O_GPIO_PAD_CONFIG_10)
#define GPIO_PAD_CONFIG_11 HWREG(OCP_SHARED_BASE | OCP_SHARED_O_GPIO_PAD_CONFIG_11)

/* So, first of all, we want to know what GPIOs we want to control.
 *   From the schematics we can see that the LEDs are controlled by GPIO #9,
 *   #10 and #11. But looking in the MCU manual we can see that internally we
 *   have four 8-bit GPIO ports. How do they correspond to pin numbering?
 *   In the very end on GPIO section, on page 129 we can see the answer: GPIO
 *   09-11 correspond to bits 1-3 of GPIOA1. So let's define a mask for
 *   explicitness: a more succint way would be to write ALL_LEDS_MASK as 0x0e*/
#define ALL_LEDS_MASK ((1<<1) | (1<<2) | (1<<3))

/* This function will turn all LEDs on if "on" parameter in non-zero and off
 *   otherwise */
void all_on_off(int on)
{
	/* Read carefully section 5.2.1.2 of functional description of GPIO module.
	 *   CC3200 hardware allows software to modify individual bits using masking.
	 *   The mask is encoded in the address, that is why we couldn't create register
	 *   define for DATA.
	 *   So let's take a closer look at the code. HWREG as you remember is a macro
	 *   that interprets it's argument as an address. GPIOA1_BASE | GPIO_O_GPIO_DATA
	 *   is an address of DATA register of GPIOA1. According to 5.2.1.2 the mask
	 *   (shifted by 2 to the right) is added to the register address, but as the
	 *   address is alligned by 0x400 it can as well be or'ed. So we use a mask
	 *   to access all three LEDs and set them all to '1' or to '0' depending on
	 *   the function argument */
	HWREG(GPIOA1_BASE | GPIO_O_GPIO_DATA | (ALL_LEDS_MASK << 2)) = (on) ? ALL_LEDS_MASK : 0;
}


void main()
{
	uint32_t i;
	int led = 0;

	/* Let's read section 5.4 on initialization. First we need to define clocking
	 *   to the GPIO block. Without clocking the GPIO hardware will be inactive,
	 *   so it makes no sense to write to other GPIO-related registers before the
	 *   clock is enabled. We've got 5 GPIOCLKEN registers listed for 4 GPIO
	 *   modules. Moreover, if we go for example to GPIO1CLKEN (p. 436) the
	 *   register description will say that it controls GPIO_B, while in GPIO
	 *   section only GPIOA0, GPIOA1, GPIOA2 and GPIOA3 are described. It is not
	 *   uncommon to see such inconsistencies in the documentation, since every
	 *   module is basically a separate product with different engineers working
	 *   on them at different time and even in different companies. Two guesses
	 *   seem equally reasonable: one that GPIO0CLKEN, which is said to control
	 *   GPIO_A switches clock to all GPIOA blocks, the other would be that 
	 *   GPIO0CLKEN controls GPIOA0, GPIO1CLKEN controls GPIOA1 etc. By experiment
	 *   we find that the second guess is right, we need to enable GPIO1CLK. */
	RCM_GPIO1CLKEN = 0x00000001;

	/* Now we have clock on and can write to GPIO registers. By default all pins
	 *   are configured as inputs, let's make them outputs.*/
	GPIO1_DIR = ALL_LEDS_MASK;

	/* Finally, point 3 of section 5.4 tells us to configure pad functions. Because
	 * it is hard and expensive to have a dedicated pin for every peripheral device on
	 * MCU, external pins are usually multiplexed between multiple devices. Through
	 * dedicated registers we can control what device can use each pin. GPIO_PAD_CONFIG
	 * registers are described in section 16. In table 16-12 on p. 498 general structure
	 * of GPIO_PAD_CONFIG_n is described: we can control function, maximum
	 * current and override output buffer to control it directly from this register.
	 * Having studied table 16-7 starting on p. 482 we learn that for all pins of
	 * interest mode 0 switches then to GPIO module. So let's set for all pins modes
	 * to 0, driver strength to 10mA (default value) and disable output buffer overriding*/
	GPIO_PAD_CONFIG_09 &= 0x060;
	GPIO_PAD_CONFIG_10 &= 0x060;
	GPIO_PAD_CONFIG_11 &= 0x060;

	/* MCU firmware normally never "finishes" so we have an infinite loop as our
	 *   main control structure.*/
	while(1) {
		/* We can see from the board schematics that our external system clock
		 *   has frequency of 40MHz. In the MCU doc a PLL is mentioned (e.g. 
		 *   block diagram on p.23 or "Clock, reset, and Power management
		 *   on p. 30") that can make up to 240 MHz from it, but a quick
		 *   search for "PLL" in the manual tells us that it is not used
		 *   for system clock (section 15.2.1 on p. 410). No dividers are
		 *   mentioned as well, so we can quite safely assume that CPU clock
		 *   is 40MHz. One loop iteration takes minimum 2 CPU cycles (one for
		 *   increment and one for branching, we can look into the assembler
		 *   listing to be sure), so with delay of 1 million iterations
		 *   LEDs will be toggled twice a second, which makes full period each
		 *   second*/
		for(i = 0; i < 10000000; i++);
		all_on_off(led);
		led = !led;
	}
}

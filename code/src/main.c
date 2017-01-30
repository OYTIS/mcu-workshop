#include "hw_memmap.h"
#include "hw_gpio.h"
#include "hw_types.h"

#define GPIO1_DIR HWREG(GPIOA1_BASE | GPIO_O_GPIO_DIR)
// define all the other registers here

void main()
{
	// write initialization code here
	while(1) {
		// write the blinking logic here
	}
}

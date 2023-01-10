/*
 * led.c -- module implementing led.h
 *
 * Assumes the LED's are connected to AXI_GPIO_0, on channel 1
 *
 * Terminal Settings:
 *  -Baud: 115200
 *  -Data bits: 8
 *  -Parity: no
 *  -Stop bits: 1
 */

static XGpio port;									/* GPIO port connected to the leds */
static int channel = 1; /* assuming channel 1 */

void led_init(void)
{
	// todo: crosscheck
	return;
}

void led_set(u32 led, bool tostate)
{
	if (led == ALL) {
		// set all LEDs
		if (tostate) {
			XGpio_DiscreteWrite(&port, channel, 0xf);
		} else {
			XGpio_DiscreteWrite(&port, channel, 0x0);
		}
	} else {
		// modify specific LEDs
//		u32 current = XGpio_DiscreteRead(&port, channel);
//		if (tostate) {
//			current &= 0xf;
//			XGpio_DiscreteWrite(&port, channel, current);
//		} else {
//			current ;
//			XGpio_DiscreteWrite(&port, channel, current);
//		}
	}
}

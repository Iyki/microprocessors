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

/*
* led_init -- initialize the LED module
*/
void led_init(void)
{
	// todo: crosscheck
	return;
}

/*
* led_set -- set the state of the LEDs
*/
void led_set(u32 led, bool tostate)
{
    u23 state = XGpio_DiscreteRead(&port, channel);
    u23 new_state; 
	if (led == ALL) {
		// all LEDs
		if (tostate) {
            // turn on all 4 i.e. set the last 4 bits
            new_state = state | 0xf;
			XGpio_DiscreteWrite(&port, channel, new_state);
		} else {
            // turn off all 4 i.e. clear the last 4 bits
            new_state = state & 0xfffffff0;
			XGpio_DiscreteWrite(&port, channel, new_state);
		}
	} else {
        // modify specific LEDs
        if (tostate) { 
            
        } else { 
            
        }
	}
}


bool led_get(u32 led) 
{
    // read all LEDs
    u23 state = XGpio_DiscreteRead(&port, channel); 

    // shift the desired LED to the LSB
    state >> (led - 1);
    return state % 2; // return if it is even or odd
}

void led_toggle(u32 led) 
{
    u23 state = XGpio_DiscreteRead(&port, channel);
    
}

/* led.c -- module implementing led.h
*
* Assumes the LED's are connected to AXI_GPIO_0, on channel 1
*
* Terminal Settings:
*  -Baud: 115200
*  -Data bits: 8
*  -Parity: no
*  -Stop bits: 1
*/

#include "led.h"


#define OUTPUT 0x0							/* setting GPIO direction to output */
#define CHANNEL1 1							/* channel 1 of the GPIO port */

static XGpio port;
									/* GPIO port connected to the leds */

/*
 * Initialize the led module
 */
void led_init(void)
{
    // initialize AXI_GPIO_0
    XGpio_Initialize(&port, XPAR_AXI_GPIO_0_DEVICE_ID);

    ///* set tristate buffer to output */
    XGpio_SetDataDirection(&port, CHANNEL1, OUTPUT);

    // todo: crosscheck
    return;
}

/*
 * Set <led> to one of {LED_ON,LED_OFF,...}
 *
 * <led> is either ALL or a number >= 0
 * Does nothing if <led> is invalid
 */
void led_set(u32 led, bool tostate)
{
    u32 status, mask, bit_flip_mask;
    if (led == ALL) {
        // set all LEDs
        if (tostate) {
            XGpio_DiscreteWrite(&port, CHANNEL1, 0xf);
        } else {
            XGpio_DiscreteWrite(&port, CHANNEL1, 0x0);
        }
    } else if (led <= 3) {
        //get led status
        status = XGpio_DiscreteRead(&port, CHANNEL1);
        // 1 for which led to turn on, everything else zero. ex:100 means turn led 2 on
        mask = 1 << led;

        if (tostate){
            status |= mask;
        }else{
            // flip the bits (0 for which led and everything else zero. ex: 011)
            bit_flip_mask = ~mask;
            status &= mask;
        }
        XGpio_DiscreteWrite(&port, CHANNEL1, status);
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


/*
 * Get the status of <led>
 *
 * <led> is a number >= 0
 * returns {LED_ON,LED_OFF,...}; LED_OFF if <led> is invalid
 */
bool led_get(u32 led){

    u32 status, mask;

    if (led <= 3){
        //get led status
        status = XGpio_DiscreteRead(&port, CHANNEL1);
        mask = 1 << led;
        status &= mask;
        if (status > 0){
            return LED_ON
        }
    }
    // return LED_OFF if led invalid
    return LED_OFF
}



/*
 * Toggle <led>
 *
 * <led> is a value >= 0
 * Does nothing if <led> is invalid
 */
void led_toggle(u32 led) {
    if (led <= 3){
        if (led_get(led)){
            led_set(led, LED_OFF)
        }else{
            led_set(led, LED_ON)
        }
    }
}

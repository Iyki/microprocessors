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

#define LED4PIN 7



static XGpio port; 							/* GPIO port connected to the leds */
static XGpio port3; 							/* GPIO port connected to the colored led */

static XGpioPs  psport;

static XGpioPs_Config* psconfig;

/*
 * Initialize the led module
 */
void led_init(void)
{

    // initialize AXI_GPIO_0
    XGpio_Initialize(&port, XPAR_AXI_GPIO_0_DEVICE_ID);

    ///* set tristate buffer to output */
    XGpio_SetDataDirection(&port, CHANNEL1, OUTPUT);

    // initialize AXI_GPIO_1
    XGpio_Initialize(&port3, XPAR_AXI_GPIO_1_DEVICE_ID);

    // initialize AXI_GPIO_3
    XGpio_Initialize(&port3, XPAR_AXI_GPIO_3_DEVICE_ID);

    ///* set tristate buffer to output */
	XGpio_SetDataDirection(&port3, CHANNEL1, OUTPUT);

    // processing system

    //find the config file
    psconfig = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
    //initialize the device
    XGpioPs_CfgInitialize(&psport, psconfig, psconfig->BaseAddr);
    // SEt direction to output for LED 4
    XGpioPs_SetDirectionPin(&psport, LED4PIN , 1);

    //Enable the output
    XGpioPs_SetOutputEnablePin(&psport, LED4PIN, 1);
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
    status = XGpio_DiscreteRead(&port, CHANNEL1); // get led status
    if (led == ALL) {
        // set all LEDs
        if (tostate) {
	    status = status | 0xf;
        } else {
	    status = status & 0xfffffff0;
        }
	XGpio_DiscreteWrite(&port, CHANNEL1, status);
    } else if (led <= 3) {
        // 1 for which led to turn on, everything else zero. ex:100 means turn led 2 on
        mask = 1 << led;

        if (tostate){
            status |= mask;
        }else{
            // flip the bits (0 for which led and everything else zero. ex: 011)
            bit_flip_mask = ~mask;
            status &= bit_flip_mask;
        }
        XGpio_DiscreteWrite(&port, CHANNEL1, status);
    }else if (led == 4){
    	XGpioPs_WritePin(&psport, LED4PIN, tostate);
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
            return LED_ON;
        }
    }else if (led == 4){
    	status = XGpioPs_ReadPin(&psport, LED4PIN);
    	if (status > 0){
			return LED_ON;
		}
    }
    // return LED_OFF if led invalid
    return LED_OFF;
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
            led_set(led, LED_OFF);
        }else{
            led_set(led, LED_ON);
        }
    }
}


void led6_set(Color c){
	XGpio_DiscreteWrite(&port3, CHANNEL1, 0x0);

	if (c == BLUE){
		XGpio_DiscreteWrite(&port3, CHANNEL1, 0x1);
	}else if (c == GREEN){
		XGpio_DiscreteWrite(&port3, CHANNEL1, 0x2);
	}else if (c == RED){
		XGpio_DiscreteWrite(&port3, CHANNEL1, 0x4);
	}else if (c == YELLOW){
		XGpio_DiscreteWrite(&port3, CHANNEL1, 0x6);
	}else if (c == OFF){
		XGpio_DiscreteWrite(&port3, CHANNEL1, 0x0);
	}

}


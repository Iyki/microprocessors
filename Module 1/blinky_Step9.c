/*
 * blinky.c -- working with Serial I/O and GPIO
 *
 * Assumes the LED's are connected to AXI_GPIO_0, on channel 1
 *
 * Terminal Settings:
 *  -Baud: 115200
 *  -Data bits: 8
 *  -Parity: no
 *  -Stop bits: 1
 */
#include <stdio.h>							/* printf(), getchar() */
#include "xil_types.h"					/* u32, u16 etc */
#include "platform.h"						/* ZYBOboard interface */
#include <xgpio.h>							/* Xilinx GPIO functions */
#include "xparameters.h"				/* constants used by the hardware */
#include <stdbool.h> 				// bool type
#include <stdlib.h> //strtol
#include "led.h"	// led functions 

#define OUTPUT 0x0							/* setting GPIO direction to output */
#define CHANNEL1 1							/* channel 1 of the GPIO port */


#define LINELEN 32


void read_save_echo_line(char line[]){
    char c;
    int i = 0; // where to save read char in line
    c = getchar();

    // \r is carriage return which is what lines end with in this system
    // https://developer.arm.com/documentation/ka003309/latest#:~:text=In%20most%20C%20compilers%2C%20including,return%20is%20'%5Cr'.
    while (c != '\r'){
        // save read char
        *(line + i) = c;
        i++;

        // echo back read char
        putchar(c);

        // read new char
        c = getchar();
    }
    // null terminate line after reaching a new line char
    *(line+i) = '\0';
}

int main() {

	XGpio port;									/* GPIO port connected to the leds */
	char line[LINELEN];
	bool done = false;
	char* end_char; // first non-integer char of a line (used in strtol())
	int strtol_n; // strtol return
	bool status; // status of the LED


	init_platform();							/* initialize the hardware platform */

	/*
	 * set stdin unbuffered, forcing getchar to return immediately when
	 * a character is typed .
	 */
	setvbuf(stdin,NULL,_IONBF,0);

	printf("[Hello]\n");

	led_init(); // initialize led module
	led_set(0x0, LED_ON); // turn on led0
	// XGpio_Initialize(&port, XPAR_AXI_GPIO_0_DEVICE_ID);	/* initialize device AXI_GPIO_0 */
	// XGpio_SetDataDirection(&port, CHANNEL1, OUTPUT);	    /* set tristate buffer to output */
	// XGpio_DiscreteWrite(&port, CHANNEL1, 0x1);						/* turn on led0 */


	while(!done){

		printf(">");
		// write everything in stdout buffer in case
		fflush(stdout);

		read_save_echo_line(line);

		if (strcmp(line, "q") == 0){
			done = true;
		}else{
			strtol_n = (int)strtol(line, &end_char, 10); // convert the content of line to base 10
			/*
	           detect a number between 0 and 3
	               man strtol:
	               If endptr is not NULL, strtol() stores the address of the first invalid
	               character  in *endptr.  If there were no digits at all, strtol() stores
	               the original value of nptr in *endptr (and returns 0).  In  particular,
	               if  *nptr is not '\0' but **endptr is '\0' on return, the entire string
	               is valid.
			 */
			if (*line != '\0' && *end_char == '\0'){ //or *end_char == '\0' && line != end_char && n >=0){
				if (strtol_n >= 0 && strtol_n <= 3){
					// Step 9: Toggle the expected LED & print
					led_toggle((u32)strtol_n);
					status = led_get((u32)strtol_n);
					if (status) {
						printf("\n[%d on]",strtol_n);
					} else { 
						printf("\n[%d off]",strtol_n);
					}
					//printf("\n[%d]",strtol_n);
				}
				// if (strtol_n == 0){
				// 	// toggle led0
				// 	led_toggle(0x0);
				// 	// u32 led0 = XGpio_DiscreteRead(&port, CHANNEL1);
				// 	// led0 ^= 0x1;
				// 	// XGpio_DiscreteWrite(&port, CHANNEL1, led0);
				// }
			}
		}
		printf("\n");
	}

	led_set(0x0, LED_OFF); // turn off led0
	//XGpio_DiscreteWrite(&port, CHANNEL1, 0x0);						/* STEP 6: turn off led0 */
	cleanup_platform();					/* cleanup the hardware platform */
	return 0;
}

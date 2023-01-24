/*
 * main.c -- A program to print a dot each time button 0 is pressed.
 *
 *  Some useful values:
 */
#include <stdio.h>		/* getchar,printf */
#include <stdlib.h>		/* strtod */
#include <stdbool.h>		/* type bool */
#include <unistd.h>		/* sleep */
#include <string.h>

#include "platform.h"		/* ZYBO board interface */
#include "xil_types.h"		/* u32, s32 etc */
#include "xparameters.h"	/* constants used by hardware */

#include "gic.h"		/* interrupt controller interface */
#include "xgpio.h"		/* axi gpio interface */
#include "led.h" // led interface
#include "io.h" //
#include "ttc.h"





/* hidden private state */
//static XGpio btnport;	       /* btn GPIO port instance */
// static int pushes=0;	       /* variable used to count interrupts */
//static bool pushed = false; 			// true when a button goes from low to high

#define LINELEN 32
#define FREQ 1 // 1 hz = 1 sec
#define LED4 4

static bool led4_on = false;

void read_save_echo_line(char line[]){
    char c;
    int i = 0; // where to save read char in line
    c = getchar();

    // \r is carriage return which is what lines end with in tahis system
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


// toggle led 4
static void ttc_callback(void){
	if (led4_on){
		led_set(LED4, LED_OFF);
	}else{
		led_set(LED4, LED_ON);
	}
	led4_on = !led4_on;
}

void btn_callback(u32 btn) {
	led_toggle(btn);
	//pushes++;
}


int main() {
  /* variables
   *
   */
	char line[LINELEN];
	bool done = false;
	char* end_char; // first non-integer char of a line (used in strtol())
	int strtol_n; // strtol return

	// initialize platform
	init_platform();
	// initialize gic
	gic_init();
	// initialize buttons
	io_btn_init(btn_callback);
	// initialize switches
	io_sw_init(btn_callback);
	// initialize leds
	led_init();
	// initialize and start ttc
	ttc_init(FREQ, ttc_callback);
	ttc_start();

	//set led 4 on to signify program is on
	//led_set(4, LED_ON);

	printf("[hello]\n"); /* so we are know its alive */


	// listening for stdin
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
					}
				}
			}
			printf("\n");
		}


	printf("\n[done]\n");

	ttc_stop();
	ttc_close();
	io_btn_close();
	io_sw_close();
	gic_close();
	led_set(ALL, LED_OFF); // turn off led0
	//led_set(4, LED_OFF);
	cleanup_platform();					/* cleanup the hardware platform */
	return 0;
}

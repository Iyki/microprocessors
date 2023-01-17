/*
 * main.c -- A program to print a dot each time button 0 is pressed.
 *
 *  Some useful values:
 *  -- XPAR_AXI_GPIO_1_DEVICE_ID -- xparameters.h
 *  -- XPAR_FABRIC_GPIO_1_VEC_ID -- xparameters.h
 *  -- XGPIO_IR_CH1_MASK         -- xgpio_l.h (included by xgpio.h)
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
#include "led.h"
#include "io.h"


#define CHANNEL1 1							/* channel 1 of the GPIO port */
#define INPUT 0xFFFFFFFF


/* hidden private state */
static XGpio btnport;	       /* btn GPIO port instance */
static int pushes=0;	       /* variable used to count interrupts */
static bool pushed = false; 			// true when a button goes from low to high


u32 convert(u32 input_num) {
	if (input_num == 0) {
		printf("wtf are you doing??\n");
		fflush(stdout);
		return 0;
	}
	u32 num = 0;
	while (input_num >> num != 1) {
		num++;
	}
	return num;
}


void btn_callback(u32 btn) {
	u32 led = convert(btn);
	led_toggle(led);
	//pushes++;
}


int main() {
  init_platform();				

  //gic_init();

  io_btn_init(btn_callback);
  //io_sw_init(btn_callback);


  led_init();

  printf("[hello]\n"); /* so we are know its alive */
  pushes=0;
  while(pushes<15) /* do nothing and handle interrups */
	  ;

  printf("\n[done]\n");

  io_btn_close();
  //io_sw_close();

  //gic_close();
  cleanup_platform();					/* cleanup the hardware platform */
  return 0;
}

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


/*
 * controll is passed to this function when a button is pushed
 *
 * devicep -- ptr to the device that caused the interrupt
 */
void btn_handler(void *devicep) {
	/* coerce the generic pointer into a gpio */
	XGpio *dev = (XGpio*)devicep;

	u32 btn_num;
	u32 led;

	if (!pushed){
		btn_num =  XGpio_DiscreteRead(dev,CHANNEL1);
		pushed = true;
		/*
		 * btn 0 = 0x1 = 1
		 * btn 1 = 0x10 = 2
		 * btn 2 = 0x100 = 4
		 * btn 3 = 0x1000 = 8
		 */
		led = convert(btn_num);
		led_toggle(led);
		pushes++;


	}else{
		pushed = false;
	}

	XGpio_InterruptClear(dev, XGPIO_IR_CH1_MASK);


}



int main() {
  init_platform();				

  /* initialize the gic (c.f. gic.h) */
  gic_init();

  /* initialize btnport (c.f. module 1) and immediately dissable interrupts */
  if(XGpio_Initialize(&btnport, XPAR_AXI_GPIO_1_DEVICE_ID)!=XST_SUCCESS) {
    		printf("Unable to initialize btn GPIO(1)");
    		return 0;
    }
  // disable golabl interrupt to the processor
  XGpio_InterruptGlobalDisable(&btnport);

  // diable channel interrups
  /*
   * Mask is the mask to disable. Bits set to 1 are disabled. This
   * mask is formed by OR'ing bits from XGPIO_IR* bits which are
   * contained in xgpio_l.h.
   */
  XGpio_InterruptDisable(&btnport, XGPIO_IR_CH1_MASK);

  //set tri state buffer to input
  XGpio_SetDataDirection(&btnport, CHANNEL1, INPUT);

  /* connect handler to the gic (c.f. gic.h) */
  if( gic_connect(XPAR_FABRIC_GPIO_1_VEC_ID, btn_handler, &btnport) != XST_SUCCESS) {
	  printf("Unable to connect btn int to gic");
	  return 0;
  }
  /* enable interrupts on channel (c.f. table 2.1) */
  XGpio_InterruptEnable(&btnport, XGPIO_IR_CH1_MASK);

  /* enable interrupt to processor (c.f. table 2.1) */
  XGpio_InterruptGlobalEnable(&btnport);

  led_init();

  printf("[hello]\n"); /* so we are know its alive */
  pushes=0;
  while(pushes<15) /* do nothing and handle interrups */
	  ;

  printf("\n[done]\n");

  /* disconnect the interrupts (c.f. gic.h) */
  gic_disconnect(XPAR_FABRIC_GPIO_1_VEC_ID);

  /* close the gic (c.f. gic.h) */
  gic_close();

  cleanup_platform();					/* cleanup the hardware platform */
  return 0;
}

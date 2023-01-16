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

/* hidden private state */
static XGpio btnport;	       /* btn GPIO port instance */
static int pushes=0;	       /* variable used to count interrupts */

/*
 * controll is passed to this function when a button is pushed
 *
 * devicep -- ptr to the device that caused the interrupt
 */
void btn_handler(void *devicep) {
	/* coerce the generic pointer into a gpio */
	XGpio *dev = (XGpio*)devicep;

	pushes++;
	printf(".");
	fflush(stdout);

}


int main() {
  init_platform();				

  /* initialize the gic (c.f. gic.h) */
  /* initialize btnport (c.f. module 1) and immediately dissable interrupts */
  /* connect handler to the gic (c.f. gic.h) */
  /* enable interrupts on channel (c.f. table 2.1) */
  /* enable interrupt to processor (c.f. table 2.1) */

  printf("[hello]\n"); /* so we are know its alive */
  pushes=0;
  while(pushes<5) /* do nothing and handle interrups */
	  ;

  printf("\n[done]\n");

  /* disconnect the interrupts (c.f. gic.h) */
  /* close the gic (c.f. gic.h)
  cleanup_platform();					/* cleanup the hardware platform */
  return 0;
}


#include <stdio.h>		/* getchar,printf */
#include <stdlib.h>		/* strtod */
#include <stdbool.h>		/* type bool */
#include <unistd.h>		/* sleep */
#include <string.h>

#include "platform.h"		/* ZYBO board interface */
#include "xil_types.h"		/* u32, s32 etc */
#include "xparameters.h"	/* constants used by hardware */
#include "xuartps.h"


#include "gic.h"		/* interrupt controller interface */
#include "xgpio.h"		/* axi gpio interface */
#include "led.h" 		// led interface
#include "io.h" //
#include "ttc.h"



#define TGREEN 0
#define YELLOW_RED 1
#define TRED 2
#define YELLOW_GREEN 3
#define TRAIN 4
#define CLEAR 5
#define MAINTENANCE 6
#define START 7


#define FREQ 1


static bool done; // end of program
static int state;

static bool time_3min = false;
static bool cross = false;
static bool time_3sec = false;
static bool time_30sec = false;
static int ttc_counter = 0;




static void btn_callback(u32 btn) {
	led_toggle(btn);

	if (btn == 0){
		cross = true;
	}
	//pushes++;
}




static void t3min_callback(){
	ttc_counter++;

	if (ttc_counter == 10){
		time_3min = true;
		ttc_stop();
		ttc_close();
		ttc_counter = 0;
	}

}

static void start_3min(){
	// initialize and start ttc
	ttc_init(FREQ, t3min_callback);
	ttc_start();
}


static void t30sec_callback(){
	ttc_counter++;

	if (ttc_counter == 10){
		time_30sec = true;
		ttc_stop();
		ttc_close();
		ttc_counter = 0;
	}

}

static void start_30sec(){
	// initialize and start ttc
	ttc_init(FREQ, t30sec_callback);
	ttc_start();
}



static void t3sec_callback(){
	ttc_counter++;
	if (ttc_counter == 3){
		time_3sec = true;
		ttc_stop();
		ttc_close();
		ttc_counter = 0;
	}

}

static void start_3sec(){
	// initialize and start ttc
	ttc_init(FREQ, t3sec_callback);
	ttc_start();
}



/*  the state transition function */
static void change_state() {
	switch(state) {        /* based on current state */
	case START:
		state = TGREEN;
		start_3min();
		led6_set(GREEN);
		break;
	case TGREEN:
		printf("ssss");
		fflush(stdout);
		if (time_3min && cross){
			time_3min = false;
			cross = false;
			state = YELLOW_RED;
			led6_set(YELLOW);
			start_3sec();
		}
		break;

	case YELLOW_RED:
		if (time_3sec){
			time_3sec= false;
			state = TRED;
			led6_set(RED);
			start_30sec();
		}
		break;

	case TRED:

		if (time_30sec){
			time_30sec = false;
			state = YELLOW_GREEN;
			led6_set(YELLOW);
			start_3sec();
		}
		break;
	case YELLOW_GREEN:
		if (time_3sec){
			time_3sec= false;
			state = TGREEN;
			led6_set(GREEN);
		}

	}


}




int main(void) {


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


	state = START;


	change_state();
	change_state();
	 led6_set(BLUE);

	printf("[hello]\n");
	done = false;
	while(!done){

	}

	printf("[done]\n");
	sleep(1); /* included from <unistd.h> */


	io_btn_close();
	gic_close();
	cleanup_platform();

}

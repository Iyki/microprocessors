
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
#include "servo.h"
#include "adc.h"



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
static bool train_arriving = false;
static bool clear = false;
static bool key_in = false;
static bool key_out = false;
static bool blue_on = false;
static bool switch_up = false;
static float pot = 0;




static void btn_callback(u32 btn) {
	led_toggle(btn);

	if (btn == 0){
		cross = true;
		printf("%d", cross);
	}else if (btn == 1){
		train_arriving = true;
	}else if (btn == 2){
		clear = true;
	}
}


static void sw_callback(u32 sw){
	if (sw == 0){
		if (switch_up == false){
			switch_up = true;
			key_in = true;
		}else{
			switch_up = false;
			key_out = true;
		}
	}
}




static void t3min_callback(){
	ttc_counter++;
	// the timer times 1 sec. Instead of 3 min we will do 10 sec.
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

static void close_gate(){
	servo_set(MINPOINT);
	return;
}
static void open_gate(){
	servo_set(MAXPOINT);
	return;
}


static void change_to_train(){
	// only when maintenance is off
	if (!key_in){
		state = TRAIN;
		train_arriving = false;
		led6_set(RED);

		// close timers if there is any
		ttc_stop();
		ttc_close();
		ttc_counter = 0;

		//reset outputs
		time_3min = false;
		time_3sec= false;
		time_30sec = false;
		cross = false;

		//close gate
		close_gate();
	}
}


static void t1sec_callback(){

	if (blue_on){
		blue_on = false;
		led6_set(OFF);
	}else{
		blue_on = true;
		led6_set(BLUE);
	}
}


static void start_1sec(){
	// initialize and start ttc
	ttc_init(FREQ, t1sec_callback);
	ttc_start();
}


static void pot_callback(){
	pot = adc_get_pot();
	pot = pot * (MAXPOINT-MINPOINT) + MINPOINT;
	servo_set(pot);

	ttc_counter++;
	if (ttc_counter == 10){
		ttc_counter = 0;
		if (blue_on){
			blue_on = false;
			led6_set(OFF);
		}else{
			blue_on = true;
			led6_set(BLUE);
		}
}


}
static void start_pot_timer(){

	ttc_init(10, pot_callback);
	ttc_start();

}

static void blink_blue(){
	led6_set(BLUE);
	blue_on = true;
	start_1sec();

}

static void change_to_maintenance(){

	state = MAINTENANCE;
	key_in = false;

	// close timers if there is any
	ttc_stop();
	ttc_close();
	ttc_counter = 0;

	//reset outputs
	time_3min = false;
	time_3sec= false;
	time_30sec = false;
	cross = false;

	led6_set(YELLOW);
	start_3sec();

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
			start_3min();
		}
		break;

	case TRAIN:
		if (clear){
			state = CLEAR;
			clear = false;
			start_3min();
			open_gate();

		}
		break;

	case CLEAR:
		if (time_3min){
			time_3min = false;
			cross = false;
			state = TGREEN;
			start_3min();
			led6_set(GREEN);
		}
		break;


	case MAINTENANCE:
		if (time_3sec){
			time_3sec= false;
			led6_set(RED);
			start_30sec();
		}

		if (time_30sec){
			time_30sec = false;
			close_gate();
//			blink_blue();
			led6_set(BLUE);
			blue_on = true;
			start_pot_timer();
		}

		if (key_out){
			ttc_stop();
			ttc_close();
			key_out = false;
			state = TRED;
			led6_set(RED);
			start_30sec();
			open_gate();
		}


		break;

	}

	if (train_arriving){
		change_to_train();
	}

	if (key_in){
		 change_to_maintenance();
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
	io_sw_init(sw_callback);
	// initialize leds
	led_init();
	servo_init(); // starts at initial servo dutycycle
	 // initialize analog to degital converter
	 adc_init();

	servo_set(MAXPOINT);


	state = START;


	change_state();
	change_state();

	printf("[hello]\n");
	done = false;
	while(!done){
		change_state();
	}

	printf("[done]\n");
	sleep(1); /* included from <unistd.h> */


	io_btn_close();
	gic_close();
	cleanup_platform();

}

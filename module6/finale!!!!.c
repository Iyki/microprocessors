
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



// states
#define TGREEN 0
#define YELLOW_RED 1
#define TRED 2
#define YELLOW_GREEN 3
#define TRAIN 4
#define CLEAR 5
#define MAINTENANCE 6
#define START 7


#define FREQ 1

// wifi
#define ID 1
#define UPDATE 2


static XUartPs uart_ps_0;
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


static int counter = 0; // for utc recieve

typedef struct {
	int type;
	int id;
	int value;
} update_request_t;

typedef struct {
	int type;
	int id;
	int average;
	int values[30];
} update_response_t;



static update_response_t update;







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


static void timer_callback(){

	if (state != MAINTENANCE){
		switch (state){
			case TGREEN:
				ttc_counter++;
				// the timer times 1/10 sec. Instead of 3 min we will do 10 sec.
				if (ttc_counter == 100){
					time_3min = true;
					ttc_counter = 0;
				}
				break;
			case CLEAR:
				ttc_counter++;
				// the timer times 1/10 sec. Instead of 3 min we will do 10 sec.
				if (ttc_counter == 100){
					time_3min = true;
					ttc_counter = 0;
				}
				break;
			case YELLOW_RED:
				ttc_counter++;
				if (ttc_counter == 30){
					time_3sec = true;
					ttc_counter = 0;
				}
				break;
			case YELLOW_GREEN:
				ttc_counter++;
				if (ttc_counter == 30){
					time_3sec = true;
					ttc_counter = 0;
				}
				break;

			case TRED:
				ttc_counter++;
				if (ttc_counter == 100){
					time_30sec = true;;
					ttc_counter = 0;
				}

			}
	}else{
		ttc_counter++;
		if (ttc_counter == 30){
			time_3sec = true;
		}

		if (ttc_counter == 130){
			time_30sec = true;
		}

		pot = adc_get_pot();
		pot = pot * (MAXPOINT-MINPOINT) + MINPOINT;
		servo_set(pot);

		if (ttc_counter > 130 && ttc_counter % 10 == 0){
			if (blue_on){
				blue_on = false;
				led6_set(OFF);
			}else{
				blue_on = true;
				led6_set(BLUE);
			}
		}

	}


	update_request_t update_req = {2, 0, 1};
	XUartPs_Send(&uart_ps_0, (u8*)&update_req, sizeof(update_request_t));




}


static void start_timer(){
	ttc_init(10, timer_callback);
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



static void change_to_maintenance(){

	state = MAINTENANCE;
	key_in = false;

	// close timers if there is any
	ttc_counter = 0;

	//reset outputs
	time_3min = false;
	time_3sec= false;
	time_30sec = false;
	cross = false;

	led6_set(YELLOW);

}


/*  the state transition function */
static void change_state() {
	switch(state) {        /* based on current state */
	case START:
		state = TGREEN;
		led6_set(GREEN);
		break;
	case TGREEN:
		if (time_3min && cross){
			time_3min = false;
			cross = false;
			state = YELLOW_RED;
			led6_set(YELLOW);
		}
		break;

	case YELLOW_RED:
		if (time_3sec){
			time_3sec= false;
			state = TRED;
			led6_set(RED);
		}
		break;

	case TRED:
		if (time_30sec){
			time_30sec = false;
			state = YELLOW_GREEN;
			led6_set(YELLOW);
		}
		break;
	case YELLOW_GREEN:
		if (time_3sec){
			time_3sec= false;
			state = TGREEN;
			led6_set(GREEN);
		}
		break;

	case TRAIN:
		if (clear){
			state = CLEAR;
			clear = false;
			open_gate();

		}
		break;

	case CLEAR:
		if (time_3min){
			time_3min = false;
			cross = false;
			state = TGREEN;
			led6_set(GREEN);
		}
		break;


	case MAINTENANCE:
		if (time_3sec){
			time_3sec= false;
			led6_set(RED);
		}

		if (time_30sec){
			time_30sec = false;
			close_gate();
			led6_set(BLUE);
			blue_on = true;
		}

		if (key_out){
			ttc_counter = 0;
			key_out = false;
			state = TRED;
			led6_set(RED);
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




static void Uart0_Handler( void *CallBackRef, u32 Event, unsigned int EventData) {

	XUartPs *uart_ptr = (XUartPs *) CallBackRef;
	u8 recieve_buffer;

	if (Event == XUARTPS_EVENT_RECV_DATA) {
		XUartPs_Recv(uart_ptr, &recieve_buffer, 1); // recieve normally

		*((u8*)&update + counter) = recieve_buffer;
		counter = counter + 1;
		if (counter == sizeof(update_response_t)){

			counter = 0;


			int is_maintainance = update.values[ID];


			if (is_maintainance == 1){
				if(state != MAINTENANCE){
					change_to_maintenance();
				}
			}else if(is_maintainance == 0){
				if (state == MAINTENANCE){
					key_out = true;
				}
			}
		}
	}
}



int main(void) {



	XUartPs_Config *Uart0_config;


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




	//############################uart0 setup
	// find uart0 config
	Uart0_config = XUartPs_LookupConfig(XPAR_PS7_UART_0_DEVICE_ID);
	// initialize uart0
	XUartPs_CfgInitialize(&uart_ps_0, Uart0_config, Uart0_config->BaseAddress);
	// enable RX FIFO trigger interrupt
	XUartPs_SetInterruptMask(&uart_ps_0, XUARTPS_IXR_RXOVR);
	// set threshold
	XUartPs_SetFifoThreshold(&uart_ps_0, 1);
	// set handler
	XUartPs_SetHandler(&uart_ps_0, (XUartPs_Handler)Uart0_Handler, &uart_ps_0);
	//set baud rate
	XUartPs_SetBaudRate(&uart_ps_0, 9600);
	// connect to interrupt controller
	gic_connect(XPAR_XUARTPS_0_INTR, (Xil_ExceptionHandler)XUartPs_InterruptHandler, (void *) &uart_ps_0);

	servo_set(MAXPOINT);
	state = START;

	start_timer();





	printf("[hello]\n");
	done = false;
	while(!done){
		change_state();
	}

	printf("[done]\n");
	sleep(1); /* included from <unistd.h> */


	XUartPs_DisableUart(&uart_ps_0);
	gic_disconnect(XPAR_XUARTPS_0_INTR);

	ttc_stop();
	ttc_close();
	io_btn_close();
	gic_close();
	cleanup_platform();

}

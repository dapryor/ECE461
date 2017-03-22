#include <MKL25Z4.H>
#include "gpio_defs.h"

/*
	Code to demonstrate flashing RGB LEDs in sequence based on switch presses.
	Wiring Needed:
			Switch 1: SW1_POS PTD7
			Switch 2: SW2_POS PTD6	
		
*/

void Delay(volatile unsigned int time_del) {
	volatile int n;
	while (time_del--) {
		n = 1000;
		while (n--)
			;
	}
}

void Initialize_Ports(void) {
	// Enable clock to ports A, B and D
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;;
	
	// Make 3 pins GPIO
	PORTB->PCR[RED_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[RED_LED_POS] |= PORT_PCR_MUX(1);          
	PORTB->PCR[GREEN_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[GREEN_LED_POS] |= PORT_PCR_MUX(1);          
	PORTD->PCR[BLUE_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTD->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(1);          
	
	// Set LED port bits to outputs
	PTB->PDDR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
	PTD->PDDR |= MASK(BLUE_LED_POS);

	// Select port D on pin mux, enable pull-up resistors
	PORTD->PCR[SW1_POS] = PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK;
	PORTD->PCR[SW2_POS] = PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK;

	// Clear switch bits to input
	PTD->PDDR &= ~MASK(SW1_POS); 
	PTD->PDDR &= ~MASK(SW2_POS); 
	
	// Turn off LEDs
	PTB->PSOR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
	PTD->PSOR |= MASK(BLUE_LED_POS);
}

void Control_RGB_LEDs(int r_on, int g_on, int b_on) {
	if (r_on)
		PTB->PCOR = MASK(RED_LED_POS);
	else
		PTB->PSOR = MASK(RED_LED_POS);
	if (g_on)
		PTB->PCOR = MASK(GREEN_LED_POS);
	else
		PTB->PSOR = MASK(GREEN_LED_POS);
	if (b_on)
		PTD->PCOR = MASK(BLUE_LED_POS);
	else
		PTD->PSOR = MASK(BLUE_LED_POS);
}

//////////////////////
// Use hardware timer
void Init_PIT(unsigned channel, unsigned period) {
	// Enable clock to PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	// Disable module
	PIT->MCR |= PIT_MCR_MDIS_MASK;
	// Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	// Disable timer, clear control register
	PIT->CHANNEL[channel].TCTRL = 0;
	// Initialize PIT0 to count down from argument 
	// Time delay = (period + 1)/24 MHz 
	PIT->CHANNEL[channel].LDVAL = PIT_LDVAL_TSV(period);
	// No chaining
	PIT->CHANNEL[channel].TCTRL &= PIT_TCTRL_CHN_MASK;
	// Do not generate interrupts
	PIT->CHANNEL[channel].TCTRL &= ~PIT_TCTRL_TIE_MASK;
	// Reset interrupt/overflow flag
	PIT->CHANNEL[channel].TFLG = PIT_TFLG_TIF_MASK;
}

void Start_PIT(unsigned channel) {
	// Enable counter
	PIT->CHANNEL[channel].TCTRL |= PIT_TCTRL_TEN_MASK;
}

void Stop_PIT(unsigned channel) {
	// Disable counter
	PIT->CHANNEL[channel].TCTRL &= ~PIT_TCTRL_TEN_MASK;
}

unsigned PIT_Expired(unsigned channel) {
	return PIT->CHANNEL[channel].TFLG & PIT_TFLG_TIF_MASK; // Is 1 on underflow (time expired)
}

// Task code

int Task_Color_Sequence(unsigned int delay_duration) {
	int flash_num;
	int response_delay=0;
	
	Control_RGB_LEDs(1, 0, 0); // Red
	Delay(delay_duration);
	if (SWITCH_PRESSED(SW1_POS)) { 
		Control_RGB_LEDs(1, 0, 1); // Magenta
		Delay(delay_duration);
		response_delay = 0;
		do {
			Control_RGB_LEDs(0, 0, 1); // Blue
			Delay(delay_duration/2);
			Control_RGB_LEDs(1, 0, 0); // Red
			Delay(delay_duration/2);
			response_delay++;
		} while (!SWITCH_PRESSED(SW2_POS));
	} else {
		Control_RGB_LEDs(1, 1, 0); // Yellow
		Delay(delay_duration);
	}
	for (flash_num = 0; flash_num < 3; flash_num++) {
		Control_RGB_LEDs(0, 1, 0); // Green
		Delay(delay_duration);
		Control_RGB_LEDs(0, 0, 0); // Off
		Delay(delay_duration);
	}
	return response_delay;
}

int Task_Color_Sequence_FSM(int new_input_data, unsigned int delay_duration) {
	int flash_num;
	static unsigned int local_delay_duration=100;
	static enum {S1A, S1B, S1C1, S1C2, S2} next_state = S1A;
	static int response_delay=0;
	static int return_value=0;

	if (new_input_data)
		local_delay_duration = delay_duration;
	
	switch (next_state) {
		case S1A:
			// S1A
			Control_RGB_LEDs(1, 0, 0); // Red
			Delay(local_delay_duration);
			if (SWITCH_PRESSED(SW1_POS)) {
				next_state = S1C1;
			} else {
				next_state = S1B;
			}
			break;
		case S1C1:
			// S1C1
			Control_RGB_LEDs(1, 0, 1); // Magenta
			Delay(local_delay_duration);
			response_delay = 0;
			next_state = S1C2;
			break;
		case S1C2:
			// S1C2
			Control_RGB_LEDs(0, 0, 1); // Blue
			Delay(local_delay_duration/2);
			Control_RGB_LEDs(1, 0, 0); // Red
			Delay(local_delay_duration/2);
			response_delay++;
			if (SWITCH_PRESSED(SW2_POS))
				next_state = S2;
			else
				next_state = S1C2;
			break;
		case S1B:
			// S1B
			Control_RGB_LEDs(1, 1, 0); // Yellow
			Delay(local_delay_duration);
			next_state = S2;
			break;
		case S2:
			// S2
			for (flash_num = 0; flash_num < 3; flash_num++) {
				Control_RGB_LEDs(0, 1, 0); // Green
				Delay(local_delay_duration);
				Control_RGB_LEDs(0, 0, 0); // Off
				Delay(local_delay_duration);
			}
			return_value = response_delay;
			next_state = S1A;
			break;
		default: next_state = S1A;
	}
	return return_value;
}

void Scheduler(void) {
	int n;
	while (1) {
		n = Task_Color_Sequence(1000);
		// Do something with n in another task here
	}
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	Initialize_Ports();
	Scheduler();
}

// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   

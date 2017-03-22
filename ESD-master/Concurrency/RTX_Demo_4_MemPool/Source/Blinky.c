#include <cmsis_os.h>
#include <MKL25Z4.H>
#include "gpio_defs.h"

/*
	Wiring Needed:
			Switch 1: SW1_POS PTD7	(J2-19)
			Switch 2: SW2_POS PTD6	(J2-17)
*/

#define NUM_Q_ENTRIES 8

#define MAX_NUM_LED_STEPS 16
#define LED_PATTERN_POOL_COUNT 8

typedef struct {
	uint8_t R:1;
	uint8_t G:1;
	uint8_t B:1;
} RGB_t;

typedef struct {
	RGB_t Step[MAX_NUM_LED_STEPS];
	uint8_t NumSteps;
} LED_pattern_t;

// Declare memory pool 
osPoolDef(LED_pattern_pool, LED_PATTERN_POOL_COUNT, LED_pattern_t);

// Memory pool ID
osPoolId(LED_pattern_pool_id);

// Thread IDs for the threads
osThreadId t_RS;                        
osThreadId t_RGB;      

void Thread_Read_Switches(void const * arg);
void Thread_RGB(void const * arg);

osThreadDef(Thread_Read_Switches, osPriorityNormal, 1, 0);
osThreadDef(Thread_RGB, osPriorityNormal, 1, 0);

osMessageQId switch_msgq;
osMessageQDef(switch_msgq, NUM_Q_ENTRIES, uint32_t);

uint32_t g_RGB_delay=1000; 	// delay for RGB sequence

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

void Thread_Read_Switches(void const * arg) {
	int count = 0;
	LED_pattern_t * pattern;
	int cur_step = 0;
	
	while (1) {
		osDelay(250);
		if (SWITCH_PRESSED(SW1_POS)) { 
			if (count == 0) { 	// when switch is first pressed, allocate an object
				pattern = (LED_pattern_t *) osPoolAlloc(LED_pattern_pool_id);
				if (!pattern) {				// out of memory blocks!
					while (1) {
						osDelay(250);
					}
				}
				cur_step = 0;
				pattern->NumSteps = 0;
			}
			count++;
			
			if (cur_step < MAX_NUM_LED_STEPS) {
				// load current step of pattern based on count value
				pattern->Step[cur_step].R = (count & 1)? 1 : 0;
				pattern->Step[cur_step].G = (count & 2)? 1 : 0;
				pattern->Step[cur_step].B = (count & 4)? 1 : 0;
				cur_step++;
			}
			Control_RGB_LEDs(0, 1, 0);
			osDelay(g_RGB_delay/5);
			Control_RGB_LEDs(0, 0, 0);
		} else {
				if (count > 0) {
					// Complete the message
					pattern->NumSteps = cur_step;
					// Send the message
					osMessagePut(switch_msgq, (uint32_t) pattern, osWaitForever);
					// Reset button hold-time counter
					count = 0;
				}
		}
		
		if (SWITCH_PRESSED(SW2_POS)) {		
		}	else {
		}
	}
}

void Thread_RGB(void const * arg) {
	osEvent event; 
	LED_pattern_t * pattern;
	uint8_t cur_step;
	
	while (1) {
		event = osMessageGet(switch_msgq, osWaitForever);
		if (event.status == osEventMessage) {
			pattern = (LED_pattern_t *) (event.value.p);
			for (cur_step = 0; cur_step < pattern->NumSteps; cur_step++) {
				Control_RGB_LEDs(pattern->Step[cur_step].R, pattern->Step[cur_step].G, 
					pattern->Step[cur_step].B);
				osDelay(g_RGB_delay);
			}
			// Now free memory block for pattern
			osPoolFree(LED_pattern_pool_id, pattern);
			
			// Turn off LEDs
			Control_RGB_LEDs(0, 0, 0);
		}
	}
}

int main (void) {
	osKernelInitialize();
	Initialize_Ports();

	// Create memory pool
	LED_pattern_pool_id = osPoolCreate(osPool(LED_pattern_pool));

	// Create message queue
	switch_msgq = osMessageCreate(osMessageQ(switch_msgq), NULL);
	
	t_RS = osThreadCreate(osThread(Thread_Read_Switches), NULL);
	t_RGB = osThreadCreate(osThread(Thread_RGB), NULL);

	osKernelStart(); 
}


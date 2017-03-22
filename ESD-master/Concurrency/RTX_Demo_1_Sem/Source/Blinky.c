#include <cmsis_os.h>
#include <MKL25Z4.H>
#include "gpio_defs.h"

/*
Demonstrate basic task concepts: OS delay, semaphore

	Wiring Needed:
			Switch 1: SW1_POS PTD7	(J2-19)
			Switch 2: SW2_POS PTD6	(J2-17)
*/

// Thread IDs for the threads
osThreadId t_RS;                        
osThreadId t_RGB;                                            

void Thread_Read_Switches(void const * arg);
void Thread_RGB(void const * arg);


osThreadDef(Thread_Read_Switches, osPriorityNormal, 1, 0);
osThreadDef(Thread_RGB, osPriorityNormal, 1, 0);

// Synchronization
osSemaphoreId RGB_sem;
osSemaphoreDef(RGB_sem);

uint32_t g_RGB_delay=ON_TIME; 	// delay for RGB sequence

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

void Thread_RGB(void const * arg) {
	volatile static int32_t t=0;
	while (1) {
		t = osSemaphoreWait(RGB_sem, osWaitForever);
		Control_RGB_LEDs(1, 0, 0);
		osDelay(g_RGB_delay);
		Control_RGB_LEDs(0, 1, 0);
		osDelay(g_RGB_delay);
		Control_RGB_LEDs(0, 0, 1);
		osDelay(g_RGB_delay);
		Control_RGB_LEDs(0, 0, 0);
	}
}

void Thread_Read_Switches(void const * arg) {
	while (1) {
		osDelay(POLL_PERIOD);

		PTD->PTOR |= MASK(BLUE_LED_POS);
		osDelay(5);
		PTD->PTOR |= MASK(BLUE_LED_POS);
		
		if (SWITCH_PRESSED(SW1_POS)) { // Do RGB Sequence
			osSemaphoreRelease(RGB_sem);
		}
		
		if (SWITCH_PRESSED(SW2_POS)) { // Placeholder for switch 2 action
		}	else {
		}
	}
}

int main (void) {
	osKernelInitialize();
	Initialize_Ports();

	// Bug with osSemaphoreCreate: doesn't want to create binary semaphore when given 1 as max value
	// Instead creates counting semaphore and initializes it to that value
	RGB_sem = osSemaphoreCreate(osSemaphore(RGB_sem), 0);
	
	t_RS = osThreadCreate(osThread(Thread_Read_Switches), NULL);
	t_RGB = osThreadCreate(osThread(Thread_RGB), NULL);

	osKernelStart(); 
}

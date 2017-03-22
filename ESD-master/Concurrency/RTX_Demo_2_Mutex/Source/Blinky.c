#include <cmsis_os.h>
#include <MKL25Z4.H>
#include "gpio_defs.h"

#define USE_MUTEX		1

const uint32_t led_mask[] = {1UL << 18, 1UL << 19, 1UL << 1};

#define LED_RED    0
#define LED_GREEN  1
#define LED_BLUE   2

// Thread IDs for the threads
osThreadId t_RS;                        
osThreadId t_R;                        
osThreadId t_G;                        
osThreadId t_B;                        

void Thread_Read_Switches(void const * arg);
void Thread_Control_RedLED(void const * arg);
void Thread_Control_GreenLED(void const * arg);
void Thread_Control_BlueLED(void const * arg);

osThreadDef(Thread_Read_Switches, osPriorityNormal, 1, 0);
osThreadDef(Thread_Control_RedLED, osPriorityNormal, 1, 0);
osThreadDef(Thread_Control_GreenLED, osPriorityNormal, 1, 0);
osThreadDef(Thread_Control_BlueLED, osPriorityNormal, 1, 0);

// Synchronization
osMutexId LED_mutex;
osMutexDef(LED_mutex);

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
	while (1) {
		osDelay(250);
		if (SWITCH_PRESSED(SW1_POS)) { // Do RGB Sequence
		}
		
		if (SWITCH_PRESSED(SW2_POS)) {
		}	else {
		}
	}
}

void Thread_Control_RedLED (void const * arg) {
  for (;;) {
		osDelay(g_RGB_delay);
#if USE_MUTEX
		// Turning on an LED, so need to take mutex first
		osMutexWait(LED_mutex, osWaitForever);
#endif
    FPTB->PCOR = led_mask[LED_RED];
		osDelay(g_RGB_delay);
    FPTB->PSOR = led_mask[LED_RED];
#if USE_MUTEX
		// Just turned off an LED, so can release the mutex 
		osMutexRelease(LED_mutex);
#endif
  }
}

void Thread_Control_BlueLED (void const * arg) {
	osDelay(3*g_RGB_delay/2);
  for (;;) {
		osDelay(g_RGB_delay);
#if USE_MUTEX
		// Turning on an LED, so need to take mutex first
		osMutexWait(LED_mutex, osWaitForever);
#endif
    FPTD->PCOR = led_mask[LED_BLUE];
		osDelay(g_RGB_delay);
    FPTD->PSOR = led_mask[LED_BLUE];
#if USE_MUTEX
		// Just turned off an LED, so can release the mutex 
		osMutexRelease(LED_mutex);
#endif
		}
}

void Thread_Control_GreenLED (void const * arg) {
	osDelay(g_RGB_delay);
  for (;;) {
		osDelay(g_RGB_delay);
#if USE_MUTEX
		// Turning on an LED, so need to take mutex first
		osMutexWait(LED_mutex, osWaitForever);
#endif
    FPTB->PCOR = led_mask[LED_GREEN];
		osDelay(g_RGB_delay);
    FPTB->PSOR = led_mask[LED_GREEN];
#if USE_MUTEX
		// Just turned off an LED, so can release the mutex 
		osMutexRelease(LED_mutex);
#endif
  }
}



int main (void) {
	osKernelInitialize();
	Initialize_Ports();

	LED_mutex = osMutexCreate(osMutex(LED_mutex));
	
	t_RS = osThreadCreate(osThread(Thread_Read_Switches), NULL);

	t_R = osThreadCreate(osThread(Thread_Control_RedLED), NULL);
	t_G = osThreadCreate(osThread(Thread_Control_GreenLED), NULL);
	t_B = osThreadCreate(osThread(Thread_Control_BlueLED), NULL);

	osKernelStart(); 
}

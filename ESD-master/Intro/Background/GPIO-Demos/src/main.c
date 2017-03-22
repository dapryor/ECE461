/*----------------------------------------------------------------------------
GPIO Demonstrations
*----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include "gpio_defs.h"
#include "LCD_4bit.h"
/*------------------------------------------------------------------------------
	Example for switching external LEDs
	*------------------------------------------------------------------------------*/

void Basic_Light_Switching_Example(void) {
  // Enable Clock to Port A 
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;          
  
	// Make 3 pins GPIO
	PORTA->PCR[LED1_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTA->PCR[LED1_POS] |= PORT_PCR_MUX(1);          
	PORTA->PCR[LED2_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTA->PCR[LED2_POS] |= PORT_PCR_MUX(1);          
	PORTA->PCR[SW1_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTA->PCR[SW1_POS] |= PORT_PCR_MUX(1);          

	// set LED bits to outputs
  PTA->PDDR |= MASK(LED1_POS) | MASK (LED2_POS); 
	// clear Switch bit to input
  PTA->PDDR &= ~MASK(SW1_POS); 
  	// turn on LED1, turn off LED2
	PTA->PDOR = MASK(LED2_POS);  
    
  while (1) {
		if (PTA->PDIR & MASK(SW1_POS)) {
			// switch is not pressed, then light LED 2
			PTA->PDOR = MASK(LED2_POS);
		} else {
			// switch is pressed, so light LED 1
			PTA->PDOR = MASK(LED1_POS);
		}
	}
}

/*------------------------------------------------------------------------------
	Example for Flashing RGB LED on Freedom Board
	*------------------------------------------------------------------------------*/


void Delay(volatile unsigned int time_del) {
	while (time_del--) {
		;
	}
}

void Freedom_Blinking_Lights(void) {
	unsigned num;
	
	// Enable clock to ports B and D
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;
	
	// Make 3 pins GPIO
	PORTB->PCR[RED_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[RED_LED_POS] |= PORT_PCR_MUX(1);          
	PORTB->PCR[GREEN_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[GREEN_LED_POS] |= PORT_PCR_MUX(1);          
	PORTD->PCR[BLUE_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTD->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(1);          
	
	// Set ports to outputs
	PTB->PDDR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
	PTD->PDDR |= MASK(BLUE_LED_POS);

	// Turn on LEDs
	PTB->PCOR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
	PTD->PCOR |= MASK(BLUE_LED_POS);

	
	while (1) {
		for (num = 0; num < 8; num++) {
			if (num & 1)
					PTB->PSOR = MASK(RED_LED_POS);
			else
					PTB->PCOR = MASK(RED_LED_POS); 
			if (num & 2)
					PTB->PSOR = MASK(GREEN_LED_POS);
			else
					PTB->PCOR = MASK(GREEN_LED_POS); 
			if (num & 4)
					PTD->PSOR = MASK(BLUE_LED_POS);
			else
					PTD->PCOR = MASK(BLUE_LED_POS); 
			Delay(2000000);		
		}
	}
}

/*------------------------------------------------------------------------------
	Example for driving external speaker
	*------------------------------------------------------------------------------*/
void Init_Speaker( void ) {
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; /* enable clock for port C */
	PORTC->PCR[SPKR_POS] |= PORT_PCR_MUX(1);
	PTC->PDDR |= MASK(SPKR_POS); // set I/O bit -direction to output
	PTC->PDOR |= MASK(SPKR_POS); // set to 1 initially
}

void Beep(void) {
	unsigned int period=20000;
	
	while (1) {
		PTC->PTOR = MASK(SPKR_POS);
		Delay(period/2);
	}
}

void Speaker_Example(void) {
	Init_Speaker();
	Beep();
}

/*------------------------------------------------------------------------------
	Example for LCD interface
	*------------------------------------------------------------------------------*/
void LCD_Example(void) {
	Init_LCD();
	Clear_LCD();
	Set_Cursor(0,0);
	Print_LCD("     Hello, World!");
	Set_Cursor(0,1);
	Print_LCD("  Text LCD Module Test");
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {

//	Uncomment a demo

//	Basic_Light_Switch_Example();	

  Freedom_Blinking_Lights();

//	Speaker_Example();

//	LCD_Example();
	
}

// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   

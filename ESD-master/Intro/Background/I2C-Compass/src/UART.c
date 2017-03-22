#include "UART.h"
#include <stdio.h>

Q_T TxQ, RxQ;

/* BEGIN - UART0 Device Driver

	Code created by Shannon Strutz
	Date : 5/7/2014
	Licensed under CC BY-NC-SA 3.0
	http://creativecommons.org/licenses/by-nc-sa/3.0/

	Modified by Alex Dean 9/13/2016
	
*/


struct __FILE
{
  int handle;
};

FILE __stdout;  //Use with printf
FILE __stdin;		//use with fget/sscanf, or scanf


//Retarget the fputc method to use the UART0
int fputc(int ch, FILE *f){
	while(!(UART0->S1 & UART_S1_TDRE_MASK) && !(UART0->S1 & UART_S1_TC_MASK));
	UART0->D = ch;
	return ch;
}

//Retarget the fgetc method to use the UART0
int fgetc(FILE *f){
	while(!(UART0->S1 & UART_S1_RDRF_MASK));
	return UART0->D;
}

void Init_UART0(uint32_t baud_rate) {
	int sysclk = 48000000;
	// int baud = 115200;
	uint32_t osr = 15;
	uint16_t sbr;
	uint8_t temp;
	
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK; 										// enable clock gating to uart0 module 
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;											// enable clock gating to port A 	
	UART0->C2 &= ~UART0_C2_TE_MASK & ~UART0_C2_RE_MASK; 		// disable rx and tx for register programming 
	// set uart clock to oscillator clock 
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(01);
	SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;
	// set pins to uart0 rx/tx 
	PORTA->PCR[1] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Rx
	PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Tx
	
	sbr = (uint16_t)((sysclk)/(baud_rate * osr+1));
	temp = UART0->BDH & ~(UART0_BDH_SBR(0x1F));
	UART0->BDH = temp | UART_BDH_SBR(((sbr & 0x1F00) >> 8));
	UART0->BDL = (uint8_t)(sbr & UART_BDL_SBR_MASK);
	UART0->C4 |= UART0_C4_OSR(osr);													// set oversampling ratio 
	
	// keep default settings for parity and loopback
	UART0->C1 = 0;
	UART0->C3 |= 0;
	UART0->MA1 = 0;
	UART0->MA2 = 0;
	UART0->S1 |= 0x00;
	UART0->S2 = 0x00;
	UART0->C2 |= UART0_C2_TE_MASK | UART0_C2_RE_MASK;				//enable UART
	
}

/* END - UART0 Device Driver 
	Code created by Shannon Strutz
	Date : 5/7/2014
	Licensed under CC BY-NC-SA 3.0
	http://creativecommons.org/licenses/by-nc-sa/3.0/

	Modified by Alex Dean 9/13/2016
	
*/

void Init_UART2(uint32_t baud_rate) {
	uint32_t divisor;
	
	// enable clock to UART and Port A
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;


	// select UART pins
	PORTE->PCR[22] = PORT_PCR_MUX(4); // Tx
	PORTE->PCR[23] = PORT_PCR_MUX(4); // Rx
	
	UART2->C2 &=  ~(UARTLP_C2_TE_MASK | UARTLP_C2_RE_MASK);
		
	// Set baud rate to 4800 baud
	divisor = BUS_CLOCK/(baud_rate*16);
	UART2->BDH = UART_BDH_SBR(divisor>>8);
	UART2->BDL = UART_BDL_SBR(divisor);
	
	// No parity, 8 bits, two stop bits, other settings;
	UART2->C1 = 0; 
	UART2->S2 = 0;
	UART2->C3 = 0;
	
// Enable transmitter and receiver but not interrupts
	UART2->C2 = UART_C2_TE_MASK | UART_C2_RE_MASK;
	
#if USE_UART_INTERRUPTS
	NVIC_SetPriority(UART2_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(UART2_IRQn); 
	NVIC_EnableIRQ(UART2_IRQn);

//	UART2->C2 |= UART_C2_TIE_MASK | UART_C2_RIE_MASK;
	UART2->C2 |= UART_C2_RIE_MASK;
	Q_Init(&TxQ);
	Q_Init(&RxQ);
#endif

}

void UART2_Transmit_Poll(uint8_t data) {
		while (!(UART2->S1 & UART_S1_TDRE_MASK))
			;
		UART2->D = data;
}	

uint8_t UART2_Receive_Poll(void) {
		while (!(UART2->S1 & UART_S1_RDRF_MASK))
			;
		return UART2->D;
}	

void UART2_IRQHandler(void) {
	NVIC_ClearPendingIRQ(UART2_IRQn);
	if (UART2->S1 & UART_S1_TDRE_MASK) {
		// can send another character
		if (!Q_Empty(&TxQ)) {
			UART2->D = Q_Dequeue(&TxQ);
		} else {
			// queue is empty so disable transmitter
			UART2->C2 &= ~UART_C2_TIE_MASK;
		}
	}
	if (UART2->S1 & UART_S1_RDRF_MASK) {
		// received a character
		if (!Q_Full(&RxQ)) {
			Q_Enqueue(&RxQ, UART2->D);
		} else {
			// error - queue full.
			while (1)
				;
		}
	}
	if (UART2->S1 & (UART_S1_OR_MASK |UART_S1_NF_MASK | 
		UART_S1_FE_MASK | UART_S1_PF_MASK)) {
			// handle the error
			// clear the flag
			/*
			UART2->S1 = UART_S1_OR_MASK | UART_S1_NF_MASK | 
				UART_S1_FE_MASK | UART_S1_PF_MASK;
			*/
		}
}
void Send_String(uint8_t * str) {
	// enqueue string
	while (*str != '\0') { // copy characters up to null terminator
		while (Q_Full(&TxQ))
			; // wait for space to open up
		Q_Enqueue(&TxQ, *str);
		str++;
	}
	// start transmitter if it isn't already running
	if (!(UART2->C2 & UART_C2_TIE_MASK)) {
		UART2->C2 |= UART_C2_TIE_MASK;
		//		UART2->D = Q_Dequeue(&TxQ); 
	}
}

uint32_t Get_Num_Rx_Chars_Available(void) {
	return Q_Size(&RxQ);
}

uint8_t	Get_Char(void) {
	return Q_Dequeue(&RxQ);
}
// *******************************ARM University Program Copyright � ARM Ltd 2013*************************************   

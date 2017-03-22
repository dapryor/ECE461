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
	
	SIM->SCGC4 |= 0x0400; /* enable clock for UART0 */
	SIM->SOPT2 |= 0x04000000; /* use FLL output for UART Baud rate generator */
	UART0->C2 = 0; /* turn off UART0 while changing configurations */

	UART0->BDH = 0x00;
	UART0->BDL = 0x1A; /* 115200 Baud */
	UART0->C4  = 0x0F; /* Over Sampling Ratio 16 */
	UART0->C1  = 0x00; /* 8-bit data */
	UART0->C2  = 0x24; /* enable receive and receive interrupt*/
	UART0->C2 |= 0x08; /* enable transmit */	
	//	NVIC->ISER[0] |= 0x00001000; /* enable INT12 (bit 12 of ISER[0]) */
	SIM->SCGC5 |= 0x0200; /* enable clock for PORTA */
	PORTA->PCR[1] = 0x0200; /* make PTA1 UART0_Rx pin */
	PORTA->PCR[2] = 0x0200; /* make PTA2 UART0_Tx pin */
  
	Q_Init(&TxQ);
	Q_Init(&RxQ);

}

/* END - UART0 Device Driver 
	Code created by Shannon Strutz
	Date : 5/7/2014
	Licensed under CC BY-NC-SA 3.0
	http://creativecommons.org/licenses/by-nc-sa/3.0/

	Modified by Alex Dean 9/13/2016
	
*/

void UART0_IRQHandler(void){
#if 0
	if (UART0->S1 & UART_S1_TDRE_MASK) {
		// can send another character
		if (!Q_Empty(&TxQ)) {
			UART0->D = Q_Dequeue(&TxQ);
		} else {
			// queue is empty so disable transmitter
			UART0->C2 &= ~UART_C2_TIE_MASK;
		}
	}
	if (UART0->S1 & UART_S1_RDRF_MASK) {
		// received a character
		Q_Enqueue(&RxQ, UART0->D);
		// NMEA_Receive();
	}
#else
	if (UART0->S1 & UART0_S1_RDRF_MASK) {
		// received a character
		Q_Enqueue(&RxQ, UART0->D);
	}
	if ( (UART0->C2 & UART0_C2_TIE_MASK) && // transmitter interrupt enabled
			(UART0->S1 & UART0_S1_TDRE_MASK) ) { // tx buffer empty
		// can send another character
		if (!Q_Empty(&TxQ)) {
			UART0->D = Q_Dequeue(&TxQ);
		} else {
			// queue is empty so disable transmitter interrupt
			UART0->C2 &= ~UART0_C2_TIE_MASK;
		}
	}
#endif
}

#if 0
//function begins parsing through NMEA Sentence to check if valid
void NMEA_Receive(void){
	
	int status = INT;
	
	if ((!Q_Full(&RxQ)) && Buffer_State==1 )status = Buffer_State1();

		// check if "$", first state
		if(Buffer_State == INT){
			if(RxQ.Data[RxQ.Tail-1] == '$'){
			Buffer_State = 1;
			IRQ_NMEA_State = 1;
			}
			//reset buffer, empty
			else {
				clear_buffer(&RxQ);
			}
		}
		if(status != INT) send_error(status-1);
}
	


//function parses through body of NMEA sentence to check if valid
int Buffer_State1(void){
	
	int status = INT;
	static int count = INT;
	
				//checksum
			if(IRQ_NMEA_State == 3){
				count++;
				if(NMEA_valid(RxQ.Data[RxQ.Tail-1], 3)==0){
					Buffer_State = 0;
					IRQ_NMEA_State = 0;
					clear_buffer(&RxQ);
					status = 5;
				}
				if(count == 2){
					count = 0;
					if(checksum(&RxQ.Data[0])){
						Buffer_State = 2;
					}
					else{
						Buffer_State = 0;
						IRQ_NMEA_State = 0;
						clear_buffer(&RxQ);
						status = 4;
					}
				}
			}
			// check if Body of NMEA sentence is valid
			if(IRQ_NMEA_State == 2){
				if(NMEA_valid(RxQ.Data[RxQ.Tail-1],2) ==0){
					Buffer_State = 0;
					IRQ_NMEA_State = 0;
					clear_buffer(&RxQ);
					status = 3;
				}
				// "*" to advance
				if(RxQ.Data[RxQ.Tail-1] == '*') IRQ_NMEA_State = 3;
			}
			// check if first 5 characters of NMEA sentence are valid
			if(IRQ_NMEA_State == 1){
				if(NMEA_valid(RxQ.Data[RxQ.Tail-1],1)==0){
					Buffer_State = 0;
					IRQ_NMEA_State = 0;
					clear_buffer(&RxQ);
					status = 2;
				}
				// "," to advance
				if(RxQ.Data[RxQ.Tail-1] == ',') IRQ_NMEA_State = 2;
			}
			return status;
	}
#endif

	void Send_String(uint8_t * str) {
	// enqueue string
	while (*str != '\0') { // copy characters up to null terminator
		while (Q_Full(&TxQ))
			; // wait for space to open up
		Q_Enqueue(&TxQ, *str);
		str++;
	}
	// start transmitter if it isn't already running
	if (!(UART0->C2 & UART_C2_TIE_MASK)) {
		UART0->C2 |= UART_C2_TIE_MASK;
		UART0->D = Q_Dequeue(&TxQ); 
	}
}

uint32_t Get_Num_Rx_Chars_Available(void) {
	return Q_Size(&RxQ);
}

uint8_t	Get_Char(void) {
	return Q_Dequeue(&RxQ);
}
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   

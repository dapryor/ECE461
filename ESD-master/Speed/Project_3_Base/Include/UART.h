#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <MKL25Z4.H>
#include "queue.h"

#define USE_UART_INTERRUPTS (1)

#define UART_OVERSAMPLE (16)
#define BUS_CLOCK 			(24e6)


extern char RMC_Speed[16];
extern char RMC_Track[16];
extern char VHW_Speed[16];
extern char VHW_Track[16];

extern Q_T TxQ, RxQ;
extern int volatile Buffer_State;

void Init_UART0(uint32_t baud_rate);

void Send_String(uint8_t * str);
uint32_t Get_Num_Rx_Chars_Available(void);
uint8_t	Get_Char(void);

extern Q_T Tx_Data, Rx_Data;




#endif
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   

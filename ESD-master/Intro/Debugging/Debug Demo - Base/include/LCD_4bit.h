#include <MKL25Z4.H>

#define LCD_COLUMNS   16  // Number of LCD columns in characters
#define LCD_ROWS     	2   // Number of LCD rows

#define USE_LCD				0 	// Set to 1 to use the LCD, 0 otherwise

/*-------------------- LCD interface hardware definitions --------------------*/

/* Connections from LCD to MCU port bits: 
 DB4 through DB8 are contiguous, starting with LSB at bit position PIN_DATA_SHIFT
 
  For example:
   - DB4 = PTD0
   - DB5 = PTD1
   - DB6 = PTD2
   - DB7 = PTD3
	 
   - E   = PTD4
   - RW  = PTA13
   - RS  = PTD5                                                              */

#define PIN_DATA_PORT					PORTC
#define PIN_DATA_PT						PTC
#define PIN_DATA_SHIFT				( 3 )

#define PIN_E_PORT						PORTC
#define PIN_E_PT							PTC
#define PIN_E_SHIFT						( 7 )
#define PIN_E                 ( 1 << PIN_E_SHIFT)

#define PIN_RW_PORT						PORTC
#define PIN_RW_PT							PTC
#define PIN_RW_SHIFT					( 8 )
#define PIN_RW                ( 1 << PIN_RW_SHIFT)

#define PIN_RS_PORT						PORTC
#define PIN_RS_PT							PTC
#define PIN_RS_SHIFT	        ( 9 )
#define PIN_RS                ( 1 << PIN_RS_SHIFT)

#define PINS_DATA             (0x0F << PIN_DATA_SHIFT)

/* Enable Clock for peripheral driving LCD pins                               */
#define ENABLE_LCD_PORT_CLOCKS   	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTC_MASK;	

#define SET_LCD_E(x)              if (x) {PIN_E_PT->PSOR = PIN_E;} else {PIN_E_PT->PCOR = PIN_E;}
#define SET_LCD_RW(x)             if (x) {PIN_RW_PT->PSOR = PIN_RW;} else {PIN_RW_PT->PCOR = PIN_RW;}
#define SET_LCD_RS(x)             if (x) {PIN_RS_PT->PSOR = PIN_RS;} else {PIN_RS_PT->PCOR = PIN_RS;}

#define SET_LCD_DATA_OUT(x)       PIN_DATA_PT->PDOR = (PIN_DATA_PT->PDOR & ~PINS_DATA) | ((x) << PIN_DATA_SHIFT);
#define GET_LCD_DATA_IN           (((PIN_DATA_PT->PDIR & PINS_DATA) >> PIN_DATA_SHIFT) & 0x0F)

/* Setting all pins to output mode                                            */
#define SET_LCD_ALL_DIR_OUT       { PIN_DATA_PT->PDDR = PIN_DATA_PT->PDDR | PINS_DATA; \
																PIN_E_PT->PDDR = PIN_E_PT->PDDR | PIN_E; \
																PIN_RW_PT->PDDR = PIN_RW_PT->PDDR | PIN_RW; \
																PIN_RS_PT->PDDR = PIN_RS_PT->PDDR | PIN_RS; }

/* Setting DATA pins to input mode                                            */
#define SET_LCD_DATA_DIR_IN       PIN_DATA_PT->PDDR = PIN_DATA_PT->PDDR & ~PINS_DATA;

/* Setting DATA pins to output mode                                           */
#define SET_LCD_DATA_DIR_OUT      PIN_DATA_PT->PDDR = PIN_DATA_PT->PDDR | PINS_DATA;

#define LCD_BUSY_FLAG_MASK				(0x80)

/******************************************************************************/
void Init_LCD (void);
void Set_Cursor (uint8_t column, uint8_t row);
void Clear_LCD(void);
void Print_LCD (char *string);
void lcd_putchar (char c);


#include <RTL.h>
#include <MKL25Z4.H>

// Task IDs for the tasks
OS_TID t_TRL;                        
OS_TID t_TGL;                        
OS_TID t_TBL;                        

#define DEMO_INDEPENDENT 0 // R, G, B run independently.
#define DEMO_EVENTS_AND  1 // Red and Blue run independently, triggering Green with AND event
#define DEMO_EVENTS_OR  0 // Red and Blue run independently, triggering Green with OR event


#define LED_NUM     3                   /* Number of user LEDs                */
const uint32_t led_mask[] = {1UL << 18, 1UL << 19, 1UL << 1};

#define LED_RED    0
#define LED_GREEN  1
#define LED_BLUE   2

#define WAIT_FOREVER 0xffff

#if DEMO_EVENTS_AND | DEMO_EVENTS_OR
#define EV_FLAG_RED 1
#define EV_FLAG_BLUE 2
#endif

/*----------------------------------------------------------------------------
  Function that initializes LEDs
 *----------------------------------------------------------------------------*/
void LED_init(void) {
  SIM->SCGC5    |= (1UL <<  10) | (1UL <<  12);      /* Enable Clock to Port B & D */ 
  PORTB->PCR[18] = (1UL <<  8);                      /* Pin PTB18 is GPIO */
  PORTB->PCR[19] = (1UL <<  8);                      /* Pin PTB19 is GPIO */
  PORTD->PCR[1]  = (1UL <<  8);                      /* Pin PTD1  is GPIO */

  FPTB->PDOR = (led_mask[0] | 
               led_mask[1] );          /* switch Red/Green LED off  */
  FPTB->PDDR = (led_mask[0] | 
               led_mask[1] );          /* enable PTB18/19 as Output */

  FPTD->PDOR = led_mask[2];            /* switch Blue LED off  */
  FPTD->PDDR = led_mask[2];            /* enable PTD1 as Output */
}

__task void Toggle_RedLED (void) {
	os_itv_set(2000);
  for (;;) {
		os_itv_wait();
    FPTB->PTOR = led_mask[LED_RED];
#if DEMO_EVENTS_AND | DEMO_EVENTS_OR
		os_evt_set(EV_FLAG_RED, t_TGL);
#endif
  }
}

__task void Toggle_BlueLED (void) {
	os_dly_wait(250);
	os_itv_set(3000);
  for (;;) {
		os_itv_wait();
    FPTD->PTOR = led_mask[LED_BLUE];
#if DEMO_EVENTS_AND | DEMO_EVENTS_OR
		os_evt_set(EV_FLAG_BLUE, t_TGL); 
#endif
		}
}

__task void Toggle_GreenLED (void) {
#if DEMO_INDEPENDENT
	os_dly_wait(500);
#endif
	os_itv_set(1000);
  for (;;) {
#if DEMO_INDEPENDENT
		os_itv_wait();
#endif
#if DEMO_EVENTS_AND
		os_evt_wait_and(EV_FLAG_RED | EV_FLAG_BLUE, WAIT_FOREVER);
#endif
#if DEMO_EVENTS_OR
		os_evt_wait_or(EV_FLAG_RED | EV_FLAG_BLUE, WAIT_FOREVER);
#endif
    FPTB->PTOR = led_mask[LED_GREEN];
  }
}

__task void init (void) {
	
	t_TRL = os_tsk_create( Toggle_RedLED, 0);
	t_TBL = os_tsk_create( Toggle_BlueLED, 0);
	t_TGL = os_tsk_create( Toggle_GreenLED, 0);

  os_tsk_delete_self ();
}

int main (void) {
  LED_init();                              /* Initialize the LEDs           */
  os_sys_init(init);                        /* Initialize RTX and start init */
}

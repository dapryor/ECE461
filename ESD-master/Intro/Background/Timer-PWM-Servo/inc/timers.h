#ifndef TIMERS_H
#define TIMERS_H
#include "MKL25Z4.h"

#define PWM_PERIOD (30000) // 20 ms
#define PWM_WIDTH_RANGE (PWM_PERIOD/20)
#define T_NEUTRAL (PWM_PERIOD * 1.5 / 20 ) // 1.5 ms
#define T_MAX_CCW (PWM_PERIOD * 1 / 20 ) // 1 ms
#define T_MAX_CW (PWM_PERIOD * 2 / 20 ) // 2 ms

#define POS_RES			(1000)
#define POS_NEUTRAL (0)
#define POS_MAX_CCW (-POS_RES/2)
#define POS_MAX_CW 	(POS_RES/2)

void Init_PIT(unsigned period);
void Start_PIT(void);
void Stop_PIT(void);

void Init_PWM(void);
void Set_PWM_Value(uint16_t channel, uint16_t duty);

// pos ranges from -1K to 1K
void Set_Servo_Position(uint16_t channel, int32_t pos);

void Init_SysTick(void);
void SysTick_Handler(void);

#endif
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   

#ifndef HMC5883_H
#define HMC5883_H

#include <MKL25Z4.H>
#include <math.h>
#include <stdint.h>

#define DEGREE_CODE (248)

#define HMC_ADDR 0x3C


#define REG_CRA (0)
#define REG_CRB (1)
#define REG_MODE (2)
#define REG_DATA (3)

#define REG_STATUS (9)
#define REG_ID_A (10)
#define REG_ID_B (11)
#define REG_ID_C (12)


#define REG_WHOAMI 0x0D
#define REG_CTRL1  0x2A
#define REG_CTRL4  0x2D

#define WHOAMI 0x1A

#define M_PI (3.14159265)

int init_compass(void);
void read_compass_xzy(int16_t * mag);
float calc_heading(int16_t * mag);
int16_t calc_heading_dd(int16_t * mag);

#endif

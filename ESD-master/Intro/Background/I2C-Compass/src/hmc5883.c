#include <MKL25Z4.H>
#include "hmc5883.h"
#include "i2c.h"
#include "delay.h"
#include <math.h>

#if 0
int16_t acc_X=0, acc_Y=0, acc_Z=0;
float roll=0.0, pitch=0.0;

//mma data ready
extern uint32_t DATA_READY;
#endif


//initializes sensor
//i2c has to already be enabled
int init_compass()
{
	volatile char id[4];
	
	//check for device
	id[0] = i2c_read_byte(HMC_ADDR, REG_ID_A);
	Delay(20);
	id[1] = i2c_read_byte(HMC_ADDR, REG_ID_B);
	Delay(20);
	id[2] = i2c_read_byte(HMC_ADDR, REG_ID_C);
	Delay(20);
	id[3] = '\0';
#if 0
	if (strcmp(id,"H43")
		return 0;
#endif
	
	//set 8 average, 15 Hz, normal measurement
	i2c_write_byte(HMC_ADDR, REG_CRA, 0x70);
	// Gain = 5
	i2c_write_byte(HMC_ADDR, REG_CRB, 0xA0);
	//set continuous measurement mode
	i2c_write_byte(HMC_ADDR, REG_MODE, 0x0);
	Delay(100);

	return 1;
}

void read_compass_xzy(int16_t * mag)
{
	int i;
	uint8_t data[6];
	int16_t temp[3];
	
	i2c_start();
	i2c_read_setup(HMC_ADDR , REG_DATA);
	
	// Read five bytes in repeated mode
	for( i=0; i<5; i++)	{
		data[i] = i2c_repeated_read(0);
	}
	// Read last byte ending repeated mode
	data[i] = i2c_repeated_read(1);
	
	for ( i=0; i<3; i++ ) {
		temp[i] = (int16_t) ((data[2*i]<<8) | data[2*i+1]);
		mag[i] = temp[i];
	}
}

float calc_heading(int16_t * mag) {
	float h;
	// assumes compass is completely flat!
	h = atan2f(mag[2], mag[0])*180/M_PI;
	if (h<0)
		h += 360;
	return h;
}

int16_t calc_heading_dd(int16_t * mag) {
	float h;
	// assumes compass is completely flat!
	h = atan2f(mag[2], mag[0])*1800/M_PI;
	if (h<0)
		h += 3600;
	return (int16_t) h;
}

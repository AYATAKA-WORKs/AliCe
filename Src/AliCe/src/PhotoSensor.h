/*
 * PhotoSensor.h
 *
 *  Created on: 2017/09/14
 *      Author: KOTA-NPC2
 */

#ifndef PHOTOSENSOR_H_
#define PHOTOSENSOR_H_

#define SEN_F	0
#define SEN_M	1
#define SEN_S	2

class PhotoSensor {
private:
	void ADCenable(void);
	void ADCdisable(void);
public:
	int value[6];
	int value_on[6];
	int value_off[6];
	int buf[6];
	int dev[6];
	int ring_buf[6][5];
	int int_dev[6];
	int int_dev_buf[6][6];
	void getValue(char ch);
	void readSensor(void);
	void getIntDev(void);
};

#endif /* PHOTOSENSOR_H_ */

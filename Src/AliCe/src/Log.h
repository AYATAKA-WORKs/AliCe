/*
 * Log.h
 *
 *  Created on: 2017/09/24
 *      Author: KOTA-NPC2
 */

#ifndef LOG_H_
#define LOG_H_

class Log{
public:
	float buf1[2000];
	float buf2[2000];
	unsigned short num1,num2,num3,num4;
	union {
		unsigned char byte;
		struct {
			unsigned char photo_SS	: 1;	// LSB
			unsigned char photo_S	: 1;
			unsigned char photo_F	: 1;
			unsigned char velocity	: 1;
			unsigned char dps		: 1;
			unsigned char deg		: 1;
			unsigned char 			: 2;	// MSB
		} bit;
	} flag;
	void setLog(float value1);
	void setLog(float value1,float value2);
};



#endif /* LOG_H_ */

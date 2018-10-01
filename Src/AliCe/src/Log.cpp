/*
 * Log.cpp
 *
 *  Created on: 2017/10/03
 *      Author: KOTA-NPC2
 */
#include "Log.h"


void Log::setLog(float value1){
	if(num1 < 2000){
		buf1[num1] = value1;
		num1 ++;
	}
}

void Log::setLog(float value1,float value2){
	if(num2 < 2000){
		buf1[num2] = value1;
		buf2[num2] = value2;
		num2 ++;
	}
}

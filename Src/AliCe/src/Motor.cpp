#include "Motor.h"
#include "iodefine.h"
#include "initSetting.h"
#include "parameter.h"

Motor::Motor(char _LorR){
	LorR = _LorR;
}
void Motor::setParameter(char _LorR){
	if(_LorR == L){
		LorR = L;
	}
	else {
		LorR = R;
	}
}

void Motor::cw(void){
	M_STBY = 1;
	if(LorR == L){
		L_IN1 = 0;
		L_IN2 = 1;;	// L_PWM
	}
	else{
		R_IN1 = 1;
		R_IN2 = 0;	// R_PWM
	}
}

void Motor::ccw(void){
	M_STBY = 1;
	if(LorR == L){
		L_IN1 = 1;
		L_IN2 = 0;	// L_PWM
	}
	else{
		R_IN1 = 0;
		R_IN2 = 1;	// R_PWM
	}
}

void Motor::enable(void){
	M_STBY = 1;
	if(LorR == L){
		CSTR(TPU4) = 1;	// L_PWM
	}
	else{
		CSTR(MTU4) = 1;	// R_PWM
	}
}

void Motor::disable(void){
	M_STBY = 0;
	if(LorR == L){
		CSTR(TPU4) = 0;	// L_PWM
	}
	else{
		CSTR(MTU4) = 0;	// R_PWM
	}
}

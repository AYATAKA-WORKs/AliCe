#include "Encoder.h"
#include "iodefine.h"
#include "parameter.h"
//Encoder::Encoder(char _LorR, float _tire){
//	LorR = _LorR;
//	tire = _tire;
//	rpm = 0.0;
//	velocity = 0.0;
//	distance = 0.0;
//}


void Encoder::setParameter(char _LorR){
	if(_LorR == L){
		LorR = L;
		tire = TIRE_L;
	}
	else {
		LorR = R;
		tire = TIRE_L;
	}
}

float Encoder::getRpm(void){
	if(LorR == L){	// L_ENC
		CSTR(MTU2) = 0;
		if (MTU2.TCNT >= 32767)
			rpm = (MTU2.TCNT - 32767) / (4096.0*4) * 1000.0 * 60.0;
		else
			rpm = (32767 - MTU2.TCNT) / (4096.0*4) * 1000.0 * 60.0;
		CSTR(MTU2) = 1;
		MTU2.TCNT = 32767;
	}
	else if(LorR == R){		// R_ENC
		CSTR(MTU1) = 0;
		if (MTU1.TCNT >= 32767)
			rpm = (MTU1.TCNT - 32767) / (4096.0*4) * 1000.0 * 60.0;
		else
			rpm = (32767 - MTU1.TCNT) / (4096.0*4) * 1000.0 * 60.0;
		CSTR(MTU1) = 1;
		MTU1.TCNT = 32767;
	}
	return rpm;
}

float Encoder::getVelocity(void){
	if(LorR == L){	// L_ENC
		CSTR(MTU2) = 0;
		velocity = (32767 - MTU2.TCNT) / (4096.0*4.0) * 1000.0 * (11.0/40.0) * tire * 3.1415;
		CSTR(MTU2) = 1;
		MTU2.TCNT = 32767;
	}
	else{		// R_ENC
		CSTR(MTU1) = 0;
		velocity = (MTU1.TCNT - 32767) / (4096.0*4.0) * 1000.0 * (11.0/40.0) * tire * 3.1415;
		CSTR(MTU1) = 1;
		MTU1.TCNT = 32767;
	}
	return velocity;
}

float Encoder::getDistance(float dt){
	distance += velocity * dt;
	return distance;
}

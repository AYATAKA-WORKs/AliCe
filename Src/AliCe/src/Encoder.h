#pragma once

class Encoder {
private : 
	volatile char LorR;
	volatile float tire;
	float rpm;
public :
	float velocity;
	float distance;
	//Encoder(char _LorR, float _tire);
	void setParameter(char _LorR);
	float getRpm(void);
	float getVelocity(void);
	float getDistance(float dt);
};

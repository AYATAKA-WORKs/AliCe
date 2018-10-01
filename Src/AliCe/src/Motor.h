#pragma once

class Motor {
private :
	volatile char LorR;
public	:
	Motor(){}
	Motor(char _LorR);
	void setParameter(char _LorR);
	void cw(void);
	void ccw (void);
	void enable(void);
	void disable(void);
};
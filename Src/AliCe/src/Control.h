#pragma once

#include <math.h>
#include "Encoder.h"
#include "Gyro.h"
#include "Motor.h"
#include "PhotoSensor.h"
#include "initSetting.h"

class Control {
public :
	float ve_error_l[2], ve_s_error_l, ve_d_error_l;
	float ve_error_r[2], ve_s_error_r, ve_d_error_r;
	float gy_error[2], gy_s_error, gy_d_error;
	float ph_error[2][6], ph_d_error[6];
	Encoder enc_l, enc_r;
	Gyro gyro;
	PhotoSensor photo;
	union {
		unsigned short byte;
		struct {
			unsigned short enable_pwm	: 1;	// LSB
			unsigned short enable_enc 	: 1;
			unsigned short enable_gyro	: 1;
			unsigned short enable_phot	: 1;
			unsigned short straight		: 1;
			unsigned short slalom_l		: 1;
			unsigned short slalom_r		: 1;
			unsigned short turn_l		: 1;	// 8bit
			unsigned short turn_r		: 1;
			unsigned short back			: 1;
			unsigned short diagonal		: 1;
			unsigned short adati_flag	: 1;
			unsigned short wall_fix		: 1;
			unsigned short falesafe		: 1;
			unsigned short wallfix_S	: 1;
			unsigned short wallfix_SS	: 1;	// MSB
		} bit;
	} flag;
	float duty_l, duty_r;
	void getControlVelocity(float tar_velocity_l, float tar_velocity_r);
	void getControlDsp(float tar_dsp);
	float getControlWall(void);
	float getControlDiag(void);
	void getControlDuty(float tar_velocity_l, float tar_velocity_r, float tar_dsp);
	void detectFaleSafe(void);
	void satartSuction(void);
	void stopSuction(void);
};

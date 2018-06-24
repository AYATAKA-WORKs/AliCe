#ifndef RUN_H_
#define RUN_H_

#include <math.h>
#include "iodefine.h"
#include "initSetting.h"
#include "Control.h"
#include "parameter.h"

class Run : public Control{
private :
	char ref_flag, fix_flag;
	float TrapezoidalAcceleration(float accel_up, float accel_down, float start_velocity, float end_velocity, float max_velocity, float tar_distance, float n_distance);
	void Slalom(float _deg_a, float _velocity, float offset_u,float offset_d,float b_deg, float e_deg, float tar_deg);
	float cal_accel_distance(volatile float accel, volatile float init_velocity,volatile float max_velocity);
	void fixDistance(float tar_distance);
public :
	void setParameter(void);
	void initValue(void);
	void straight(float _accel, float start_velocity, float end_velocity, float max_velocity, float _distance);
	void back(float _accel,float start_velocity, float end_velocity, float max_velocity, float _distance);
	void s_turn(TurnParam param);
	//void s_turn180(float _velocity, char direction);
	void turn(float max_dps, float _deg, char direction);
	
	volatile float velocity_l,velocity_r;
	volatile float velocity_th, distance_th, accel;
	volatile float velocity_me, distance_me;
	volatile float deg, deg_v, deg_a;
	char end_flag;
	Motor motor_l, motor_r;
};


#endif /* RUN_H_ */

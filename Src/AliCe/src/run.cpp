#include "run.h"
#include "var.h"
#include "Serial.h"
#include "parameter.h"
#include "interrupt.h"

void Run::setParameter(void){
	enc_l.setParameter(L);
	enc_r.setParameter(R);
	motor_l.setParameter(L);
	motor_r.setParameter(R);
}

void Run::initValue(void){
	enc_l.velocity = 0.0;
	enc_r.velocity = 0.0;
	gyro.z_dps = 0.0;
	ve_s_error_l = 0.0;
	ve_s_error_r = 0.0;
	ve_error_l[0] = 0.0;
	ve_error_r[0] = 0.0;
	ve_error_l[1] = 0.0;
	ve_error_r[1] = 0.0;
	ve_d_error_l = 0.0;
	ve_d_error_r = 0.0;
	gy_s_error = 0.0;
	gy_error[0] = 0.0;
	gy_error[1] = 0.0;
	gy_d_error = 0.0;
	accel = 0.0;
	velocity_th = 0.0;
	distance_th = 0.0;
	deg_a = 0.0;
	deg_v = 0.0;
}

void Run::fixDistance(float tar_distance){
	static float ref[2];
	static float def[2];
	if(flag.bit.wallfix_SS == 1){
		if(flag.bit.adati_flag == 1){
			if((tar_distance - distance_th) <180 && ref_flag==0){
				ref_flag = 1;
				ref[0] = photo.value[2];
				ref[1] = photo.value[3];
			}
			if((tar_distance - distance_th) <140 && ref_flag==1 && fix_flag==0){
				 def[0]= abs(ref[0] - photo.value[2]);
				 def[1]= abs(ref[1] - photo.value[3]);
				 if(def[0] < 10 || def[1] < 10){
					 fix_flag = 1;
				 }
			}
		}
		else{
			if((tar_distance - distance_th) <90 && fix_flag==0 && flag.bit.straight == 1){
				 fix_flag = 1;
			}
		}
		if((photo.int_dev[3] > SEN_RSS_FIX_DEV) && (fix_flag == 1) && (end_flag != 1)){
			distance_th = tar_distance - FIX_DISTANCE_R;
			end_flag = 1;
			LED04 = 1;
		}
		else if((photo.int_dev[2] > SEN_LSS_FIX_DEV) && (fix_flag == 1) && (end_flag != 1)){
			distance_th = tar_distance - FIX_DISTANCE_L;
			end_flag = 1;
			LED01 = 1;
		}
	}
	else if(flag.bit.wallfix_S == 1){
		float dis_ref;
		if(flag.bit.diagonal == 1) dis_ref = 180;
		else dis_ref = 65;
		if((tar_distance - distance_th) < dis_ref && fix_flag==0 && flag.bit.straight == 1){
			 fix_flag = 1;
		}
		if((photo.int_dev[1] > SEN_LS_FIX_DEV) && (fix_flag == 1) && (end_flag != 1)){
			if(flag.bit.diagonal == 1) distance_th = tar_distance - FIX_DISTANCE_L3;
			else distance_th = tar_distance - FIX_DISTANCE_L2;
			end_flag = 1;
			LED01 = 1;
		}
		else if((photo.int_dev[4] > SEN_RS_FIX_DEV) && (fix_flag == 1) && (end_flag != 1)){
			if(flag.bit.diagonal == 1) distance_th = tar_distance - FIX_DISTANCE_R3;
			else distance_th = tar_distance - FIX_DISTANCE_R2;
			end_flag = 1;
			LED04 = 1;
		}
	}
}

float Run::cal_accel_distance(volatile float _accel, volatile float init_velocity, volatile float max_velocity){
	float accel_distance;
	accel_distance = (max_velocity*max_velocity - init_velocity*init_velocity) / _accel / 2.0;
	return accel_distance;
}

float Run::TrapezoidalAcceleration(float accel_up, float accel_down, float start_velocity, float end_velocity, float max_velocity, float tar_distance, float n_distance){
	float x_accel_up, x_const, x_accel_down, tri_dis;
	float _accel;

	x_accel_up = cal_accel_distance(accel_up, start_velocity, max_velocity);
	x_accel_down = cal_accel_distance(accel_down, end_velocity, max_velocity);
	x_const = tar_distance - x_accel_up - x_accel_down;
	tri_dis = (2 * accel_down * tar_distance +  end_velocity * end_velocity - start_velocity * start_velocity) / (2*(accel_up + accel_down));

	if(tar_distance >= x_accel_up + x_accel_down){
		if (n_distance < x_accel_up) _accel = accel_up;
		else if(n_distance >= x_accel_up && n_distance <= x_accel_up + x_const) _accel = 0;
		else if(n_distance <= tar_distance && n_distance >= tar_distance - x_accel_down) _accel = -accel_down;
	}
	else{
		if(n_distance < tri_dis) _accel = accel_up;
		else if(n_distance >= tri_dis && n_distance <= tar_distance) _accel = -accel_down;
	}
	if (n_distance >= tar_distance || velocity_th < 0.0 || deg_v < 0.0) {
		_accel = 0.0;
		velocity_th = end_velocity;
		deg_v = end_velocity;
		flag.bit.straight = 0;		// flag disable
		flag.bit.back = 0;			// flag disable
		flag.bit.turn_l = 0;		// flag disable
		flag.bit.turn_r = 0;		// flag disable
	}
	return _accel;
}

void Run::Slalom(float _deg_a, float _velocity, float offset_u,float offset_d,float b_deg, float e_deg, float tar_deg){
	// initialize
	velocity_th = _velocity;
	deg = 0.0;
	deg_v = 0.0;
	// clothoid up
	flag.bit.enable_phot = 0;
	while(deg < b_deg) {
		deg_a = _deg_a;
		// falsesafe process
		if(flag.bit.falesafe == 1){
			flag.byte &= 0xe000;
			break;
		}
	}
	// round
	flag.bit.enable_phot = 0;
	while(deg < e_deg) {
		deg_a = 0.0;
		// falsesafe process
		if(flag.bit.falesafe == 1){
			flag.byte &= 0xe000;
			break;
		}
	}
	// clothoid down
	flag.bit.enable_phot = 0;
	while(deg < tar_deg) {
		deg_a = -_deg_a;
		if(deg_v <= 0.0) {
			deg_v = 0.0;
			deg_a = 0.0;
			break;
		}
		// falsesafe process
		if(flag.bit.falesafe == 1){
			flag.byte &= 0xe000;
			break;
		}
	}
	deg_a = 0.0;
	deg_v = 0.0;
	flag.bit.diagonal = 0;
	distance_th = 0.0;
}

void Run::straight(float _accel,float start_velocity, float end_velocity, float max_velocity, float _distance){
	flag.byte |= 0x07;		// flag enable
	flag.bit.straight = 1;	// straight enable
	gyro.Kf = 1.0;
	if(start_velocity <= 0.01){
		MTU1.TCNT = 32767;	// clear  enc counter
		MTU2.TCNT = 32767;	// clear  enc counter
	}
	motor_l.enable();		// left motor on
	motor_r.enable();		// right motor on
	if(flag.bit.adati_flag == 1 && (~flag.bit.slalom_l || ~flag.bit.slalom_r)) {
		distance_th = 0.0;
		distance_me = 0.0;
	}
	else if(flag.bit.adati_flag != 1){
		distance_th = 0.0;
		distance_me = 0.0;
	}
	while(1) {
		accel = TrapezoidalAcceleration(_accel, _accel, start_velocity, end_velocity, max_velocity, _distance, distance_th);
		fixDistance(_distance);
		if(flag.bit.straight == 0) break;
		// falsesafe process
		if(flag.bit.falesafe == 1){
			flag.byte &= 0xe000;
			break;
		}
	}
	if(velocity_th < 0.0) {
		velocity_th = 0.0;
	}
	end_flag = 0;
	ref_flag = 0;
	fix_flag = 0;
	motor_l.disable();		// left motor off
	motor_r.disable();		// right motor off
}

void Run::back(float _accel,float start_velocity, float end_velocity, float max_velocity, float _distance){
	flag.byte |= 0x07;		// flag enable
	flag.bit.back = 1;		// back enable
	gyro.Kf = 1.0;
	if(start_velocity <= 0.01){
		MTU1.TCNT = 32767;	// clear  enc counter
		MTU2.TCNT = 32767;	// clear  enc counter
	}
	motor_l.enable();		// left motor on
	motor_r.enable();		// right motor on
	distance_th = 0.0;
	distance_me = 0.0;
	while(1) {
		accel = TrapezoidalAcceleration(_accel, _accel, start_velocity, end_velocity, max_velocity, _distance, distance_th);
		if(flag.bit.back == 0) break;
		// falsesafe process
		if(flag.bit.falesafe == 1){
			flag.byte &= 0xe000;
			break;
		}
	}
	if(velocity_th < 0.0) velocity_th = 0.0;
	motor_l.disable();		// left motor off
	motor_r.disable();		// right motor off
	flag.byte &= 0xfc04;	// init run flags
}

void Run::s_turn(TurnParam param){
	flag.byte |= 0x07;		// set run flags
	if(param.direction == L){
		flag.bit.slalom_l = 1;
	}
	else{
		flag.bit.slalom_r = 1;
	}
	//offset
	if(flag.bit.diagonal == 0) flag.bit.enable_phot = 1;
	else flag.bit.enable_phot = 0;
	end_flag = 1;
	straight(10000.0,velocity_th, param.velocity, param.velocity, param.u_offset);
	motor_l.enable();		// left motor on
	motor_r.enable();		// right motor on
	gyro.Kf = param.Kf;
	Slalom(param.dpss, param.velocity, param.u_offset, param.d_offset, param.b_deg, param.e_deg, param.deg);
	//offset
	if(flag.bit.diagonal == 0) flag.bit.enable_phot = 1;
	else flag.bit.enable_phot = 0;
	end_flag = 1;
	flag.bit.slalom_l = 0;
	flag.bit.slalom_r = 0;
	distance_th = 0;
	straight(10000.0, param.velocity, param.velocity, param.velocity, param.d_offset);
	motor_l.disable();		// left motor off
	motor_r.disable();		// right motor off
	flag.byte &= 0xfc00;	// init run flags
}

void Run::turn(float max_dps, float _deg, char direction){
	initValue();
	flag.byte |= 0x07;		// set run flags
	if(direction == L){
		gyro.Kf = 1.00;
		flag.bit.turn_l = 1;	// turn_l enable
	}
	else{
		gyro.Kf = 1.00;
		flag.bit.turn_r = 1;	// turn_r enable
	}
	MTU1.TCNT = 32767;		// clear  enc counter
	MTU2.TCNT = 32767;		// clear  enc counter
	initValue();			// measured value initialize
	motor_l.enable();		// left motor on
	motor_r.enable();		// right motor on
	deg = 0.0;
	flag.bit.enable_phot = 0;
	while(1) {
		deg_a = TrapezoidalAcceleration(4000.0, 4000.0, 0.0, 0.0, max_dps, _deg, deg);
		if(flag.bit.turn_l == 0 && flag.bit.turn_r == 0) break;
		// falsesafe process
		if(flag.bit.falesafe == 1){
			flag.byte &= 0xe000;
			break;
		}
	}
	velocity_th = 0.0;
	deg_v = 0.0;
	motor_l.disable();		// left motor off
	motor_r.disable();		// right motor off
	flag.byte &= 0xfc00;	// init run flags
}



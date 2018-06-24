#include "Control.h"
#include "var.h"
#include "Parameter.h"

// control Velocity
void Control::getControlVelocity(float tar_velocity_l, float tar_velocity_r) {
	float Kp = Kvp, Ki = Kvi, Kd = Kvd;
	if(flag.bit.enable_enc == 1) {
		ve_error_l[1] = (tar_velocity_l - enc_l.getVelocity());
		ve_error_r[1] = (tar_velocity_r - enc_r.getVelocity());
		ve_s_error_l += ve_error_l[1] * 0.001;
		ve_s_error_r += ve_error_r[1] * 0.001;
		ve_d_error_l = (ve_error_l[1] - ve_error_l[0]) / 0.001;
		ve_d_error_r = (ve_error_r[1] - ve_error_r[0]) / 0.001;
		duty_l = (ve_error_l[1] * Kp + ve_s_error_l * Ki + ve_d_error_l * Kd);
		duty_r = (ve_error_r[1] * Kp + ve_s_error_r * Ki + ve_d_error_r * Kd);
		ve_error_l[0] = ve_error_l[1];
		ve_error_r[0] = ve_error_r[1];
	}	
}

void Control::getControlDsp(float tar_dsp){
	float Kp = Kgp, Ki = Kgi, Kd = Kgd;
	gy_error[1] = 0.0;
	if(flag.bit.enable_gyro == 1)	{
		gyro.getDisp();
		gy_error[1] = (tar_dsp + getControlDiag() + getControlWall() - (gyro.z_dps - gyro.z_dps_offset) * gyro.Kf);
		gy_s_error += gy_error[1] * 0.001;
		gy_d_error = (gy_error[1] - gy_error[0]) / 0.001;
		duty_l = -(gy_error[1] * Kp + gy_s_error * Ki + gy_d_error * Kd);
		duty_r = (gy_error[1] * Kp + gy_s_error * Ki + gy_d_error * Kd);
		gy_error[0] = gy_error[1];

		gyro.z_deg += (gyro.z_dps - gyro.z_dps_offset) * 0.001;
	}
}

float Control::getControlWall(void){
	float c_value = 0.0;
	float Kp = Kpp, Kd = Kpd;
	if(flag.bit.enable_phot == 1 && photo.value[0] < 600 && photo.value[5] < 600 && flag.bit.diagonal != 1){
		// リファレンスとの誤差取得
		ph_error[1][3] = photo.value[3] - SEN_RSS_REF;		// 右壁
		ph_error[1][2] = photo.value[2] - SEN_LSS_REF;		// 左壁
		// 偏差計算
		ph_d_error[3] = ph_error[1][3] - ph_error[0][3];	// 右壁
		ph_d_error[2] = ph_error[1][2] - ph_error[0][2];	// 左壁
		// 前回の誤差を記録
		ph_error[0][3] = ph_error[1][3];	// 右壁
		ph_error[0][2] = ph_error[1][2];	// 左壁
		// 両壁制御
		if(photo.value[2] > SEN_LSS_TH && photo.value[3] > SEN_RSS_TH){
			if(abs(photo.buf[2] - photo.value[2]) < SEN_LSS_DEV && abs(photo.buf[3] - photo.value[3]) < SEN_RSS_DEV){
				c_value = Kp * (ph_error[1][3] - ph_error[1][2]) + Kd * (ph_d_error[3] - ph_d_error[2]);
			}
			else if(abs(photo.buf[2] - photo.value[2]) < SEN_LSS_DEV && abs(photo.buf[3] - photo.value[3]) > SEN_RSS_DEV){
				c_value = -2.0 * (Kp * ph_error[1][2] + Kd * ph_d_error[2]);
			}
			else if(abs(photo.buf[2] - photo.value[2]) > SEN_LSS_DEV && abs(photo.buf[3] - photo.value[3]) < SEN_RSS_DEV){
				c_value = 2.0 * (Kp * ph_error[1][3] + Kd * ph_d_error[3]);
			}
		}
		// 左壁制御
		else if(photo.value[2] > SEN_LSS_TH && photo.value[3] < SEN_RSS_TH){
			if(abs(photo.buf[2] - photo.value[2]) < SEN_LSS_DEV){
				c_value = -2.0 * (Kp * ph_error[1][2] + Kd * ph_d_error[2]);
			}
		}
		// 右壁制御
		else if(photo.value[2] < SEN_RSS_TH && photo.value[3] > SEN_RSS_TH){
			if(abs(photo.buf[3] - photo.value[3]) < SEN_RSS_DEV){
				c_value = 2.0 * (Kp * ph_error[1][3] + Kd * ph_d_error[3]);
			}
		}
	}
	return c_value;
}

float Control::getControlDiag(void){
	float c_value = 0.0;
	int error_l = 0.0, error_r = 0.0;
	if((flag.bit.enable_phot == 1) && (flag.bit.diagonal == 1)){
		error_l = photo.value[1];		// 左壁
		error_r = photo.value[4];		// 右壁
		// 左壁制御
		if(photo.value[1] > SEN_LSS_DIG_TH){
			c_value = - 0.1 * error_l;
			//LED01 = 1;
		}
		// 右壁制御
		else if(photo.value[4] > SEN_RSS_DIG_TH){
			c_value = 0.1 * error_r;
			//LED04 = 1;
		}
	}
	else{
		LED01 = 0;
		LED04 = 0;
	}
	return c_value;
}

void Control::getControlDuty(float tar_velocity_l, float tar_velocity_r, float tar_dsp){
	getControlVelocity(tar_velocity_l,tar_velocity_r);
	getControlDsp(tar_dsp);
	if(duty_l > 1.0)  duty_l = 1.0;
	if(duty_l < -1.0) duty_l = -1.0;
	if(duty_r > 1.0)  duty_r = 1.0;
	if(duty_r < -1.0) duty_r = -1.0;
}

void Control::detectFaleSafe(void){
	float error_s = abs(ve_s_error_l) + abs(ve_s_error_r);
	float error = (abs(ve_error_l[0]) + abs(ve_error_r[1]))/2;
	if(error > 300 && error_s > 300){
		flag.bit.falesafe = 1;
		M_STBY = 0;
		stopSuction();
	}
}

void Control::satartSuction(void){
	CSTR(MTU0) = 0;			// MTU0停止
	MTU0.TIORL.BYTE = 0x21;	// パルス波形： __↑￣↓
	CSTR(MTU0) = 1;     	// MTU0開始
}

void Control::stopSuction(void){
	CSTR(MTU0) = 0;			// MTU0停止
	MTU0.TIORL.BYTE = 0x00;	// パルス出力禁止
	//CSTR(MTU0) = 0;     	// MTU0開始
}

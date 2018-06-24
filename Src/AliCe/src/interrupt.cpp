/*
 * interrupt.c
 *
 *  Created on: 2016/10/24
 *      Author: KOTA-NPC2
 */

//#include "speaker_func.h"
#include "Serial.h"
#include "iodefine.h"
#include "var.h"
#include "speaker_func.h"
#include "parameter.h"

int g_cnt=0;
int g_cnt2=0;

//-------------------------------------------------------------------
//割り込み処理関数
//-------------------------------------------------------------------
extern "C" void interrupt_cmt0(void){
	g_cnt++;
	if(g_cnt == 2147483647) g_cnt = 0;
	if((mouse.flag.byte & 0x17) == 0x17){		// straight
		mouse.velocity_th += mouse.accel * 0.001;
		mouse.distance_th += mouse.velocity_th * 0.001;
		mouse.getControlDuty(mouse.velocity_th, mouse.velocity_th,0.0);
		mouse.velocity_me = (mouse.enc_l.velocity+mouse.enc_r.velocity)/2;
		if(log01.flag.bit.dps == 1) log01.setLog(0.0,(mouse.gyro.z_dps-mouse.gyro.z_dps_offset));
		else if(log01.flag.bit.velocity == 1) log01.setLog(mouse.ve_s_error_l,mouse.ve_s_error_r);
		else if(log01.flag.bit.deg == 1) log01.setLog(0.0,mouse.gyro.z_deg);
	}
	else if((mouse.flag.byte & 0x27) == 0x27){	// slalom left
		mouse.velocity_th += mouse.accel * 0.001;
		mouse.distance_th += mouse.velocity_th * 0.001;
		mouse.deg_v += mouse.deg_a * 0.001;
		mouse.deg += mouse.deg_v * 0.001;
		mouse.velocity_l = mouse.velocity_th - (TORED / 2.0) * mouse.deg_v * (3.1415 / 180.0);
		mouse.velocity_r = mouse.velocity_th + (TORED / 2.0) * mouse.deg_v * (3.1415 / 180.0);
		mouse.getControlDuty(mouse.velocity_l, mouse.velocity_r, mouse.deg_v);
		mouse.velocity_me = (mouse.enc_l.velocity+mouse.enc_r.velocity)/2;
		if(log01.flag.bit.dps == 1) log01.setLog(mouse.deg_v,(mouse.gyro.z_dps-mouse.gyro.z_dps_offset));
		else if(log01.flag.bit.velocity == 1) log01.setLog(mouse.velocity_th,mouse.velocity_me);
		else if(log01.flag.bit.deg == 1) log01.setLog(mouse.deg,mouse.gyro.z_deg);
	}
	else if((mouse.flag.byte & 0x47) == 0x47){	// slalom right
		mouse.velocity_th += mouse.accel * 0.001;
		mouse.distance_th += mouse.velocity_th * 0.001;
		mouse.deg_v += mouse.deg_a * 0.001;
		mouse.deg += mouse.deg_v * 0.001;
		mouse.velocity_l = mouse.velocity_th + (TORED / 2.0) * mouse.deg_v * (3.1415 / 180.0);
		mouse.velocity_r = mouse.velocity_th - (TORED / 2.0) * mouse.deg_v * (3.1415 / 180.0);
		mouse.getControlDuty(mouse.velocity_l, mouse.velocity_r, -mouse.deg_v);
		mouse.velocity_me = (mouse.enc_l.velocity+mouse.enc_r.velocity)/2;
		if(log01.flag.bit.dps == 1) log01.setLog(-mouse.deg_v,(mouse.gyro.z_dps-mouse.gyro.z_dps_offset));
		else if(log01.flag.bit.velocity == 1) log01.setLog(mouse.velocity_th,mouse.velocity_me);
		else if(log01.flag.bit.deg == 1) log01.setLog(-mouse.deg,mouse.gyro.z_deg);
	}
	else if((mouse.flag.byte & 0x107) == 0x107){	// turn right
		mouse.deg_v += mouse.deg_a * 0.001;
		mouse.deg += mouse.deg_v * 0.001;
		mouse.velocity_l = (TORED / 2.0) * mouse.deg_v * (3.1415 / 180.0);
		mouse.velocity_r = - (TORED / 2.0) * mouse.deg_v * (3.1415 / 180.0);
		mouse.getControlDuty(mouse.velocity_l, mouse.velocity_r, -mouse.deg_v);
		if(log01.flag.bit.dps == 1) log01.setLog(-mouse.deg_v,(mouse.gyro.z_dps-mouse.gyro.z_dps_offset));
		else if(log01.flag.bit.deg == 1) log01.setLog(-mouse.deg,mouse.gyro.z_deg);
	}
	else if((mouse.flag.byte & 0x087)== 0x087){	// turn left
		mouse.deg_v += mouse.deg_a * 0.001;
		mouse.deg += mouse.deg_v * 0.001;
		mouse.velocity_l = - (TORED / 2.0) * mouse.deg_v * (3.1415 / 180.0);
		mouse.velocity_r = + (TORED / 2.0) * mouse.deg_v * (3.1415 / 180.0);
		mouse.getControlDuty(mouse.velocity_l, mouse.velocity_r, mouse.deg_v);
		if(log01.flag.bit.dps == 1) log01.setLog(mouse.deg_v,(mouse.gyro.z_dps-mouse.gyro.z_dps_offset));
		else if(log01.flag.bit.deg == 1) log01.setLog(mouse.deg,mouse.gyro.z_deg);
	}
	else if((mouse.flag.byte & 0x207) == 0x207){		// back
		mouse.velocity_th += mouse.accel * 0.001;
		mouse.distance_th += mouse.velocity_th * 0.001;
		mouse.getControlDuty(-mouse.velocity_th, -mouse.velocity_th,0.0);
		mouse.velocity_me = (mouse.enc_l.velocity+mouse.enc_r.velocity)/2;
		if(log01.flag.bit.dps == 1) log01.setLog(mouse.deg_v,(mouse.gyro.z_dps-mouse.gyro.z_dps_offset));
		else if(log01.flag.bit.velocity == 1) log01.setLog(mouse.velocity_th,mouse.velocity_me);
	}
	else{
		mouse.enc_r.getVelocity();
	}
	mouse.detectFaleSafe();
}

extern "C" void interrupt_cmt1(void){
	static int count = 0;
	mouse.photo.readSensor();
	// 壁センサログ取得
//	if(log01.flag.bit.photo_SS == 1 && count == 0) log01.setLog(mouse.photo.value[2],mouse.photo.value[3]);
	if(log01.flag.bit.photo_SS == 1 && count == 0) log01.setLog(mouse.photo.int_dev[2],mouse.photo.int_dev[3]);
	else if(log01.flag.bit.photo_F == 1 && count == 0) log01.setLog(mouse.photo.value[0],mouse.photo.value[5]);
//	else if(log01.flag.bit.photo_S == 1 && count == 0) log01.setLog(mouse.photo.value[1],mouse.photo.value[4]);
	else if(log01.flag.bit.photo_S == 1 && count == 0) log01.setLog(mouse.photo.int_dev[1],mouse.photo.int_dev[4]);
	count ++;
	if(count == 5)	count = 0;
}

extern "C" void interrupt_cmt2(void){
	g_cnt2++;
	WelcomeToTheJapariPark(180,2);
	//green_melody(180,2);
	//mtdedede_melody(180, 2);
	//rokutyo_melody(170,1);
}

extern "C" void interrupt_mtu4(void){
	if(mouse.flag.bit.enable_enc==1) {
		if(mouse.duty_r > 0){
			mouse.motor_r.cw();
			MTU4.TGRB = 124 * (1.0 - mouse.duty_r);
		}
		else{
			mouse.motor_r.ccw();
			MTU4.TGRB = 124 * (1.0 + mouse.duty_r);
		}
	}
}

extern "C" void interrupt_tpu4(void){
	if(mouse.flag.bit.enable_enc==1) {
		if(mouse.duty_l > 0){
			mouse.motor_l.cw();
			TPU4.TGRB = 124 * (1.0 - mouse.duty_l);
		}
		else{
			mouse.motor_l.ccw();
			TPU4.TGRB = 124 * (1.0 + mouse.duty_l);
		}
	}
}

//-------------------------------------------------------------------
//タイマ関数
//-------------------------------------------------------------------
void wait_timer(int msec){
	g_cnt=0;
	while(g_cnt < msec);
}

//-------------------------------------------------------------------
//カウンタ関数
//-------------------------------------------------------------------
int counter(int msec){
	if(g_cnt2>=msec){
		g_cnt2 = 0;
		return 1;
	}
	else
		return 0;
}

/*
 * PhotoSensoer.cpp
 *
 *  Created on: 2017/09/14
 *      Author: KOTA-NPC2
 */

#include "PhotoSensor.h"
#include "iodefine.h"

void PhotoSensor::ADCenable(void) {
	S12AD.ADCSR.BIT.ADST = 1;     // AD変換開始
}

void PhotoSensor::ADCdisable(void) {
	S12AD.ADCSR.BIT.ADST = 0;     // AD変換停止
	S12AD.ADCSR.BIT.ADIE = 0;     // スキャン終了後の割り込み禁止
}

void PhotoSensor::getValue(char ch) {
	ADCdisable();
	switch (ch) {
	case SEN_F:
		S12AD.ADANS0.BIT.ANS0 = 0x0102; 		//　変換対象：ANS1,ANS8
		ADCenable();
		while(S12AD.ADCSR.BIT.ADST);			// 変換終了待機
		if(PORTA.PODR.BIT.B0 == 0){				// SEN_LED3(Front)
			value_off[0] = (S12AD.ADDR1>>2)/4;	// 左前 SEN_LF
			value_off[5] = (S12AD.ADDR8>>2)/4;	// 右前 SEN_RF
		}
		else if(PORTA.PODR.BIT.B0 == 1){		// SEN_LED3(Front)
			value_on[0] = (S12AD.ADDR1>>2)/4;	// 左前 SEN_LF
			value_on[5] = (S12AD.ADDR8>>2)/4;	// 右前 SEN_RF
		}
		break;
	case SEN_M:
		S12AD.ADANS0.BIT.ANS0 = 0x0044;			//　変換対象：ANS2,ANS6
		ADCenable();
		while(S12AD.ADCSR.BIT.ADST);			// 変換終了待機
		if(PORTB.PODR.BIT.B6 == 0){				// SEN_LED1(Middle)
			value_off[1] = (S12AD.ADDR2>>2)/4;	// 左中央 SEN_LS
			value_off[4] = (S12AD.ADDR6>>2)/4;	// 右中央 SEN_RS
		}
		else if(PORTB.PODR.BIT.B6 == 1){		// SEN_LED1(Middle)
			value_on[1] = (S12AD.ADDR2>>2)/4;	// 左中央 SEN_LS
			value_on[4] = (S12AD.ADDR6>>2)/4;	// 右中央 SEN_RS
		}
		break;
	case SEN_S:
		S12AD.ADANS0.BIT.ANS0 = 0x0018; 		//　変換対象：ANS3,ANS4
		ADCenable();
		while(S12AD.ADCSR.BIT.ADST);			// 変換終了待機
		if(PORTB.PODR.BIT.B0 == 0){				// SEN_LED2(Side)
			value_off[2] = (S12AD.ADDR3>>2)/4;	// 左横 SEN_LSS
			value_off[3] = (S12AD.ADDR4>>2)/4;	// 右横 SEN_RSS
		}
		else if(PORTB.PODR.BIT.B0 == 1){		// SEN_LED2(Side)
			value_on[2] = (S12AD.ADDR3>>2)/4;	// 左横 SEN_LSS
			value_on[3] = (S12AD.ADDR4>>2)/4;	// 右横 SEN_RSS
		}
		break;
	}
}

void PhotoSensor::readSensor(void){
	static char ad_count = 0;
	static char count = 0;
	switch (ad_count){
	case 0 :
		PORTA.PODR.BIT.B0 = 0;		// SEN_LED3(Front)
		PORTB.PODR.BIT.B6 = 0;		// SEN_LED1(Middle)
		PORTB.PODR.BIT.B0 = 0;		// SEN_LED2(Side)
		for(int i=0;i<500;i++);
		getValue(SEN_F);
		getValue(SEN_M);
		getValue(SEN_S);
		ad_count += 1;
		break;
	case 1 :
		PORTA.PODR.BIT.B0 = 1;		// SEN_LED3(Front)
		PORTB.PODR.BIT.B6 = 0;		// SEN_LED1(Middle)
		PORTB.PODR.BIT.B0 = 0;		// SEN_LED2(Side)
		for(int i=0;i<500;i++);
		getValue(SEN_F);
		ad_count += 1;
		break;
	case 2 :
		PORTA.PODR.BIT.B0 = 0;		// SEN_LED3(Front)
		PORTB.PODR.BIT.B6 = 1;		// SEN_LED1(Middle)
		PORTB.PODR.BIT.B0 = 0;		// SEN_LED2(Side)
		for(int i=0;i<500;i++);
		getValue(SEN_M);
		ad_count += 1;
		break;
	case 3 :
		PORTA.PODR.BIT.B0 = 0;		// SEN_LED3(Front)
		PORTB.PODR.BIT.B6 = 0;		// SEN_LED1(Middle)
		PORTB.PODR.BIT.B0 = 1;		// SEN_LED2(Side)
		for(int i=0;i<500;i++);
		getValue(SEN_S);
		ad_count += 1;
		break;
	default:
		for(int i=0;i<6;i++) buf[i] = value[i];
		for(int i=0;i<6;i++) {
			ring_buf[i][count] = value_on[i] - value_off[i];
		}
		for(int i=0;i<6;i++) {
			value[i] = (ring_buf[i][0]+ring_buf[i][1]+ring_buf[i][2]+ring_buf[i][3]+ring_buf[i][4]) / 5;
			dev[i] = value[i] - buf[i];
		}
		count ++;
		if(count == 5){
			count = 0;
		}
		ad_count = 0;
		// 壁センサの変化量前6データの積算値取得
		getIntDev();
		break;
	}
}

void PhotoSensor::getIntDev(void){
	static unsigned char count = 0;
	// 壁センサの変化量を前6データまで積算
	for(int i = 0; i<6; i++){
		int_dev_buf[i][count] = buf[i] - value[i];
	}
	for(int i=0;i<6;i++) {
		int sum = 0;
		for(int j = 0; j<6; j++) sum += int_dev_buf[i][j];
		int_dev[i] = sum;
	}
	count ++;
	if(count == 6)	count = 0;
}


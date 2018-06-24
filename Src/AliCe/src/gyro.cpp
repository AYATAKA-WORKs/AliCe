#include "iodefine.h"
#include "interrupt.h"
#include "gyro.h"
#include "parameter.h"

void Gyro::tx_1byte(unsigned char data){
	// RSPI1.SPDR.LONG = (unsigned int)data; //データ送信
	// RSPI1.SPSR.BYTE;                      //ステータスフラグの空読み
	// while (RSPI1.SPSR.BIT.IDLNF) ;        //転送終了待ち
	// gy_data = RSPI1.SPDR.LONG;            //データ受信
	RSPI1.SPDR.LONG = ((unsigned int)data & 0x000000ff);
	while(!IR(RSPI1,SPRI1));
	IR(RSPI1,SPRI1)=0;
	gy_data = RSPI1.SPDR.LONG;
}

void Gyro::tx_2byte(char adr, char data){
	RSPI1.SPCR.BIT.SPE = 1;
	tx_1byte(adr);
	tx_1byte(data);
	RSPI1.SPCR.BIT.SPE = 0;
}

void Gyro::write(char adr,char data){
	tx_2byte(adr, data);
}

void Gyro::read(char adr){
	tx_2byte((adr|0x80), 0x00);
}

void Gyro::initMPU(void){
	write(PWR_MGMT_1, 0x80);	// reset mpu-6000
	wait_timer(100);
	write(SIGNAL_PATH_RESET, 0x07);	// signal reset
	wait_timer(100);
	write(PWR_MGMT_1, 0x00);	// wake up
	wait_timer(100);
	write(USER_CTRL, 0x10);		// disable I2C
	wait_timer(1);
	write(SMPLRT_DIV, 0x00);	// sample rate: 8kHz/(0+1) = 8kHz
	wait_timer(1);
	write(CONFIG, 0x00);		// disable DLPF, gyro output rate = 8kHz
	wait_timer(1);
	write(GYRO_CONFIG, 0x18);	// gyro range: ±2000dps 16.4 LSB/°/s
	wait_timer(1);
	write(ACCEL_CONFIG, 0x00);	// accel range: ±2g
	wait_timer(1);
	
	z_dps = 0.0;
	z_deg = 0.0;
	z_dps_offset = 0.0;
}

void Gyro::calibration(void){
	short buf;
	int i;
	wait_timer(500);
	for(i=0; i<5; i++){
		LED05=1;LED06=0;
		wait_timer(100);
		LED05=0;LED06=1;
		wait_timer(100);
	}
	for(i=0; i<1000; i++){
		read(GYRO_ZOUT_H);
		buf = gy_data;
		for(int i = 0; i<10; i++);
		read(GYRO_ZOUT_L);
		buf = ((buf << 8) | gy_data);
		z_dps_offset += ((float)buf / 16.4);
		LED05=1;LED06=1;
	}
	z_dps_offset = z_dps_offset / 1000;
	for(i=0; i<5; i++){
		LED05=1;LED06=0;
		wait_timer(100);
		LED05=0;LED06=1;
		wait_timer(100);
	}
	wait_timer(50);
	LED05=0;LED06=0;
}

void Gyro::who_am_i(void){
	read(WHO_AM_I);
}

void Gyro::enable(void){
	enable_flag = 1;
}

void Gyro::disable(void){
	enable_flag = 0;
}

void Gyro::getTemp(void){
	short buf;
	read(TEMP_OUT_L);
	buf = gy_data;
	for(int i = 0; i<10; i++);
	read(TEMP_OUT_H);
	temp = (float) (((buf << 8) | gy_data) / 340 + 36.53);
}

void Gyro::getAccel(void){
	short buf;
	read(ACCEL_XOUT_H);
	buf = gy_data;
	for(int i = 0; i<10; i++);
	read(ACCEL_XOUT_L);
	buf = ((buf << 8) | gy_data);
	x_accel = (float)buf / 16384.0 * 9.8;
	
	read(ACCEL_YOUT_H);
	buf = gy_data;
	for(int i = 0; i<10; i++);
	read(ACCEL_YOUT_L);
	buf = ((buf << 8) | gy_data);
	y_accel = (float)buf / 16384.0 * 9.8;

	read(ACCEL_ZOUT_H);
	buf = gy_data;
	for(int i = 0; i<10; i++);
	read(ACCEL_ZOUT_L);
	buf = ((buf << 8) | gy_data);
	z_accel = (float)buf / 16384.0 * 9.8;
}

void Gyro::getDisp(void){
	short buf;
	read(GYRO_ZOUT_H);
	buf = gy_data;
	for(int i = 0; i<10; i++);
	read(GYRO_ZOUT_L);
	buf = ((buf << 8) | gy_data);
	z_dps = (float)buf / 16.4;

}

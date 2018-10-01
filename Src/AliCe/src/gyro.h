#ifndef GYRO_H_
#define GYRO_H_

#define SMPLRT_DIV		0x19 // MPU-6000 register address
#define CONFIG  		0x1a
#define GYRO_CONFIG		0x1b
#define ACCEL_CONFIG	0x1c
#define ACCEL_XOUT_H	0x3b
#define ACCEL_XOUT_L	0x3c
#define ACCEL_YOUT_H	0x3d
#define ACCEL_YOUT_L	0x3e
#define ACCEL_ZOUT_H	0x3f
#define ACCEL_ZOUT_L	0x40
#define TEMP_OUT_H		0x41
#define TEMP_OUT_L		0x42
#define GYRO_ZOUT_H		0x47
#define GYRO_ZOUT_L		0x48
#define SIGNAL_PATH_RESET	0x68
#define USER_CTRL		0x6a
#define PWR_MGMT_1		0x6b
#define WHO_AM_I		0x75

class Gyro {
public:
	void initMPU(void);
	void calibration(void);
	void who_am_i(void);
	void write(char adr,char data);
	void read(char adr);
	void getAccel(void);
	void getTemp(void);
	void getDisp(void);
	void enable(void);
	void disable(void);

	unsigned char gy_data;
	float x_accel, y_accel, z_accel,temp;
	float z_dps, z_deg;
	float z_dps_offset;
	char enable_flag;
	float Kf;
	
private:
	void tx_1byte(unsigned char data);
	void tx_2byte(char adr,char data);
};

#endif /* GYRO_H_ */

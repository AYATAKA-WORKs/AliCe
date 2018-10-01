/*
 * Serial.h
 *
 *  Created on: 2016/11/18
 *      Author: KOTA-NPC2
 */

#ifndef SERIAL_H_
#define SERIAL_H_

void put1byte(char c);
void putnbyte(char *buf,int len);
int myprintf(const char *fmt, ...);

#endif /* SERIAL_H_ */

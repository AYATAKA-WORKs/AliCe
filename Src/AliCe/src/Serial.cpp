/*
 * Serial.c
 *
 *  Created on: 2016/11/18
 *      Author: KOTA-NPC2
 */
 
#include <stdarg.h>
#include <stdio.h>
#include "iodefine.h"

void put1byte(char c) {
	while (SCI1.SSR.BIT.TDRE == 0);
	SCI1.SSR.BIT.TDRE = 0;
	SCI1.TDR = c;
}

void putnbyte(char *buf, int len) {
	int c;
	for (c = 0; c < len; c++) {
		put1byte(buf[c]);
	}
}

int myprintf(const char *fmt, ...) {
	static char buffer[100];
	int len;

	va_list ap;
	va_start(ap, fmt);

	len = vsprintf(buffer, fmt, ap);
	putnbyte(buffer, len);
	va_end(ap);
	return len;
}


void charput(char c)
{
	while(SCI1.SSR.BIT.TEND == 0);
	SCI1.TDR = c;
	SCI1.SSR.BIT.TEND = 0;
}

char charget(void){
	char data;

	data = SCI1.RDR;
	ICU.IR[VECT_SCI1_RXI1].BIT.IR = 0;

	return data;
}

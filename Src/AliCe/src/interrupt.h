/*
 * interrupt.h
 *
 *  Created on: 2016/10/24
 *      Author: KOTA-NPC2
 */

#ifndef INTERRUPT_H_
#define INTERRUPT_H_

extern int g_cnt2;

void interrupt_cmt0(void);
void interrupt_cmt1(void);
void interrupt_cmt2(void);
void wait_timer(int msec);
int counter(int msec);

#endif /* INTERRUPT_H_ */

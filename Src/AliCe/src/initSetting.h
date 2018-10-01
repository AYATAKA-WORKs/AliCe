/*
 * initSetting.h
 *
 *  Created on: 2016/11/18
 *      Author: KOTA-NPC2
 */

#ifndef INITSETTING_H_
#define INITSETTING_H_

#include "iodefine.h"
#include "initNonExistentPort.h"
#include "initStopModule.h"
#include "initClock.h"

class initSetting : private initClock,private initNonExistentPort,private initStopModule {
public:
	static void initModules(void);
private:
	static void initCMT(void);
	static void initSCI(void);
	static void initIO(void);
	static void initAD(void);
	static void initPwm(void);
	static void initSPI(void);
	static void initPhase(void);
};

#endif /* INITSETTING_H_ */

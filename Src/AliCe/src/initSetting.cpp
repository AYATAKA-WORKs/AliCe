/*
 * initSetting.c
 *
 *  Created on: 2016/11/18
 *      Author: KOTA-NPC2
 */

#include "initSetting.h"
#include <machine.h>

//-------------------------------------------------------------------
//各モジュール初期化関数
//-------------------------------------------------------------------
void initSetting::initModules(void) {
	setNonExistentPort();
	setStopModule();
	clrpsw_i();
	setClock();
	initCMT();
	initSCI();
	initSPI();
	initIO();
	initAD();
	initPwm();
	initPhase();
}

//-------------------------------------------------------------------
//CMTの設定
//-------------------------------------------------------------------
void initSetting::initCMT(void) {
	clrpsw_i(); //CPU割り込み禁止（"Iフラグ=0"）
	//---------------------------------------------------------------
	//CMT0の設定
	//---------------------------------------------------------------
	SYSTEM.PRCR.WORD = 0xa50b;  //プロテクトされたレジスタを全て解除
	MSTP(CMT0) = 0;             //モジュールストップ機能 0:解除,1:停止
	SYSTEM.PRCR.WORD = 0xa500;  //レジスタを全てプロテクト

	CMT.CMSTR0.BIT.STR0 = 0;    //CMT0 0:停止,1:開始
	CMT0.CMCR.BIT.CKS = 0;      //クロック周期：50/8NHz
	CMT0.CMCNT = 0x0000;
	CMT0.CMCOR = 6249;          //コンペアマッチ周期を1msに設定

	CMT0.CMCR.BIT.CMIE = 1;     //コンペアマッチ割り込み 0:禁止,1:許可
	IEN(CMT0,CMI0)=1;           //割り込み許可(ICUコントローラ)
	IPR(CMT0,CMI0)=4;           //割り込み優先度の割り当て

	CSTR(CMT0) = 0;             //CMT0 0:停止,1:開始

	//---------------------------------------------------------------
	//CMT1の設定
	//---------------------------------------------------------------
	SYSTEM.PRCR.WORD = 0xa50b;  //プロテクトされたレジスタを全て解除
	MSTP(CMT1) = 0;             //モジュールストップ機能 0:解除,1:停止
	SYSTEM.PRCR.WORD = 0xa500;  //レジスタを全てプロテクト

	CMT.CMSTR0.BIT.STR1 = 0;    //CMT1 0:停止,1:開始
	CMT1.CMCR.BIT.CKS = 0;      //クロック周期：PCLK/8
	CMT1.CMCNT = 0x0000;        //カウンタ初期化
	CMT1.CMCOR = 3124;           //コンペアマッチ周期を0.5msに設定

	CMT1.CMCR.BIT.CMIE = 1;     //コンペアマッチ割り込み 0:禁止,1:許可
	IEN(CMT1,CMI1)=1;           //割り込み許可(ICUコントローラ)
	IPR(CMT1,CMI1)=5;           //割り込み優先度の割り当て

	CSTR(CMT1) = 0;             //CMT1 0:停止,1:開始

	//---------------------------------------------------------------
	//CMT2の設定
	//---------------------------------------------------------------
	SYSTEM.PRCR.WORD = 0xa50b;  //プロテクトされたレジスタを全て解除
	MSTP(CMT2) = 0;             //モジュールストップ機能 0:解除,1:停止
	SYSTEM.PRCR.WORD = 0xa500;  //レジスタを全てプロテクト

	CMT.CMSTR0.BIT.STR1 = 0;    //CMT2 0:停止,1:開始
	CMT2.CMCR.BIT.CKS = 0;      //クロック周期：PCLK/8
	CMT2.CMCNT = 0x0000;        //カウンタ初期化
	CMT2.CMCOR = 6249;           //コンペアマッチ周期を1msに設定

	CMT2.CMCR.BIT.CMIE = 1;     //コンペアマッチ割り込み 0:禁止,1:許可
	IEN(CMT2,CMI2)=1;           //割り込み許可(ICUコントローラ)
	IPR(CMT2,CMI2)=3;           //割り込み優先度の割り当て

	CSTR(CMT2) = 0;             //CMT2 0:停止,1:開始

	setpsw_i(); //CPU割り込み許可（"Iフラグ=1"）
}

//-------------------------------------------------------------------
//SCIの設定
//-------------------------------------------------------------------
void initSetting::initSCI(void) {
	// SCI1の省電力解除
	SYSTEM.PRCR.WORD = 0xA502;  //プロテクトされたレジスタを解除
	MSTP(SCI1) = 0;             //
	SYSTEM.PRCR.WORD = 0xA500;  //レジスタをプロテクト

	//I/Oピンの設定
	MPC.PWPR.BIT.B0WI = 0;      //プロテクト解除
	MPC.PWPR.BIT.PFSWE = 1;
	MPC.P26PFS.BIT.PSEL = 0x0A; //P26をTXD1に設定
	MPC.P30PFS.BIT.PSEL = 0x0A; //P30をRXD1に設定
	MPC.PWPR.BIT.PFSWE = 0;     //プロテクト
	MPC.PWPR.BIT.B0WI = 1;
	PORT2.PMR.BIT.B6 = 1;       // P26を周辺機能として設定
	PORT3.PMR.BIT.B0 = 1;       // P30を周辺きのうとして設定

	// SCI1 Settings
	SCI1.SCR.BIT.TEIE = 0;      //TEIEを無効にする
	SCI1.SCR.BIT.MPIE = 0;      //通常の受信動作
	SCI1.SCR.BIT.RE = 0;        //シリアル受信動作を禁止
	SCI1.SCR.BIT.TE = 0;        //シリアル送信動作を禁止
	SCI1.SCR.BIT.RIE = 0;       //RXIおよびERI割り込み要求を禁止
	SCI1.SCR.BIT.TIE = 0;       //TXI割り込み要求を禁止
	SCI1.SCR.BIT.CKE = 0;       //
	SCI1.SMR.BIT.CKS = 0;       //PCLK
	SCI1.SMR.BIT.MP = 0;        //マルチプロセッサ無効
	SCI1.SMR.BIT.STOP = 0;      //ストップビット1ビット
	SCI1.SMR.BIT.PM = 0;        //パリティなし
	SCI1.SMR.BIT.CHR = 0;       //データ長8bit
	SCI1.SMR.BIT.CM = 0;        //調歩同期式
	SCI1.BRR = (unsigned char) (50000000 * 2 / (64 * 115200)) - 1; // 115200bps
	SCI1.SCR.BIT.RE = 1;        //シリアル受信動作を許可
	SCI1.SCR.BIT.TE = 1;        //シリアル送信動作を許可
}

//-------------------------------------------------------------------
//PWMの設定
//-------------------------------------------------------------------
void initSetting::initPwm(void) {
	//---------------------------------------------------------------
	//モータ用(MTU4,TPU4)の設定
	//---------------------------------------------------------------
	PORTB.PMR.BIT.B3 = 0x1u;    //PB3:周辺機器を使用 R_PWM
	PORTB.PMR.BIT.B5 = 0x1u;    //PB5:周辺機器を使用 L_PWM

	MPC.PWPR.BYTE = 0x00;       //PSWEレジスタへの書き込み禁止

	MPC.PWPR.BYTE = 0x40u;      //PFSレジスタへの書き込み許可
	MPC.PB3PFS.BIT.PSEL = 0x2u; //PB3をMTIOC4Aに設定
	MPC.PB5PFS.BIT.PSEL = 0x3u; //PB5をTIOCB4に設定
	MPC.PWPR.BYTE = 0x80u;      //PFSレジスタをプロテクト

	SYSTEM.PRCR.WORD = 0xa50b;  //プロテクトされたレジスタを全て解除
	MSTP(MTU4) = 0;             //モジュールストップ機能 0:解除,1:停止
	MSTP(TPU4) = 0;             //モジュールストップ機能 0:解除,1:停止
	SYSTEM.PRCR.WORD = 0xa500;  //レジスタを全てプロテクト

	TPU4.TCR.BIT.TPSC = 1;      	//クロック周期：PCLK/4
	TPU4.TCR.BIT.CKEG = 1;      	//立ち上がりエッジでカウント
	TPU4.TCR.BIT.CCLR = 1;      	//クリア要因：TGRA
	TPU4.TMDR.BIT.MD = 3;       	//PWMモード2
	TPU4.TIOR.BYTE = 0x0021;    	//パルス波形： __↑￣↓
	TPU4.TIER.BIT.TGIEA = 0x1u;
	TPU4.TIER.BIT.TGIEB = 0x1u;
	TPU4.TGRA = 124;
	TPU4.TGRB = TPU4.TGRA;

	MTU4.TCR.BIT.TPSC = 1;      	//クロック周期：4分周
	MTU4.TCR.BIT.CKEG = 1;      	//立ち上がりエッジでカウント
	MTU4.TCR.BIT.CCLR = 1;      	//クリア要因：TGRA
	MTU4.TMDR.BIT.MD = 2;   	 	//PWMモード1
	MTU4.TIORH.BYTE = 0x21;			//パルス波形： __↑￣↓
	MTU.TOER.BIT.OE4A = 1;			//MTIOC4A出力許可
	MTU4.TIER.BIT.TGIEA = 0x1u;
	MTU4.TIER.BIT.TGIEB = 0x1u;
	MTU.TRWER.BIT.RWE = 0x1u;   	//レジスタの読み出し/書き込み:許可
	MTU4.TGRA = 124;
	MTU4.TGRB = MTU4.TGRA;

	IPR(MTU4, TGIA4)= 0x8u;     //TGIA4の割り込み優先度
	IEN(MTU4, TGIA4)= 0x1u;     //TGIA4の割り込み許可

	IPR(TPU4, TGI4A)= 0x7u;     //TGIA2の割り込み優先度
	IEN(TPU4, TGI4A)= 0x1u;     //TGIA2の割り込み許可

	CSTR(MTU4) = 0;             //MTU0 0:停止,1:開始
	CSTR(TPU4) = 0;             //MTU0 0:停止,1:開始

	//---------------------------------------------------------------
	//スピーカー用(TPU5)の設定
	//---------------------------------------------------------------
	PORTB.PMR.BIT.B7 = 0x1u;    //PB7:周辺機器を使用

	MPC.PWPR.BYTE = 0x00;       //PSWEレジスタへの書き込み禁止
	MPC.PWPR.BYTE = 0x40u;      //PFSレジスタへの書き込み許可
	MPC.PB7PFS.BIT.PSEL = 0x3u; //PB7をMTIOC0Aに設定
	MPC.PWPR.BYTE = 0x80u;      //PFSレジスタをプロテクト

	PORTB.PDR.BIT.B7 = 0x1u;    //PB7：出力

	SYSTEM.PRCR.WORD = 0xa50b;  //プロテクトされたレジスタを全て解除
	MSTP(TPU5) = 0;             //モジュールストップ機能 0:解除,1:停止
	SYSTEM.PRCR.WORD = 0xa500;  //レジスタを全てプロテクト

	TPU5.TCR.BIT.TPSC = 1;      //クロック周期：PCLK/4
	TPU5.TCR.BIT.CKEG = 1;      //立ち上がりエッジでカウント
	TPU5.TCR.BIT.CCLR = 1;      //クリア要因：TGRA
	TPU5.TMDR.BIT.MD = 3;       //PWMモード2
	TPU5.TIOR.BYTE = 0x0021;    //パルス波形： __↑￣↓
	TPU5.TIER.BIT.TGIEA = 0x1u;
	TPU5.TIER.BIT.TGIEB = 0x1u;
	TPU5.TGRA = 2449;
	TPU5.TGRB = 2500 * 0.5 - 1;
	CSTR(TPU5) = 0;             //TPU5 0:停止,1:開始

	//---------------------------------------------------------------
	//吸引モーター(MTU0)の設定
	//---------------------------------------------------------------
	PORTB.PMR.BIT.B1 = 0x1u;    //PB1:周辺機器を使用 MTIOC0C

	MPC.PWPR.BYTE = 0x00;       //PSWEレジスタへの書き込み禁止

	MPC.PWPR.BYTE = 0x40u;      //PFSレジスタへの書き込み許可
	MPC.PB1PFS.BIT.PSEL = 0x1u; //PB3をMTIOC3Cに設定
	MPC.PWPR.BYTE = 0x80u;      //PFSレジスタをプロテクト

	SYSTEM.PRCR.WORD = 0xa50b;  //プロテクトされたレジスタを全て解除
	MSTP(MTU0) = 0;             //モジュールストップ機能 0:解除,1:停止
	SYSTEM.PRCR.WORD = 0xa500;  //レジスタを全てプロテクト

	MTU0.TCR.BIT.TPSC = 1;      	//クロック周期：4分周
	MTU0.TCR.BIT.CKEG = 1;      	//立ち上がりエッジでカウント
	MTU0.TCR.BIT.CCLR = 5;      	//クリア要因：TGRC
	MTU0.TMDR.BIT.MD = 2;   	 	//PWMモード1
	MTU0.TIORL.BYTE = 0x21;			//パルス波形： __↑￣↓
	MTU0.TIER.BIT.TGIEC = 0x1u;
	MTU0.TIER.BIT.TGIED = 0x1u;
	MTU.TRWER.BIT.RWE = 0x1u;   	//レジスタの読み出し/書き込み:許可
	MTU0.TGRC = 124;
	MTU0.TGRD = MTU0.TGRC * 0.0;

	CSTR(MTU0) = 0;             //MTU0 0:停止,1:開始
}

//-------------------------------------------------------------------
//AD変換の設定
//-------------------------------------------------------------------
void initSetting::initAD(void) {
	SYSTEM.PRCR.WORD = 0xA502;		//プロテクトされたレジスタを解除
	MSTP(S12AD) = 0;				//S12ADの省電力モード解除
	SYSTEM.PRCR.WORD = 0xA500;		//レジスタをプロテクト
	S12AD.ADCSR.BIT.ADST = 0;		//AD変換停止
	S12AD.ADCSR.BIT.ADIE = 0;		//スキャン終了後の割り込み禁止
	S12AD.ADCSR.BIT.CKS = 3;		//変換クロック選択:PCLK
	S12AD.ADCSR.BIT.ADCS = 0;		//シングルスキャンモード
	S12AD.ADADS0.WORD = 0x015e;		//加算モード選択(AN1,2,3,4,6,8)
	S12AD.ADADC.BIT.ADC = 3;		//4回加算
	S12AD.ADANS0.BIT.ANS0 = 0x01;	//AN000を変換対象とする
	S12AD.ADSSTR01.BIT.SST1 = 20;	//サンプリング時間:20ステート(20x20=400ns)
}

//-------------------------------------------------------------------
//I/Oの設定
//-------------------------------------------------------------------
void initSetting::initIO(void) {
	PORT3.PDR.BIT.B1 = 1;	//LED1
	PORT2.PDR.BIT.B7 = 1;	//LED2
	PORT5.PDR.BIT.B5 = 1;	//LED3
	PORT5.PDR.BIT.B4 = 1;	//LED4
	PORTA.PDR.BIT.B4 = 1;	//LED5
	PORTE.PDR.BIT.B5 = 1;	//LED6

	PORTB.PDR.BIT.B6 = 1;	// SEN_LED1(Middle)
	PORTB.PDR.BIT.B0 = 1;	// SEN_LED2(Side)
	PORTA.PDR.BIT.B0 = 1;	// SEN_LED3(Front)

	PORT0.PCR.BIT.B5 = 1;	// P05:入力プルアップ

	PORTC.PDR.BYTE = 0xFF;  //モータ関連
}

//-------------------------------------------------------------------
//RSPIの設定
//-------------------------------------------------------------------
void initSetting::initSPI(void) {
	SYSTEM.PRCR.WORD = 0xA502;    //プロテクトされたレジスタを解除
	MSTP(RSPI1) = 0;              //RSPI1の省電力解除
	SYSTEM.PRCR.WORD = 0xA500;    //レジスタをプロテクト

	//I/Oピンの設定
	MPC.PWPR.BYTE = 0x00;         //PSWEレジスタへの書き込み禁止
	MPC.PWPR.BYTE = 0x40u;        //PFSレジスタへの書き込み許可
	MPC.PE1PFS.BIT.PSEL = 0x0E;   //PE1をRSPCKBに設定
	MPC.PE2PFS.BIT.PSEL = 0x0E;   //PE2をMOSIBに設定
	MPC.PE3PFS.BIT.PSEL = 0x0D;   //PE3をMISOBに設定
	MPC.PE4PFS.BIT.PSEL = 0x0D;   //PE4をSSLB0に設定
	MPC.PWPR.BYTE = 0x80u;        //PFSレジスタをプロテクト

	PORTE.PMR.BIT.B1 = 1;         //PE1を周辺機能として設定
	PORTE.PMR.BIT.B2 = 1;         //PE2を周辺機能として設定
	PORTE.PMR.BIT.B3 = 1;         //PE3を周辺機能として設定
	PORTE.PMR.BIT.B4 = 1;         //PE4を周辺機能として設定

	RSPI1.SPCR.BYTE = 0;          //SPI機能停止
	RSPI1.SPSR.BYTE = 0xa0;       //ステータスレジスタクリア
	RSPI1.SPPCR.BYTE = 0x00;      //MOSI、MISO端子制御
	RSPI1.SPSCR.BYTE = 0;         //シーケンス制御
	RSPI1.SPDCR.BYTE = 0x20;      //ロングワード：SPLW=1
	RSPI1.SPCMD0.WORD = 0x0783;   //LSBF=0、SPB=7、SSLKP=1、BRDV=0、CPOL=1、CPHA=1
	RSPI1.SPBR = 49;              //ビットレート：1Mbps
	RSPI1.SPCR.BYTE = 0x98;       //SPI機能開始
	RSPI1.SPCR2.BYTE = 0;         //パリティの設定とか

	IEN(RSPI1,SPTI1)= 1;         //送信割り込み許可
	IEN(RSPI1,SPRI1)= 1;         //受信割り込み許可
}

void initSetting::initPhase(void) {
//-------------------------------------------------------------------
//R_ENCの設定
//-------------------------------------------------------------------
	SYSTEM.PRCR.WORD = 0xA502;      //プロテクトされたレジスタを解除
	MSTP(MTU1) = 0;                 //
	SYSTEM.PRCR.WORD = 0xA500;      //レジスタをプロテクト

	PORT1.PMR.BIT.B4 = 1;           //P14を周辺機能として設定
	PORT1.PMR.BIT.B5 = 1;           //P15を周辺機能として設定

	MPC.PWPR.BIT.B0WI = 0;          //プロテクト解除
	MPC.PWPR.BIT.PFSWE = 1;
	MPC.P14PFS.BIT.PSEL = 0x02;     //P14をMTCLKAに設定
	MPC.P15PFS.BIT.PSEL = 0x02;     //P15をMTCLKBに設定
	MPC.PWPR.BIT.PFSWE = 0;         //プロテクト
	MPC.PWPR.BIT.B0WI = 1;

	MTU1.TMDR.BIT.MD = 4;           //位相計数モード1に設定
	MTU1.TCNT = 32767;              //TCNT初期値

//-------------------------------------------------------------------
//L_ENCの設定
//-------------------------------------------------------------------
	SYSTEM.PRCR.WORD = 0xA502;      //プロテクトされたレジスタを解除
	MSTP(MTU2) = 0;                 //
	SYSTEM.PRCR.WORD = 0xA500;      //レジスタをプロテクト

	PORTA.PMR.BIT.B1 = 1;           //PA1を周辺機能として設定
	PORTA.PMR.BIT.B3 = 1;           //PA3を周辺機能として設定

	MTU2.TMDR.BIT.MD = 4;           //位相計数モード1に設定
	MTU2.TCNT = 32767;              //TCNT初期値

	MPC.PWPR.BIT.B0WI = 0;          //プロテクト解除
	MPC.PWPR.BIT.PFSWE = 1;
	MPC.PA1PFS.BIT.PSEL = 0x02;     //PA1をMTCLKCに設定
	MPC.PA3PFS.BIT.PSEL = 0x02;     //PA3をMTCLKDに設定
	MPC.PWPR.BIT.PFSWE = 0;         //プロテクト
	MPC.PWPR.BIT.B0WI = 1;
}

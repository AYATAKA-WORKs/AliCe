/*
 * parameter.cpp
 *
 *  Created on: 2017/09/25
 *      Author: KOTA-NPC2
 */

#include "parameter.h"

//-------------------------------------------------------------------
//	ゴール座標
//-------------------------------------------------------------------
const char X_GOAL = 7;
const char Y_GOAL = 7;

//-------------------------------------------------------------------
//	方向シンボル
//-------------------------------------------------------------------
const unsigned char L = 0;
const unsigned char R = 1;
const unsigned char STRAIGHT = 2;
const unsigned char BACK = 3;

//-------------------------------------------------------------------
//	ターンパラメータ
//-------------------------------------------------------------------
// 小回り
TurnParam sturn_L_90_500 = {4800.0, 500.0, 5.5, 13.8, 30.0, 60.0, 90.0, L, 0.998};
TurnParam sturn_R_90_500 = {4800.0, 500.0, 3.5, 21.0, 35.0, 55.0, 90.0, R, 1.000};
TurnParam sturn_L_90_700 = {11000.0, 700.0, 4.0, 23.5, 44.0, 46.0, 90.0, L, 1.000};
TurnParam sturn_R_90_700 = {11000.0, 700.0, 5.0, 30.0, 44.0, 46.0, 90.0, R, 1.00};
TurnParam sturn_L_90_800 = {4200.0, 500.0, 0.1, 4.0, 30.0, 60.0, 90.0, L, 1.00};
TurnParam sturn_R_90_800 = {4200.0, 500.0, 0.1, 4.0, 30.0, 60.0, 90.0, R, 1.00};
TurnParam sturn_L_90_1000 = {4200.0, 500.0, 0.1, 4.0, 30.0, 60.0, 90.0, L, 1.00};
TurnParam sturn_R_90_1000 = {4200.0, 500.0, 0.1, 4.0, 30.0, 60.0, 90.0, R, 1.00};
// 大回り90
TurnParam lturn_L_90_500 = {5000.0, 500.0, 0.0, 0.0, 30.0, 60.0, 90.0, L, 1.00};
TurnParam lturn_R_90_500 = {5000.0, 500.0, 0.0, 0.0, 30.0, 60.0, 90.0, R, 1.00};
TurnParam lturn_L_90_700 = {2600.0, 700.0, 15.0, 29.0, 40.0, 50.0, 90.0, L, 1.00};
TurnParam lturn_R_90_700 = {2500.0, 700.0, 14.5, 30.0, 40.0, 50.0, 90.0, R, 1.00};
TurnParam lturn_L_90_800 = {5500.0, 800.0, 0.0, 0.0, 30.0, 60.0, 90.0, L, 1.00};
TurnParam lturn_R_90_800 = {5500.0, 800.0, 0.0, 0.0, 30.0, 60.0, 90.0, R, 1.00};
TurnParam lturn_L_90_1000 = {5300.0, 1000.0, 19.0, 31.0, 30.0, 60.0, 90.0, L, 1.00};
TurnParam lturn_R_90_1000 = {5300.0, 1000.0, 11.0, 38.0, 30.0, 60.0, 90.0, R, 1.00};
// 大回り180
TurnParam lturn_L_180_700 = {3600.0, 700.0, 20.0, 33.0, 31.0, 149.0, 180.0, L, 1.00};
TurnParam lturn_R_180_700 = {3600.0, 700.0, 20.0, 37.0, 28.0, 152.0, 180.0, R, 1.00};
TurnParam lturn_L_180_800 = {5500.0, 800.0, 0.0, 0.0, 25.0, 155.0, 180.0, L, 1.00};
TurnParam lturn_R_180_800 = {5500.0, 800.0, 0.0, 0.0, 25.0, 155.0, 180.0, R, 1.00};
TurnParam lturn_L_180_1000 = {5900.0, 1000.0, 20.0, 44.0, 38.0, 142.0, 180.0, L, 1.00};
TurnParam lturn_R_180_1000 = {5900.0, 1000.0, 20.0, 44.0, 38.0, 142.0, 180.0, R, 1.00};
// 行き斜め45
TurnParam gdturn_L_45_1000 = {12000.0, 1000.0, 20.0, 81.0, 20.0, 25.0, 45.0, L, 1.00};
TurnParam gdturn_R_45_1000 = {12000.0, 1000.0, 14.0, 88.0, 20.0, 25.0, 45.0, R, 1.00};
// 行き斜め135
TurnParam gdturn_L_135_1000 = {8200.0, 1000.0, 50.0, 62.0, 65.0, 70.0, 135.0, L, 1.00};
TurnParam gdturn_R_135_1000 = {8200.0, 1000.0, 50.0, 68.0, 65.0, 70.0, 135.0, R, 1.00};
// 帰り斜め45
TurnParam rdturn_L_45_1000 = {9600.0, 1000.0, 45.0, 37.0, 20.0, 25.0, 45.0, L, 1.00};
TurnParam rdturn_R_45_1000 = {9800.0, 1000.0, 45.0, 36.5, 20.0, 25.0, 45.0, R, 1.00};
// 帰り斜め135
TurnParam rdturn_L_135_1000 = {8800.0, 1000.0, 9.0, 55.0, 32.0, 103.0, 135.0, L, 1.00};
TurnParam rdturn_R_135_1000 = {8800.0, 1000.0, 10.0, 54.0, 30.0, 105.0, 135.0, R, 1.00};
// V90斜め
TurnParam vturn_L_90_1000 = {11000.0, 1000.0, 6.5, 44.0, 44.5, 45.5, 90.0, L, 1.00};
TurnParam vturn_R_90_1000 = {12000.0, 1000.0, 15.0, 37.0, 44.5, 45.5, 90.0, R, 1.00};

TurnType turn_700 = {&sturn_L_90_700,&sturn_R_90_700,&lturn_L_90_700,&lturn_R_90_700,&lturn_L_180_700,&lturn_R_180_700,
					&gdturn_L_45_1000,&gdturn_R_45_1000,&gdturn_L_135_1000,&gdturn_R_135_1000,
					&rdturn_L_45_1000,&rdturn_R_45_1000,&rdturn_L_135_1000,&rdturn_R_135_1000,
					&vturn_L_90_1000,&vturn_R_90_1000};
TurnType turn_800 = {&sturn_L_90_800,&sturn_R_90_800,&lturn_L_90_800,&lturn_R_90_800,&lturn_L_180_800,&lturn_R_180_800,
					&gdturn_L_45_1000,&gdturn_R_45_1000,&gdturn_L_135_1000,&gdturn_R_135_1000,
					&rdturn_L_45_1000,&rdturn_R_45_1000,&rdturn_L_135_1000,&rdturn_R_135_1000,
					&vturn_L_90_1000,&vturn_R_90_1000};
TurnType turn_1000 = {&sturn_L_90_700,&sturn_R_90_700,&lturn_L_90_1000,&lturn_R_90_1000,&lturn_L_180_1000,&lturn_R_180_1000,
					&gdturn_L_45_1000,&gdturn_R_45_1000,&gdturn_L_135_1000,&gdturn_R_135_1000,
					&rdturn_L_45_1000,&rdturn_R_45_1000,&rdturn_L_135_1000,&rdturn_R_135_1000,
					&vturn_L_90_1000,&vturn_R_90_1000};

//-------------------------------------------------------------------
//	センサ閾値
//-------------------------------------------------------------------
const int SEN_LF_TH = 42;
const int SEN_RF_TH = 34;
const int SEN_LS_TH = 100;
const int SEN_RS_TH = 100;
const int SEN_LSS_TH = 100;
const int SEN_RSS_TH = 150;

const int  SEN_LSS_FIX_TH = 250;
const int  SEN_RSS_FIX_TH = 300;
const int  SEN_LSS_FIX_DEV = 300;
const int  SEN_RSS_FIX_DEV = 130;
const int  SEN_LS_FIX_TH = 100;
const int  SEN_RS_FIX_TH = 130;
const int  SEN_LS_FIX_DEV = 100;
const int  SEN_RS_FIX_DEV = 100;

const int SEN_LSS_DIG_TH = 1000;
const int SEN_RSS_DIG_TH = 800;

const int  SEN_LF_DEV = 100;
const int  SEN_RF_DEV = 100;
const int  SEN_LS_DEV = 100;
const int  SEN_RS_DEV = 100;
const int  SEN_LSS_DEV = 30;
const int  SEN_RSS_DEV = 15;

const int  SEN_LF_REF = 60;
const int  SEN_RF_REF = 40;
const int  SEN_LS_REF = 422;
const int  SEN_RS_REF = 100;
const int  SEN_LSS_REF = 750;
const int  SEN_RSS_REF = 482;

//-------------------------------------------------------------------
//	壁切れ補正
//-------------------------------------------------------------------
const float  FIX_DISTANCE_L = 63.0;
const float  FIX_DISTANCE_R = 71.0;
const float  FIX_DISTANCE_L2 = 46.0;
const float  FIX_DISTANCE_R2 = 55.0;
const float  FIX_DISTANCE_L3 = 160.0;
const float  FIX_DISTANCE_R3 = 100.0;

//-------------------------------------------------------------------
//	制御定数
//-------------------------------------------------------------------
// 速度関連
const float Kvp = 0.0001 * 0.6;
const float Kvi = 0.0001 / (0.150 * 0.5);
const float Kvd = 0.0001 * (0.150 * 0.125);
// 角速度関連
const float Kgp = 0.0005 * 0.6;
const float Kgi = 0.0005 / (0.150 * 0.5);
const float Kgd = 0.0005 * (0.030 * 0.125);
// 距離センサ関連
const float Kpp = 0.40;
const float Kpd = 0.05;

//-------------------------------------------------------------------
//	機体特性
//-------------------------------------------------------------------
const float TIRE_R = 25.0;
const float TIRE_L = 25.0;
const float TORED = 72.0;



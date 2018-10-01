/*
 * speaker_func.h
 *
 *  Created on: 2016/10/24
 *      Author: KOTA-NPC2
 */

#ifndef SPEAKER_FUNC_H_
#define SPEAKER_FUNC_H_

#define REST	0
#define A_2		110
#define Bb_2	117
#define B_2		123
#define C_3		131
#define Db_3	139
#define D_3		147
#define Eb_3	156
#define E_3   165
#define F_3		175
#define Gb_3	185
#define G_3   196
#define Ab_3  208
#define A_3		220
#define Bb_3	233
#define B_3		247
#define C_4		262
#define Db_4	277
#define D_4		294
#define Eb_4	311
#define E_4		330
#define F_4		349
#define Gb_4	370
#define G_4		392
#define Ab_4	415
#define A_4		440
#define Bb_4	466
#define B_4		494
#define C_5		523
#define Db_5	554
#define D_5		587
#define Eb_5	622
#define E_5		659
#define F_5		698
#define Gb_5	740
#define G_5		784
#define Ab_5	831
#define A_5		880
#define Bb_5	932
#define B_5		988
#define C_6		1047
#define Db_6	1109
#define D_6		1175
#define Eb_6	1245
#define E_6		1319
#define F_6		1397
#define Gb_6	1480
#define G_6		1568
#define Ab_6	1661
#define A_6		1760
#define Bb_6	1865
#define B_6		1976
#define C_7		2093
#define Db_7	2217
#define D_7		2349
#define Eb_7	2489
#define E_7		2637
#define F_7		2794
#define Gb_7	2960
#define G_7		3136
#define Ab_7	3322
#define A_7		3520
#define Bb_7	3729
#define B_7		3951
#define C_8		4186
#define Db_8	4435
#define D_8		4699
#define Eb_8	4978
#define E_8		5274
#define F_8		5588
#define Gb_8	5920
#define G_8		6272
#define Ab_8	6645

void SpeakerDisable(void);

void SpeakerEnable(void);

void buzzer_on(int frq, float beat, int bpm);

void KirbyDance(int bpm, int pitch);
void gotiusa_melody(int bpm, int pitch);
void rokutyo_melody(int bpm, int pitch);
void tori_melody(int bpm, int pitch);
void rzone_melody(int bpm, int pitch);
void ys_melody(int bpm, int pitch);
void mtdedede_melody(int bpm , int pitch);
void green_melody(int bpm,int pitch);
void JimRSini_melody(int bpm,int pitch);
void MGS_melody(int bpm,int pitch);
void miss_melody(int bpm, int pitch);
void levanPoloka (int bpm, int pitch);
void WelcomeToTheJapariPark(int bpm, int pitch);
void StartUpSound(int bpm, int pitch);

#endif /* SPEAKER_FUNC_H_ */

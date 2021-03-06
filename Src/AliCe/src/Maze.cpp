/*
 * Maze.cpp
 *
 *  Created on: 2017/09/21
 *      Author: KOTA-NPC2
 */
#include "Maze.h"
#include "var.h"
#include "interrupt.h"
#include "speaker_func.h"
#include "Serial.h"

void LEDOFF(void) {
	LED01 = 0;
	LED02 = 0;
	LED03 = 0;
	LED04 = 0;
	LED05 = 0;
	LED06 = 0;
}

void Maze::rot_matrix (int turn_direction){
	int x, y;
	x=direction_vectle[0];
	y=direction_vectle[1];
	if(turn_direction == R){
		direction_vectle[0] = y;
		direction_vectle[1] = -x;
	}
	else if(turn_direction == L){
		direction_vectle[0] = -y;
		direction_vectle[1] = x;
	}
	else if(turn_direction == BACK){
		direction_vectle[0] = -x;
		direction_vectle[1] = -y;
	}
}

void Maze::getNowPosition(int turn_direction){
	rot_matrix(turn_direction);
	now_position[0] += direction_vectle[0];
	now_position[1] += direction_vectle[1];
}

void Maze::getWallData(void){
	if(direction_vectle[0] == 0 && direction_vectle[1] == 1){
		if(mouse.photo.value[2] > SEN_LSS_TH){
			//絶対的左
			wall.data_v[now_position[0]] |= (0x1 << now_position[1]);
		}
		if(mouse.photo.value[3] > SEN_RSS_TH){
			//絶対的右
			wall.data_v[now_position[0]+ 1] |= (0x1 << now_position[1]);
		}
		if((mouse.photo.value[0] + mouse.photo.value[5]) > (SEN_LF_TH + SEN_RF_TH)){
			//絶対的上
			wall.data_h[now_position[1]+1] |= (0x1 << now_position[0]);
		}
	}
	else if(direction_vectle[0] == 0 && direction_vectle[1] == -1){
		LEDOFF();
		if(mouse.photo.value[2] > SEN_LSS_TH){
			//絶対的右
			wall.data_v[now_position[0] + 1] |= (0x1 << now_position[1]);
		}
		if(mouse.photo.value[3] > SEN_RSS_TH){
			//絶対的左
			wall.data_v[now_position[0]] |= (0x1 << now_position[1]);
		}
		if((mouse.photo.value[0] + mouse.photo.value[5]) > (SEN_LF_TH + SEN_RF_TH)){
			//絶対的下
			wall.data_h[now_position[1]] |= (0x1 << now_position[0]);
		}
	}
	else if(direction_vectle[0] == 1 && direction_vectle[1] == 0){
		LEDOFF();
		if(mouse.photo.value[2] > SEN_LSS_TH){
			//絶対的上
			wall.data_h[now_position[1] + 1] |= (0x1 << now_position[0]);
		}
		if(mouse.photo.value[3] > SEN_RSS_TH){
			//絶対的下
			wall.data_h[now_position[1]] |= (0x1 << now_position[0]);
		}
		if((mouse.photo.value[0] + mouse.photo.value[5]) > (SEN_LF_TH+ SEN_RF_TH)){
			//絶対的右
			wall.data_v[now_position[0] + 1] |= (0x1 << now_position[1]);
		}
	}
	else if(direction_vectle[0] == -1 && direction_vectle[1] == 0){
		LEDOFF();
		if(mouse.photo.value[2] > SEN_LSS_TH){
			//絶対的下
			wall.data_h[now_position[1]] |= (0x1 << now_position[0]);
		}
		if(mouse.photo.value[3] > SEN_RSS_TH){
			//絶対的上
			wall.data_h[now_position[1] + 1] |= (0x1 << now_position[0]);
		}
		if((mouse.photo.value[0] + mouse.photo.value[5]) > (SEN_LF_TH + SEN_RF_TH)){
			//絶対的左
			wall.data_v[now_position[0]] |= (0x1 << now_position[1]);
		}
	}
	// 探索情報
	wall.search_h[now_position[1]] |= (0x1 << now_position[0]);
	wall.search_h[now_position[1] + 1] |= (0x1 << now_position[0]);
	wall.search_v[now_position[0]] |= (0x1 << now_position[1]);
	wall.search_v[now_position[0] + 1] |= (0x1 << now_position[1]);
}

void Maze::getMap(char goal_x , char goal_y){
	volatile unsigned char x, y;
	volatile unsigned char q[257];      // 区画の座標(0～255)を入れる配列 左下0、右下15、左上240、右上255
	volatile short head, tail;          // 先頭位置, 末尾位置

	for(x = 0 ; x < X_SIZE ; x++){		// マップの初期化
		for(y = 0 ; y < Y_SIZE ; y++){
			step_number[x][y] = 255;
		}
	}

	step_number[goal_x][goal_y] = 0;	// 目標地点に距離０を書き込む
	q[0] = (goal_x * 16 + goal_y );		// 目標地点の座標を記憶
	head = 0;							// 先頭位置を初期化
	tail = 1;							// 末尾位置は、最後の情報位置＋１

	while( head != tail ){				// 配列の中身が空ならループを抜ける
		y = q[head] & 0x0f;       		// 配列から区画の座標を取り出す
		x = q[head] >> 4;
		head++;							// 情報を取り出したので先頭位置をずらす

		if(y < Y_SIZE){// 北側
			if(((wall.data_h[y+1] >> x) & 0x0001) == 0){//北に移動可能(壁がないか，未探索であれば移動可能)
				if(step_number[x][y+1] == 255){
					step_number[x][y+1] = step_number[x][y] + 1;
					q[tail]=( x * 16 + y + 1 );     // 次の区画の座標を記憶
					tail++;     // 情報を入れたので末尾位置をずらす
				}
			}
		}
		if(x < X_SIZE){// 東側
			if(((wall.data_v[x+1] >> y) & 0x0001) == 0){//東に移動可能(壁がないか，未探索であれば移動可能)
				if(step_number[x+1][y] == 255){
					step_number[x+1][y] = step_number[x][y] + 1;
					q[tail]=( ( x + 1 ) * 16 + y );     // 次の区画の座標を記憶
					tail++;     // 情報を入れたので末尾位置をずらす
				}
			}
		}
		if (y > 0) {// 南側
			if (((wall.data_h[y] >> x) & 0x0001) == 0) {//南に移動可能(壁がないか，未探索であれば移動可能)
				if (step_number[x][y - 1] == 255) {
					step_number[x][y - 1] = step_number[x][y] + 1;
					q[tail] = (x * 16 + y - 1);     // 次の区画の座標を記憶
					tail++;     // 情報を入れたので末尾位置をずらす
				}
			}
		}
		if(x > 0){// 西側
			if((((wall.data_v[x] >> y) & 0x0001)) == 0){//西に移動可能(壁がないか，未探索であれば移動可能)
				if(step_number[x-1][y] == 255){
					step_number[x-1][y] = step_number[x][y] + 1;
					q[tail]=( ( x - 1 ) * 16 + y );     // 次の区画の座標を記憶
					tail++;     // 情報を入れたので末尾位置をずらす
				}
			}
		}
	}
}

void Maze::initMap(char x_goal,char y_goal){
	//map情報初期化
	for (int i = 0; i < X_SIZE + 1; i++) {
		wall.data_h[i] = 0x00;
		wall.search_h[i] = 0x00;
	}
	for (int i = 0; i < Y_SIZE + 1; i++) {
		wall.data_v[i] = 0x00;
		wall.search_v[i] = 0x00;
	}
	wall.data_h[0] = 0xffff;
	wall.data_v[0] = 0xffff;
	wall.data_h[16] = 0xffff;
	wall.data_v[16] = 0xffff;
	wall.search_h[0] = 0xffff;
	wall.search_v[0] = 0xffff;
	wall.search_h[16] = 0xffff;
	wall.search_v[16] = 0xffff;

	wall.data_v[1] |= 0x0001;
	wall.search_v[1] |= 0x0001;

	//歩数情報初期化
		for(int i=0;i<16;i++){
			for(int j=0;j<16;j++){
				step_number[i][j] = 255;
			}
		}
		step_number[x_goal][y_goal] = 0;
}

int Maze::checkSearched(int _pos_x, int _pos_y, WALL _wall) {
	char check = 0;
	check += ((_wall.search_v[_pos_x] >> _pos_y) & 1);
	check += ((_wall.search_v[_pos_x + 1] >> _pos_y) & 1);
	check += ((_wall.search_h[_pos_y] >> _pos_x) & 1);
	check += ((_wall.search_h[_pos_y + 1] >> _pos_x) & 1);
	if (check == 4) {
		return 1;
	}
	else {
		return 0;
	}
}

void Maze::hidaritehou(char goal_x,char goal_y){
	// 初期化
	direction_vectle[0] = 0;
	direction_vectle[1] = 1;
	now_position[0] = 0;
	now_position[1] = 1;

	mouse.straight(5000.0,0.0,MAX_VELOCITY,MAX_VELOCITY, 90.0);

	while(1){
		getWallData();
		if(mouse.photo.value[2] < SEN_LSS_TH){
			mouse.s_turn(sturn_L_90_500);
			getNowPosition(L);
		}
		else if(mouse.photo.value[0] < SEN_LF_TH && mouse.photo.value[5] < SEN_RF_TH){
			mouse.flag.bit.enable_phot = 1;
			mouse.straight(5000.0,mouse.velocity_th, MAX_VELOCITY, MAX_VELOCITY, 180.0);
			getNowPosition(STRAIGHT);
		}
		else if(mouse.photo.value[3] < SEN_RSS_TH){
			mouse.s_turn(sturn_R_90_500);
			getNowPosition(R);
		}
		else{
			mouse.flag.bit.enable_phot = 0;
			mouse.straight(5000.0,mouse.velocity_th, 0.0, mouse.velocity_th, 90.0);
			wait_timer(500);
			mouse.distance_th=0;
			mouse.turn(800, 180,R);
			mouse.distance_th=0;
			mouse.straight(5000.0,0.0, MAX_VELOCITY, MAX_VELOCITY, 90.0);
			getNowPosition(BACK);
		}
		if (now_position[0]==goal_x && now_position[1]==goal_y) {
			getWallData();
			mouse.distance_th=0;
			mouse.straight(5000.0,mouse.velocity_th, 0.0, mouse.velocity_th, 90.0);
			mouse.distance_th=0;
			mouse.turn(800.0, 180.0, R);
			mouse.motor_l.disable();
			mouse.motor_r.disable();
			//現在位置・方向の初期化
			getNowPosition(BACK);
			CSTR (CMT2) = 0;	// CMT2停止
			buzzer_on(REST, 16.000000, 200);
			wait_timer(500);
			miss_melody(180, 2);
			break;
		}
	}
}

void Maze::adatihou (char x_goal,char y_goal){
	// 初期化
	goal_flag = 0;
	getMap(x_goal,y_goal);
	if(x_goal == 0 && y_goal == 0) {
		mouse.end_flag = 1;
		mouse.straight(7000.0,0.0,MAX_VELOCITY,MAX_VELOCITY, 90.0);
	}
	else{
		mouse.end_flag = 0;
		mouse.straight(7000.0,0.0,MAX_VELOCITY,MAX_VELOCITY, 160.0);
	}
	getNowPosition(STRAIGHT);

	if(mouse.flag.bit.falesafe == 0){
		mouse.flag.bit.falesafe = 0;
		while(1){
			mouse.distance_th=0;
			//足立法
			getWallData();
			getMap(x_goal,y_goal);
			if((direction_vectle[0] == 0) && (direction_vectle[1] == 1)){
				if((now_position[1] < 15) && ((mouse.photo.value[0] + mouse.photo.value[5] ) < (SEN_LF_TH +SEN_RF_TH)) &&
						step_number[now_position[0]][now_position[1] + 1] + 1 == step_number[now_position[0]][now_position[1]] ){
					mouse.flag.bit.enable_phot = 1;
					mouse.straight(7000.0,mouse.velocity_th, MAX_VELOCITY, MAX_VELOCITY, 180.0);
					LEDOFF();
					getNowPosition(STRAIGHT);
				}
				else if((now_position[0]) > 0 && mouse.photo.value[2] < SEN_LSS_TH &&
						step_number[now_position[0]-1][now_position[1]] + 1 == step_number[now_position[0]][now_position[1]]){
					LED06 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_L_90_700);
					getNowPosition(L);
				}
				else if((now_position[0] < 15) && mouse.photo.value[3] < SEN_RSS_TH &&
						step_number[now_position[0]+1][now_position[1]] + 1 == step_number[now_position[0]][now_position[1]]){
					LED05 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_R_90_700);
					getNowPosition(R);
				}
				else {
					mouse.end_flag = 1;
					mouse.flag.bit.enable_phot = 1;
					mouse.straight(7000.0,mouse.velocity_th, 0.0, mouse.velocity_th, 90.0);
					wait_timer(500);
					mouse.distance_th=0;
					mouse.turn(800, 180,R);
					mouse.distance_th=0;
					mouse.end_flag = 0;
					mouse.back(5000.0,0.0, 0.0, 200.0, 40.0);
					mouse.flag.bit.enable_phot = 1;
					mouse.straight(7000.0,0.0, MAX_VELOCITY, MAX_VELOCITY, 135.0);
					getNowPosition(BACK);
				}
			}

			else if((direction_vectle[0] == 0) && (direction_vectle[1] == -1)){
				if( (now_position[1] > 0) && ((mouse.photo.value[0] + mouse.photo.value[5] ) < (SEN_LF_TH +SEN_RF_TH)) &&
						step_number[now_position[0]][now_position[1] - 1] +1 == step_number[now_position[0]][now_position[1]] ){
					mouse.flag.bit.enable_phot = 1;
					mouse.end_flag = 0;
					mouse.straight(7000.0,mouse.velocity_th,MAX_VELOCITY,MAX_VELOCITY, 180.0);
					LEDOFF();
					getNowPosition(STRAIGHT);
				}
				else if((now_position[0] < 15) && mouse.photo.value[2] < SEN_LSS_TH &&
						step_number[now_position[0]+1][now_position[1]] +1 == step_number[now_position[0]][now_position[1]] ){
					LED06 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_L_90_700);
					getNowPosition(L);
				}
				else if((now_position[0] > 0) && mouse.photo.value[3] < SEN_RSS_TH &&
						step_number[now_position[0]-1][now_position[1]] +1 == step_number[now_position[0]][now_position[1]]){
					LED05 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_R_90_700);
					getNowPosition(R);
				}
				else {
					mouse.end_flag = 1;
					mouse.flag.bit.enable_phot = 1;
					mouse.straight(7000.0,mouse.velocity_th, 0.0, mouse.velocity_th, 90.0);
					wait_timer(500);
					mouse.distance_th=0;
					mouse.turn(800, 180,R);
					mouse.distance_th=0;
					mouse.end_flag = 0;
					mouse.back(5000.0,0.0, 0.0, 200.0, 40.0);
					mouse.flag.bit.enable_phot = 1;
					mouse.straight(7000.0,0.0, MAX_VELOCITY, MAX_VELOCITY, 135.0);
					getNowPosition(BACK);
				}
			}

			else if((direction_vectle[0] == 1) && (direction_vectle[1] == 0)){
				if((now_position[0] < 15) && ((mouse.photo.value[0] + mouse.photo.value[5] ) < (SEN_LF_TH +SEN_RF_TH)) &&
						step_number[now_position[0]+1][now_position[1]] +1 == step_number[now_position[0]][now_position[1]] ){
					mouse.flag.bit.enable_phot = 1;
					mouse.end_flag = 0;
					mouse.straight(7000.0,mouse.velocity_th,MAX_VELOCITY,MAX_VELOCITY, 180.0);
					LEDOFF();
					getNowPosition(STRAIGHT);
				}
				else if((now_position[1] < 15) &&  (mouse.photo.value[2] < SEN_LSS_TH) &&
						(step_number[now_position[0]][now_position[1]+1] +1 == step_number[now_position[0]][now_position[1]])){
					LED06 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_L_90_700);
					getNowPosition(L);
				}
				else if((now_position[1] > 0) && (mouse.photo.value[3] < SEN_RSS_TH) &&
						(step_number[now_position[0]][now_position[1]-1] +1 == step_number[now_position[0]][now_position[1]])){
					LED05 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_R_90_700);
					getNowPosition(R);
				}
				else {
					mouse.end_flag = 1;
					mouse.flag.bit.enable_phot = 1;
					mouse.straight(7000.0,mouse.velocity_th, 0.0, mouse.velocity_th, 90.0);
					wait_timer(500);
					mouse.distance_th=0;
					mouse.turn(800, 180,R);
					mouse.distance_th=0;
					mouse.end_flag = 0;
					mouse.back(5000.0,0.0, 0.0, 200.0, 40.0);
					mouse.flag.bit.enable_phot = 1;
					mouse.straight(7000.0,0.0, MAX_VELOCITY, MAX_VELOCITY, 135.0);
					getNowPosition(BACK);
				}
			}
			else if((direction_vectle[0] == -1) && (direction_vectle[1] == 0)){
				if((now_position[0] > 0) && ((mouse.photo.value[0] + mouse.photo.value[5] ) < (SEN_LF_TH +SEN_RF_TH)) &&
						step_number[now_position[0]-1][now_position[1]] +1 == step_number[now_position[0]][now_position[1]] ){
					mouse.flag.bit.enable_phot = 1;
					mouse.end_flag = 0;
					mouse.straight(7000.0,mouse.velocity_th,MAX_VELOCITY,MAX_VELOCITY, 180.0);
					LEDOFF();
					getNowPosition(STRAIGHT);
				}
				else if((now_position[1] > 0) && mouse.photo.value[2] < SEN_LSS_TH &&
						step_number[now_position[0]][now_position[1]-1] +1 == step_number[now_position[0]][now_position[1]] ){
					LED06 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_L_90_700);
					getNowPosition(L);
				}
				else if((now_position[1] < 15) && mouse.photo.value[3] < SEN_RSS_TH &&
						step_number[now_position[0]][now_position[1]+1] +1 == step_number[now_position[0]][now_position[1]]){
					LED05 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_R_90_700);
					getNowPosition(R);
				}
				else {
					mouse.end_flag = 1;
					mouse.flag.bit.enable_phot = 1;
					mouse.straight(7000.0,mouse.velocity_th, 0.0, mouse.velocity_th, 90.0);
					wait_timer(500);
					mouse.distance_th=0;
					mouse.turn(800, 180,R);
					mouse.distance_th=0;
					mouse.end_flag = 0;
					mouse.back(5000.0,0.0, 0.0, 200.0, 40.0);
					mouse.flag.bit.enable_phot = 1;
					mouse.straight(7000.0,0.0, MAX_VELOCITY, MAX_VELOCITY, 135.0);
					getNowPosition(BACK);
				}
			}
			else {
				mouse.flag.bit.enable_phot = 1;
				mouse.end_flag = 1;
				mouse.straight(7000.0,mouse.velocity_th, 0.0, mouse.velocity_th, 90.0);
				wait_timer(500);
				mouse.end_flag = 1;
				mouse.distance_th=0;
				mouse.turn(800, 180,R);
				mouse.distance_th=0;
				mouse.back(5000.0,0.0, 0.0, 200.0, 40.0);
				mouse.flag.bit.enable_phot = 1;
				mouse.straight(7000.0,0.0, MAX_VELOCITY, MAX_VELOCITY, 135.0);
				getNowPosition(BACK);
			}

			// ゴール処理
			if (now_position[0]==x_goal && now_position[1]==y_goal) {
				getWallData();
				mouse.distance_th=0;
				mouse.end_flag = 1;
				mouse.straight(7000.0,mouse.velocity_th, 0.0, mouse.velocity_th, 90.0);
				mouse.distance_th=0;
				mouse.turn(800.0, 180.0, R);
				mouse.distance_th=0;
	//			mouse.end_flag = 0;
	//			mouse.back(5000.0,0.0, 0.0, 500.0, 40.0);
				mouse.motor_l.disable();
				mouse.motor_r.disable();
				//現在位置・方向の初期化
				rot_matrix(BACK);
				//MAP情報のコピー
				wall_buf = wall;
				CSTR (CMT2) = 0;	// CMT2停止
				buzzer_on(REST, 16.000000, 200);
				wait_timer(500);
				KirbyDance(180, 2);
				break;
			}

			// 迷子処理
			if(step_number[now_position[0]][now_position[1]]==255){
				mouse.motor_l.disable();
				mouse.motor_r.disable();
				CSTR (CMT2) = 0;	// CMT2停止
				buzzer_on(REST, 16.000000, 200);
				wait_timer(500);
				miss_melody(180, 2);
				break;
			}

			// フェールセーフ処理
			if(mouse.flag.bit.falesafe == 1){
				SpeakerEnable();
				miss_melody(180, 2);
				SpeakerDisable();
				break;
			}

			LED05 = 0;
			LED06 = 0;
		}
	}
}

void Maze::FurukawaSearch (char x_goal,char y_goal){
	// 初期化
	goal_flag = 0;
	getMap(x_goal,y_goal);
	if(x_goal == 0 && y_goal == 0) {
		mouse.end_flag = 1;
		mouse.straight(7000.0,0.0,MAX_VELOCITY,MAX_VELOCITY, 90.0);
	}
	else{
		mouse.end_flag = 0;
		mouse.straight(7000.0,0.0,MAX_VELOCITY,MAX_VELOCITY, 160.0);
	}
	getNowPosition(STRAIGHT);

	if(mouse.flag.bit.falesafe == 0){
		mouse.flag.bit.falesafe = 0;
		while(1){
			mouse.distance_th=0;
			//古川法
			getWallData();
			getMap(x_goal,y_goal);
			if((direction_vectle[0] == 0) && (direction_vectle[1] == 1)){
				//上方向、未探索直進
				if((now_position[1] < 15) && ((mouse.photo.value[0] + mouse.photo.value[5] ) < (SEN_LF_TH +SEN_RF_TH)) &&
						checkSearched(now_position[0] + 1, now_position[1], wall) == 0 ){
					mouse.flag.bit.enable_phot = 1;
					mouse.straight(7000.0,mouse.velocity_th, MAX_VELOCITY, MAX_VELOCITY, 180.0);
					LEDOFF();
					getNowPosition(STRAIGHT);
				}
				//上方向、未探索左折
				else if((now_position[0]) > 0 && mouse.photo.value[2] < SEN_LSS_TH &&
						checkSearched(now_position[0] - 1, now_position[1], wall) == 0){
					LED06 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_L_90_700);
					getNowPosition(L);
				}
				//上方向、未探索右折
				else if((now_position[0] < 15) && mouse.photo.value[3] < SEN_RSS_TH &&
						checkSearched(now_position[0] + 1, now_position[1], wall) == 0){
					LED05 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_R_90_700);
					getNowPosition(R);
				}
				else if((now_position[1] < 15) && ((mouse.photo.value[0] + mouse.photo.value[5] ) < (SEN_LF_TH +SEN_RF_TH)) &&
						step_number[now_position[0]][now_position[1] + 1] < step_number[now_position[0]][now_position[1]] ){
					mouse.flag.bit.enable_phot = 1;
					mouse.straight(7000.0,mouse.velocity_th, MAX_VELOCITY, MAX_VELOCITY, 180.0);
					LEDOFF();
					getNowPosition(STRAIGHT);
				}
				else if((now_position[0]) > 0 && mouse.photo.value[2] < SEN_LSS_TH &&
						step_number[now_position[0] - 1][now_position[1]] < step_number[now_position[0]][now_position[1]]){
					LED06 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_L_90_700);
					getNowPosition(L);
				}
				else if((now_position[0] < 15) && mouse.photo.value[3] < SEN_RSS_TH &&
						step_number[now_position[0] + 1][now_position[1]] < step_number[now_position[0]][now_position[1]]){
					LED05 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_R_90_700);
					getNowPosition(R);
				}
				else {
					mouse.end_flag = 1;
					mouse.flag.bit.enable_phot = 1;
					mouse.straight(7000.0,mouse.velocity_th, 0.0, mouse.velocity_th, 90.0);
					wait_timer(500);
					mouse.distance_th=0;
					mouse.turn(800, 180,R);
					mouse.distance_th=0;
					mouse.end_flag = 0;
					mouse.back(5000.0,0.0, 0.0, 200.0, 40.0);
					mouse.flag.bit.enable_phot = 1;
					mouse.straight(7000.0,0.0, MAX_VELOCITY, MAX_VELOCITY, 135.0);
					getNowPosition(BACK);
				}
			}

			else if((direction_vectle[0] == 0) && (direction_vectle[1] == -1)){
				//下方向、未探索直進
				if( (now_position[1] > 0) && ((mouse.photo.value[0] + mouse.photo.value[5] ) < (SEN_LF_TH +SEN_RF_TH)) &&
						checkSearched(now_position[0], now_position[1] - 1, wall) == 0){
					mouse.flag.bit.enable_phot = 1;
					mouse.end_flag = 0;
					mouse.straight(7000.0,mouse.velocity_th,MAX_VELOCITY,MAX_VELOCITY, 180.0);
					LEDOFF();
					getNowPosition(STRAIGHT);
				}
				//下方向、未探索左折
				else if((now_position[0] < 15) && mouse.photo.value[2] < SEN_LSS_TH &&
						checkSearched(now_position[0] + 1, now_position[1], wall) == 0){
					LED06 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_L_90_700);
					getNowPosition(L);
				}
				//下方向、未探索右折
				else if((now_position[0] > 0) && mouse.photo.value[3] < SEN_RSS_TH &&
						checkSearched(now_position[0] - 1, now_position[1], wall) == 0){
					LED05 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_R_90_700);
					getNowPosition(R);
				}
				else if( (now_position[1] > 0) && ((mouse.photo.value[0] + mouse.photo.value[5] ) < (SEN_LF_TH +SEN_RF_TH)) &&
						step_number[now_position[0]][now_position[1] - 1] < step_number[now_position[0]][now_position[1]] ){
					mouse.flag.bit.enable_phot = 1;
					mouse.end_flag = 0;
					mouse.straight(7000.0,mouse.velocity_th,MAX_VELOCITY,MAX_VELOCITY, 180.0);
					LEDOFF();
					getNowPosition(STRAIGHT);
				}
				else if((now_position[0] < 15) && mouse.photo.value[2] < SEN_LSS_TH &&
						step_number[now_position[0] + 1][now_position[1]] < step_number[now_position[0]][now_position[1]] ){
					LED06 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_L_90_700);
					getNowPosition(L);
				}
				else if((now_position[0] > 0) && mouse.photo.value[3] < SEN_RSS_TH &&
						step_number[now_position[0] - 1][now_position[1]] < step_number[now_position[0]][now_position[1]]){
					LED05 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_R_90_700);
					getNowPosition(R);
				}
				else {
					mouse.end_flag = 1;
					mouse.flag.bit.enable_phot = 1;
					mouse.straight(7000.0,mouse.velocity_th, 0.0, mouse.velocity_th, 90.0);
					wait_timer(500);
					mouse.distance_th=0;
					mouse.turn(800, 180,R);
					mouse.distance_th=0;
					mouse.end_flag = 0;
					mouse.back(5000.0,0.0, 0.0, 200.0, 40.0);
					mouse.flag.bit.enable_phot = 1;
					mouse.straight(7000.0,0.0, MAX_VELOCITY, MAX_VELOCITY, 135.0);
					getNowPosition(BACK);
				}
			}

			else if((direction_vectle[0] == 1) && (direction_vectle[1] == 0)){
				//右方向、未探索直進
				if((now_position[0] < 15) && ((mouse.photo.value[0] + mouse.photo.value[5] ) < (SEN_LF_TH +SEN_RF_TH)) &&
						checkSearched(now_position[0] + 1, now_position[1], wall) == 0){
					mouse.flag.bit.enable_phot = 1;
					mouse.end_flag = 0;
					mouse.straight(7000.0,mouse.velocity_th,MAX_VELOCITY,MAX_VELOCITY, 180.0);
					LEDOFF();
					getNowPosition(STRAIGHT);
				}
				//右方向、未探索左折
				else if((now_position[1] < 15) &&  mouse.photo.value[2] < SEN_LSS_TH &&
						checkSearched(now_position[0], now_position[1] + 1, wall) == 0){
					LED06 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_L_90_700);
					getNowPosition(L);
				}
				//右方向、未探索右折
				else if((now_position[1] > 0) && mouse.photo.value[3] < SEN_RSS_TH &&
						checkSearched(now_position[0], now_position[1] - 1, wall) == 0){
					LED05 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_R_90_700);
					getNowPosition(R);
				}
				else if((now_position[0] < 15) && ((mouse.photo.value[0] + mouse.photo.value[5] ) < (SEN_LF_TH +SEN_RF_TH)) &&
						step_number[now_position[0]+1][now_position[1]] < step_number[now_position[0]][now_position[1]] ){
					mouse.flag.bit.enable_phot = 1;
					mouse.end_flag = 0;
					mouse.straight(7000.0,mouse.velocity_th,MAX_VELOCITY,MAX_VELOCITY, 180.0);
					LEDOFF();
					getNowPosition(STRAIGHT);
				}
				else if((now_position[1] < 15) &&  mouse.photo.value[2] < SEN_LSS_TH &&
						step_number[now_position[0]][now_position[1] + 1] < step_number[now_position[0]][now_position[1]]){
					LED06 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_L_90_700);
					getNowPosition(L);
				}
				else if((now_position[1] > 0) && mouse.photo.value[3] < SEN_RSS_TH &&
						step_number[now_position[0]][now_position[1] - 1] < step_number[now_position[0]][now_position[1]]){
					LED05 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_R_90_700);
					getNowPosition(R);
				}
				else {
					mouse.end_flag = 1;
					mouse.flag.bit.enable_phot = 1;
					mouse.straight(7000.0,mouse.velocity_th, 0.0, mouse.velocity_th, 90.0);
					wait_timer(500);
					mouse.distance_th=0;
					mouse.turn(800, 180,R);
					mouse.distance_th=0;
					mouse.end_flag = 0;
					mouse.back(5000.0,0.0, 0.0, 200.0, 40.0);
					mouse.flag.bit.enable_phot = 1;
					mouse.straight(7000.0,0.0, MAX_VELOCITY, MAX_VELOCITY, 135.0);
					getNowPosition(BACK);
				}
			}
			else if((direction_vectle[0] == -1) && (direction_vectle[1] == 0)){
				//左方向、未探索直進
				if((now_position[0] > 0) && ((mouse.photo.value[0] + mouse.photo.value[5] ) < (SEN_LF_TH +SEN_RF_TH)) &&
						checkSearched(now_position[0] - 1, now_position[1], wall) == 0){
					mouse.flag.bit.enable_phot = 1;
					mouse.end_flag = 0;
					mouse.straight(7000.0,mouse.velocity_th,MAX_VELOCITY,MAX_VELOCITY, 180.0);
					LEDOFF();
					getNowPosition(STRAIGHT);
				}
				//左方向、未探索左折
				else if((now_position[1] > 0) && mouse.photo.value[2] < SEN_LSS_TH &&
						checkSearched(now_position[0], now_position[1] - 1, wall) == 0){
					LED06 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_L_90_700);
					getNowPosition(L);
				}
				//左方向、未探索右折
				else if((now_position[1] < 15) && mouse.photo.value[3] < SEN_RSS_TH &&
						checkSearched(now_position[0], now_position[1] + 1, wall) == 0){
					LED05 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_R_90_700);
					getNowPosition(R);
				}
				else if((now_position[0] > 0) && ((mouse.photo.value[0] + mouse.photo.value[5] ) < (SEN_LF_TH +SEN_RF_TH)) &&
						step_number[now_position[0]-1][now_position[1]] < step_number[now_position[0]][now_position[1]] ){
					mouse.flag.bit.enable_phot = 1;
					mouse.end_flag = 0;
					mouse.straight(7000.0,mouse.velocity_th,MAX_VELOCITY,MAX_VELOCITY, 180.0);
					LEDOFF();
					getNowPosition(STRAIGHT);
				}
				else if((now_position[1] > 0) && mouse.photo.value[2] < SEN_LSS_TH &&
						step_number[now_position[0]][now_position[1]-1] < step_number[now_position[0]][now_position[1]] ){
					LED06 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_L_90_700);
					getNowPosition(L);
				}
				else if((now_position[1] < 15) && mouse.photo.value[3] < SEN_RSS_TH &&
						step_number[now_position[0]][now_position[1]+1] < step_number[now_position[0]][now_position[1]]){
					LED05 = 1;
					mouse.end_flag = 1;
					mouse.s_turn(sturn_R_90_700);
					getNowPosition(R);
				}
				else {
					mouse.end_flag = 1;
					mouse.flag.bit.enable_phot = 1;
					mouse.straight(7000.0,mouse.velocity_th, 0.0, mouse.velocity_th, 90.0);
					wait_timer(500);
					mouse.distance_th=0;
					mouse.turn(800, 180,R);
					mouse.distance_th=0;
					mouse.end_flag = 0;
					mouse.back(5000.0,0.0, 0.0, 200.0, 40.0);
					mouse.flag.bit.enable_phot = 1;
					mouse.straight(7000.0,0.0, MAX_VELOCITY, MAX_VELOCITY, 135.0);
					getNowPosition(BACK);
				}
			}
			else {
				mouse.flag.bit.enable_phot = 1;
				mouse.end_flag = 1;
				mouse.straight(7000.0,mouse.velocity_th, 0.0, mouse.velocity_th, 90.0);
				wait_timer(500);
				mouse.end_flag = 1;
				mouse.distance_th=0;
				mouse.turn(800, 180,R);
				mouse.distance_th=0;
				mouse.back(5000.0,0.0, 0.0, 200.0, 40.0);
				mouse.flag.bit.enable_phot = 1;
				mouse.straight(7000.0,0.0, MAX_VELOCITY, MAX_VELOCITY, 135.0);
				getNowPosition(BACK);
			}

			// ゴール処理
			if (now_position[0]==x_goal && now_position[1]==y_goal) {
				getWallData();
				mouse.distance_th=0;
				mouse.end_flag = 1;
				mouse.straight(7000.0,mouse.velocity_th, 0.0, mouse.velocity_th, 90.0);
				mouse.distance_th=0;
				mouse.turn(800.0, 180.0, R);
				mouse.distance_th=0;
	//			mouse.end_flag = 0;
	//			mouse.back(5000.0,0.0, 0.0, 500.0, 40.0);
				mouse.motor_l.disable();
				mouse.motor_r.disable();
				//現在位置・方向の初期化
				rot_matrix(BACK);
				//MAP情報のコピー
				wall_buf = wall;
				CSTR (CMT2) = 0;	// CMT2停止
				buzzer_on(REST, 16.000000, 200);
				wait_timer(500);
				KirbyDance(180, 2);
				break;
			}

			// 迷子処理
			if(step_number[now_position[0]][now_position[1]]==255){
				direction_vectle[0] = 0;
				direction_vectle[1] = 1;
				now_position[0] = 0;
				now_position[1] = 0;
				mouse.motor_l.disable();
				mouse.motor_r.disable();
				CSTR (CMT2) = 0;	// CMT2停止
				buzzer_on(REST, 16.000000, 200);
				wait_timer(500);
				miss_melody(180, 2);
				break;
			}

			// フェールセーフ処理
			if(mouse.flag.bit.falesafe == 1){
				direction_vectle[0] = 0;
				direction_vectle[1] = 1;
				now_position[0] = 0;
				now_position[1] = 0;
				SpeakerEnable();
				miss_melody(180, 2);
				SpeakerDisable();
				break;
			}

			LED05 = 0;
			LED06 = 0;
		}
	}
}

void Maze::getPath(char x_goal,char y_goal){
	// 初期化
	int _num = 0;
	path.num = 0;
	for(int i=0; i<512; i++){
		path.type[i] = STGT;
		path.step[i] = 0;
	}
	//	未知壁を塞ぐ
	for(int i=0; i<17; i++){
		for(int j=0; j<16; j++){
			if(((wall.search_h[i]>>j)&0x0001)==0)	wall.data_h[i] |= (0x0001 << j);
			if(((wall.search_v[i]>>j)&0x0001)==0)	wall.data_v[i] |= (0x0001 << j);
		}
	}

	// 初期条件
	direction_vectle[0] = 0;
	direction_vectle[1] = 1;
	now_position[0] = 0;
	now_position[1] = 1;
	path.type[_num] = STGT;
	path.step[_num] = 1;

	while(1){
		getMap(x_goal,y_goal);
		//足立法
		if((direction_vectle[0] == 0) && (direction_vectle[1] == 1)){
			if((step_number[now_position[0]][now_position[1] + 1] +1 == step_number[now_position[0]][now_position[1]])&&(((wall.data_h[now_position[1]+1] >> now_position[0]) & 0x0001) == 0)){
				if(path.type[_num] != STGT)	_num++;
				path.type[_num] = STGT;
				path.step[_num] += 2;
				getNowPosition(STRAIGHT);
			}
			else if((step_number[now_position[0]-1][now_position[1]] +1 == step_number[now_position[0]][now_position[1]])&&(((wall.data_v[now_position[0]] >> now_position[1]) & 0x0001) == 0)){
				if(path.type[_num] != SMLL)	_num++;
				path.type[_num] = SMLL;
				path.step[_num] += 1;
				getNowPosition(L);
			}
			else if((step_number[now_position[0]+1][now_position[1]] +1 == step_number[now_position[0]][now_position[1]])&&(((wall.data_v[now_position[0]+1] >> now_position[1]) & 0x0001) == 0)){
				if(path.type[_num] != SMLR)	_num++;
				path.type[_num] = SMLR;
				path.step[_num] += 1;
				getNowPosition(R);
			}
			else {
				getNowPosition(BACK);
			}
		}

		else if((direction_vectle[0] == 0) && (direction_vectle[1] == -1)){
			if((step_number[now_position[0]][now_position[1] - 1] +1 == step_number[now_position[0]][now_position[1]] )&&(((wall.data_h[now_position[1]] >> now_position[0]) & 0x0001) == 0)){
				if(path.type[_num] != STGT)	_num++;
				path.type[_num] = STGT;
				path.step[_num] += 2;
				getNowPosition(STRAIGHT);
			}
			else if(step_number[now_position[0]+1][now_position[1]] +1 == step_number[now_position[0]][now_position[1]]&&(((wall.data_v[now_position[0]+1] >> now_position[1]) & 0x0001) == 0)){
				if(path.type[_num] != SMLL)	_num++;
				path.type[_num] = SMLL;
				path.step[_num] += 1;
				getNowPosition(L);
			}
			else if(step_number[now_position[0]-1][now_position[1]] +1 == step_number[now_position[0]][now_position[1]]&&(((wall.data_v[now_position[0]] >> now_position[1]) & 0x0001) == 0)){
				if(path.type[_num] != SMLR)	_num++;
				path.type[_num] = SMLR;
				path.step[_num] += 1;
				getNowPosition(R);
			}
			else {
				getNowPosition(BACK);
			}
		}

		else if((direction_vectle[0] == 1) && (direction_vectle[1] == 0)){
			if(step_number[now_position[0]+1][now_position[1]] +1 == step_number[now_position[0]][now_position[1]]&&(((wall.data_v[now_position[0]+1] >> now_position[1]) & 0x0001) == 0)){
				if(path.type[_num] != STGT)	_num++;
				path.type[_num] = STGT;
				path.step[_num] += 2;
				getNowPosition(STRAIGHT);
			}
			else if(step_number[now_position[0]][now_position[1]+1] +1 == step_number[now_position[0]][now_position[1]]&&(((wall.data_h[now_position[1]+1] >> now_position[0]) & 0x0001) == 0)){
				if(path.type[_num] != SMLL)	_num++;
				path.type[_num] = SMLL;
				path.step[_num] += 1;
				getNowPosition(L);
			}
			else if(step_number[now_position[0]][now_position[1]-1] +1 == step_number[now_position[0]][now_position[1]]&&(((wall.data_h[now_position[1]] >> now_position[0]) & 0x0001) == 0)){
				if(path.type[_num] != SMLR)	_num++;
				path.type[_num] = SMLR;
				path.step[_num] += 1;
				getNowPosition(R);
			}
			else {
				getNowPosition(BACK);
			}
		}
		else if((direction_vectle[0] == -1) && (direction_vectle[1] == 0)){
			if(step_number[now_position[0]-1][now_position[1]] +1 == step_number[now_position[0]][now_position[1]]&&(((wall.data_v[now_position[0]] >> now_position[1]) & 0x0001) == 0)){
				if(path.type[_num] != STGT)	_num++;
				path.type[_num] = STGT;
				path.step[_num] += 2;
				getNowPosition(STRAIGHT);
			}
			else if(step_number[now_position[0]][now_position[1]-1] +1 == step_number[now_position[0]][now_position[1]]&&(((wall.data_h[now_position[1]] >> now_position[0]) & 0x0001) == 0)){
				if(path.type[_num] != SMLL)	_num++;
				path.type[_num] = SMLL;
				path.step[_num] += 1;
				getNowPosition(L);
			}
			else if(step_number[now_position[0]][now_position[1]+1] +1 == step_number[now_position[0]][now_position[1]]&&(((wall.data_h[now_position[1]+1] >> now_position[0]) & 0x0001) == 0)){
				if(path.type[_num] != SMLR)	_num++;
				path.type[_num] = SMLR;
				path.step[_num] += 1;
				getNowPosition(R);
			}
			else {
				getNowPosition(BACK);
			}
		}
		else {
		}

		path.num = _num;

		// ゴール処理
		if (now_position[0]==x_goal && now_position[1]==y_goal) {
			getNowPosition(BACK);
			break;
		}
	}
}

void Maze::runPath(char mode, float maxVelocity, TurnType type){
	mouse.accel = 0.0;
	mouse.velocity_th = 0.0;
	mouse.distance_th = 0.0;
	switch(mode){
	case 0:
		ShortestRunS(6000.0, maxVelocity, type);
		break;
	case 1:
		ShortestRunL(6000.0, maxVelocity, type);
		break;
	default:
		break;
	}
}

void Maze::ShortestRunS(float accel, float maxVelocity, TurnType type){
	for(int i=0; i <= path.num; i++){
		switch(path.type[i]){
		case STGT:
			mouse.flag.bit.enable_phot = 1;
			if(i != 0){
				if(path.step[i]==1){
					mouse.end_flag = 1;
					mouse.straight(9000.0,mouse.velocity_th, type.s_turn_l->velocity, type.s_turn_l->velocity, 90.0 * path.step[i]);
				}
				else{
					mouse.end_flag = 1;
					mouse.straight(accel,mouse.velocity_th, type.s_turn_l->velocity, maxVelocity, 90.0 * path.step[i]);
				}
			}
			else{
				if(path.step[i]==1){
					mouse.end_flag = 1;
					mouse.straight(9000.0,0.0, type.s_turn_l->velocity, type.s_turn_l->velocity, 90.0 * path.step[i]);
				}
				else{
					mouse.end_flag = 1;
					mouse.straight(accel,0.0, type.s_turn_l->velocity, maxVelocity, 90.0 * path.step[i]);
				}
			}
			break;
		case SMLL:
			for(int j=0; j<path.step[i]; j++){
				mouse.end_flag = 1;
				mouse.s_turn(*type.s_turn_l);
			}
			break;
		case SMLR:
			for(int j=0; j<path.step[i]; j++){
				mouse.end_flag = 1;
				mouse.s_turn(*type.s_turn_r);
			}
			break;
		}
	}
	mouse.straight(accel,mouse.velocity_th, 0.0, mouse.velocity_th, 90.0);
}

void Maze::ShortestRunL(float accel, float maxVelocity, TurnType type){
	for(int i=0; i <= path.num; i++){
		switch(path.type[i]){
		case STGT:
			mouse.flag.bit.enable_phot = 1;
			if(i != 0){
				if(path.step[i]==1){
					mouse.end_flag = 1;
					mouse.straight(5000.0,mouse.velocity_th, type.s_turn_l->velocity, type.s_turn_l->velocity, 90.0 * path.step[i]);
				}
				else{
					mouse.end_flag = 1;
					mouse.straight(accel,mouse.velocity_th, type.s_turn_l->velocity, maxVelocity, 90.0 * path.step[i]);
				}
			}
			else{
				if(path.step[i]==1){
					mouse.end_flag = 1;
						mouse.straight(5000.0,0.0, type.s_turn_l->velocity, type.s_turn_l->velocity, 90.0 * path.step[i]);
				}
				else{
					mouse.end_flag = 1;
					mouse.straight(accel,0.0, type.s_turn_l->velocity, maxVelocity, 90.0 * path.step[i]);
				}

			}
			break;
		case SMLL:
			if(path.step[i]==1){
				mouse.end_flag = 1;
				mouse.s_turn(*type.s_turn_l);
				myprintf("%f\r\n",type.s_turn_l->velocity);
			}
			else if(path.step[i]==2){
				mouse.end_flag = 1;
				mouse.s_turn(*type.l_turn180_l);
				myprintf("%f\r\n",type.l_turn180_r->velocity);
			}
			break;
		case SMLR:
			if(path.step[i]==1){
				mouse.end_flag = 1;
				mouse.s_turn(*type.s_turn_r);
				myprintf("%f\r\n",type.s_turn_r->velocity);
			}
			else if(path.step[i]==2){
				mouse.end_flag = 1;
				mouse.s_turn(*type.l_turn180_r);
				myprintf("%f\r\n",type.l_turn180_r->velocity);
			}
			break;
		}
	}
	mouse.straight(accel,mouse.velocity_th, 0.0, mouse.velocity_th, 90.0);
}

void Maze::printfMap(WALL _wall){
	for (int i = 16; i > 0; i--) {
		for (int j = 0; j < 16; j++) {
			if (((_wall.data_h[i] >> j) & 0x01) == 1) myprintf("+---");
			else myprintf("+   ");
		}
		myprintf("+\n\r");
		for (int j = 0; j <16; j++) {
			if (((_wall.data_v[j] >> (i-1)) & 0x01) == 1) myprintf("|%3d", step_number[j][i-1]);
			else myprintf(" %3d", step_number[j][i-1]);
		}
		myprintf("|\n\r");
	}
	for (int j = 0; j < 16; j++) {
		if (((_wall.data_h[0] >> j) & 0x01) == 1) myprintf("+---");
	}
	myprintf("+\n");
}

void Maze::makePathMap(char x_goal, char y_goal, WALL _wall) {
	// 未探索壁を埋める
	for (int i = 0; i<17; i++) {
		for (int j = 0; j<16; j++) {
			if (((_wall.search_h[i] >> j) & 0x0001) == 0)	_wall.data_h[i] |= (0x0001 << j);
			if (((_wall.search_v[i] >> j) & 0x0001) == 0)	_wall.data_v[i] |= (0x0001 << j);
		}
	}

	// ノードと壁情報の対応付け
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			node[i][2 * j].enable = ((_wall.data_v[i + 1] >> j) & 0x1);
		}
		for (int j = 1; j < 16; j++) {
			node[i][2 * j - 1].enable = ((_wall.data_h[j] >> i) & 0x1);
		}
	}

	// キューによる歩数マップ作成
	volatile unsigned char x, y;
	volatile unsigned short q[257];     // 区画の座標を入れる配列
	volatile short head, tail;          // 先頭位置, 末尾位置

	for (x = 0; x < 16; x++) {		// マップの初期化
		for (y = 0; y < 31; y++) {
			node[x][y].step = 4095;
		}
	}

	head = 0;							// 先頭位置を初期化
	tail = 0;							// 末尾位置を初期化

	// 目標地点に距離0を書き込む
	if (x_goal - 1 >= 0) {
		if (node[x_goal - 1][2 * y_goal].enable == 0) {
			node[x_goal - 1][2 * y_goal].step = 0;
			q[tail] = ((x_goal - 1) * 256 + 2 * y_goal);		// 目標地点の座標を記憶
			tail++;
		}
	}
	if (2 * y_goal - 1 >= 0) {
		if (node[x_goal][2 * y_goal - 1].enable == 0) {
			node[x_goal][2 * y_goal - 1].step = 0;
			q[tail] = (x_goal * 256 + 2 * y_goal - 1);			// 目標地点の座標を記憶
			tail++;
		}
	}
	if (2 * y_goal + 1 <= 30) {
		if (node[x_goal][2 * y_goal].enable == 0) {
			node[x_goal][2 * y_goal].step = 0;
			q[tail] = (x_goal * 256 + 2 * y_goal);				// 目標地点の座標を記憶
			tail++;
		}
	}
	if (2 * y_goal <= 30) {
		if (node[x_goal][2 * y_goal + 1].enable == 0) {
			node[x_goal][2 * y_goal + 1].step = 0;
			q[tail] = (x_goal * 256 + 2 * y_goal + 1);			// 目標地点の座標を記憶
			tail++;
		}
	}

	while (head != tail) {				// 配列の中身が空ならループを抜ける

		y = q[head % 257] & 0xff;   	// 配列から区画の座標を取り出す
		x = q[head % 257] >> 8;
		head++;							// 情報を取り出したので先頭位置をずらす

		if (y % 2 == 0) {	// 東壁にいる時の処理
			if (x - 1 >= 0) {// (x-1,y)
				if (node[x - 1][y].enable == 0) {//(x-1,y)に移動可能(壁がないか，未探索であれば移動可能)
					if (node[x - 1][y].step == 4095) {
						node[x - 1][y].step = node[x][y].step + 1;
						q[tail % 257] = ((x - 1) * 256 + y);     // 次の区画の座標を記憶
						tail++;     // 情報を入れたので末尾位置をずらす
					}
				}
			}
			if (y + 1 < 31) {// (x,y+1)
				if (node[x][y + 1].enable == 0) {//(x,y+1)に移動可能(壁がないか，未探索であれば移動可能)
					if (node[x][y + 1].step == 4095) {
						node[x][y + 1].step = node[x][y].step + 1;
						q[tail % 257] = (x * 256 + y + 1);     // 次の区画の座標を記憶
						tail++;     // 情報を入れたので末尾位置をずらす
					}
				}
			}
			if (y - 1 >= 0) {// (x,y-1)
				if (node[x][y - 1].enable == 0) {//(x,y-1)に移動可能(壁がないか，未探索であれば移動可能)
					if (node[x][y - 1].step == 4095) {
						node[x][y - 1].step = node[x][y].step + 1;
						q[tail % 257] = (x * 256 + y - 1);     // 次の区画の座標を記憶
						tail++;     // 情報を入れたので末尾位置をずらす
					}
				}
			}
			if (x + 1 <16 && y - 1 >= 0) {// (x+1,y-1)
				if (node[x + 1][y - 1].enable == 0) {//(x+1,y-1)に移動可能(壁がないか，未探索であれば移動可能)
					if (node[x + 1][y - 1].step == 4095) {
						node[x + 1][y - 1].step = node[x][y].step + 1;
						q[tail % 257] = ((x + 1) * 256 + y - 1);     // 次の区画の座標を記憶
						tail++;     // 情報を入れたので末尾位置をずらす
					}
				}
			}
			if (x + 1 <16 && y + 1 < 31) {// (x+1,y+1)
				if (node[x + 1][y + 1].enable == 0) {//(x-1,y+1)に移動可能(壁がないか，未探索であれば移動可能)
					if (node[x + 1][y + 1].step == 4095) {
						node[x + 1][y + 1].step = node[x][y].step + 1;
						q[tail % 257] = ((x + 1) * 256 + y + 1);     // 次の区画の座標を記憶
						tail++;     // 情報を入れたので末尾位置をずらす
					}
				}
			}
			if (x + 1 < 16) {// (x + 1,y)
				if (node[x + 1][y].enable == 0) {//(x+1,y)に移動可能(壁がないか，未探索であれば移動可能)
					if (node[x + 1][y].step == 4095) {
						node[x + 1][y].step = node[x][y].step + 1;
						q[tail % 257] = ((x + 1) * 256 + y);     // 次の区画の座標を記憶
						tail++;     // 情報を入れたので末尾位置をずらす
					}
				}
			}
		}
		if (y % 2 == 1) {	//北壁にいる時の処理
			if (y + 2 < 31) {// (x,y+2)
				if (node[x][y + 2].enable == 0) {//(x,y+2)に移動可能(壁がないか，未探索であれば移動可能)
					if (node[x][y + 2].step == 4095) {
						node[x][y + 2].step = node[x][y].step + 1;
						q[tail % 257] = (x * 256 + y + 2);     // 次の区画の座標を記憶
						tail++;     // 情報を入れたので末尾位置をずらす
					}
				}
			}
			if (y + 1 < 31) {// (x,y+1)
				if (node[x][y + 1].enable == 0) {//(x,y+1)に移動可能(壁がないか，未探索であれば移動可能)
					if (node[x][y + 1].step == 4095) {
						node[x][y + 1].step = node[x][y].step + 1;
						q[tail % 257] = (x * 256 + y + 1);     // 次の区画の座標を記憶
						tail++;     // 情報を入れたので末尾位置をずらす
					}
				}
			}
			if (y - 1 >= 0) {// (x,y-1)
				if (node[x][y - 1].enable == 0) {//(x,y-1)に移動可能(壁がないか，未探索であれば移動可能)
					if (node[x][y - 1].step == 4095) {
						node[x][y - 1].step = node[x][y].step + 1;
						q[tail % 257] = (x * 256 + y - 1);     // 次の区画の座標を記憶
						tail++;     // 情報を入れたので末尾位置をずらす
					}
				}
			}
			if (y - 2 >= 0) {// (x,y-2)
				if (node[x][y - 2].enable == 0) {//(x,y-2)に移動可能(壁がないか，未探索であれば移動可能)
					if (node[x][y - 2].step == 4095) {
						node[x][y - 2].step = node[x][y].step + 1;
						q[tail % 257] = (x * 256 + y - 2);     // 次の区画の座標を記憶
						tail++;     // 情報を入れたので末尾位置をずらす
					}
				}
			}
			if (x - 1 >= 0 && y + 1 < 31) {// (x-1,y+1)
				if (node[x - 1][y + 1].enable == 0) {//(x-1,y+1)に移動可能(壁がないか，未探索であれば移動可能)
					if (node[x - 1][y + 1].step == 4095) {
						node[x - 1][y + 1].step = node[x][y].step + 1;
						q[tail % 257] = ((x - 1) * 256 + y + 1);     // 次の区画の座標を記憶
						tail++;     // 情報を入れたので末尾位置をずらす
					}
				}
			}
			if (x - 1 >= 0 && y - 1 >= 0) {// (x-1,y-1)
				if (node[x - 1][y - 1].enable == 0) {//(x-1,y+1)に移動可能(壁がないか，未探索であれば移動可能)
					if (node[x - 1][y - 1].step == 4095) {
						node[x - 1][y - 1].step = node[x][y].step + 1;
						q[tail % 257] = ((x - 1) * 256 + y - 1);     // 次の区画の座標を記憶
						tail++;     // 情報を入れたので末尾位置をずらす
					}
				}
			}
		}
	}
}

void Maze::makePathOrigin(void) {
	// 初期化
	int _num = 0;
	path_origin.num = 0;
	for (int i = 0; i<512; i++) {
		path_origin.type[i] = STGT;
		path_origin.step[i] = 0;
	}

	// 初期条件
	vrmouse.pos_x = 0;
	vrmouse.pos_y = 1;
	vrmouse.dir = North;
	path_origin.type[_num] = STGT;
	path_origin.step[_num] = 1;

	while (1) {
		if (vrmouse.dir == West) {	// 機体西向き
			// 北西(x,y+1)に移動
			if (vrmouse.pos_y + 1 < 31 && node[vrmouse.pos_x][vrmouse.pos_y + 1].enable == 0
				&& node[vrmouse.pos_x][vrmouse.pos_y].step == node[vrmouse.pos_x][vrmouse.pos_y + 1].step + 1) {
				if (path_origin.type[_num] != SMLR)	_num++;
				path_origin.type[_num] = SMLR;
				path_origin.step[_num] += 1;
				vrmouse.pos_x += 0;
				vrmouse.pos_y += 1;
				vrmouse.dir = North;
			}
			// 南西(x,y-1)に移動
			else if (vrmouse.pos_y - 1 >= 0 && node[vrmouse.pos_x][vrmouse.pos_y - 1].enable == 0
				&& node[vrmouse.pos_x][vrmouse.pos_y].step == node[vrmouse.pos_x][vrmouse.pos_y - 1].step + 1) {
				if (path_origin.type[_num] != SMLL)	_num++;
				path_origin.type[_num] = SMLL;
				path_origin.step[_num] += 1;
				vrmouse.pos_x += 0;
				vrmouse.pos_y += -1;
				vrmouse.dir = South;
			}
			// 西(x-1,y)に移動
			else if (vrmouse.pos_x - 1 >= 0 && node[vrmouse.pos_x - 1][vrmouse.pos_y].enable == 0
				&& node[vrmouse.pos_x][vrmouse.pos_y].step == node[vrmouse.pos_x - 1][vrmouse.pos_y].step + 1) {
				if (path_origin.type[_num] != STGT)	_num++;
				path_origin.type[_num] = STGT;
				path_origin.step[_num] += 2;
				vrmouse.pos_x += -1;
				vrmouse.pos_y += 0;
				vrmouse.dir = West;
			}
		}
		else if (vrmouse.dir == East) {	// 機体東向き
			// 南東(x+1,y-1)に移動
			if (vrmouse.pos_x + 1 < 16 && vrmouse.pos_y - 1 >= 0 && node[vrmouse.pos_x + 1][vrmouse.pos_y - 1].enable == 0
				&& node[vrmouse.pos_x][vrmouse.pos_y].step == node[vrmouse.pos_x + 1][vrmouse.pos_y - 1].step + 1) {
				if (path_origin.type[_num] != SMLR)	_num++;
				path_origin.type[_num] = SMLR;
				path_origin.step[_num] += 1;
				vrmouse.pos_x += 1;
				vrmouse.pos_y += -1;
				vrmouse.dir = South;
			}
			// 北東(x+1,y+1)に移動
			else if (vrmouse.pos_x + 1 < 16 && vrmouse.pos_y + 1 < 31 && node[vrmouse.pos_x + 1][vrmouse.pos_y + 1].enable == 0
				&& node[vrmouse.pos_x][vrmouse.pos_y].step == node[vrmouse.pos_x + 1][vrmouse.pos_y + 1].step + 1) {
				if (path_origin.type[_num] != SMLL)	_num++;
				path_origin.type[_num] = SMLL;
				path_origin.step[_num] += 1;
				vrmouse.pos_x += 1;
				vrmouse.pos_y += 1;
				vrmouse.dir = North;
			}
			// 東(x+1,y)に移動
			else if (vrmouse.pos_x + 1 < 16 && node[vrmouse.pos_x + 1][vrmouse.pos_y].enable == 0
				&& node[vrmouse.pos_x][vrmouse.pos_y].step == node[vrmouse.pos_x + 1][vrmouse.pos_y].step + 1) {
				if (path_origin.type[_num] != STGT)	_num++;
				path_origin.type[_num] = STGT;
				path_origin.step[_num] += 2;
				vrmouse.pos_x += 1;
				vrmouse.pos_y += 0;
				vrmouse.dir = East;
			}
		}
		else if (vrmouse.dir == North) {	// 機体北向き
			// 北東(x,y+1)に移動
			if (vrmouse.pos_y + 1 < 31 && node[vrmouse.pos_x][vrmouse.pos_y + 1].enable == 0
				&& node[vrmouse.pos_x][vrmouse.pos_y].step == node[vrmouse.pos_x][vrmouse.pos_y + 1].step + 1) {
				if (path_origin.type[_num] != SMLR)	_num++;
				path_origin.type[_num] = SMLR;
				path_origin.step[_num] += 1;
				vrmouse.pos_x += 0;
				vrmouse.pos_y += 1;
				vrmouse.dir = East;
			}
			// 北西(x-1,y+1)に移動
			else if (vrmouse.pos_x - 1 >= 0 && vrmouse.pos_y + 1 < 31 && node[vrmouse.pos_x - 1][vrmouse.pos_y + 1].enable == 0
				&& node[vrmouse.pos_x][vrmouse.pos_y].step == node[vrmouse.pos_x - 1][vrmouse.pos_y + 1].step + 1) {
				if (path_origin.type[_num] != SMLL)	_num++;
				path_origin.type[_num] = SMLL;
				path_origin.step[_num] += 1;
				vrmouse.pos_x += -1;
				vrmouse.pos_y += 1;
				vrmouse.dir = West;
			}
			// 北(x,y+2)に移動
			else if (vrmouse.pos_y + 2 < 31 && node[vrmouse.pos_x][vrmouse.pos_y + 2].enable == 0
				&& node[vrmouse.pos_x][vrmouse.pos_y].step == node[vrmouse.pos_x][vrmouse.pos_y + 2].step + 1) {
				if (path_origin.type[_num] != STGT)	_num++;
				path_origin.type[_num] = STGT;
				path_origin.step[_num] += 2;
				vrmouse.pos_x += 0;
				vrmouse.pos_y += 2;
				vrmouse.dir = North;
			}
		}
		else if (vrmouse.dir == South) {	// 機体南向き
			// 南西(x-1,y-1)に移動
			if (vrmouse.pos_x - 1 >= 0 && vrmouse.pos_y - 1 >= 0 && node[vrmouse.pos_x - 1][vrmouse.pos_y - 1].enable == 0
				&& node[vrmouse.pos_x][vrmouse.pos_y].step == node[vrmouse.pos_x - 1][vrmouse.pos_y - 1].step + 1) {
				if (path_origin.type[_num] != SMLR)	_num++;
				path_origin.type[_num] = SMLR;
				path_origin.step[_num] += 1;
				vrmouse.pos_x += -1;
				vrmouse.pos_y += -1;
				vrmouse.dir = West;
			}
			// 南東(x,y-1)に移動
			else if (vrmouse.pos_y - 1 >= 0 && node[vrmouse.pos_x][vrmouse.pos_y - 1].enable == 0
					&& node[vrmouse.pos_x][vrmouse.pos_y].step == node[vrmouse.pos_x][vrmouse.pos_y - 1].step + 1) {
					if (path_origin.type[_num] != SMLL)	_num++;
					path_origin.type[_num] = SMLL;
					path_origin.step[_num] += 1;
					vrmouse.pos_x += 0;
					vrmouse.pos_y += -1;
					vrmouse.dir = East;
			}
			// 南(x,y-2)に移動
			else if (vrmouse.pos_y - 2 >= 0 && node[vrmouse.pos_x][vrmouse.pos_y - 2].enable == 0
					&& node[vrmouse.pos_x][vrmouse.pos_y].step == node[vrmouse.pos_x][vrmouse.pos_y - 2].step + 1) {
					if (path_origin.type[_num] != STGT)	_num++;
					path_origin.type[_num] = STGT;
					path_origin.step[_num] += 2;
					vrmouse.pos_x += 0;
					vrmouse.pos_y += -2;
					vrmouse.dir = South;
			}
		}
		if (node[vrmouse.pos_x][vrmouse.pos_y].step == 0) {
			if (path_origin.type[_num] != STGT)	_num++;
			path_origin.type[_num] = STGT;
			path_origin.step[_num] += 1;
			path_origin.num = _num;
			break;
		}
	}
}

void Maze::makePath(char x_goal, char y_goal, char mode) {

	// パス用歩数マップ生成
	makePathMap(x_goal, y_goal, wall_buf);
	printfPathMap(wall_buf);

	// パスの元生成
	makePathOrigin();
	printfPathOrigin();

	// パス初期化
	path.num = 0;
	for (int i = 0; i<512; i++) {
		path.type[i] = STGT;
		path.step[i] = 0;
	}
	int _num = 1,__num = 1;	// 最初の直進は見ない

	// パス生成
	switch (mode){
	//-------------------------------------------------------------------
	//	斜め最短
	//-------------------------------------------------------------------
	case 0:
		while (_num < path_origin.num) {
			path.num = __num;
			switch (path_origin.type[_num]) {
			//-------------------------------------------------------------------
			//	左ターン関連
			//-------------------------------------------------------------------
			case SMLL:
				if (_num > 0) {
					// 左大回り90
					if (path_origin.step[_num] == 1 && path_origin.type[_num - 1] == STGT && path_origin.type[_num + 1] == STGT) {
						// 前後の直進分を記入
						path.type[__num - 1] = path_origin.type[_num - 1];
						path_origin.step[_num - 1] -= 1;
						path.step[__num - 1] = path_origin.step[_num - 1];
						path.type[__num + 1] = path_origin.type[_num + 1];
						path_origin.step[_num + 1] -= 1;
						path.step[__num + 1] = path_origin.step[_num + 1];
						if (path.step[__num - 1] == 0) __num--;	// 直進0をはじく処理
						// ターン記入
						path.type[__num] = LL90;
						path.step[__num] = 1;
						_num += 2;
						__num += 2;
					}
					// 左大回り180
					else if (path_origin.step[_num] == 2 && path_origin.type[_num - 1] == STGT && path_origin.type[_num + 1] == STGT){
						// 前後の直進分を記入
						path.type[__num - 1] = path_origin.type[_num - 1];
						path_origin.step[_num - 1] -= 1;
						path.step[__num - 1] = path_origin.step[_num - 1];
						path.type[__num + 1] = path_origin.type[_num + 1];
						path_origin.step[_num + 1] -= 1;
						path.step[__num + 1] = path_origin.step[_num + 1];
						if (path.step[__num - 1] == 0) __num--;	// 直進0をはじく処理
						// ターン記入
						path.type[__num] = LL180;
						path.step[__num] = 1;
						_num += 2;
						__num += 2;
					}
					// 斜め直進
					else if (path_origin.step[_num] == 1 && path_origin.type[_num - 1] == SMLR && path_origin.type[_num + 1] == SMLR) {
						if (path.type[__num - 1] == DRSTGT) __num--;
						path.type[__num] = DRSTGT;
						path.step[__num] += 1;
						_num += 1;
						__num += 1;
					}
					// 行き左斜め45
					else if (path_origin.step[_num] == 1 && path_origin.type[_num - 1] == STGT && path_origin.type[_num + 1] == SMLR) {
						// 前後の直進分を記入
						path.type[__num - 1] = path_origin.type[_num - 1];
						path_origin.step[_num - 1] -= 1;
						path.step[__num - 1] = path_origin.step[_num - 1];
						if (path.step[__num - 1] == 0) __num--;	// 直進0をはじく処理
						// ターン記入
						path.type[__num] = GODL45;
						path.step[__num] = 1;
						_num += 1;
						__num += 1;
					}
					// 行き左斜め135
					else if (path_origin.step[_num] == 2 && path_origin.type[_num - 1] == STGT && path_origin.type[_num + 1] == SMLR) {
						// 前後の直進分を記入
						path.type[__num - 1] = path_origin.type[_num - 1];
						path_origin.step[_num - 1] -= 1;
						path.step[__num - 1] = path_origin.step[_num - 1];
						if (path.step[__num - 1] == 0) __num--;	// 直進0をはじく処理
						// ターン記入
						path.type[__num] = GODL135;
						path.step[__num] = 1;
						_num += 1;
						__num += 1;
					}
					// 帰り左斜め45
					else if (path_origin.step[_num] == 1 && path_origin.type[_num - 1] == SMLR &&  path_origin.type[_num + 1] == STGT) {
						// 前後の直進分を記入
						path.type[__num + 1] = path_origin.type[_num + 1];
						path_origin.step[_num + 1] -= 1;
						path.step[__num + 1] = path_origin.step[_num + 1];
						// ターン記入
						path.type[__num] = REDL45;
						path.step[__num] = 1;
						_num += 2;
						__num += 2;
					}
					// 帰り左斜め135
					else if (path_origin.step[_num] == 2 && path_origin.type[_num - 1] == SMLR && path_origin.type[_num + 1] == STGT) {
						// 前後の直進分を記入
						path.type[__num + 1] = path_origin.type[_num + 1];
						path_origin.step[_num + 1] -= 1;
						path.step[__num + 1] = path_origin.step[_num + 1];
						// ターン記入
						path.type[__num] = REDL135;
						path.step[__num] = 1;
						_num += 2;
						__num += 2;
					}
					// 左斜めV90
					else if (path_origin.step[_num] == 2 && path_origin.type[_num - 1] == SMLR && path_origin.type[_num + 1] == SMLR) {
						path.type[__num] = VL90;
						path.step[__num] = 1;
						_num += 1;
						__num += 1;
					}
				}
				break;
			//-------------------------------------------------------------------
			//	右ターン関連
			//-------------------------------------------------------------------
			case SMLR:
				if (_num > 0) {
					// 右大回り90
					if (path_origin.step[_num] == 1 && path_origin.type[_num - 1] == STGT && path_origin.type[_num + 1] == STGT) {
						// 前後の直進分を記入
						path.type[__num - 1] = path_origin.type[_num - 1];
						path_origin.step[_num - 1] -= 1;
						path.step[__num - 1] = path_origin.step[_num - 1];
						path.type[__num + 1] = path_origin.type[_num + 1];
						path_origin.step[_num + 1] -= 1;
						path.step[__num + 1] = path_origin.step[_num + 1];
						if (path.step[__num - 1] == 0) __num--;	// 直進0をはじく処理
						// ターン記入
						path.type[__num] = LR90;
						path.step[__num] = 1;
						_num += 2;
						__num += 2;
					}
					// 右大回り180
					else if (path_origin.step[_num] == 2 && path_origin.type[_num - 1] == STGT && path_origin.type[_num + 1] == STGT) {
						// 前後の直進分を記入
						path.type[__num - 1] = path_origin.type[_num - 1];
						path_origin.step[_num - 1] -= 1;
						path.step[__num - 1] = path_origin.step[_num - 1];
						path.type[__num + 1] = path_origin.type[_num + 1];
						path_origin.step[_num + 1] -= 1;
						path.step[__num + 1] = path_origin.step[_num + 1];
						if (path.step[__num - 1] == 0) __num--;	// 直進0をはじく処理
						// ターン記入
						path.type[__num] = LR180;
						path.step[__num] = 1;
						_num += 2;
						__num += 2;
					}
					// 斜め直進
					else if (path_origin.step[_num] == 1 && path_origin.type[_num - 1] == SMLL && path_origin.type[_num + 1] == SMLL) {
						if (path.type[__num - 1] == DRSTGT) __num--;
						path.type[__num] = DRSTGT;
						path.step[__num] += 1;
						_num += 1;
						__num += 1;
					}
					// 行き右斜め45
					else if (path_origin.step[_num] == 1 && path_origin.type[_num - 1] == STGT && path_origin.type[_num + 1] == SMLL) {
						// 前後の直進分を記入
						path.type[__num - 1] = path_origin.type[_num - 1];
						path_origin.step[_num - 1] -= 1;
						path.step[__num - 1] = path_origin.step[_num - 1];
						if (path.step[__num - 1] == 0) __num--;	// 直進0をはじく処理
						// ターン記入
						path.type[__num] = GODR45;
						path.step[__num] = 1;
						_num += 1;
						__num += 1;
					}
					// 行き右斜め135
					else if (path_origin.step[_num] == 2 && path_origin.type[_num - 1] == STGT && path_origin.type[_num + 1] == SMLL) {
						// 前後の直進分を記入
						path.type[__num - 1] = path_origin.type[_num - 1];
						path_origin.step[_num - 1] -= 1;
						path.step[__num - 1] = path_origin.step[_num - 1];
						if (path.step[__num - 1] == 0) __num--;	// 直進0をはじく処理
						// ターン記入
						path.type[__num] = GODR135;
						path.step[__num] = 1;
						_num += 1;
						__num += 1;
					}
					// 帰り右斜め45
					else if (path_origin.step[_num] == 1 && path_origin.type[_num - 1] == SMLL && path_origin.type[_num + 1] == STGT) {
						// 前後の直進分を記入
						path.type[__num + 1] = path_origin.type[_num + 1];
						path_origin.step[_num + 1] -= 1;
						path.step[__num + 1] = path_origin.step[_num + 1];
						// ターン記入
						path.type[__num] = REDR45;
						path.step[__num] = 1;
						_num += 2;
						__num += 2;
					}
					// 帰り右斜め135
					else if (path_origin.step[_num] == 2 && path_origin.type[_num - 1] == SMLL &&  path_origin.type[_num + 1] == STGT) {
						// 前後の直進分を記入
						path.type[__num + 1] = path_origin.type[_num + 1];
						path_origin.step[_num + 1] -= 1;
						path.step[__num + 1] = path_origin.step[_num + 1];
						// ターン記入
						path.type[__num] = REDR135;
						path.step[__num] = 1;
						_num += 2;
						__num += 2;
					}
					// 右斜めV90
					else if (path_origin.step[_num] == 2 && path_origin.type[_num - 1] == SMLL &&  path_origin.type[_num + 1] == SMLL) {
						path.type[__num] = VR90;
						path.step[__num] = 1;
						_num += 1;
						__num += 1;
					}
				}
				break;
			//-------------------------------------------------------------------
			//	例外処理
			//-------------------------------------------------------------------
			default:
				break;
			}
		}
		break;
	//-------------------------------------------------------------------
	//	大回り最短
	//-------------------------------------------------------------------
	case 1:
		while (_num <= path_origin.num + 1) {
			path.num = __num;
			switch (path_origin.type[_num]) {
				//-------------------------------------------------------------------
				//	左ターン関連
				//-------------------------------------------------------------------
			case SMLL:
				if (_num > 0) {
					// 左大回り90
					if (path_origin.step[_num] == 1 && path_origin.type[_num - 1] == STGT && path_origin.type[_num + 1] == STGT) {
						// 前後の直進分を記入
						path.type[__num - 1] = path_origin.type[_num - 1];
						path_origin.step[_num - 1] -= 1;
						path.step[__num - 1] = path_origin.step[_num - 1];
						path.type[__num + 1] = path_origin.type[_num + 1];
						path_origin.step[_num + 1] -= 1;
						path.step[__num + 1] = path_origin.step[_num + 1];
						if (path.step[__num - 1] == 0) __num--;	// 直進0をはじく処理
																// ターン記入
						path.type[__num] = LL90;
						path.step[__num] = 1;
						_num += 2;
						__num += 2;
					}
					// 左大回り180
					else if (path_origin.step[_num] == 2 && path_origin.type[_num - 1] == STGT && path_origin.type[_num + 1] == STGT) {
						// 前後の直進分を記入
						path.type[__num - 1] = path_origin.type[_num - 1];
						path_origin.step[_num - 1] -= 1;
						path.step[__num - 1] = path_origin.step[_num - 1];
						path.type[__num + 1] = path_origin.type[_num + 1];
						path_origin.step[_num + 1] -= 1;
						path.step[__num + 1] = path_origin.step[_num + 1];
						if (path.step[__num - 1] == 0) __num--;	// 直進0をはじく処理
																// ターン記入
						path.type[__num] = LL180;
						path.step[__num] = 1;
						_num += 2;
						__num += 2;
					}
					else {
						path.type[__num - 1] = path_origin.type[_num - 1];
						path.step[__num - 1] = path_origin.step[_num - 1];
						path.type[__num] = path_origin.type[_num];
						path.step[__num] = path_origin.step[_num];
						_num += 1;
						__num += 1;
					}
				}
				break;
				//-------------------------------------------------------------------
				//	右ターン関連
				//-------------------------------------------------------------------
			case SMLR:
				if (_num > 0) {
					// 右大回り90
					if (path_origin.step[_num] == 1 && path_origin.type[_num - 1] == STGT && path_origin.type[_num + 1] == STGT) {
						// 前後の直進分を記入
						path.type[__num - 1] = path_origin.type[_num - 1];
						path_origin.step[_num - 1] -= 1;
						path.step[__num - 1] = path_origin.step[_num - 1];
						path.type[__num + 1] = path_origin.type[_num + 1];
						path_origin.step[_num + 1] -= 1;
						path.step[__num + 1] = path_origin.step[_num + 1];
						if (path.step[__num - 1] == 0) __num--;	// 直進0をはじく処理
																// ターン記入
						path.type[__num] = LR90;
						path.step[__num] = 1;
						_num += 2;
						__num += 2;
					}
					// 右大回り180
					else if (path_origin.step[_num] == 2 && path_origin.type[_num - 1] == STGT && path_origin.type[_num + 1] == STGT) {
						// 前後の直進分を記入
						path.type[__num - 1] = path_origin.type[_num - 1];
						path_origin.step[_num - 1] -= 1;
						path.step[__num - 1] = path_origin.step[_num - 1];
						path.type[__num + 1] = path_origin.type[_num + 1];
						path_origin.step[_num + 1] -= 1;
						path.step[__num + 1] = path_origin.step[_num + 1];
						if (path.step[__num - 1] == 0) __num--;	// 直進0をはじく処理
																// ターン記入
						path.type[__num] = LR180;
						path.step[__num] = 1;
						_num += 2;
						__num += 2;
					}
					else {
						path.type[__num - 1] = path_origin.type[_num - 1];
						path.step[__num - 1] = path_origin.step[_num - 1];
						path.type[__num] = path_origin.type[_num];
						path.step[__num] = path_origin.step[_num];
						_num += 1;
						__num += 1;
					}
				}
				break;
				//-------------------------------------------------------------------
				//	例外処理
				//-------------------------------------------------------------------
			default:
				path.type[__num] = path_origin.type[_num];
				path.step[__num] = path_origin.step[_num];
				_num += 1;
				__num += 1;
				break;
			}
		}
		break;
	default:
		break;
	}
	// 最後の直線パスが抜けるのを防ぐ処理(ターンごとにパスを入れているため最後の直線は最後に別で処理する必要がある)
	if ((path.type[__num - 2] >= LL90 && path.type[__num - 2] <= LR180) || (path.type[__num - 2] >= REDL45 && path.type[__num - 2] <= REDR135)) { // 大回りか斜めのとき
		path.type[__num - 1] = path_origin.type[_num - 1];
		path.step[__num - 1] = path_origin.step[_num - 1];
		path.num = __num - 1;
	}
}

void Maze::printfPathMap(WALL _wall) {
	// 未探索壁を埋める
	for (int i = 0; i<17; i++) {
		for (int j = 0; j<16; j++) {
			if (((_wall.search_h[i] >> j) & 0x0001) == 0)	_wall.data_h[i] |= (0x0001 << j);
			if (((_wall.search_v[i] >> j) & 0x0001) == 0)	_wall.data_v[i] |= (0x0001 << j);
		}
	}
	// パスマップの描画
	for (int j = 0; j < 16; j++) {
		if (((_wall.data_h[16] >> j) & 0x01) == 1) myprintf("+---");
	}
	myprintf("+\r\n");
	for (int i = 15; i > 0; i--) {
		myprintf("|   ");
		for (int j = 0; j <16; j++) {
			if (node[j][2 * i].enable == 1) {
				myprintf("|   ");
			}
			else {
				if (node[j][2 * i].step == 4095) myprintf("#   ");
				else myprintf("%-3d ", node[j][2 * i].step);
			}
		}
		myprintf("\r\n");
		for (int j = 0; j < 16; j++) {
			if (node[j][2 * i - 1].enable == 1) {
				myprintf("+---");
			}
			else {
				if (node[j][2 * i - 1].step == 4095) myprintf("+ # ");
				else myprintf("+%3d", node[j][2 * i - 1].step);
			}
		}
		myprintf("+\r\n");
	}
	myprintf("|   ");
	for (int j = 0; j <16; j++) {
		if (node[j][0].enable == 1) myprintf("|   ");
		else {
			if (node[j][0].step == 4095) myprintf("#   ");
			else myprintf("%-3d ", node[j][0].step);
		}
	}
	myprintf("\r\n");
	for (int j = 0; j < 16; j++) {
		if (((_wall.data_h[0] >> j) & 0x01) == 1) myprintf("+---");
	}
	myprintf("+\r\n");
}

void Maze::ShortestRun(char mode, char x_goal, char y_goal, float accel, float maxVelocity,TurnType type,float diagVelocity){
	mouse.flag.bit.wallfix_SS = 0;
	mouse.flag.bit.wallfix_S = 1;
	mouse.flag.bit.falesafe = 0;
	makePath(x_goal, y_goal, mode);
	printfPathMap(wall_buf);
	printfPath();
	for(int i=0; i <= path.num; i++){
		// 走行準備処理
		LEDOFF();
		mouse.flag.bit.diagonal = 0; // 斜め走行フラグ閉じる（速度がゼロになる原因？）(2018_10_02)
		// 走行処理
		switch(path.type[i]){
		case STGT:
			// 壁センサON
			mouse.flag.bit.enable_phot = 1;
			// 壁切れ種類切り替え
			if(path.type[i + 1] == SMLL) mouse.flag.bit.wallfix_SS = 1;
			else if(path.type[i + 1] == SMLR) mouse.flag.bit.wallfix_SS = 1;
			//else if(path.type[i + 1] == STGT) mouse.flag.bit.wallfix_SS = 1;	このケースには入らない？(2018_10_02)
			else if(path.type[i + 1] == LL90) mouse.flag.bit.wallfix_S = 1;
			else if(path.type[i + 1] == LR90) mouse.flag.bit.wallfix_S = 1;
			else if(path.type[i + 1] == LL180) mouse.flag.bit.wallfix_S = 1;
			else if(path.type[i + 1] == LR180) mouse.flag.bit.wallfix_S = 1;
			else if(path.type[i + 1] == GODL45) mouse.flag.bit.wallfix_S = 1;
			else if(path.type[i + 1] == GODR45) mouse.flag.bit.wallfix_S = 1;
			else if(path.type[i + 1] == GODL135) mouse.flag.bit.wallfix_S = 1;
			else if(path.type[i + 1] == GODR135) mouse.flag.bit.wallfix_S = 1;
			// 直進処理
			if(mouse.flag.bit.wallfix_SS)
			mouse.straight(accel, mouse.velocity_th, type.l_turn90_l->velocity, maxVelocity, 90.0 * path.step[i] + 20.0);	// 探索時壁切れ補正(2018_10_02)
			else if(mouse.flag.bit.wallfix_S)
			mouse.straight(accel, mouse.velocity_th, type.l_turn90_l->velocity, maxVelocity, 90.0 * path.step[i] + 90.0);	// オフセット区間を20→90へ増加（速度がゼロになるのを防げるか？）(2018_10_02)
			else
			mouse.straight(accel, mouse.velocity_th, type.l_turn90_l->velocity, maxVelocity, 90.0 * path.step[i]);	// 壁切れ補正しない場合(2018_10_02)
			break;
		case SMLL:
			for(int j=0; j<path.step[i]; j++){
				mouse.end_flag = 1;
				mouse.s_turn(*type.s_turn_l);
			}
			break;
		case SMLR:
			for(int j=0; j<path.step[i]; j++){
				mouse.end_flag = 1;
				mouse.s_turn(*type.s_turn_r);
			}
			break;
		case LL90:
			if(i==0){
				mouse.end_flag = 1;
				mouse.straight(9000.0,0.0, type.s_turn_l->velocity, type.s_turn_l->velocity, 90.0);
				for(int j=0; j<path.step[i]; j++){
					mouse.end_flag = 1;
					mouse.s_turn(*type.l_turn90_l);
				}
			}
			else{
				for(int j=0; j<path.step[i]; j++){
					mouse.end_flag = 1;
					mouse.s_turn(*type.l_turn90_l);
				}
			}
			break;
		case LR90:
			if(i==0){
				mouse.end_flag = 1;
				mouse.straight(9000.0,0.0, type.s_turn_r->velocity, type.s_turn_r->velocity, 90.0);
				for(int j=0; j<path.step[i]; j++){
					mouse.end_flag = 1;
					mouse.s_turn(*type.s_turn_r);
				}
				mouse.straight(9000.0, type.s_turn_r->velocity, type.s_turn_r->velocity, type.s_turn_r->velocity, 90.0);
			}
			else{
				for(int j=0; j<path.step[i]; j++){
					mouse.end_flag = 1;
					mouse.s_turn(*type.l_turn90_r);
				}
			}
			break;
		case LL180:
			for(int j=0; j<path.step[i]; j++){
				mouse.end_flag = 1;
				mouse.s_turn(*type.l_turn180_l);
			}
			break;
		case LR180:
			for(int j=0; j<path.step[i]; j++){
				mouse.end_flag = 1;
				mouse.s_turn(*type.l_turn180_r);
			}
			break;
		case DRSTGT:
			mouse.end_flag = 1;
			mouse.flag.bit.diagonal = 1;
			mouse.flag.bit.enable_phot = 0;
			mouse.flag.bit.wallfix_SS = 0;
			mouse.flag.bit.wallfix_S = 0;
			mouse.end_flag = 0;
			mouse.straight(accel, mouse.velocity_th, diagVelocity, maxVelocity, 90.0 * 1.414 * path.step[i]);
			break;
		case GODL45:
			mouse.end_flag = 1;
			mouse.flag.bit.diagonal = 1;
			mouse.s_turn(*type.gd_turn45_l);
			break;
		case GODR45:
			mouse.end_flag = 1;
			mouse.flag.bit.diagonal = 1;
			mouse.s_turn(*type.gd_turn45_r);
			break;
		case GODL135:
			mouse.end_flag = 1;
			mouse.flag.bit.diagonal = 1;
			mouse.s_turn(*type.gd_turn135_l);
			break;
		case GODR135:
			mouse.end_flag = 1;
			mouse.flag.bit.diagonal = 1;
			mouse.s_turn(*type.gd_turn135_r);
			break;
		case REDL45:
			mouse.end_flag = 1;
			mouse.flag.bit.diagonal = 1;
			mouse.s_turn(*type.rd_turn45_l);
			break;
		case REDR45:
			mouse.end_flag = 1;
			mouse.flag.bit.diagonal = 1;
			mouse.s_turn(*type.rd_turn45_r);
			break;
		case REDL135:
			mouse.end_flag = 1;
			mouse.flag.bit.diagonal = 1;
			mouse.s_turn(*type.rd_turn135_l);
			break;
		case REDR135:
			mouse.end_flag = 1;
			mouse.flag.bit.diagonal = 1;
			mouse.s_turn(*type.rd_turn135_r);
			break;
		case VL90:
			mouse.end_flag = 1;
			mouse.flag.bit.diagonal = 1;
			mouse.s_turn(*type.v_turn90_l);
			break;
		case VR90:
			mouse.end_flag = 1;
			mouse.flag.bit.diagonal = 1;
			mouse.s_turn(*type.v_turn90_r);
			break;
		}
	}
	if(mouse.photo.value[0] < SEN_LF_TH && mouse.photo.value[5] < SEN_RF_TH)
	mouse.straight(accel,mouse.velocity_th, 0.0, mouse.velocity_th, 180.0 * 3);
	// フェールセーフ処理
	if(mouse.flag.bit.falesafe == 1){
		SpeakerEnable();
		miss_melody(180, 2);
		SpeakerDisable();
	}
	else{
		SpeakerEnable();
		KirbyDance(180, 2);
		SpeakerDisable();
	}
	wait_timer(1000);
	mouse.stopSuction();
}

void Maze::printfPathOrigin(void) {
	for (int i = 0; i <= path_origin.num; i++) {
		switch (path_origin.type[i])
		{
		case STGT:
			myprintf("直進 %d\r\n", path_origin.step[i]);
			break;
		case SMLL:
			myprintf("左 %d\r\n", path_origin.step[i]);
			break;
		case SMLR:
			myprintf("右 %d\r\n", path_origin.step[i]);
			break;
		default:
			break;
		}
	}
}

void Maze::printfPath(void) {
	for (int i = 0; i <= path.num + 1; i++) {
		switch (path.type[i])
		{
		case STGT:
			myprintf("直進                  %d\r\n", path.step[i]);
			break;
		case SMLL:
			myprintf("左小回り90   %d\r\n", path.step[i]);
			break;
		case SMLR:
			myprintf("右小回り90   %d\r\n", path.step[i]);
			break;
		case LL90:
			myprintf("左大回り90   %d\r\n", path.step[i]);
			break;
		case LR90:
			myprintf("右大回り90   %d\r\n", path.step[i]);
			break;
		case LL180:
			myprintf("左大回り180  %d\r\n", path.step[i]);
			break;
		case LR180:
			myprintf("右大回り180  %d\r\n", path.step[i]);
			break;
		case DRSTGT:
			myprintf("斜め直進            %d\r\n", path.step[i]);
			break;
		case GODL45:
			myprintf("行き左斜め45  %d\r\n", path.step[i]);
			break;
		case GODR45:
			myprintf("行き右斜め45  %d\r\n", path.step[i]);
			break;
		case GODL135:
			myprintf("行き左斜め135 %d\r\n", path.step[i]);
			break;
		case GODR135:
			myprintf("行き右斜め135 %d\r\n", path.step[i]);
			break;
		case REDL45:
			myprintf("帰り左斜め45  %d\r\n", path.step[i]);
			break;
		case REDR45:
			myprintf("帰り右斜め45  %d\r\n", path.step[i]);
			break;
		case REDL135:
			myprintf("帰り左斜め135 %d\r\n", path.step[i]);
			break;
		case REDR135:
			myprintf("帰り右斜め135 %d\r\n", path.step[i]);
			break;
		case VL90:
			myprintf("左斜めV90    %d\r\n", path.step[i]);
			break;
		case VR90:
			myprintf("右斜めV90    %d\r\n", path.step[i]);
			break;
		default:
			break;
		}
	}
}

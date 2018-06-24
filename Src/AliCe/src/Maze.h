/*
 * Maze.h
 *
 *  Created on: 2017/09/21
 *      Author: KOTA-NPC2
 */

#ifndef MAZE_H_
#define MAZE_H_

#include "parameter.h"

#define X_SIZE 16
#define Y_SIZE 16

#define MAX_VELOCITY 700.0

typedef struct{
	unsigned short data_h[17];
	unsigned short data_v[17];
	unsigned short search_h[17];
	unsigned short search_v[17];
}WALL;

typedef struct {
	unsigned short step : 12;
	unsigned short enable : 1;
}NODE;

typedef struct {
	unsigned char pos_x;
	unsigned char pos_y;
	unsigned char dir;
}VRMouse;

typedef struct {
	unsigned short num;
	unsigned char type[512];
	unsigned char step[512];
}PATH;

enum {
	North, South, East, West
};

enum {
	STGT,		// 直進
	SMLL,		// 左小回り
	SMLR,		// 右小回り
	LL90,		// 左大回り90
	LR90,		// 右大回り90
	LL180,		// 左大回り180
	LR180,		// 右大回り180
	DRSTGT,		// 斜め直進
	GODL45,		// 行き斜め左45
	GODR45,		// 行き斜め右45
	GODL135,	// 行き斜め左135
	GODR135,	// 行き斜め右135
	REDL45,		// 帰り斜め左45
	REDR45,		// 帰り斜め右45
	REDL135,	// 帰り斜め左135
	REDR135,	// 帰り斜め右135
	VL90,		// 斜め左V90
	VR90		// 斜め右V90
};

class Maze {
private:
	void rot_matrix (int turn_direction);
	void getWallData(void);
	void getMap(char goal_x , char goal_y);
	void ShortestRunS(float accel,float maxVelocity,TurnType type);
	void ShortestRunL(float accel,float maxVelocity,TurnType type);
	int checkSearched(int _pos_x, int _pos_y, WALL _wall);
public:
	unsigned char step_number[16][16];
	short now_position[2];
	short direction_vectle[2];
	char goal_flag;
	WALL wall;
	WALL wall_buf;
	NODE node[16][31];
	VRMouse vrmouse;
	PATH path_origin, path;
	void getNowPosition(int turn_direction);
	void initMap(char x_goal,char y_goal);
	void hidaritehou(char goal_x,char goal_y);
	void adatihou(char x_goal,char y_goal);
	void FurukawaSearch(char x_goal,char y_goal);
	void getPath(char x_goal,char y_goal);
	void runPath(char mode, float maxVelocity,TurnType type);
	void printfMap(WALL _wall);
	void makePathMap(char x_goal, char y_goal, WALL _wall);
	void makePathOrigin(void);
	void makePath(char goal_x, char goal_y, char mode);
	void ShortestRun(char mode, char x_goal, char y_goal, float accel, float maxVelocity, TurnType type, float diagVelocity);
	void printfPathMap(WALL _wall);
	void printfPathOrigin(void);
	void printfPath(void);
};

#endif /* MAZE_H_ */

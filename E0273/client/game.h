#define _CRT_SECURE_NO_WARNINGS 1
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include <stdbool.h>


#define MUM_TILES_X 9
#define MUM_TILES_Y 9
#define MUM_MINES 10

#define RANDOM_NUMBER_SEED 42
#define MINE_REVEALED 0x10
#define IS_MINE 0x11

typedef struct
{
	char username[20];
	int w_time;
	int won_NUM;
	int played_NUM;
	bool is_record;
}Leaderboard;

typedef struct
{
	int adjacent_mines;
	bool revealed;
	bool is_mine;
	bool is_place;
}Tile;

typedef struct
{
	Tile tiles[MUM_TILES_X+2][MUM_TILES_Y+2];
}GameState;
 
 
int menu();
int menu2();
void c_display_adjacent_mines(GameState game_state); 
void display_adjacent_mines(GameState game_state); 
void display_mine_location(GameState game_state); 
void place_mines(GameState* game_state, int first_f);
int Sweep(GameState* game_state,int x, int y);
int get_num(GameState* game_state, int x, int y);

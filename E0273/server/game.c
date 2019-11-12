#include"game.h"
 

int menu()
{
	printf("=========================================================\n");
	printf("Welcome to the online Minesweeper gaming system\n");
	printf("=========================================================\n");
	printf("\n");
	printf("You are required to log on with your registered user name and password.\n");
	return 0;
}

int menu2()
{
	printf("Welcome to the online Minesweeper gaming system\n");
	printf("\n");
	printf("Please enter a selection:\n");
	printf("<1> Play Minesweeper\n");
	printf("<2> Show Leaderboard\n");
	printf("<3> Quit\n");
	printf("\n");
	printf("Selection option(1-3): ");
	return 0;
}

void place_mines(GameState* game_state,int first_f)
{
	int count = MUM_MINES;
	int i = 0;
	int j = 0;
	int x = 0;
	int y = 0;
	if(first_f)
	{
		srand(RANDOM_NUMBER_SEED);
	}
	if(first_f == 0)
	{
		srand(time(NULL));
	}
	while (count)
	{
		x = rand() % MUM_TILES_X + 1;
		y = rand() % MUM_TILES_Y + 1;
		if (game_state->tiles[x][y].is_mine == 0)
		{
			game_state->tiles[x][y].is_mine = 1;
			count--;
		}
	}
	for (i = 1; i <= MUM_TILES_X ; i++)
	{
		for (j = 1; j <= MUM_TILES_Y ; j++)
		{
			game_state->tiles[i][j].adjacent_mines = get_num(game_state, i, j);
		}
	}
}
void c_display_adjacent_mines(GameState game_state)  
{
	int i = 0;
	int j = 0;
	char a = 'A';
	
	printf("   ");
	for (i = 1; i <= MUM_TILES_Y; i++)
	{
		printf(" %d ", i);
	}
	printf("\n");
	printf("-----------------------------\n");
	for (i = 1; i <= MUM_TILES_X; i++)
	{
		printf("%c |", a+i-1);
		for (j = 1; j <= MUM_TILES_Y; j++)
		{
			if(game_state.tiles[i][j].revealed)
			{
				printf(" %d ", game_state.tiles[i][j].adjacent_mines);
			}else if(game_state.tiles[i][j].is_place)
			{
				printf(" + ");
			}else
			{
				printf("   ");
			}

		}
		printf("\n");
	}
	
}

void display_adjacent_mines(GameState game_state)  
{
	int i = 0;
	int j = 0;
	char a = 'A';
	
	printf("   ");
	for (i = 1; i <= MUM_TILES_Y; i++)
	{
		printf(" %d ", i);
	}
	printf("\n");
	printf("-----------------------------\n");
	for (i = 1; i <= MUM_TILES_X; i++)
	{
		printf("%c |", a+i-1);
		for (j = 1; j <= MUM_TILES_Y; j++)
		{
			printf(" %d ", game_state.tiles[i][j].adjacent_mines);
		}
		printf("\n");
	}
	
}
void display_mine_location(GameState game_state)  
{
	int i = 0;
	int j = 0;
	char a = 'A';
	//printf("mine location\n");
	printf("   ");
	for (i = 1; i <= MUM_TILES_Y; i++)
	{
		printf(" %d ", i);
	}
	printf("\n");
	printf("-----------------------------\n");
	for (i = 1; i <= MUM_TILES_X; i++)
	{
		printf("%c |", a+i-1);
		for (j = 1; j <= MUM_TILES_Y; j++)
		{
			if(game_state.tiles[i][j].is_mine)
			{
				printf(" * ");
			}else
			{
				printf("   ");
			}
		}
		printf("\n");
	}
	
}

int get_num(GameState* game_state, int x, int y)
{
	int count = 0;
	if (game_state->tiles[x - 1][y - 1].is_mine == 1)
	{
		count++;
	}
	if (game_state->tiles[x - 1][y].is_mine == 1)
	{
		count++;
	}
	if (game_state->tiles[x - 1][y + 1].is_mine == 1)
	{
		count++;
	}
	if (game_state->tiles[x][y - 1].is_mine == 1)
	{
		count++;
	}
	if (game_state->tiles[x][y + 1].is_mine == 1)
	{
		count++;
	}
	if (game_state->tiles[x + 1][y - 1].is_mine == 1)
	{
		count++;
	}
	if (game_state->tiles[x + 1][y].is_mine == 1)
	{
		count++;
	}
	if (game_state->tiles[x + 1][y + 1].is_mine == 1)
	{
		count++;
	}
	return  count;
}

int Sweep(GameState* game_state,int x, int y)
{
	if(game_state->tiles[x][y].revealed)
		return MINE_REVEALED;
	game_state->tiles[x][y].revealed = 1;
	if (game_state->tiles[x][y].is_mine)
	{
		return IS_MINE;
	}
	return game_state->tiles[x][y].adjacent_mines;
}


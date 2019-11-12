#include "threadpool.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <unistd.h>
#include"game.h"
FILE *fp;
pthread_mutex_t mutex1; 
pthread_mutex_t mutex2;
void change_state(GameState* s_game_state,GameState game_state)
{
	int i,j;
	for (i = 0; i < MUM_TILES_X + 2; i++)
	{
		for (j = 0; j < MUM_TILES_Y + 2; j++)
		{
			if(game_state.tiles[i][j].revealed)
			{
				s_game_state->tiles[i][j].is_mine = game_state.tiles[i][j].is_mine;
				s_game_state->tiles[i][j].revealed = game_state.tiles[i][j].revealed;
				s_game_state->tiles[i][j].adjacent_mines = game_state.tiles[i][j].adjacent_mines;
				s_game_state->tiles[i][j].is_place = game_state.tiles[i][j].is_place;
			}
		}
	}
}

void recursion(GameState* game_state, int x, int y)
{
	if(x>9 || y>9)
		return;
	if(x<1 || y<1)
		return;
	if(game_state->tiles[x][y].adjacent_mines == 0)
	{
		//s_game_state->tiles[x][y].adjacent_mines = 0;
		game_state->tiles[x][y].revealed = 1;
		if((x-1)>0 && (y-1)>0 && game_state->tiles[x-1][y-1].revealed == 0)
			recursion(game_state,x-1,y-1);
		if((x-1)>0 && (y)>0 && game_state->tiles[x-1][y].revealed == 0)
			recursion(game_state,x-1,y);
		if((x-1)>0 && (y+1)>0 && game_state->tiles[x-1][y+1].revealed == 0)
			recursion(game_state,x-1,y+1);
		if((x)>0 && (y-1)>0 && game_state->tiles[x][y-1].revealed == 0)
			recursion(game_state,x,y-1);
		if((x)>0 && (y+1)>0 && game_state->tiles[x][y+1].revealed == 0)
			recursion(game_state,x,y+1);
		if((x+1)>0 && (y-1)>0 && game_state->tiles[x+1][y-1].revealed == 0)
			recursion(game_state,x+1,y-1);
		if((x+1)>0 && (y)>0 && game_state->tiles[x+1][y].revealed == 0)
			recursion(game_state,x+1,y);
		if((x+1)>0 && (y+1)>0 && game_state->tiles[x+1][y+1].revealed == 0)
			recursion(game_state,x+1,y+1);
	}
	else
	{
		//s_game_state->tiles[x][y].adjacent_mines = game_state.tiles[x][y].adjacent_mines;
		game_state->tiles[x][y].revealed = 1;
	}
}

int server_show(int socketCon)
{
	int s_len,n=0;
	bool empty_F = 0;
	if ((fp=fopen("Leaderboard.txt","r"))==NULL)
    {	
        printf("cannot open file\n");
        return 0;
    }

	//printf("server_show+++++++++\n");
	Leaderboard LBt;
	char str[20];
	memset(&LBt,0,sizeof(Leaderboard));
	if(feof(fp))
	{
		empty_F = 1;
		//printf("The file is empty.\n");
		write(socketCon,&empty_F,sizeof(empty_F));
		fclose(fp);
		return 1;
	}
	write(socketCon,&empty_F,sizeof(empty_F));
	
	while(!feof(fp))
	{
		fscanf(fp,"%s",str);
		strcpy(LBt.username,str);
		fscanf(fp,"%s",str);
		LBt.w_time = atoi(str);
		fscanf(fp,"%s",str);
		LBt.won_NUM = atoi(str);
		fscanf(fp,"%s",str);
		LBt.played_NUM = atoi(str);
		LBt.is_record = 1;
		n++;
	}
	//printf("n = %d\n",n);
	write(socketCon,&n,sizeof(n));
	fseek(fp,0L,0);
	while(!feof(fp))
	{
		fscanf(fp,"%s",str);
		//printf("server_show str = %s\n",str);
		strcpy(LBt.username,str);
		fscanf(fp,"%s",str);
		LBt.w_time = atoi(str);
		fscanf(fp,"%s",str);
		LBt.won_NUM = atoi(str);
		fscanf(fp,"%s",str);
		LBt.played_NUM = atoi(str);
		LBt.is_record = 1;
		//printf("server_show  write!!!!!!!!!\n");
		write(socketCon,&LBt,sizeof(LBt));
		n--;
		if(n == 0)
			break;
		memset(&LBt,0,sizeof(Leaderboard));
	}
	//printf("server_show-----------\n");
	fclose(fp);
	return 0;
} 

int read_Leaderboard(Leaderboard* L_B)
{
	if ((fp=fopen("Leaderboard.txt","r"))==NULL)
    {	
        printf("cannot open file\n");
        return 0;
    }
	char str[20];
	while(!feof(fp))
	{
		fscanf(fp,"%s",str);
		if(strcmp(str,L_B->username)==0)
		{
			fscanf(fp,"%s",str);
			L_B->w_time = atoi(str);
			fscanf(fp,"%s",str);
			L_B->won_NUM = atoi(str);
			fscanf(fp,"%s",str);
			L_B->played_NUM = atoi(str);
			L_B->is_record = 1;
			return 1;
		}
		fscanf(fp,"%s",str);
	}
	L_B->is_record = 0;
	L_B->w_time = 0;
	L_B->won_NUM = 0;
	L_B->played_NUM = 0;
	fclose(fp);
	return 0;
}
int write_Leaderboard(Leaderboard L_B)
{
	int len = strlen(L_B.username);
	FILE* fpt = fopen("temp.txt", "w+");  
	if ((fp=fopen("Leaderboard.txt","r+"))==NULL)
    {	
        printf("cannot open file\n");
        return 0;
    }
	char buf[1024];
	memset(&buf,0,sizeof(buf));
	if(feof(fp))
	{
		fprintf(fp,"%s %d %d %d\n",L_B.username,L_B.w_time,L_B.won_NUM,L_B.played_NUM);
		//fwrite(buf,sizeof(buf),1,fp);
		fgets(buf,sizeof(buf), fp); 
		//printf("file empty write buf:%s\n",buf);
		fclose(fp);
		fclose(fpt);
		return 1;
	}
	
	bzero(&buf,sizeof(buf));
	
	//printf("Document content\n");
	
	while(!feof(fp))
	{
		fgets(buf,sizeof(buf), fp);
		//printf("line 189: %s | %s | %d\n",buf,L_B.username,len);
		if(strncmp(buf,L_B.username,len)==0)
		{
			//printf("11111111111111111111111111\n");
		}else{
			fwrite(buf,sizeof(buf),1,fpt);
			fprintf(fpt,"%s",buf);
		}
		bzero(&buf,sizeof(buf));
	}
	fclose(fp);
	fclose(fpt);
	
	fpt = fopen("temp.txt", "r"); 
	//clear
	if ((fp=fopen("Leaderboard.txt","w+"))==NULL)
    {	
        printf("cannot open file\n");
        return 0;
    }
	
	while(!feof(fpt))
	{
		fgets(buf,sizeof(buf), fpt); 
		//fwrite(buf,sizeof(buf),1,fp);
		fprintf(fp,"%s",buf);
	}
	fclose(fpt);
	
	//printf("line 197 write_Leaderboard: %s %d %d %d\n",L_B.username,L_B.w_time,L_B.won_NUM,L_B.played_NUM);
	fprintf(fp,"%s %d %d %d\n",L_B.username,L_B.w_time,L_B.won_NUM,L_B.played_NUM);
	//fwrite(buf,sizeof(buf),1,fp);

	fclose(fp);
	return 0;
}
int recog_pwd(char *name,char *pwd)
{
	
    FILE *fp1;
    char str[20];
	
    if ((fp1=fopen("Authentication.txt","r"))==NULL)
    {	
        printf("cannot open file\n");
        return 0;
     }

	while(!feof(fp1))
	{
		fscanf(fp1,"%s",str);
		if(strcmp(str,name)==0)
		{
			//printf("the name :%s is ok\n",name);
			fscanf(fp1,"%s",str);
			if(strcmp(str,pwd)==0)
			{
				//printf("the pwd :%s is ok\n",pwd);
				return 1;
			}else
			{
				//printf("the pwd :%s is wrong\n",pwd);
				fclose(fp1);
				return 0;
			}
				
		}
	fscanf(fp1,"%s",str);
		
	}
	fclose(fp1);
    return 0;
 
}

int coordinates_change(char character)
{
	int ret = 1;
	if(character >= 'A' && character <= 'Z')
	{
		ret += character - 'A';
	}
	if(character >= 'a' && character <= 'z')
	{
		ret += character - 'a';
	}
	if(character >= '0' && character <= '9')
	{
		ret = character - '0';
	}
	return ret;
}


void* work(void* socketConnect)
{
	int first_f = 1;
	Leaderboard LB;
	//int won_NUM = 0;
	//int num_games = 0;
	int g_o = 0;
	char buf[1024];
	char username[20];
	char password[20];
	char recv_coordinates[10];
	int readMsg_len,sendMsg_len,ret;
	int socketCon = *((int *)socketConnect);
	
	GameState s_gamestate, send_gamestate;
	int i = 0;
	int j = 0;
	int x = 0;
	int y = 0;
	char ch;
	int count = MUM_MINES;
	time_t start_t, end_t;
	int s_time;
	int i_choice = 0;
	int w_ret=0;
	
	

	
	read(socketCon, username, sizeof(username));
	read(socketCon, password, sizeof(password));
	
	

	//readMsg_len = read(socketCon, buf, sizeof(buf));

	if(recog_pwd(username,password))
	{
		sprintf(buf,"ok\n");
		sendMsg_len = write(socketCon,buf,sizeof(buf));
		bzero(&buf,sizeof(buf));
	}else{
		sprintf(buf,"error\n");
		sendMsg_len = write(socketCon,buf,sizeof(buf));
		bzero(&buf,sizeof(buf));
		//exit
		close(socketCon);
		return;
	}
	
	strcpy(LB.username,username);

	pthread_mutex_lock(&mutex1);
	read_Leaderboard(&LB);
	pthread_mutex_unlock(&mutex1);
	//printf("read_Leaderboard: %s %d %d %d\n",LB.username,LB.w_time,LB.won_NUM,LB.played_NUM);
	
	flag:
	
	memset(&s_gamestate,0,sizeof(GameState));
	memset(&send_gamestate,0,sizeof(GameState));
	
	read(socketCon,&i_choice,sizeof(i_choice));
	if(i_choice == 2)
	{
		pthread_mutex_lock(&mutex1);
		server_show(socketCon);
		pthread_mutex_unlock(&mutex1);
		close(socketCon);
		return;
	}
	if(i_choice == 3)
	{
		close(socketCon);
		return;
	}
	//game start
	//int input = 0;

	for (i = 0; i < MUM_TILES_X + 2; i++)
	{
		for (j = 0; j < MUM_TILES_Y + 2; j++)
		{
			s_gamestate.tiles[i][j].is_mine = 0;
			s_gamestate.tiles[i][j].revealed = 0;
			s_gamestate.tiles[i][j].adjacent_mines = 0;
			s_gamestate.tiles[i][j].is_place = 0;
		}
	}
	pthread_mutex_lock(&mutex2);
	if(first_f)
	{
		place_mines(&s_gamestate,first_f);//set mines
	}
	if(first_f == 0)
	{
		place_mines(&s_gamestate,first_f);//set mines
	}
	pthread_mutex_unlock(&mutex2);
	first_f = 0;
	//display_adjacent_mines(s_gamestate);
	//display_mine_location(s_gamestate);
	
	start_t = time(NULL);
	
	while((readMsg_len = read(socketCon,&ch,sizeof(ch)))>0)
	{
		if((ch == 'R') || (ch == 'r'))
		{
			read(socketCon, recv_coordinates, sizeof(recv_coordinates));
			//printf("recv_coordinates:%s\n",recv_coordinates);
			x = coordinates_change(recv_coordinates[0]);
			y = coordinates_change(recv_coordinates[1]);
			//printf("R recv:%d %d\n",x,y);
			bzero(&recv_coordinates,sizeof(recv_coordinates));
			ret = Sweep(&s_gamestate,x,y);
			//printf("R ret = %d\n",ret);
			if(ret == IS_MINE)
			{
				pthread_mutex_lock(&mutex1);
				read_Leaderboard(&LB);
				pthread_mutex_unlock(&mutex1);
				if(LB.is_record)
				{
					LB.played_NUM++;
					pthread_mutex_lock(&mutex1);
					write_Leaderboard(LB);
					pthread_mutex_unlock(&mutex1);
				}
				write(socketCon,&ret,sizeof(ret));
				write(socketCon, &s_gamestate, sizeof(GameState));
				goto flag;
			}else if(ret == MINE_REVEALED)
			{
				write(socketCon,&ret,sizeof(ret));
			}else
			{
				write(socketCon,&ret,sizeof(ret));
				//printf("R sendMsg_len = %d\n",sendMsg_len);
				
				recursion(&s_gamestate,x,y);
				change_state(&send_gamestate,s_gamestate);
				//c_display_adjacent_mines(send_gamestate);
				//printf("********************");
				write(socketCon,&send_gamestate,sizeof(GameState));
				
			}
			
			

		}else if((ch == 'P') || (ch == 'p'))
		{
			read(socketCon, recv_coordinates, sizeof(recv_coordinates));
			//printf("recv_coordinates:%s\n",recv_coordinates);
			x = coordinates_change(recv_coordinates[0]);
			y = coordinates_change(recv_coordinates[1]);
			//printf("P recv:%d %d\n",x,y);
			bzero(&recv_coordinates,sizeof(recv_coordinates));
			ret = Sweep(&s_gamestate,x,y);
			//printf("P ret = %d\n",ret);
			if(ret == IS_MINE)
			{
				ret = --count;
				//printf("line 425 P ret = %d\n",ret);
				s_gamestate.tiles[x][y].is_place = 1;
			}else
			{
				ret = count;
			}
			w_ret=write(socketCon,&ret,sizeof(ret));//return mines
			//printf("line 433 P w_ret = %d\n",w_ret);
			end_t = time(NULL);
			s_time = end_t - start_t;
			w_ret=write(socketCon,&s_time,sizeof(s_time));//return time
			//printf("line 437 P w_ret = %d\n",w_ret);
			read(socketCon, &g_o, sizeof(g_o));
			if(g_o)
			{
				//printf("game won\n");
				LB.played_NUM++;
				LB.won_NUM++;
				LB.w_time = s_time;
				pthread_mutex_lock(&mutex1);
				write_Leaderboard(LB);
				pthread_mutex_unlock(&mutex1);
				//printf("]]]]]]]]]]]]]]]]\n");
				count = MUM_MINES;
				g_o = 0;
				goto flag;
			}
		}else if((ch == 'Q') || (ch == 'q'))
		{
			LB.played_NUM++;
			goto flag;
		}
	}
	//game over
	

	close(socketCon);
	return;
}

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		printf("You need to input the port number.\n");
		return 0;
	}
	int port = atoi(argv[1]);
	pthread_mutex_init(&mutex1,NULL);
	pthread_mutex_init(&mutex2,NULL);
	FILE* fpt;

	if ((fp=fopen("Leaderboard.txt","w+"))==NULL)
    {	
        printf("cannot open file\n");
        return 0;
    }
	fclose(fp);
	if ((fpt=fopen("temp.txt","w+"))==NULL)
    {	
        printf("cannot open file\n");
        return 0;
    }
	fclose(fpt);
    struct threadpool *pool = threadpool_init(10, 20);
	
	int socketListen = socket(AF_INET, SOCK_STREAM, 0);
    if(socketListen < 0){
        printf("socket failed\n");
        exit(-1);
    }else{
        printf("socket success\n");
    }
   
    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(struct sockaddr_in));
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY); 
    server_addr.sin_port=htons(port);
    if(bind(socketListen, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) != 0){
        perror("bind failed\n");
        exit(-1);
    }else{
        printf("bind success\n");
    }

	if(listen(socketListen, 10) != 0){
        printf("listen failed\n");
        exit(-1);
    }else{
        printf("listen success\n");
    }
	
	int sockaddr_in_size = sizeof(struct sockaddr_in);
	struct sockaddr_in client_addr;
	
	while(1)
	{
		int socketCon = accept(socketListen, (struct sockaddr *)(&client_addr), (socklen_t *)(&sockaddr_in_size));
		if(socketCon < 0){
			printf("connection failed\n");
		}else{
			printf("connection success ip: %s:%d\r\n",inet_ntoa(client_addr.sin_addr),client_addr.sin_port);
		}
		//printf("socketCon：%d\n",socketCon);
		threadpool_add_job(pool, work, &socketCon);
		//printf(" 1 work\n");
	}

	
    //sleep(5);
	pthread_mutex_destroy(&mutex1); 
	pthread_mutex_destroy(&mutex2);
	
    threadpool_destroy(pool);
	fclose(fp);
	close(socketListen);
    return 0;
}
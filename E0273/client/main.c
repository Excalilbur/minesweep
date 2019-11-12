#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
 #include"game.h"
void change_LB(Leaderboard* a,Leaderboard* b)
{
	
}
void sort(Leaderboard* h_LBt,int n)
{
	Leaderboard* p_LBt = h_LBt;
	Leaderboard t_LBt;
	int i = 0,j = 0,min = 0;
	for(i=0;i<n-1;i++)
	{
		for(j=0;j<n-i-1;j++)
		{
			if(p_LBt[j].w_time < p_LBt[j+1].w_time)
			{
				memcpy(&t_LBt,&p_LBt[j],sizeof(p_LBt[j]));
				memcpy(&p_LBt[j],&p_LBt[j+1],sizeof(p_LBt[j+1]));
				memcpy(&p_LBt[j+1],&t_LBt,sizeof(t_LBt));
			}
		}
	}
    for(i=0;i<n;i++)
	{   
		printf("%s\t\t %d seconds\t %d games won, %d games played\n",p_LBt[i].username,p_LBt[i].w_time,p_LBt[i].won_NUM,p_LBt[i].played_NUM);
	}
}
void update_gs(GameState* c_gamestate,GameState recv_gamestate)
{
	int i,j;
	for (i = 0; i < MUM_TILES_X + 2; i++)
	{
		for (j = 0; j < MUM_TILES_Y + 2; j++)
		{
			c_gamestate->tiles[i][j].is_mine = recv_gamestate.tiles[i][j].is_mine;
			c_gamestate->tiles[i][j].revealed = recv_gamestate.tiles[i][j].revealed;
			c_gamestate->tiles[i][j].adjacent_mines = recv_gamestate.tiles[i][j].adjacent_mines;
			c_gamestate->tiles[i][j].is_place = recv_gamestate.tiles[i][j].is_place;
		}
	}
}
int client_show(int socketCon)
{
	Leaderboard LBt;
	Leaderboard* p_LBt=NULL;
	Leaderboard* h_LBt=NULL;
	memset(&LBt,0,sizeof(Leaderboard));
	int n = 0,readMsg_len = 0,t = 0;
	bool empty_F = 0;
	char ch;
	read(socketCon,&empty_F,sizeof(empty_F));
	if(empty_F)
	{
		printf("================================================\n");
		printf("\n");
		printf("\n");
		printf("================================================\n");
		return 1;
	}
	read(socketCon,&n,sizeof(n));
	t = n;
	printf("================================================\n");
	printf("\n");
	
	p_LBt = (Leaderboard *)malloc(sizeof(Leaderboard)*(n-1));
	h_LBt = p_LBt;
	while(n>1)
	{
		read(socketCon,&LBt,sizeof(LBt));
		n--;
		memcpy(p_LBt,&LBt,sizeof(LBt));
		//printf("%s\t\t %d seconds\t %d games won, %d games played\n",LBt.username,LBt.w_time,LBt.won_NUM,LBt.played_NUM);
		//printf("%s\t\t %d seconds\t %d games won, %d games played\n",p_LBt->username,p_LBt->w_time,p_LBt->won_NUM,p_LBt->played_NUM);
		
		p_LBt++;
		memset(&LBt,0,sizeof(Leaderboard));
	}
	sort(h_LBt,t-1);
	printf("\n");
	printf("================================================\n");
	
	free(h_LBt);
	p_LBt=NULL;
	h_LBt=NULL;
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

int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		printf("You need to input the IP address and port number correctly.\n");
		return 0;
	}
	int port = atoi(argv[2]);
	int g_o = 0;
	int s_time;
	int sendMsg_len,readMsg_len,ret = 0;
	char buf[1024];
	char recv_coordinates[10];
	char username[20];
	char password[20];
	
	GameState c_gamestate,recv_gamestate;
	int i = 0;
	int j = 0;
	int x = 0;
	int y = 0;
	int i_choice = 0;
	
	int num_mines = MUM_MINES;
	char ch;
	int r_ret;
	
    //printf("start socket\n");
  
    int socketCon = socket(AF_INET, SOCK_STREAM, 0);
    if(socketCon < 0){
        printf("socket failed\n");
        exit(-1);
    }
    
    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(struct sockaddr_in));
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=inet_addr(argv[1]);
    server_addr.sin_port=htons(port);
    
    int res_con = connect(socketCon,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr));
    if(res_con != 0){
        printf("connect failed\n");
        exit(-1);
    }
    //printf("connect success res_con = %d\n",res_con);
	
	menu();
	printf("\n");
	printf("Username:");
	scanf("%s",username);
	printf("Password:");
	scanf("%s",password);
	
	write(socketCon,username,sizeof(username));
	write(socketCon,password,sizeof(password));
	
	
	readMsg_len = read(socketCon, buf, sizeof(buf));
	//printf("test: %s\n",buf);
	if(!strncmp(buf,"ok",2))
	{
		printf("Login success\n");
	}else
	{
		printf("You entered either an incorrect username or password.Disconnecting\n");
		close(socketCon);
		return 0;
	}
	bzero(&buf,sizeof(buf));
	
	flag:
	
	printf("\n");
	menu2();
	memset(&c_gamestate,0,sizeof(GameState));
	memset(&recv_gamestate,0,sizeof(GameState));

	
	for (i = 0; i < MUM_TILES_X + 2; i++)
	{
		for (j = 0; j < MUM_TILES_Y + 2; j++)
		{
			c_gamestate.tiles[i][j].is_mine = 0;
			c_gamestate.tiles[i][j].revealed = 0;
			c_gamestate.tiles[i][j].adjacent_mines = 0;
			c_gamestate.tiles[i][j].is_place = 0;
		}
	}
	scanf("%d",&i_choice);
	//fflush(stdin);
	switch(i_choice)
	{
		case 1:
			write(socketCon,&i_choice,sizeof(i_choice));
			break;
		case 2:
			write(socketCon,&i_choice,sizeof(i_choice));
			client_show(socketCon);
			
		case 3:
			
		default:
			close(socketCon);
			return 0;
			break;
	}
	printf("\n");
	//printf("game start\n");
	printf("\n");
	
   
    while(1){
		printf("\n");
		printf("Remaining mines: %d\n",num_mines);
		c_display_adjacent_mines(c_gamestate);
		
		printf("Choose an option:\n");
		printf("<R> Reveal tile\n");
		printf("<P> Place flag\n");
		printf("<Q> Quit game\n");
		printf("\n");
		printf("Option(R,P,Q): ");
		scanf(" %c",&ch);
		if((ch == 'R') || (ch == 'r'))
		{
			write(socketCon,&ch,sizeof(ch));
			
			printf("\n");
			printf("Enter tile coordinates: ");
			scanf("%s",recv_coordinates);
			sendMsg_len = write(socketCon,recv_coordinates,sizeof(recv_coordinates));
			x = coordinates_change(recv_coordinates[0]);
			y = coordinates_change(recv_coordinates[1]);
			//printf("([0],[1])=(%c,%c)\n",recv_coordinates[0],recv_coordinates[0]);
			//printf("(x,y)=(%d,%d)\n",x,y);
			bzero(&recv_coordinates,sizeof(recv_coordinates));
			if(sendMsg_len > 0){
				//printf("send success,socketCon:%d\n",socketCon);
			}else{
				//printf("send failed\n");
			}
			readMsg_len = read(socketCon, &ret, sizeof(ret));
			//printf("R readMsg_len = %d\n",readMsg_len);
			//printf("line171 R ret = %d\n",ret);
			if(ret == IS_MINE)
			{
				c_gamestate.tiles[x][y].is_mine = 1;
				printf("Game over! You hit a mine.\n");// game over
				read(socketCon, &c_gamestate, sizeof(GameState));
				display_mine_location(c_gamestate);
				goto flag;
			}else if(ret == MINE_REVEALED)
			{
				printf("The Mine is revealed. Please choose again.\n");
			}else
			{//****************************
				c_gamestate.tiles[x][y].adjacent_mines = ret;
				c_gamestate.tiles[x][y].revealed = 1;
				read(socketCon, &recv_gamestate, sizeof(GameState));
				update_gs(&c_gamestate,recv_gamestate);
			}
		}else if((ch == 'P') || (ch == 'p'))
		{
			write(socketCon,&ch,sizeof(ch));
			
			printf("\n");
			printf("Enter tile coordinates: ");
			scanf("%s",recv_coordinates);
			sendMsg_len = write(socketCon,recv_coordinates,sizeof(recv_coordinates));
			x = coordinates_change(recv_coordinates[0]);
			y = coordinates_change(recv_coordinates[1]);
			//printf("([0],[1])=(%c,%c)\n",recv_coordinates[0],recv_coordinates[0]);
			//printf("(x,y)=(%d,%d)\n",x,y);
			bzero(&recv_coordinates,sizeof(recv_coordinates));
			if(sendMsg_len > 0){
				//printf("send success,socketCon:%d\n",socketCon);
			}else{
				//printf("send failed\n");
			}
			
			r_ret=read(socketCon, &ret, sizeof(ret));
			//printf("line 249 r_ret = %d\n",r_ret);
			r_ret=read(socketCon, &s_time, sizeof(s_time));
			//printf("line 251 r_ret = %d\n",r_ret);
			//printf("line232 P ret = %d\n",ret);
			//printf("line233 P s_time = %d\n",s_time);
			if(num_mines != ret)
			{
				c_gamestate.tiles[x][y].is_place = 1;
				num_mines = ret;
			}
			if(num_mines == ret)
			{
				printf("(%d,%d) no mine\n",x,y);
			}

			if(num_mines == 0)
			{
				printf("Congratulations! You have located all the mines.\n");
				printf("You won in %d seconds!\n",s_time);
				g_o = 1;
				write(socketCon,&g_o,sizeof(g_o));
				num_mines = MUM_MINES;
				g_o = 0;
				goto flag;
				break;
			}
			write(socketCon,&g_o,sizeof(g_o));
		}else if((ch == 'Q') || (ch == 'q'))
		{
			write(socketCon,&ch,sizeof(ch));
			goto flag;
		}

    }
    
    close(socketCon);
    return 0;
}


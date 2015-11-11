#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>

int ServerDice[4], ServerResult;//莊家的骰子&點數
int roll=1;
int fd, ret, player, input, jump=0, banker=0, money=10000 /*flag=0, flag_again=1,*/ ; 
int client1Quit = 0, client2Quit = 0;
int sockfd;
int connfd[5];
int flag[5] = {0}, choice=0, space=0;
struct sockaddr_in addr_svr;
struct sockaddr_in addr_cli;
char buf[20];

int sLen = sizeof(addr_cli);
pthread_t th[5];

/**************************************************
					  Dice
***************************************************/
int DiceStart(int dice[],int size)
{
	int total[6]={0}, 
		i, j, k, 
		dice_again=1, result=0, max,
		counter1, counter2, counter3, counter4;
  
	while(dice_again)//擲骰子
	{
		result=0;
		counter1=0; counter2=0; counter3=0; counter4=0;
		srand(time(NULL));

		for(i=0; i<6; i++)
			total[i]=0;//初始=0


		for(i=0; i<4; i++)//4個骰子的點數
			dice[i] = rand() % 6 + 1;
		
		for(i=0; i<4; i++)//紀錄骰子各點數的出現次數
			total[ dice[i]-1 ]++;

		for(i=0; i<6; i++)//計算各點數出現次數 
		{
			if(total[i]==1) counter1++;
			if(total[i]==2) counter2++;
			if(total[i]==3) counter3++;
			if(total[i]==4) counter4++;
			//printf("Num of %d = %d\n", i+1, total[i]);
		}
 

		if(counter4==1 && counter3==0 && counter2==0 && counter1==0) //4個骰子相同
		{
			result=13;
			dice_again = 0;
		}

		else if(counter4==0 && counter3==0 && counter2==2 && counter1==0) //兩兩相同
		{	
			max=0;
			for(i=0; i<6; i++)
			{
				if(total[i]==2) 
					max = i;	
			}

			result = (max+1)*2;	
			
			dice_again = 0;
		}

		else if(counter4==0 && counter3==1 && counter2==0 && counter1==1) //3個相同
		{
			max=0;
			for(i=0; i<6; i++)
			{
				if(total[i]==3)
					max = i;				
			}
			result = max+1;
			for(i=0; i<6; i++)
			{
				if(i != max)
					result += (i+1)*total[i];	
			} 
			dice_again = 0;
		}

		else if(counter4==0 && counter3==0 && counter2==1 && counter1==2) //2個相同 2個不同
		{
			max=0;
			for(i=0; i<6; i++)
			{	
				if(total[i]==2) 
					max = i;	
			}
			for(i=0; i<6; i++)
			{
				if(i != max)
					result += (i+1)*total[i];	
			}
			dice_again = 0;
		}

		else if(counter4==0 && counter3==0 && counter2==0 && counter1==4) //4個都不同
			dice_again = 1;
	}//end while
    //printf("dice1= %d, dice2= %d, dice3= %d, dice4= %d\n",dice[0],dice[1],dice[2],dice[3]);
	//printf("點數: %d\n",result);
	return result;
}

/**************************************************************************
							  int 轉 string
**************************************************************************/
void int2str(int i, char *s) 
{
  sprintf(s,"%d",i);
}


/**************************************************************************
							   Thread
**************************************************************************/
void* Thread1(void* ptr)//給第1個client用的thread
{ 
	printf("******************** 有玩家進入遊戲 ************************\n");
	client1Quit = 0;
	int btn1=0;
	int GameResult=0;
	int PlayAgain=1, bet=0;
	int ClientBankrupt, ServerBankrupt = 0;

	while(PlayAgain==1)
	{
		btn1=0;
		GameResult=0;
		bet=0;
		int Dice[4]={0}, Result=0;
		char sDice1[15]={0},sDice2[15]={0},sDice3[15]={0},sDice4[15]={0}, 
	   	     sResult[15]={0};

		while( (bet%10 != 0) || (bet == 0) )
		{
		  read(connfd[0], &bet, sizeof(bet));
		}
    
		printf("莊家還剩現金 %d 元\n", money);
		printf("玩家 1 押賭金: %d 元\n\n",bet);	
    
		roll=2;
		while(roll==2)
		{
			if(client2Quit == 1)
				break;
		}//hold on
    
    
		printf("******************** 莊家擲骰子 **************************\n");
		ServerResult = DiceStart(ServerDice,4);
		printf("Dice1= %d, Dice2= %d, Dice3= %d, Dice4= %d\n",ServerDice[0],ServerDice[1],ServerDice[2],ServerDice[3]);
		printf( "點數: %d\n", ServerResult);
		printf("********************************************************\n\n");

    
		
		// Read Data From Client
		while(btn1!=1)
		{
			if( read(connfd[0], &btn1, sizeof(btn1)) == -1 )//讀取client端所按的'1'
			{
				printf("Error: read()\n");
				exit(1);
			}
		}
		
		if(btn1 == 1)//若使用者按1
		{
			//printf("玩家1 按了button %d\n",btn1);
			sleep(1);
			Result = DiceStart(Dice,4);//玩家1擲骰子
			

		}
		printf("******************** 玩家擲骰子 **************************\n");
     	printf("Dice1= %d, Dice2= %d, Dice3= %d, Dice4= %d\n",
				Dice[0],Dice[1],Dice[2],Dice[3]);
		printf( "點數 : %d\n", Result);
		printf("************************************************************\n");
		
		
		if(Result > ServerResult)//玩家贏
		{
			GameResult = 0;
			money -= bet; //莊家扣錢
		}
		if(Result == ServerResult)//平手
			GameResult = 1;
		if(Result < ServerResult)//莊家贏
		{
			GameResult = 2;
			money += bet;
		}
		
		printf("莊家還剩 %d 元\n\n", money);
		//printf("server:  geme result=%d\n", GameResult);

		write(connfd[0], &ServerDice[0], sizeof(ServerDice[0]));//傳骰子點數回Client
		write(connfd[0], &ServerDice[1], sizeof(ServerDice[1]));//傳骰子點數回Client
		write(connfd[0], &ServerDice[2], sizeof(ServerDice[2]));//傳骰子點數回Client
		write(connfd[0], &ServerDice[3], sizeof(ServerDice[3]));//傳骰子點數回Client
		write(connfd[0], &ServerResult, sizeof(ServerResult));//傳骰子點數回Client

		write(connfd[0], &Dice[0], sizeof(Dice[0]));//傳骰子點數回Client
		write(connfd[0], &Dice[1], sizeof(Dice[1]));//傳骰子點數回Client
		write(connfd[0], &Dice[2], sizeof(Dice[2]));//傳骰子點數回Client
		write(connfd[0], &Dice[3], sizeof(Dice[3]));//傳骰子點數回Client
		write(connfd[0], &Result, sizeof(Result));//傳骰子點數回Client
		
		write(connfd[0], &GameResult, sizeof(GameResult));//傳遊戲結果(輸,贏,或平手)回client
		

		if(money <= 0)
		{
			printf("** 莊家破產 ! **\n");
			ServerBankrupt = 1;
			write(connfd[0], &ServerBankrupt, sizeof(ServerBankrupt));
		}
		else
			write(connfd[0], &ServerBankrupt, sizeof(ServerBankrupt));

		if(ServerBankrupt == 1)
			break;
		//while(ClientBankrupt != 0 || ClientBankrupt != 1)

		read(connfd[0], &ClientBankrupt, sizeof(ClientBankrupt));//讀取client端是否破產
			
		//printf("test: ClientBankrupt=%d\n", ClientBankrupt);//test用
		

		roll=2;
		while(roll==2)
		{
			if(client2Quit==1)
				break;
		}

		if(ClientBankrupt != 1)
		{
			read(connfd[0], &PlayAgain, sizeof(PlayAgain));//讀取client端所按的'1'	
			while(PlayAgain!=1 && PlayAgain!=2)//一直讀到client端輸入1或2為止 
			{
				read(connfd[0], &PlayAgain, sizeof(PlayAgain));
			}	
			
			if(PlayAgain==1)
			{}
				//printf("PlayAgain=%d\n\n\n",PlayAgain);
			else if(PlayAgain==2)
			{
				//printf("PlayAgain=%d\n",PlayAgain);
				client1Quit = 1;
			}
		}
		else
		{
				printf("玩家1已破產!\n");
				PlayAgain = 0;
				client1Quit = 1;
		}
    
		roll=2;
		while(roll==2)
		{
    		if(client2Quit == 1)
			break;
		}
	}//end while
	
	flag[0] = 0;
	printf("玩家退出遊戲 ~~~\n");
}

void* Thread2(void* ptr)//給第2個client用的thread
{ 
	printf("******************** 有玩家進入遊戲 ************************\n");
	client2Quit = 0;
	int btn1=0;
	int GameResult=0;
	int PlayAgain=1, bet=0;
	int ClientBankrupt, ServerBankrupt = 0;

	while(PlayAgain==1)
	{
		btn1=0;
		GameResult=0;
		bet=0;
		int Dice[4]={0}, Result=0;
		char sDice1[15]={0},sDice2[15]={0},sDice3[15]={0},sDice4[15]={0}, 
	   	     sResult[15]={0};

		while( (bet%10 != 0) || (bet == 0) )
		{
			read(connfd[1], &bet, sizeof(bet));
		}
		
		while(roll==1)
		{
			if(client1Quit == 1)
				break;
		}//hold on

		//printf("莊家還剩現金 %d 元\n", money);
		printf("玩家 2 押賭金: %d 元\n\n",bet);	

		if(client1Quit == 1)
		{
			printf("******************** 莊家 **************************\n");
			ServerResult = DiceStart(ServerDice,4);
			printf("Dice1= %d, Dice2= %d, Dice3= %d, Dice4= %d\n",ServerDice[0],ServerDice[1],ServerDice[2],ServerDice[3]);
			printf( "點數: %d\n", ServerResult);
			printf("********************************************************\n\n");
		}

		roll=1;
    
		// Read Data From Client
		while(btn1!=1)
		{
			if( read(connfd[1], &btn1, sizeof(btn1)) == -1 )//讀取client端所按的'1'
			{
				printf("Error: read()\n");
				exit(1);
			}
		}
		
		if(btn1 == 1)//若使用者按1
		{
			//printf("玩家2 按了button %d\n",btn1);
			Result = DiceStart(Dice,4);//玩家1擲骰子
			Result = DiceStart(Dice,4);//玩家1擲骰子
		}

		
				while(roll==1)//hold
				{
					if(client1Quit == 1)
						break;
				}


		printf("******************** 玩家擲骰子 **************************\n");
     	printf("Dice1= %d, Dice2= %d, Dice3= %d, Dice4= %d\n",
				Dice[0],Dice[1],Dice[2],Dice[3]);
		printf( "點數 : %d\n", Result);
		printf("************************************************************\n");
		
		
		if(Result > ServerResult)//玩家贏
		{
			GameResult = 0;
			money -= bet; //莊家扣錢
		}
		if(Result == ServerResult)//平手
			GameResult = 1;
		if(Result < ServerResult)//莊家贏
		{
			GameResult = 2;
			money += bet;
		}
		
		printf("莊家還剩 %d 元\n\n", money);
		//printf("server:  geme result=%d\n", GameResult);

		
		write(connfd[1], &ServerDice[0], sizeof(ServerDice[0]));//傳骰子點數回Client
		write(connfd[1], &ServerDice[1], sizeof(ServerDice[1]));//傳骰子點數回Client
		write(connfd[1], &ServerDice[2], sizeof(ServerDice[2]));//傳骰子點數回Client
		write(connfd[1], &ServerDice[3], sizeof(ServerDice[3]));//傳骰子點數回Client
		write(connfd[1], &ServerResult, sizeof(ServerResult));//傳骰子點數回Client
		



		write(connfd[1], &Dice[0], sizeof(Dice[0]));//傳骰子點數回Client
		write(connfd[1], &Dice[1], sizeof(Dice[1]));//傳骰子點數回Client
		write(connfd[1], &Dice[2], sizeof(Dice[2]));//傳骰子點數回Client
		write(connfd[1], &Dice[3], sizeof(Dice[3]));//傳骰子點數回Client
		write(connfd[1], &Result, sizeof(Result));//傳骰子點數回Client
		
		write(connfd[1], &GameResult, sizeof(GameResult));//傳遊戲結果(輸,贏,或平手)回client
		

		if(money <= 0)
		{
			printf("** 莊家破產 ! **\n");
			ServerBankrupt = 1;
			write(connfd[1], &ServerBankrupt, sizeof(ServerBankrupt));
		}
		else
			write(connfd[1], &ServerBankrupt, sizeof(ServerBankrupt));

		if(ServerBankrupt == 1)
			break;
		//while(ClientBankrupt != 0 || ClientBankrupt != 1)

		read(connfd[1], &ClientBankrupt, sizeof(ClientBankrupt));//讀取client端是否破產
			
		//printf("test: ClientBankrupt=%d\n", ClientBankrupt);//test用
		

		roll=1;
		while(roll==1)
		{
			if(client1Quit==1)
				break;
		}

		if(ClientBankrupt != 1)
		{
			read(connfd[1], &PlayAgain, sizeof(PlayAgain));//讀取client端所按的'1'	
			while(PlayAgain!=1 && PlayAgain!=2)//一直讀到client端輸入1或2為止 
			{
				read(connfd[1], &PlayAgain, sizeof(PlayAgain));
			}	
			
			if(PlayAgain==1)
			{}
				//printf("PlayAgain=%d\n\n\n",PlayAgain);
			else if(PlayAgain==2)
			{
				//printf("PlayAgain=%d\n",PlayAgain);
				client2Quit = 1;
			}
		}
		else
		{
				printf("玩家2已破產!\n");
				PlayAgain = 0;
				client2Quit = 1;
		}
    roll=1;
	}//end while
	
	flag[1] = 0;
	printf("玩家退出遊戲 ~~~\n");
}


/***********************************************************************
								 main
************************************************************************/
int main(int argc, char * argv[]) 
{	
	int i;
	
	// Set Server Address
	memset( &addr_svr, 0, sizeof(addr_svr));
	addr_svr.sin_family = AF_INET;		
	// default port value
	addr_svr.sin_port= htons(80); 
	// default IP value
	addr_svr.sin_addr.s_addr = inet_addr("140.113.144.94"); 
	
	if (argc>=3)
	{
		addr_svr.sin_port= htons(atoi(argv[2])); // set up the port manually
	}
	if (argc>=2)
	{
		// set up the IP manually
		addr_svr.sin_addr.s_addr = inet_addr(argv[1]); 
	}
	
	// Create Socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	
	// Bind Socket
	if( bind(sockfd, (struct sockaddr *)&addr_svr, sizeof(addr_svr)) == -1) 
	{		
		printf("Error: bind()\n");
		exit(1);
	}
	
	// Make Listening Socket
	if( listen(sockfd, 10) == -1) 
	{		
		printf("Error: listen()\n");
		exit(1);	
	}	
	
	for(;;)
	{
		srand(time(NULL));
		space=0;//剩餘的空位
		for(i=0; i<2; i++)//判斷哪個執行緒是空的
		{
			if(flag[i]==0)
			{
				choice = i;
				space++;
			}
		}
		if(space==2)//第一個人來先用thread1
			choice=0;
		if(space==0)//代表客滿
			continue;
		
		// Get Connected Socket
		if(choice == 0)
		{
			connfd[0] = accept(sockfd, (struct sockaddr *)&addr_cli, &sLen);	
			flag[0] = 1;//設為1 避免別的client連進來
		
			if(connfd[0] == -1) 
			{			
				printf("Error: accept()\n");
				exit(1);		
			}
				
			pthread_create(&th[0], NULL, Thread1, NULL); 
			continue;
			pthread_join(th[0], NULL);
		}

		if(choice == 1)
		{	
			connfd[1] = accept(sockfd, (struct sockaddr *)&addr_cli, &sLen);
			flag[1] = 1;
			if( connfd[1] == -1) 
			{			
				printf("Error: accept()\n");
				exit(1);		
			}
				
			pthread_create(&th[1], NULL, Thread2, NULL);
			continue;
			pthread_join(th[1], NULL);
		}
		
	}
	
	return 0;
}


 
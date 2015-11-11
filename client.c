#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char * argv[]) 
{
	struct sockaddr_in addr_svr;
		
	int sockfd;
		
	char ibuf[20];
	char obuf[20];
	int dice[4], result, serverdice[4], serverresult;
	int btn1, money=100, bet; 
	int GameResult, PlayAgain=1;
	int ClientBankrupt = 0, ServerBankrupt = 0;

		
	// Set Server Address
	memset(&addr_svr, 0, sizeof(addr_svr));
	addr_svr.sin_family= AF_INET;
	// default port value
	addr_svr.sin_port= htons(80); 
	// default IP value
	addr_svr.sin_addr.s_addr = inet_addr("140.113.144.94"); 

	if (argc>=3)
	{
	// set up the port manually
	addr_svr.sin_port= htons(atoi(argv[2])); 
	}
	if (argc>=2)
	{
	// set up the IP manually
	addr_svr.sin_addr.s_addr = inet_addr(argv[1]); 
	}

	// Create Client Socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
		
	if( sockfd == -1) {
			printf("Error: socket()\n");
			exit(1);
	}

	// Connect to Server
	if(connect(sockfd,(struct sockaddr *)&addr_svr,sizeof(addr_svr)) == -1) {
			printf("Error: connect()\n");
			exit(1);
	}
		
	// Input String
	printf("******** �o�O��l�C�� ! ********\n\n");
    
	while(PlayAgain == 1)
	{
		printf("�A�{�b�� %d ��\n\n", money);

		do
		{
			printf("�A�n��h�j? (10������)\n");
			scanf("%d",&bet);
		}
		while( (bet > money) || (bet%10 != 0) );
		
		printf("��%d��\n",bet);
		write(sockfd, &bet, sizeof(bet));//�ǽ����server

		printf("�Ы� 1 �}�l�Y��l\n ");
		scanf("%d", &btn1);
		
		// Write String to Server
		write(sockfd, &btn1, sizeof(btn1));
		
		// Read String from Server
		read(sockfd, &serverdice[0], sizeof(serverdice[0]));//ŪServer�Ǧ^�Ӫ���l�I��
		read(sockfd, &serverdice[1], sizeof(serverdice[1]));
		read(sockfd, &serverdice[2], sizeof(serverdice[2]));
		read(sockfd, &serverdice[3], sizeof(serverdice[3]));
		read(sockfd, &serverresult, sizeof(serverresult));



		read(sockfd, &dice[0], sizeof(dice[0]));//ŪServer�Ǧ^�Ӫ���l�I��
		read(sockfd, &dice[1], sizeof(dice[1]));
		read(sockfd, &dice[2], sizeof(dice[2]));
		read(sockfd, &dice[3], sizeof(dice[3]));
		read(sockfd, &result, sizeof(result));
		
		read(sockfd, &GameResult, sizeof(GameResult));
		
		// Output result
		printf("--------------- ���a����l ------------------\n");
		printf("Dice1= %d, Dice2= %d, Dice3= %d, Dice4= %d\n",serverdice[0],serverdice[1],serverdice[2],serverdice[3]);
		printf("�I�� : %d\n", serverresult);
		printf("-----------------------------------------------\n\n");



		printf("--------------- �z���Y����l ------------------\n");
		printf("Dice1= %d, Dice2= %d, Dice3= %d, Dice4= %d\n",dice[0],dice[1],dice[2],dice[3]);
		printf("�I�� : %d\n", result);
		printf("-----------------------------------------------\n\n");

		/*printf("�z���Y����l��:\n");
		printf("Dice 1:  %d\n", dice[0]);
		printf("Dice 2:  %d\n", dice[1]);
		printf("Dice 3:  %d\n", dice[2]);
		printf("Dice 4:  %d\n", dice[3]);
		printf("�I�� :  %d\n\n", result);*/	
		
		if(GameResult==0)
		{
			printf("*** �AĹ�F !***\n\n");
			money += bet;
		}
		else if(GameResult==1)
			printf("*** ���� ***\n\n");	
		else if(GameResult==2)
		{
			printf("*** �A��F... ***\n\n");
			money -= bet;
		}
		printf("�A�ٳ� %d ��\n\n", money);


		read(sockfd, &ServerBankrupt, sizeof(ServerBankrupt));
		//printf("test: ServerBankrupt=%d\n", ServerBankrupt);//test��
		if(ServerBankrupt == 1)
		{
			printf("���a�}�� �C������\n\n");
			PlayAgain = 0;
		}


		if(money <= 0)//if ���a�S��
		{
			printf("�A�w�g�S���F �ֺu�a!\n\n");
			ClientBankrupt = 1;//�N���a�}��	
		}

		write(sockfd, &ClientBankrupt, sizeof(ClientBankrupt));//��O�_�}���x�T���ǵ�server
		
		if(ClientBankrupt == 1)
		{
			PlayAgain = 0;
		}

		if(ClientBankrupt != 1 && ServerBankrupt != 1)
		{
			printf("�O�_�~��C��?  1 : Yes    2 : No\n");
			scanf("%d", &PlayAgain);
			printf("\n\n\n");
			//printf("playagain=%d\n",PlayAgain);
			write(sockfd, &PlayAgain, sizeof(PlayAgain));//�ǩҫ����䵹server
		}
	}
	
	//printf("client: game result=%d",GameResult);

	// Close socket
	close(sockfd);

	return 0;
}


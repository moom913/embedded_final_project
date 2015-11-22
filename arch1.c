#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

struct player{
    int job;
    int building[4];
    int money;
    int rCard;
    int bCard;
    bool death;
    int ID;
};

int main(int argc, const char * argv[]) {
    int order1[3]={1,2,3}; //選職業的順序
    int order2[3];         //職業的順序
    struct player a[3];
    int i,j,temp,connfd;
    bool gameOver=0;
    int card[5]; //card[0]盜賊 card[1]術士 card[2]貴族 card[3]商人 card[4]軍人
    char sendCard[5];
    
    //client連線
    for(i=0;i<3;i++){
        //clients連線...
        if (connfd == -1) {
            errexit("Error: accepet\n");
        }else{
            //儲存socket number && 回傳成功連線的訊息
            a[i].ID=i;
            a[i].building[0]=0;
            a[i].building[1]=0;
            a[i].building[2]=0;
            a[i].building[3]=0;
        }
    }
    while (!gameOver) {
        for (i=0; i<5; i++) {
            card[i]=1;
        }
        //第一輪選職業
        for (i=0; i<3; i++) {
            for (j=0; j<5; j++) {
                if (card[j]==1) {
                    char *s;
                    sprintf(s,"%d", j);
                    strcat(sendCard,s);   //傳sendCard給玩家（ex:124->盜賊術士商人還在）
                }
            }
            //socket給第i個玩家手牌
            //接回傳值
            //修改原本的牌庫
        }
        //第二輪行使職業效果
        
        //判斷城堡數量
        for (i=0; i<3; i++) {
            temp=0;
            for (j=0; j<4; j++) {
                temp = temp + a[i].building[j];
            }
            if (temp >= 5) {
                //LED亮 遊戲結束
                gameOver=1;
            }

        }
    }
    return 0;
}

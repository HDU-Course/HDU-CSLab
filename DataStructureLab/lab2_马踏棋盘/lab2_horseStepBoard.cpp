#include<stdio.h>
#include<stdlib.h>
int chess[8][8]={0};        //定义棋盘
int move[8][2]={{1,-2},{2,-1},{2,1},{1,2},{-1,2},{-2,1},{-2,-1},{-1,-2}};     //遍历下一个日的位置
int cnt=1;                            //记录马踏的每一步
int sum=1 ;                           //记录解的个数

void print()
{
	int i,j;
	printf("\n");
	printf("%d:\n",sum++);
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
			printf("%5d",chess[i][j]);
		printf("\n");
	}
}

void horse(int x,int y)
{
	int a,b,i;
	for(i=0;i<8;i++)
	{
		a=x+move[i][0];
		b=y+move[i][1];
		if(a>=0 && a<8 && b>=0 && b<8 && !chess[a][b])
		{
			chess[a][b]=++cnt;
			if(cnt<64) horse(a,b);
			else  print();   
			chess[a][b]=0;
			cnt--;
		}
	}
}

void Init()
{
	int i,j;
	for(i=0; i<8; i++)
	{
		for(j=0; j<8; j++)
		chess[i][j]=0;
	}
	cnt=1;
}

int main(void)
{
    int i,j;
    printf("请以(x,y)的形式输入马的初始位置(注意1<=x,y<=8):");
    scanf("(%d,%d)", &i, &j);
    if(i>=1 && i <=8 & j>=1 && j<=8){
        Init();
	    chess[i-1][j-1]=1;     //直接将马置于该位置，该位置为第一步
	    horse(i-1,j-1);	
    } else {
        printf("输入格式有误，请检查");
    }
	return 0;
}

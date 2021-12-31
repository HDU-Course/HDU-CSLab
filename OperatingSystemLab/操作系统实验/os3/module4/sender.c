#include "my_shm.h"

int main()
{
    init_signal();

    //初始化共享内存
    memset((char *)shmp, '\0', MEM_MIN_SIZE);
    int flag2 = 0;
    char s_str[MEM_MIN_SIZE];
    char temp[MEM_MIN_SIZE];
    while(1)
    {
    	sem_wait(sem_send);
    	printf("输入你要发送的信息:\n");
    	scanf("%s", s_str);
    	
    	//输入到共享内存中
    	strcpy(temp, (char *)shmp);
    	if(strcmp(temp, "") == 0)
    		flag2 = 1;
    	memset((char *)shmp, '\0', MEM_MIN_SIZE);
    	strcat(temp, "\n");
        strcat(temp, s_str);
        strcpy((char *)shmp, temp);
	
	if(flag2)
    	{
    		sem_post(sem_receive);
    		flag2 = 0;
    	}
    	sem_post(sem_send);  
    	if(strcmp(s_str, "exit") == 0)
    	{
    		sem_wait(sem_over);
    		char r_str[100];
    		strcpy(r_str, (char *)shmp);
    	
		if(strcmp(r_str, "over") == 0)
    		{
    			printf("接收到: %s\n", r_str);
    			printf("quit sender!!\n");
    			break;
    		}
    	}	
    	
   }
    
    sem_unlink("send");
    sem_unlink("over");
    sem_unlink("receive");
    shmdt(shmp);

    exit(0);
}

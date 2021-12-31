#include "my_shm.h"


int main()
{
    init_signal();
    int flag1 = 0;
    
    while(1)
    {
        sem_wait(sem_receive);
        char recv[MEM_MIN_SIZE];
    	strcpy(recv, (char *)shmp);
    	memset((char *)shmp, '\0', 1024);
    	printf("receive: %s\n",recv);
        
	if(strstr(recv, "exit"))
	{
		sem_wait(sem_send);
    		char send[100]="over";
    		memset((char *)shmp, '\0', 1024);
    		strcpy((char *)shmp, send);
       	sem_post(sem_over);  
       	flag1 = 1;
	}
    	
    	if(flag1)
    	    break;
    	
    }
    
    shmdt(shmp);
    printf("quit receiver!\n");
    exit(0);
}

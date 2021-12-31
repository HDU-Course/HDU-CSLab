#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <glob.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


#define MEM_MIN_SIZE 1024
#define KEY 1111

sem_t *sem_send;
sem_t *sem_receive;
sem_t *sem_over;


key_t key;
int shmid;
void *shmp;

void init_signal()
{
    //初始化信号量
    sem_send = sem_open("send", O_CREAT, 0666, 1);
    sem_receive = sem_open("receive", O_CREAT, 0666, 0);
    sem_over = sem_open("over", O_CREAT, 0666, 0);
 

    shmid = shmget(KEY, MEM_MIN_SIZE, 0666|IPC_CREAT); //IPC_CREAT  如果共享内存不存在，则创建一个共享内存，否则打开操作。成功则返回id (一个与key相关的标识符)
    if(shmid < 0)
    {
        printf("创建共享内存出错！\n");
        exit(-1);
    }

    shmp = shmat(shmid, NULL, 0);//指定共享内存映射到新虚拟地址空间，返回起始地址
    if((int)shmp == -1)
    {
        printf("映射内存出错！\n");
        exit(-1);
    }
}

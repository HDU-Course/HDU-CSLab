#define _GNU_SOURCE
#include <unistd.h>
#include<sys/syscall.h>
#include<stdio.h>
#include<stdlib.h>
int main(){
    pid_t pid;
    int nicevalue;
    int flag;
    int p = 0;
    int n = 0;
    int *prio;
    int *nice;
    prio = &p;
    nice = &n;

    printf("输入pid：");
    scanf("%d", &pid);
  
    
    printf("pid = %d\n", pid);
    
    printf("输入nice：");
    scanf("%d",&nicevalue);

    printf("输入flag：");
    scanf("%d",&flag);

    syscall(335,pid,flag,nicevalue,prio,nice);

    printf("现在的nice为%d,prio为%d\n",n,p);
    return 0;
}



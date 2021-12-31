#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define BUF_MAX_SIZE 8192

// 如果x为假，则报错
void CHECK(int x){
	if(!x){
		printf("error appear");
		exit(-1);
	}	
}
int main(int argc, char **argv) {
    int pipefd[2], pid, i = 0;
    int flag = 0, n;
    char buf[BUF_MAX_SIZE];//管道缓冲池
    char str[BUF_MAX_SIZE];

    sem_t *write_mutex;//sem_t本质上是一个长整形 对于管道进行写操作的互斥信号量
    sem_t *read_mutex1;//进程二三都发完消息的信号量
    sem_t *read_mutex2;
    write_mutex = sem_open("pipe_test_wm", O_CREAT | O_RDWR, 0666, 0);
    read_mutex1 = sem_open("pipe_test_rm_1", O_CREAT | O_RDWR, 0666, 0);
    read_mutex2 = sem_open("pipe_test_rm_2", O_CREAT | O_RDWR, 0666, 0);
	/*
		创建有名信号量，默认值都为0
		sem_t *sem_open(const char *name,int oflag,mode_t mode,unsigned int value);
		name   	信号量的外部名字
		oflag   选择创建或打开一个现有的信号量
				O_CREAT | O_RDWR 若不存在则创建，若存在则直接打开
		mode 	权限位
				权限0666：
				第一个 0 表示这个数是八进制
				第一个 6 表示文件拥有者有读写权，但没有执行权限
				第二个 6 表示文件拥有者同组用户有读写权限，但没有执行权限
				第三个 6 表示其它用户有读写权限，但没有执行权限
		value 	信号量初始值 该初始不能超过 SEM_VALUE_MAX，这个常值必须低于为32767
				此次实验中默认为0		
				
		返回值：成功时返回指向信号量的指针，出错时为SEM_FAILED
	*/

    memset(buf, 0, BUF_MAX_SIZE);
    memset(str, 0, BUF_MAX_SIZE);
	/*
		 在<string.h>下  
		 void *memset(void *s, int c, size_t n) 初始化数组
		 数组名 0 数组长度
	*/
    
    CHECK(pipe(pipefd) == 0);// 创建管道并检查操作是否成功
	/*
		<unistd.h>
		int pipe(int pipefd[2]); 成功：0；失败：-1
		pipefd[0]:读管道 pipefd[1]:写管道
		两个文件描述符引用，一个表示读端，一个表示写端
		管道实为内核使用环形队列机制，借助内核缓冲区(4k)实现。
	*/

    CHECK((pid = fork()) >= 0);// 创建第一个子进程并检查操作是否成功

    // 第一个子进程，利用非阻塞写测试管道大小
    if (pid == 0) {
        int count = 0;
        close(pipefd[0]);//关闭读端
        int flags = fcntl(pipefd[1], F_GETFL);//获取写端状态
		/*
			int fcntl(int fd, int cmd);
			F_GETFL    取得fd的文件状态标志
			fcntl()针对(文件)描述符提供控制.参数fd是被参数cmd操作(如下面的描述)的描述符.            
　　　　	针对cmd的值,fcntl能够接受第三个参数（arg）

			fcntl函数有5种功能：

　　　　 	1.复制一个现有的描述符（cmd=F_DUPFD）.
			2.获得／设置文件描述符标记(cmd=F_GETFD或F_SETFD).
            3.获得／设置文件状态标记(cmd=F_GETFL或F_SETFL).
            4.获得／设置异步I/O所有权(cmd=F_GETOWN或F_SETOWN).
            5.获得／设置记录锁(cmd=F_GETLK,F_SETLK或F_SETLKW).
		*/
		

        // 管道默认是阻塞写，通过`fcntl`设置成非阻塞写，在管道满无法继续写入时返回-EAGAIN，作为循环终止条件
        fcntl(pipefd[1], F_SETFL, flags | O_NONBLOCK);
		/*
			int fcntl(int fd, int cmd, long arg);
			F_SETFL 设置文件描述符状态旗标，参数arg为新旗标，但只允许O_APPEND、O_NONBLOCK和O_ASYNC位的改变，其他位的改变将不受影响
		*/
    
        // 写入管道
        while (!flag) {
            n = write(pipefd[1], buf, BUF_MAX_SIZE);//一次装入8192B数据 ，因为非阻塞写，所以写满了就会跳出循环，算出管道大小
            if (n == -1) {
                flag = 1;
            } else {
                count++;
                printf("children 1 write %dB\n", n);
            }
        }
        printf("space = %dKB\n", (count * BUF_MAX_SIZE) / 1024);
        exit(0);
    }

    // 创建第二个子进程并检查操作是否成功
    CHECK((pid = fork()) >= 0);
    if (pid == 0) {//当前子进程
        sem_wait(write_mutex);//检查是否有人在写，有则等待
        close(pipefd[0]);//关闭读端
        n = write(pipefd[1], "This is the second children.\n", 29);//写入数据
        printf("children 2 write %dB\n", n);
        sem_post(write_mutex);//释放写权限
        sem_post(read_mutex1);//告诉父进程我信息发完了
        exit(0);//退出该进程
    }

    // 创建第三个子进程并检查操作是否成功
    CHECK((pid = fork()) >= 0);
    if (pid == 0) {
        sem_wait(write_mutex);
        close(pipefd[0]);
        n = write(pipefd[1], "This is the third children.\n", 28);
        printf("children 3 write %dB\n", n);
        sem_post(write_mutex);
        sem_post(read_mutex2);
        exit(0);
    }

    // 等待第一个子进程运行完成，父进程继续运行
    wait(0);
    close(pipefd[1]);//关闭写端口，父进程读取子进程一的数据清空管道
    int flags = fcntl(pipefd[0], F_GETFL);// 取得文件状态标志

    // 设置非阻塞性读，作为循环结束标志
    fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);
    while (!flag) {
        n = read(pipefd[0], str, BUF_MAX_SIZE);
        if (n == -1) {
            flag = 1;
        } else {
            printf("%dB read\n", n);
        }
    }
    sem_post(write_mutex);//释放写权限

    // 等待子进程二、三写入完毕
    sem_wait(read_mutex1);
    sem_wait(read_mutex2);
    n = read(pipefd[0], str, BUF_MAX_SIZE);
    printf("%dB read\n", n);
    for (i = 0; i < n; i++) {
        printf("%c", str[i]);
    }

    sem_close(write_mutex);
    sem_close(read_mutex1);
    sem_close(read_mutex2);
	/*
		int sem_close(sem_t *sem)
		关闭有名信号量 成功返回0，失败返回-1
		一个进程终止时，内核对其上仍打开的所有有名信号量自动执行关闭操作，不论进程是自愿还是非自愿终止，这种自动关闭都会发生
		但关闭一个信号量并没有将他从系统中删除
		Posix有名信号灯至少是随内核持续的：即使当前没有进程打开着某个信号灯，他的值仍然保持
		多进程打开时候，一边sem_close后，仍可以打开已经打开的信号量
	*/
    sem_unlink("pipe_test_wm");
    sem_unlink("pipe_test_rm_1");
    sem_unlink("pipe_test_rm_2");
	/*
		int sem_unlink(const char *name)
		系统中删除信号灯 成功返回0，失败返回-1
		有名信号灯使用sem_unlink从系统中删除
		每个信号灯有一个引用计数器记录当前的打开次数，sem_unlink必须等待这个数为0时才能把name所指的信号灯从文件系统中删除
		也就是要等待最后一个sem_close发生
	*/
    return 0;
}

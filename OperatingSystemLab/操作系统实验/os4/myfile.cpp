#include <stdlib.h>
#include <string.h>
#include "iostream"
#include "fstream"
#include "time.h"
using namespace std;

/***** 定义的常量 *****/
#define BLOCKSIZE       1024        // 磁盘块大小
#define SIZE            1024000     // 虚拟磁盘空间大小
#define END             65535       // FAT中的文件结束标志
#define FREE            0           // FAT中盘块空闲标志
#define MAXOPENFILE     10          // 最多同时打开文件个数

#define MAX_TEXT_SIZE  10000        //允许读入的最大文件大小

/***** 定义数据结构 *****/

// 文件控制块
// 用于记录文件的描述和控制信息，每个文件设置一个FCB，它也是文件的目录项的内容
typedef struct FCB {
    char filename[8];           // 文件名
    char exname[5];             // 文件扩展名
    unsigned char metadata;     // 文件属性字段 1表示数据文件，0表示目录文件
    unsigned short time;        // 文件创建时间
    unsigned short date;        // 文件创建日期
    unsigned short first;       // 文件起始盘块号
    unsigned long length;       // 文件长度（字节数）
    char free;                  // 表示目录项是否为空，若值为0，表示空，值为1，表示已分配
} fcb;
// 文件分配表
typedef struct FAT {
    unsigned short id;   //free代表未分配，否则代表某文件下一磁盘块的块号
} fat;
// 用户打开文件表
typedef struct USEROPEN {
    char filename[8];           // 文件名
    char exname[5];             // 文件扩展名
    unsigned char  metadata;    // 文件属性字段 1表示数据文件，0表示目录文件
    unsigned short time;        // 文件创建时间
    unsigned short date;        // 文件创建日期
    unsigned short first;       // 文件起始盘块号
    unsigned long  length;      // 文件长度（对数据文件是字节数，对目录文件可以是目录项个数）
    char free;                  // 表示目录项是否为空，若值为0，表示空，值为1，表示已分配

    int  dirno;                 // 相应打开文件的目录项在父目录文件中的盘块号
    int  diroff;                // 相应打开文件的目录项在父目录文件的dirno盘块中的目录项序号
    char dir[80];               // 相应打开文件所在的目录名，这样方便快速检查出指定文件是否已经打开
    int  filePtr;               // 读写指针在文件中的位置
    char fcbstate;              // 是否修改了文件的FCB的内容，如果修改了置为1，否则为0
    char topenfile;             // 表示该用户打开表项是否为空，若值为0，表示为空，否则表示已被某打开文件占据
} useropen;
// 引导块 BootBlock，存放虚拟磁盘的相关描述信息
typedef struct BootBlock {
    char magic_number[8];       // 文件系统的魔数
    char information[200];      // 描述信息
    unsigned short root;        // 根目录文件的起始盘块号
    unsigned char *startblock;  // 虚拟磁盘上数据区开始位置
} block0;

/***** 全局变量定义 *****/
unsigned char *v_addr0;             // 指向虚拟磁盘的起始地址
useropen openfilelist[MAXOPENFILE]; // 用户打开文件表数组
int currfd;                         // 记录当前目录的文件描述符fd
unsigned char *startp;              // 记录虚拟磁盘上数据区开始位置
char *FILENAME = "FileSys.txt";     // 文件系统名
unsigned char buffer[SIZE];         // 缓冲区

/***** 函数申明 *****/
int  main();
void startsys();                                     // 进入文件系统
void my_format();                                    // 磁盘格式化函数
void my_cd(char *dirname);                           // 用于更改当前目录
void my_mkdir(char *dirname);                        // 创建子目录
void my_rmdir(char *dirname);                        // 删除子目录
void my_ls();                                        // 显示目录中的内容
int  my_create(char *filename);                      // 创建文件
void my_rm(char *filename);                          // 删除文件
int  my_open(char *filename);                        // 打开文件
int  my_close(int fd);                               // 关闭文件
int  my_write(int fd);                               // 写文件
int  do_write(int fd,char *text,int len,char wstyle);// 实际写文件
int  my_read(int fd);                                // 读文件
int  do_read(int fd, int len, char *text);           // 实际读文件
void my_exitsys();                                   // 退出文件系统

unsigned short getFreeBLOCK();                       // 获取一个空闲的磁盘块
int  get_Free_Openfile();                            // 获取一个空闲的文件打开表项
int  find_father_dir(int fd);                        // 寻找一个打开文件的父目录打开文件
void show_help();                                    // 获取命令帮助
void error(char *command);                           // 输出错误信息

unsigned short int getFreeBLOCK(){
    fat* fat1 = (fat*)(v_addr0 + BLOCKSIZE);
    for(int i=0; i < (int)(SIZE/BLOCKSIZE); i++){
        if(fat1[i].id == FREE){
            return i;
        }
    }
    return END;
}
int  get_Free_Openfile(){
    for(int i=0; i<MAXOPENFILE; i++){
        if(openfilelist[i].topenfile == 0){
            openfilelist[i].topenfile = 1;
            return i;
        }
    }
    return -1;
}
int  find_father_dir(int fd){
    for(int i=0; i<MAXOPENFILE; i++){
        if(openfilelist[i].first == openfilelist[fd].dirno){
            return i;
        }
    }
    return -1;
}
void show_help(){
    printf("命令名\t\t命令参数\t\t命令功能\n\n");
    printf("cd\t\t目录名(路径名)\t\t切换当前目录到指定目录\n");
    printf("mkdir\t\t目录名\t\t\t在当前目录创建新目录\n");
    printf("rmdir\t\t目录名\t\t\t在当前目录删除指定目录\n");
    printf("ls\t\t无\t\t\t显示当前目录下的目录和文件\n");
    printf("create\t\t文件名\t\t\t在当前目录下创建指定文件\n");
    printf("rm\t\t文件名\t\t\t在当前目录下删除指定文件\n");
    printf("open\t\t文件名\t\t\t在当前目录下打开指定文件\n");
    printf("write\t\t无\t\t\t在打开文件状态下，写该文件\n");
    printf("read\t\t读入长度\t\t在打开文件状态下，读取该文件\n");
    printf("close\t\t无\t\t\t在打开文件状态下，关闭该文件\n");
    printf("exit\t\t无\t\t\t退出系统\n\n");
}
void error(char *command){
    printf("%s : 缺少参数\n", command);
    printf("输入 'help' 来查看命令提示.\n");
}


void startsys(){
    v_addr0 = (unsigned char *)malloc(SIZE);
    printf("开始读取文件...");
    FILE *file;
    if((file = fopen(FILENAME, "r")) != NULL){
        fread(buffer, SIZE, 1, file);   //将二进制文件读取到缓冲区
        fclose(file);
        if(memcmp(buffer,"10101010",8) == 0){
            memcpy(v_addr0,buffer,SIZE);
            cout << "myfsys文件读取成功!" <<endl;
        }
            //有文件但是开头不是文件魔数
        else{
            cout << "myfsys文件系统不存在，现在开始创建文件系统" <<endl;
            my_format();
            memcpy(buffer,v_addr0,SIZE);
        }
    }
    else{
        cout << "myfsys文件系统不存在，现在开始创建文件系统" <<endl;
        my_format();
        memcpy(buffer,v_addr0,SIZE);
    }


    //初始化用户打开文件表，将表项0分配给根目录文件使用，并填写根目录文件的相关信息，由于根目录没有上级目录，所以表项中的dirno和diroff分别置为5（根目录所在起始块号）和0；并将ptrcurdir指针指向该用户打开文件表项。
    //将当前目录设置为根目录。
    fcb *root;
    root = (fcb *)(v_addr0 + 5 * BLOCKSIZE);
    strcpy(openfilelist[0].filename, root->filename);
    strcpy(openfilelist[0].exname, root->exname);
    openfilelist[0].metadata = root->metadata;
    openfilelist[0].time = root->time;
    openfilelist[0].date = root->date;
    openfilelist[0].first = root->first;
    openfilelist[0].length = root->length;
    openfilelist[0].free = root->free;


    openfilelist[0].dirno = 5;
    openfilelist[0].diroff = 0;
    strcpy(openfilelist[0].dir, "\\root\\");
    openfilelist[0].filePtr = 0;
    openfilelist[0].fcbstate = 0;
    openfilelist[0].topenfile = 1;

    //初始化全局变量
    //startp指向数据区的开头
    startp = ((block0*)v_addr0)->startblock;
    currfd = 0;
    return ;
}
void my_format(){
    //引导块信息
    block0 *boot = (block0 *)v_addr0;
    strcpy(boot->magic_number,"10101010");
    strcpy(boot->information,"文件系统,外存分配方式:FAT,磁盘空间管理:结合于FAT的位示图,目录结构:单用户多级目录结构.");
    boot->root = 5;
    boot->startblock = v_addr0 + BLOCKSIZE*5;

    //设置两个FAT表信息
    fat* fat1 = (fat*)(v_addr0 + BLOCKSIZE);
    for(int i=0; i<5; i++){  //前5个设置为已分配 后995个设置为free
        fat1[i].id = END;
    }
    for(int i=5; i<1000; i++){
        fat1[i].id = FREE;
    }
    fat* fat2 = (fat*)(v_addr0 + BLOCKSIZE*3);
    memcpy(fat2,fat1,BLOCKSIZE);

    //5号盘块被根目录区占用了
    fat1[5].id = fat2[5].id = END;

    //根目录区的fcb,特殊目录项., 指向自己
    fcb *root = (fcb*)(v_addr0 + BLOCKSIZE*5);
    strcpy(root->filename,".");
    strcpy(root->exname,"di");
    root->metadata = 0;

    time_t rawTime = time(NULL);
    struct tm *time = localtime(&rawTime);
    //root->time 是unsigned short int类型的数据, 32位编译器下长16位,64位编译器下长32位
    //这里考虑32位编译器, 所以完整的表示一个时间的秒数是不够长的,所以,保存秒数的一半
    //这样小时占5位,分钟占6位,秒占5位
    root->time = time->tm_hour * 2048 + time->tm_min*32 + time->tm_sec/2;

    root->date = (time->tm_year-100)*512 + (time->tm_mon+1)*32 + (time->tm_mday);
    root->first = 5;
    root->free = 1;
    root->length = 2 * sizeof(fcb);

    //root2 指向根目录区的第二个fcb,即特殊目录项..,因为根目录区没有上级目录,所以指向自己
    fcb* root2 = root + 1;
    memcpy(root2,root,sizeof(fcb));
    strcpy(root2->filename,"..");

    for(int i=2; i < int(BLOCKSIZE / sizeof(fcb)); i++){
        root2 ++;
        strcpy(root2->filename,"");
        root2->free = 0;
    }

    //写到文件里面去
    FILE *fp = fopen(FILENAME, "w");
    fwrite(v_addr0, SIZE, 1, fp);
    fclose(fp);
}
void my_cd(char *dirname){
    if(openfilelist[currfd].metadata == 1){
        cout << "数据文件里不能使用cd, 要是退出文件, 请用close指令" <<endl;
        return ;
    }
    //如果是目录文件
    else{
        //寻找目录文件里面有没有匹配的名字, 先把目录文件的信息读取到buf里
        char *buf = (char *)malloc(MAX_TEXT_SIZE);
        openfilelist[currfd].filePtr = 0;
        do_read(currfd,openfilelist[currfd].length,buf);
        int i = 0;
        fcb* fcbPtr = (fcb*)buf;
        for(; i < int(BLOCKSIZE / sizeof(fcb)); i++,fcbPtr++){
            if(strcmp(fcbPtr->filename, dirname) == 0 && fcbPtr->metadata == 0){
                break;
            }
        }
        //不允许cd非文件
        if(strcmp(fcbPtr->exname, "di") != 0){
            cout << "不允许cd非目录文件!" <<endl;
            return;
        }
        //如果cd 了一个目录文件, 那么判断是.还是..还是子文件,如果是子文件则打开这个目录文件到openfilelist里
        else{
            //cd .不会有反应
            if(strcmp(fcbPtr->filename,".") == 0){
                return;
            }
            //cd ..需要判断现在是不是根目录, 如果是根目录,不操作, 否则,返回上一层
            else if(strcmp(fcbPtr->filename, "..") == 0){
                if(currfd == 0){
                    return;
                }
                else{
                    currfd = my_close(currfd);
                    return;
                }
            }
            //cd 子文件
            else{
                int fd = get_Free_Openfile();
                if(fd == -1){
                    return;
                }
                else{
                    openfilelist[fd].metadata = fcbPtr->metadata;
                    openfilelist[fd].filePtr = 0;
                    openfilelist[fd].date = fcbPtr->date;
                    openfilelist[fd].time = fcbPtr->time;
                    strcpy(openfilelist[fd].filename, fcbPtr->filename);
                    strcpy(openfilelist[fd].exname,fcbPtr->exname);
                    openfilelist[fd].first = fcbPtr->first;
                    openfilelist[fd].free = fcbPtr->free;
                    //前面是FCB内容
                    openfilelist[fd].fcbstate = 0;
                    openfilelist[fd].length = fcbPtr->length;
                    // 修改 openfilelist[fd].dir[fd] = openfilelist[currfd].dir[currfd] + dirname;
                    strcpy(openfilelist[fd].dir,
                   (char*)(string(openfilelist[currfd].dir) + string(dirname) + string("\\")).c_str());
                    openfilelist[fd].topenfile = 1;
                    openfilelist[fd].dirno = openfilelist[currfd].first;
                    openfilelist[fd].diroff = i;
                    currfd = fd;
                }
            }
        }

    }

}
void my_mkdir(char *dirname){
    //判断dirname是否合法
    char* fname = strtok(dirname,".");
    char* exname = strtok(NULL,".");
    if(exname){
        cout << "不允许输入后缀名!" << endl;
        return ;
    }
    char text[MAX_TEXT_SIZE];
    openfilelist[currfd].filePtr = 0;
    int fileLen = do_read(currfd, openfilelist[currfd].length, text);
    //text里的内容就是一个个fcb
    fcb *fcbPtr = (fcb*)text;
    for(int i=0; i < (int)(fileLen/sizeof(fcb)); i++){
        if(strcmp(dirname,fcbPtr[i].filename) == 0 && fcbPtr[i].metadata == 0){
            cout << "目录名已经存在!"<<endl;
            return;
        }
    }
    //在打开文件表里找一个空文件表项
    int fd = get_Free_Openfile();
    if(fd == -1){
        cout << "打开文件表已全部被占用" << endl;
        return;
    }
    //在FAT里找一个空盘块
    unsigned short int blockNum = getFreeBLOCK();
    if(blockNum == END){
        cout << "盘块已经用完" << endl;
        openfilelist[fd].topenfile = 0;
        return ;
    }
    fat *fat1 = (fat *)(v_addr0 + BLOCKSIZE);
    fat *fat2 = (fat *)(v_addr0 + BLOCKSIZE*3);
    fat1[blockNum].id = END;
    fat2[blockNum].id = END;
    //在当前目录里面添加一个我们要的目录项
    int i = 0;
    for(; i < (int)(fileLen/sizeof(fcb)); i++){
        if(fcbPtr[i].free == 0){
            break;
        }
    }
    openfilelist[currfd].filePtr = i * sizeof(fcb);
    //修改了fcb,fcbstate置1
    openfilelist[currfd].fcbstate = 1;
    //修改新建的目录项,即fcb内容
   
    fcb* fcbtmp = new fcb;
    fcbtmp->metadata = 0;
    time_t rawtime = time(NULL);
    struct tm* time = localtime(&rawtime);
    fcbtmp->date = (time->tm_year-100)*512 + (time->tm_mon+1)*32 + (time->tm_mday);
    fcbtmp->time = (time->tm_hour)*2048 + (time->tm_min)*32 + (time->tm_sec) / 2;
    strcpy(fcbtmp->filename , dirname);
    strcpy(fcbtmp->exname, "di");
    fcbtmp->first = blockNum;
    fcbtmp->length = 2 * sizeof(fcb);
    fcbtmp->free = 1;
    //用do_write把fcbtmp写入到目录文件里
    do_write(currfd,(char *)fcbtmp,sizeof(fcb),1);

    //设置打开文件表项
    openfilelist[fd].metadata = 0;
    openfilelist[fd].filePtr = 0;
    openfilelist[fd].date = fcbtmp->date;
    openfilelist[fd].time = fcbtmp->time;
    openfilelist[fd].dirno = openfilelist[currfd].first;
    openfilelist[fd].diroff = i;
    strcpy(openfilelist[fd].exname,"di");
    strcpy(openfilelist[fd].filename,dirname);
    openfilelist[fd].fcbstate = 0;
    openfilelist[fd].first = fcbtmp->first;
    openfilelist[fd].free = fcbtmp->free;
    openfilelist[fd].length = fcbtmp->length;
    openfilelist[fd].topenfile = 1;
    // 修改 openfilelist[fd].dir[fd] = openfilelist[currfd].dir[currfd] + dirname;
    strcpy(openfilelist[fd].dir, (char*)(string(openfilelist[currfd].dir) + string(dirname) + string("\\")).c_str());

    //在对应的盘块里添加两个特殊的目录.和..
   
    fcbtmp->metadata = 0;
    fcbtmp->date = fcbtmp->date;
    fcbtmp->time = fcbtmp->time;
    strcpy(fcbtmp->filename, ".");
    strcpy(fcbtmp->exname, "di");
    fcbtmp->first = blockNum;
    fcbtmp->length = 2 * sizeof(fcb);
    do_write(fd,(char*)fcbtmp,sizeof(fcb),1);
    //复制..目录
    fcb *fcbtmp2 = new fcb;
    memcpy(fcbtmp2,fcbtmp,sizeof(fcb));
    strcpy(fcbtmp2->filename,"..");
    fcbtmp2->first = openfilelist[currfd].first;
    fcbtmp2->length = openfilelist[currfd].length;
    fcbtmp2->date = openfilelist[currfd].date;
    fcbtmp2->time = openfilelist[currfd].time;
    do_write(fd,(char*)fcbtmp2,sizeof(fcb),1);

    my_close(fd);
    //更新本currfd目录文件的fcb
    fcbPtr = (fcb *)text;
    fcbPtr->length =  openfilelist[currfd].length;
    openfilelist[currfd].filePtr = 0;
    do_write(currfd,(char*)fcbPtr,sizeof(fcb),1);
    openfilelist[currfd].fcbstate = 1;
    delete fcbtmp;
    delete fcbtmp2;
}
void my_rmdir(char *dirname){

    char* fname = strtok(dirname, ".");
    char* exname = strtok(NULL, ".");
    //不允许删除.和..这两个特殊目录文件
    if(strcmp(dirname,".") == 0 || strcmp(dirname,"..") == 0){
        cout << "不能删除" << dirname <<"这个特殊目录项" <<endl;
        return ;
    }
    if(exname){
        cout << "删除目录文件不用输入后缀名!" << endl;
        return;
    }
    //读取currfd对应的目录文件到buf
    char buf[MAX_TEXT_SIZE];
    openfilelist[currfd].filePtr = 0;
    do_read(currfd,openfilelist[currfd].length,buf);
    int i;
    fcb* fcbPtr = (fcb*)buf;
    //寻找叫这个名字的文件目录项
    for(i=0; i < int(BLOCKSIZE / sizeof(fcb)); i++, fcbPtr++){
        if(strcmp(fcbPtr->filename,fname) == 0  && fcbPtr->metadata == 0){
            break;
        }
    }
    if( i == int(BLOCKSIZE / sizeof(fcb))){
        cout << "没有这个目录文件" <<endl;
        return;
    }
    //判断这个目录文件里,清空了没有,不允许删除没有清空的目录
    if(fcbPtr->length > 2 * sizeof(fcb)){
        cout << "请先清空这个目录下的所有文件,再删除目录文件" << endl;
        return;
    }
    //清空这个目录项占据的FAT
    int blockNum = fcbPtr->first;
    fat* fat1 = (fat*)(v_addr0 + BLOCKSIZE);
    int next = 0;
    while(1){
        next = fat1[blockNum].id;
        fat1[blockNum].id = FREE;
        if(next != END){
            blockNum = next;
        }
        else{
            break;
        }
    }
    //备份fat2
    fat1 = (fat*)(v_addr0 + BLOCKSIZE);
    fat* fat2 = (fat*)(v_addr0 + BLOCKSIZE*3);
    memcpy(fat2, fat1, sizeof(fat));
    //修改这个fcb为空
    fcbPtr->date = 0;
    fcbPtr->time = 0;
    fcbPtr->exname[0] = '\0';
    fcbPtr->filename[0] = '\0';
    fcbPtr->first = 0;
    fcbPtr->free = 0;
    fcbPtr->length = 0;
    //写到磁盘上去, 更新fcb内容为空
    openfilelist[currfd].filePtr = i * sizeof(fcb);
    do_write(currfd,(char*)fcbPtr,sizeof(fcb),1);
    openfilelist[currfd].length -= sizeof(fcb);
    //更新.目录项的长度
    fcbPtr = (fcb*)buf;
    fcbPtr->length = openfilelist[currfd].length;
    openfilelist[currfd].filePtr = 0;
    do_write(currfd,(char*)fcbPtr,sizeof(fcb),1);
    openfilelist[currfd].fcbstate = 1;
}
void my_ls(){

    if(openfilelist[currfd].metadata == 1){
        cout << "在数据文件里不能使用ls" << endl;
        return;
    }
    char buf[MAX_TEXT_SIZE];

    openfilelist[currfd].filePtr = 0;
    do_read(currfd, BLOCKSIZE, buf);

    fcb* fcbPtr = (fcb*)buf;
    printf("name\tsize \ttype\t\tdate\t\ttime\n");
    for(int i=0; i < (int)(BLOCKSIZE / sizeof(fcb)); i++){
        if(fcbPtr->free == 1){
            //目录文件
            //同理,年份占7位,月份占4位,日期占5位
            //小时占5位,分钟占6位,秒占5位	
            if(fcbPtr->metadata == 0){
                printf("%s\t%dB\t<DIR>\t%d/%d/%d\t%02d:%02d:%02d\n",
                       fcbPtr->filename,fcbPtr->length,
                       (fcbPtr->date>>9)+2000,
                       (fcbPtr->date>>5)&0x000f,
                       (fcbPtr->date)&0x001f,
                       (fcbPtr->time>>11),
                       (fcbPtr->time>>5)&0x003f,
                       (fcbPtr->time)&0x001f * 2);
            }
            else{
                // 普通文件length - 2 是因为末尾有/n和/0两个字符
                unsigned int length = fcbPtr->length;
                if(length != 0)length -= 2;

                printf("%s.%s\t%dB\t<File>\t%d/%d/%d\t%02d:%02d:%02d\n",
                       fcbPtr->filename,
                       fcbPtr->exname,
                       length,
                       (fcbPtr->date>>9)+2000,
                       (fcbPtr->date>>5)&0x000f,
                       (fcbPtr->date)&0x001f,
                       (fcbPtr->time>>11),
                       (fcbPtr->time>>5)&0x003f,
                       (fcbPtr->time)&0x001f * 2);
            }
        }
        fcbPtr++;
    }
}
int  my_create(char *filename){

    char* fname = strtok(filename,".");
    char* exname = strtok(NULL,".");
 
    if(strcmp(fname,"") == 0){
        cout << "请输入文件名!" << endl;
        return -1;
    }
    if(!exname){
        cout << "请输入后缀名!" << endl;
        return -1;
    }
    if(openfilelist[currfd].metadata == 1){
        cout << "数据文件下不允许使用create" << endl;
        return -1;
    }
    //读取currfd对应的文件
    openfilelist[currfd].filePtr = 0;
    char buf[MAX_TEXT_SIZE];
    do_read(currfd, openfilelist[currfd].length, buf);
    int i;
    fcb* fcbPtr = (fcb*)(buf);
    //看看有没有重名文件
    for(i=0; i < int(BLOCKSIZE / sizeof(fcb)); i++,fcbPtr++){
        if(strcmp(fcbPtr->filename,filename)==0 && strcmp(fcbPtr->exname,exname)==0){
            cout << "已有同名文件存在!" << endl;
            return -1;
        }
    }
    //寻找空的fcb块
    fcbPtr = (fcb*)(buf);
    fcb* debug = (fcb*)(buf);
    for(i=0; i < int(BLOCKSIZE / sizeof(fcb)); i++,fcbPtr++){
        if(fcbPtr->free == 0)break;
    }

    //取一个盘块
    int blockNum = getFreeBLOCK();
    if(blockNum == -1){
        return -1;
    }
    fat* fat1 = (fat*)(v_addr0 + BLOCKSIZE);
    fat1[blockNum].id = END;
    fat* fat2 = (fat*)(v_addr0 + BLOCKSIZE * 3);
    fat2[blockNum].id = END;
    //往fcb里写信息
    strcpy(fcbPtr->filename,filename);
    strcpy(fcbPtr->exname,exname);
   
    time_t rawtime = time(NULL);
    struct tm* time = localtime(&rawtime);
    fcbPtr->date = (time->tm_year-100)*512 + (time->tm_mon+1)*32 + (time->tm_mday);
    fcbPtr->time = (time->tm_hour)*2048 + (time->tm_min)*32 + (time->tm_sec) / 2;
    fcbPtr->first = blockNum;
    fcbPtr->free = 1;
    fcbPtr->length = 0;
    fcbPtr->metadata = 1;
    openfilelist[currfd].filePtr = i * sizeof(fcb);
    do_write(currfd,(char *)fcbPtr,sizeof(fcb),1);
    //修改当前目录文件的.目录项的长度
    fcbPtr = (fcb*)buf;
    fcbPtr->length = openfilelist[currfd].length;
    openfilelist[currfd].filePtr = 0;
    do_write(currfd,(char*)fcbPtr,sizeof(fcb),1);
    openfilelist[currfd].fcbstate = 1;
}
void my_rm(char *filename){

    char* fname = strtok(filename, ".");
    char* exname = strtok(NULL, ".");
    if(!exname){
        cout << "请输入后缀名!" << endl;
        return;
    }
    if(strcmp(exname,"di") == 0){
        cout << "不能删除目录项" << endl;
        return ;
    }
    //读取currfd对应的目录文件到buf
    char buf[MAX_TEXT_SIZE];
    openfilelist[currfd].filePtr = 0;
    do_read(currfd,openfilelist[currfd].length,buf);
    int i;
    fcb* fcbPtr = (fcb*)buf;
    //寻找叫这个名字的文件目录项
    for(i=0; i < int(BLOCKSIZE / sizeof(fcb)); i++, fcbPtr++){
        if(strcmp(fcbPtr->filename,fname) == 0  && strcmp(fcbPtr->exname,exname) == 0){
            break;
        }
    }
    if( i == int(openfilelist[currfd].length / sizeof(fcb))){
        cout << "没有这个文件" <<endl;
        return;
    }
    //清空这个目录项占据的FAT
    int blockNum = fcbPtr->first;
    fat* fat1 = (fat*)(v_addr0 + BLOCKSIZE);
    int next = 0;
    while(1){
        next = fat1[blockNum].id;
        fat1[blockNum].id = FREE;
        if(next != END){
            blockNum = next;
        }
        else{
            break;
        }
    }
    //备份fat2
    fat1 = (fat*)(v_addr0 + BLOCKSIZE);
    fat* fat2 = (fat*)(v_addr0 + BLOCKSIZE*3);
    memcpy(fat2, fat1, sizeof(fat));
    //修改这个fcb为空
    fcbPtr->date = 0;
    fcbPtr->time = 0;
    fcbPtr->exname[0] = '\0';
    fcbPtr->filename[0] = '\0';
    fcbPtr->first = 0;
    fcbPtr->free = 0;
    fcbPtr->length = 0;
    //写到磁盘上去, 更新fcb内容为空
    openfilelist[currfd].filePtr = i * sizeof(fcb);
    do_write(currfd,(char*)fcbPtr,sizeof(fcb),1);
    openfilelist[currfd].length -= sizeof(fcb);
    //更新.目录项的长度
    fcbPtr = (fcb*)buf;
    fcbPtr->length = openfilelist[currfd].length;
    openfilelist[currfd].filePtr = 0;
    do_write(currfd,(char*)fcbPtr,sizeof(fcb),1);
    openfilelist[currfd].fcbstate = 1;
}
int  my_open(char *filename){
    //把当前目录文件读取到buf里
    char buf[MAX_TEXT_SIZE];
    openfilelist[currfd].filePtr = 0;
    do_read(currfd, openfilelist[currfd].length, buf);
    char *fname = strtok(filename,".");
    char *exname = strtok(NULL, ".");

    if(!exname){
        cout << "请输入后缀名" << endl;
        return -1 ;
    }
    int i;
    fcb* fcbPtr = (fcb*)buf;
    //寻找此文件
    for(i=0; i < int(BLOCKSIZE / sizeof(fcb)); i++, fcbPtr++){
    	
        if(strcmp(fcbPtr->filename,fname) == 0 && strcmp(fcbPtr->exname,exname) == 0 && fcbPtr->metadata == 1){
            break;
        }
    }
    if(i == int(BLOCKSIZE / sizeof(fcb))){
        cout << "不存在此文件!" << endl;
        return -1;
    }
    //为它创建一个打开文件表项
    int fd = get_Free_Openfile();
    if(fd == -1){
        cout << "用户打开文件表已经用满" <<endl;
        return -1;
    }
    
    openfilelist[fd].metadata = 1;
  
    openfilelist[fd].filePtr = 0;
   
    openfilelist[fd].date = fcbPtr->date;
 
    openfilelist[fd].time = fcbPtr->time;
   
    strcpy(openfilelist[fd].exname, exname);

    strcpy(openfilelist[fd].filename,fname);

    openfilelist[fd].length = fcbPtr->length;
    
    openfilelist[fd].first = fcbPtr->first;
   
    strcpy(openfilelist[fd].dir,(string(openfilelist[currfd].dir) + string(filename)).c_str());
   
    openfilelist[fd].dirno = openfilelist[currfd].first;

    openfilelist[fd].diroff = i;

    openfilelist[fd].free = 1;

    openfilelist[fd].topenfile = 1;

    openfilelist[fd].fcbstate = 0;

    
  
    currfd = fd;
    return 1;
}
int  my_close(int fd){
 

    if(fd > MAXOPENFILE || fd < 0){
        cout << "不存在这个打开文件" << endl;
        return-1;
    }
    else{
        //判断父目录文件是否存在, 不存在报错
        int fatherFd = find_father_dir(fd);
        if(fatherFd == -1){
            cout << "父目录不存在!" <<endl;
            return -1;
        }
        //fcb被修改了, 要写回去
        
        if(openfilelist[fd].fcbstate == 1){
            char buf[MAX_TEXT_SIZE];
            do_read(fatherFd,openfilelist[fatherFd].length, buf);
            //更新fcb内容
            fcb* fcbPtr = (fcb *)(buf + sizeof(fcb) * openfilelist[fd].diroff);
            strcpy(fcbPtr->exname, openfilelist[fd].exname);
            strcpy(fcbPtr->filename, openfilelist[fd].filename); 
            fcbPtr->first = openfilelist[fd].first;
            fcbPtr->free = openfilelist[fd].free;
            fcbPtr->length = openfilelist[fd].length;
            openfilelist[fatherFd].filePtr = 0;
            fcbPtr->time = openfilelist[fd].time;
            fcbPtr->date = openfilelist[fd].date;
            fcbPtr->metadata = openfilelist[fd].metadata;
            openfilelist[fatherFd].filePtr = openfilelist[fd].diroff * sizeof(fcb);
            do_write(fatherFd, (char*)fcbPtr, sizeof(fcb), 1);
        }
        memset(&openfilelist[fd], 0, sizeof(useropen));
        currfd = fatherFd;
        return fatherFd;
    }

}
int  my_write(int fd){
    if(fd < 0 || fd >= MAXOPENFILE){
        cout << "文件不存在" <<endl;
        return -1;
    }
    int wstyle;
    while(1){
        cout << "输入: 0=截断写, 1=覆盖写, 2=追加写" <<endl;
        cin >> wstyle;
        if(wstyle > 2 || wstyle < 0){
            cout << "指令错误!" << endl;
        }
        else{
            break;
        }
    }
    char text[MAX_TEXT_SIZE] = "\0";
    char textTmp[MAX_TEXT_SIZE] = "\0";
    char Tmp[MAX_TEXT_SIZE]  = "\0"; 
    char Tmp2[4]             = "\0"; 

    cout << "请输入文件数据, 以END为文件结尾" <<endl;
    getchar();
    while(fgets(Tmp,100,stdin)){
    	for(int i=0;i<strlen(Tmp)-1;i++)
        {
              textTmp[i]   = Tmp[i];
              textTmp[i+1] = '\0';
        }
        if(strlen(Tmp) >= 3)
        {
            Tmp2[0] = Tmp[strlen(Tmp)-4];
            Tmp2[1] = Tmp[strlen(Tmp)-3];
            Tmp2[2] = Tmp[strlen(Tmp)-2];
            Tmp2[3] = '\0';
        }
    	if(strcmp(textTmp,"END")==0 || strcmp(Tmp2,"END")==0)
        {
        	break;
        }
        textTmp[strlen(textTmp)] = '\n';
        strcat(text,textTmp);

    }
    text[strlen(text)] = '\0';
    //+1是因为要把结尾的\0也写进去
    do_write(fd,text,strlen(text)+1,wstyle);
    openfilelist[fd].fcbstate = 1;
    return 1;
}
int  do_write(int fd, char *text, int len, char wstyle){

    //盘块号
    int blockNum = openfilelist[fd].first;
    fat *fatPtr = (fat*)(v_addr0 + BLOCKSIZE) + blockNum ;
    //三种写入方式预处理
    // 0截断写,直接从头开始写,偏移量就是0了,而且长度变成0
    fat* fat1 = (fat*)(v_addr0 + BLOCKSIZE);
    if(wstyle == 0){
        openfilelist[fd].filePtr = 0;
        openfilelist[fd].length = 0;
    }
    //1,覆盖写, 如果是数据文件,那么要考虑删除文件末尾的\0才能继续往下写
    else if(wstyle == 1){
        if(openfilelist[fd].metadata == 1 && openfilelist[fd].length != 0){
            openfilelist[fd].filePtr -= 1;
        }
    }
    //2追加写,就把游标指向末尾
    else if(wstyle == 2){
        if(openfilelist[fd].metadata == 0){
            openfilelist[fd].filePtr = openfilelist[fd].length;
        }
        //同理,如果是数据文件要删除末尾的\0
        else if(openfilelist[fd].metadata == 1 && openfilelist[fd].length != 0){
            openfilelist[fd].filePtr = openfilelist[fd].length - 1;
        }
    }


    int off = openfilelist[fd].filePtr;									

    //如果off > BLOCKSIZE,也就是游标现在指向的不是文件中的第一个盘块,那么需要找到那个盘块
    //另外,如果游标很大,但是在寻找对应盘块的时候发现没有那个盘块,那么把缺少的盘块全都补上
    while(off >= BLOCKSIZE){
        blockNum = fatPtr->id;
        if(blockNum == END){
            blockNum = getFreeBLOCK();
            if(blockNum == END){
                cout << "盘块不足"<<endl;
                return -1;
            }
            else{
                //update FAT
                fatPtr->id = blockNum;
                fatPtr = (fat*)(v_addr0 + BLOCKSIZE + blockNum);
                fatPtr->id = END;
            }
        }
        fatPtr = (fat*)(v_addr0 + BLOCKSIZE) + blockNum;
        off -= BLOCKSIZE;
    }

    unsigned char *buf = (unsigned char*)malloc(BLOCKSIZE * sizeof(unsigned char));
    if(buf == NULL){
        cout << "申请内存空间失败!";
        return -1;
    }


    fcb *dBlock = (fcb *)(v_addr0 + BLOCKSIZE * blockNum);
    fcb *dFcb = (fcb *)(text);
    unsigned char *blockPtr = (unsigned char *)(v_addr0 + BLOCKSIZE * blockNum);	//盘块指针
    int lenTmp = 0;
    char *textPtr = text;
    fcb *dFcbBuf = (fcb *)(buf);
    //第二个循环,读取盘块内容到buf, 把text内容写入buf, 然后再从buf写入到盘块
    while(len > lenTmp){
        //盘块内容读取到buf里
        memcpy(buf,blockPtr,BLOCKSIZE);
        //把text内容写到buf里面去
        for (; off < BLOCKSIZE; off++){
            *(buf + off) = *textPtr;
            textPtr ++;
            lenTmp++;
            if(len == lenTmp){
                break;
            }
        }
        //把buf内容写到盘块里面去
        memcpy(blockPtr, buf, BLOCKSIZE);
        //如果off==BLCOKSIZE,意味着buf写满了, 如果len != lebTmp 意味着数据还没写完, 那么就要看看这个文件还有没有剩余盘块
        //没有剩余盘块,那就要分配新的盘块了
        if(off == BLOCKSIZE && len != lenTmp){
            off = 0;
            blockNum = fatPtr->id;
            if(blockNum == END){
                blockNum = getFreeBLOCK();
                if(blockNum == END){
                    cout << "盘块用完了" <<endl;
                    return -1;
                }
                else{
                    blockPtr = (unsigned char *)(v_addr0 + BLOCKSIZE * blockNum);
                    fatPtr->id = blockNum;
                    fatPtr = (fat *)(v_addr0 + BLOCKSIZE) + blockNum;
                    fatPtr->id = END;
                }
            }
            else{
                blockPtr = (unsigned char *)(v_addr0 + BLOCKSIZE * blockNum);
                fatPtr = (fat *)(v_addr0 + BLOCKSIZE ) + blockNum;
            }
        }
    }
    openfilelist[fd].filePtr += len;
    //若读写指针大于原来文件的长度，则修改文件的长度
    if(openfilelist[fd].filePtr > openfilelist[fd].length)
        openfilelist[fd].length = openfilelist[fd].filePtr;
    free(buf);
    //如果原来文件占几个盘块,现在修改了文件,结果占用的盘块变少了,那就要把后面占用的盘块全部释放掉
    int i = blockNum;
    while (1){
        //如果这个fat的下一个fat不是end,那么就是释放掉它,一路释放下去
        if(fat1[i].id != END){
            int next = fat1[i].id;
            fat1[i].id = FREE;
            i = next;
        }
        else{
            break;
        }
    }
    //按照上面这种操作,会把本文件的最后一个盘块也变成free,这里要把他重新设置成END
    fat1[blockNum].id = END;
    //update fat2
    memcpy((fat*)(v_addr0 + BLOCKSIZE * 3), (fat*)(v_addr0 + BLOCKSIZE), 2*BLOCKSIZE);
    return len;

}
int  do_read(int fd, int len, char *text){

    int lenTmp = len;

    unsigned char* buf = (unsigned char*)malloc(1024);
    if(buf == NULL){
        cout << "do_read申请内存空间失败" << endl;
        return -1;
    }

    int off = openfilelist[fd].filePtr;
    //当前fd对应的起始盘块号, 后面变成当前盘块号
    int blockNum = openfilelist[fd].first;
    //fatPtr 当前盘块对应的fat
    fat* fatPtr = (fat *)(v_addr0+BLOCKSIZE) + blockNum;
    while(off >= BLOCKSIZE){
        off -= BLOCKSIZE;
        blockNum = fatPtr->id;
        if(blockNum == END){
            cout <<"do_read寻找的块不存在" <<endl;
            return -1;
        }
        fatPtr = (fat*)(v_addr0 + BLOCKSIZE) + blockNum;
    }
    //当前盘块号对应的盘块
    unsigned char* blockPtr = v_addr0 + BLOCKSIZE*blockNum;
    //把文件内容读进buf
    memcpy(buf, blockPtr, BLOCKSIZE);
    char *textPtr = text;
    fcb* debug = (fcb*)text;
    while(len > 0 ){
        //一个盘块就能放的下
        if( BLOCKSIZE - off > len){
            memcpy(textPtr,buf + off, len);
            textPtr += len;
            off += len;
            openfilelist[fd].filePtr += len;
            len = 0;
        }
        else{
            memcpy(textPtr, buf + off, BLOCKSIZE - off);
            textPtr += BLOCKSIZE - off;
            off = 0;
            len -= BLOCKSIZE - off;
            //寻找下一个块
            blockNum = fatPtr->id;
            if(blockNum == END){
                break;
            }
            fatPtr = (fat*)(v_addr0 + BLOCKSIZE ) + blockNum;
            blockPtr = v_addr0 + BLOCKSIZE * blockNum;
            memcpy(buf,blockPtr,BLOCKSIZE);
        }
    }
    free(buf);
    return lenTmp - len;
}
int  my_read(int fd, int len){
    if(fd >= MAXOPENFILE || fd < 0){
        cout << "文件不存在" <<endl;
        return -1;
    }
    openfilelist[fd].filePtr = 0;
    char text[MAX_TEXT_SIZE] = "\0";

    if(len > openfilelist[fd].length)
    {
        printf("读取长度已经超过文件大小，默认读到文件末尾.\n");
        len = openfilelist[fd].length;
    }
    do_read(fd,len,text);
    cout << text<<endl ;
    return 1;
}
void my_exitsys(){

    while(currfd){
        my_close(currfd);
    }
    FILE *fp = fopen(FILENAME, "w");
    fwrite(v_addr0, SIZE, 1, fp);
    fclose(fp);
}


int  main(){
    char cmd[15][10] = {"mkdir", "rmdir", "ls", "cd", "create", "rm", "open", "close", "write", "read", "exit", "help"};
    char temp[30],command[30], *sp, *len, yesorno;
    int indexOfCmd, i;
	int length = 0;

    printf("\n\n************************ 文件系统 **************************************************\n");
    printf("************************************************************************************\n\n");
    startsys();
    printf("文件系统已开启.\n\n");
    printf("输入help来显示帮助页面.\n\n");


    while(1){
        printf("%s>", openfilelist[currfd].dir);
        fgets(temp,100,stdin);
        indexOfCmd = -1;

        for(int i=0;i<strlen(temp)-1;i++)
        {
             command[i]   = temp[i];     //清除fgets读入的\n
             command[i+1] = '\0';
        }



        if (strcmp(command, "")){       // 不是空命令
            sp = strtok(command, " ");  // 把空格前的命令分离出来
            //printf("%s\n",sp);
            for (i = 0; i < 15; i++){
                if (strcmp(sp, cmd[i]) == 0){
                    indexOfCmd = i;
                    break;
                }
            }
            switch(indexOfCmd){
                case 0:         // mkdir
                    sp = strtok(NULL, " ");
                    //printf("%s\n",sp);
                    if (sp != NULL)
                        my_mkdir(sp);
                    else
                        error("mkdir");
                    break;
                case 1:         // rmdir
                    sp = strtok(NULL, " ");
                    if (sp != NULL)
                        my_rmdir(sp);
                    else
                        error("rmdir");
                    break;
                case 2:         // ls
                    my_ls();
                    break;
                case 3:         // cd
                    sp = strtok(NULL, " ");
                    if (sp != NULL)
                        my_cd(sp);
                    else
                        error("cd");
                    break;
                case 4:         // create
                    sp = strtok(NULL, " ");
                    if (sp != NULL)
                        my_create(sp);
                    else
                        error("create");
                    break;
                case 5:         // rm
                    sp = strtok(NULL, " ");
                    if (sp != NULL)
                        my_rm(sp);
                    else
                        error("rm");
                    break;
                case 6:         // open
                    sp = strtok(NULL, " ");
                    if (sp != NULL)
                        my_open(sp);
                    else
                        error("open");
                    break;
                case 7:         // close
                    if (openfilelist[currfd].metadata == 1)
                        my_close(currfd);
                    else
                        cout << "当前没有的打开的文件" << endl;
                    break;
                case 8:         // write

                    if (openfilelist[currfd].metadata == 1)
                        my_write(currfd);
                    else
                        cout << "请先打开文件,然后再使用wirte操作" <<endl;
                    break;
                case 9:         // read
                    sp = strtok(NULL, " ");
                    length = 0;
                    if (sp != NULL)  
                    {
                         for(int i=0;i<strlen(sp);i++)
                              length = length*10+sp[i]-'0';
                    }
                    if (length == 0)
                           error("read");
                    else if (openfilelist[currfd].metadata == 1)
                           my_read(currfd,length);    
                    else
                           cout << "请先打开文件,然后再使用read操作" <<endl;
                    break;

                case 10:        // exit
                    my_exitsys();
                    printf("退出文件系统.\n");
                    return 0;
                    break;
                case 11:        // help
                    show_help();
                    break;
                default:
                    printf("没有 %s 这个命令\n", sp);
                    break;
            }
        }
        else
            printf("\n");
    }
    return 0;
}




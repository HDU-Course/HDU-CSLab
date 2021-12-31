/*参考文档中状态与集合的表示方法，用位的方式表示状态，数表示集合
int 型可以表示32个状态 
*/
#include <stdio.h>
#include <fstream> 
#include <set>
#include <queue>
using namespace std;
#define maxs 32    //最大状态数目
#define maxt 20		//符号的最大数目 
#define maxq 100	//最大的dfa集合数目 
struct NFAState{
	int i;	//编号（1的位->1<<i） 1-32
	set<int>nexts[maxt];	//不同符号导致的下一个状态可能不同 
}nst[maxs];

struct DFAState{
	int Q;		//不是编号，是集合
	int nextq[maxt];	//dfa的任意符号的下一状态集确定，且符号不含空 
	int isjs;		//是否为结束符 
}dst[maxq]; 
 
int numt,nums,send;	//字符数、nfa状态数、结束符号（约定结束符号只有1个、开始符号约定为1） 
char zfj[maxt];		//字符集 

set<int> indfa;		//表示状态集已经在dfa的状态集中了 
int closure[maxs] = {0};		//每一个nfa状态的空符闭包(用整数表示集合) 
set<int>::iterator it;

string sr;		//用于输入下一状态的字符串 
int numq = 0;		//dfa状态集数目 

ifstream fin;
ofstream fout;
void srfx(int s,int t){
	nst[s].nexts[t].clear();		//初始化set 
	if(sr.length()==1&&sr[0]=='0'){	//0	(0表示无下一状态)
		return;
	}	
	int n = 0;
	for(int i = 0;i < sr.length();i ++){	//可能有多个状态，因此进行分析 
		if(sr[i] == ','){		//输入一个状态成功 
			nst[s].nexts[t].insert(n);	//插入的是（2^n）
			n = 0;				//清零 
		}
		else{
			n *= 10;
			n += sr[i] - '0'; 
		}
	}
	//最后不会有多余的，因此自己再加上最后的n
	nst[s].nexts[t].insert(n);
} 
void init(){		//读入文件中的NFA 
	fin.open("in.txt");
	fin >> numt;
	numt += 1;		//加上空符 
	char zf;
	for(int i = 0;i < numt-1;i ++){
		fin >> zfj[i]; 
	}
	fin >> nums >> send;			//约定状态是从1开始的连续数字 
	for(int i = 1;i <= nums; i ++){
		nst[i].i = 1 << (i-1); 		//状态用位表示对应2^i 
		for(int j = 0;j < numt;j ++){
			sr = "";
			fin >> sr ;				//下一状态有多个时，用,分隔 
			srfx(i,j);				//读取所有的下一状态并记入 
		}
	}
	for(int i=0;i<maxq;i++){
		dst[i].Q = 0;dst[i].isjs = 0;
	} 
	indfa.clear();
	fin.close();	
}

void count_closure(int s)		//空符闭包计算，这里直接计算所有nfa状态的闭包，保存备用 
{
	queue<int> cl;
	closure[s] |= nst[s].i;		//加上自己 
	for( it= nst[s].nexts[0].begin();it!=nst[s].nexts[0].end();it++ ){	//0列是空集可达的状态 
		cl.push(*it);		//先入队，然后再进行闭包的计算 
	} 
	while(!cl.empty()){
		int nows = cl.front();
		cl.pop();
		if((closure[s]&(1<<(nows-1) )) == 0){	//未在c闭包中找到则加入 
			closure[s] |= 1<<(nows-1);
		//	printf("%d\n",closure[s]);
			for(it= nst[nows].nexts[0].begin();it!=nst[nows].nexts[0].end();it++)
				cl.push(*it);	 
		}	
	}
}
void todfa(int ds){			//ds指的是dfa中的集合状态 
	int newq = 0,xh=0;
	for(int t = 1;t < numt ; t++) //每一个符号 
	{	newq = 0;
		for(int i=1;i<=nums;i++){		//每一个状态所能到达的下一个状态集 
			if((dst[ds].Q & (1<<(i-1))) == 0) continue;	//为0表示不包含状态i，不需处理 
			for(it = nst[i].nexts[t].begin();it!=nst[i].nexts[t].end();it++){
				newq |= closure[*it];				
			} 
		}
		if(indfa.find(newq)==indfa.end()&&newq!=0)	//说明是新的状态集 
		{
			numq++;
			dst[numq].Q = newq;
			dst[ds].nextq[t-1] = numq; 
			indfa.insert(newq);
		}
		else if(newq!=0){	//不是新状态则直接加边
			xh = 0;
			for(int i=0;i<=numq;i++){
				if(dst[i].Q == newq){xh = i;break;}
			} 
			dst[ds].nextq[t-1] = xh;
		}
		else{	//newq == 0,无边 
			dst[ds].nextq[t-1] = -1;	
		} 	
	}
} 
void print(){
	fout.open("nfatodfa.txt");
		fout << numq+1<<"\n";			//dfa状态数 
		fout << numt-1<<"\n";		//dfa 字符数 
		printf("dfa状态\t");
		for(int i=0;i<numt-1;i++){
			printf("%c\t",zfj[i]);fout << zfj[i] <<" ";		//dfa字符 
		} printf("nfa状态集\n");fout<<"\n";
		for(int i=0;i<=numq;i++){
			printf(" %d \t",i+1);	
			for(int j=0;j<numt-1;j++){
				printf("%d\t",dst[i].nextq[j]+1); fout << dst[i].nextq[j]+1 << " ";	//转移 
			}printf("{");
			for(int j=1;j<=nums;j++){
				if((dst[i].Q&(1<<(j-1)))!=0) {
					printf("%d ",j);
					if( j == send)	//终结符
						dst[i].isjs = 1; 
				} 
			}
			printf("}");
			if(dst[i].isjs == 1){
				printf("*");fout<<"*";	//用*号表示含终结 
			}
			else fout<<"!";
			printf("\n"); fout << "\n";
		}
	fout.close();		
}

char sc[3];
void jcdfa(int s,int d,int limit){
	if( d>= limit){
		if(dst[s].isjs == 1) printf("%s\n",sc);
		return;
	}
	for(int i=0;i<numt-1;i++){
		if(dst[s].nextq[i] == -1) continue;		//无边则跳过 
		sc[d] = zfj[i];
		jcdfa(dst[s].nextq[i],d+1,limit);	
	}
}
void pathnfa(int s,int d,int limit){
	if( d >= limit){
		if((closure[s]&(1<<(send-1)))>0) {
			printf("%s\n",sc);
			return;
		}
		return;
	}
	for(int i=1;i<numt;i++){
		for(set<int>::iterator it2 = nst[s].nexts[i].begin();it2!=nst[s].nexts[i].end();it2++){
			sc[d] = zfj[i-1];
			pathnfa(*it2,d+1,limit);
		}
	}
	for(set<int>::iterator it2 = nst[s].nexts[0].begin();it2!=nst[s].nexts[0].end();it2++){
			pathnfa(*it2,d,limit);
	}
} 


int main(){
	init();
	for(int i=1;i<=nums;i++){
		count_closure(i);
	}
	dst[numq].Q = closure[1];		//初始化dfa第一个状态集（初始状态）
	indfa.insert(dst[numq].Q);
	int ds = 0;      
	                                                        
	while(ds<=numq){
		todfa(ds);
		ds++;
	}
	print();
	printf("\n根据nfa，该文法长度为3的字符串：\n");
	pathnfa(1,0,3); 
	printf("\n检查dfa正确性--该文法长度为3的字符串：\n");
	jcdfa(0,0,3);
} 

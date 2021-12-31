#include <stdio.h>
#include <fstream>
#include <set> 
using namespace std;
#define maxq 100	//最大的dfa集合数目 
#define maxt 20		//符号的最大数目 
struct DFAState{
	int nextq[maxt];	//dfa的任意符号的下一状态集确定，且符号不含空
	bool isjs; 
}dst[maxq]; 
char zfj[maxt];		//字符集 
int numq;		//状态数 
int numt;		//符号数 

struct MINState{
	set<int> q;			//最小化后的dfa集合（位表示状态，数表示集合） 
	int nextj[maxt];
	bool isjs;
}mins[maxq];
	 
int numj = 2;			//dfa集合数量，初始化为非接受集与接受集两种 
int firstS;
int nextS;		//对于每一个状态根据符号进入的第一个状态集
int beginT ;		//min后的开始集 
ifstream fin; 
ofstream fout;
set<int>::iterator it;

void mindfa(){
	int flag=1;
	int iswb = 0; 
	while(flag){		//划分过程 
	flag = 0; 
		for(int t=0;t<numt;t++){	
			for(int i=0; i<numj; i++){	
				iswb = 0;
				it =mins[i].q.begin(); 
				//if(it == mins[i].q.end()) continue;
				firstS = *it;	
				nextS = dst[firstS].nextq[t];	//nextS可能为0，因此需要进行考虑 
				if(nextS == 0) iswb = 1;
				for(int k=0;k<numj;k++){		//查询第一个状态对于符号t进入的状态所在的状态集 
					if(mins[k].q.find(nextS)!=mins[k].q.end()){
						nextS = k;				//nextS为第一个状态对符号t到达的下一符号集 
						break;
					}
				}
				it = mins[i].q.begin();it++;
				for(;it != mins[i].q.end();it++){
					if(iswb == 1 && dst[*it].nextq[t] == 0) continue;		//如果都无边则不需要划分
					else if(iswb == 1 && dst[*it].nextq[t] != 0){			//一个无边一个有边 
						mins[numj].q.insert(*it);
						flag = 1;		//有新的状态集产生，需要继续划分 
						continue; 
					} 
					else if(mins[nextS].q.find(dst[*it].nextq[t]) == mins[nextS].q.end() )	//与第一个不同说明要被划分到新的集合 
					{
						mins[numj].q.insert(*it);
						flag = 1;continue;
					}			//否则相同不做划分 	
				}
			} 	
		}
		if(flag == 1){ 	//说明有新的集合出现，numj++ 
		//再将原来集合中多的删掉
			int sq;
			for(it = mins[numj].q.begin();it!=mins[numj].q.end();it++){
				sq = *it;
				for(int i =0;i<numj;i++) {
					if(mins[i].q.find(sq)!=mins[i].q.end())	{
						mins[i].q.erase(sq);
						break;
					}
				}	
			}
			numj++;	
		}
	}
	//状态转移计算过程
	for(int i=0;i<numj;i++){
		it = mins[i].q.begin();
		for(int t=0;t<numt;t++){
			nextS = dst[*it].nextq[t];
			if(nextS == 0) {
				mins[i].nextj[t] = -1;
				continue;	
			}
			for(int k=0;k<numj;k++){
				if(mins[k].q.find(nextS)!=mins[k].q.end()){
					mins[i].nextj[t] = k;break;
				}
			}
		}
	}
	for(int i=0;i<numj;i++){
		mins[i].isjs = 0;
		for(it = mins[i].q.begin();it!=mins[i].q.end();it++){
			if(*it == 1) beginT = i;		//确定开始状态
			if(dst[*it].isjs == 1) mins[i].isjs = 1; 
		}
	} 
} 

void init(){		//读入文件中的NFA 
	fin.open("nfatodfa.txt");
	fin >> numq;fin >> numt;
	char zf;
	for(int i = 0;i < numt;i ++){
		fin >> zfj[i]; 
	}
	char x;
	for(int i = 1;i <= numq; i ++){
		for(int j = 0;j < numt;j ++){
			fin >> dst[i].nextq[j] ;				//下一状态
		}
		fin>>x;
		if(x == '*') {
			dst[i].isjs = 1;			//是*号说明是接收状态 
			mins[1].q.insert(i); 			//加入到接受集 		
		} 
		else{ 
			dst[i].isjs = 0;					//否则是非接收状态 
			mins[0].q.insert(i);				//加入非接受状态集 
		} 
	}
	if(mins[0].q.empty()){
		mins[0].q = mins[1].q;
		mins[1].q.clear();
		numj = 1;
	}else if(mins[1].q.empty()){
		numj = 1;
	} 
	fin.close();
}

void print(){
	fout.open("mindfa.txt");
	fout<<"最小化后dfa状态数为："<< numj<<"\n";
	fout<<"开始状态为："<<beginT+1<<"\n";
	printf("min状态\t");fout<<"min状态\t";
	for(int i=0;i<numt;i++){
		printf("%c ",zfj[i]);fout << zfj[i] <<" ";		//dfa字符 
	} 
	printf("dfa状态集\n");fout<<"dfa状态集\n";
	for(int i=0;i<numj;i++){
		printf("%d\t",i+1);	fout<<i+1<<"\t";
		for(int t=0;t<numt;t++){
			fout<< mins[i].nextj[t]+1 <<" ";
			printf("%d ",mins[i].nextj[t]+1);
		}
		printf("{");fout<<"{";
		for(it = mins[i].q.begin();it !=mins[i].q.end();it++){
			fout << *it << " ";
			printf("%d ",*it);
		}
		printf("}");fout<<"}";
		if (mins[i].isjs == 1 ){
			printf("*\n");fout<<"*\n";
		}
		else {
			printf("\n");
			fout<<"\n";
		}
	}
	fout.close();
}

char sc[3];
void jcmin(int s,int d,int limit){
	if( d>=limit){
		if(mins[s].isjs == 1) 
			printf("%s\n",sc);
		return;
	}
	for(int i=0;i<numt;i++){
		if(mins[s].nextj[i] == -1) continue;		//无边则跳过 
		sc[d] = zfj[i];
		jcmin(mins[s].nextj[i],d+1,limit);	
	}
}

int main(){
	init();
	mindfa();	
	print();
	printf("\n检查最小化dfa--该文法长度为3的字符串：\n");
	jcmin(beginT,0,3);
}

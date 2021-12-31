#include <stdio.h>
#include <fstream>
#include <set> 
using namespace std;
#define maxq 100	//����dfa������Ŀ 
#define maxt 20		//���ŵ������Ŀ 
struct DFAState{
	int nextq[maxt];	//dfa��������ŵ���һ״̬��ȷ�����ҷ��Ų�����
	bool isjs; 
}dst[maxq]; 
char zfj[maxt];		//�ַ��� 
int numq;		//״̬�� 
int numt;		//������ 

struct MINState{
	set<int> q;			//��С�����dfa���ϣ�λ��ʾ״̬������ʾ���ϣ� 
	int nextj[maxt];
	bool isjs;
}mins[maxq];
	 
int numj = 2;			//dfa������������ʼ��Ϊ�ǽ��ܼ�����ܼ����� 
int firstS;
int nextS;		//����ÿһ��״̬���ݷ��Ž���ĵ�һ��״̬��
int beginT ;		//min��Ŀ�ʼ�� 
ifstream fin; 
ofstream fout;
set<int>::iterator it;

void mindfa(){
	int flag=1;
	int iswb = 0; 
	while(flag){		//���ֹ��� 
	flag = 0; 
		for(int t=0;t<numt;t++){	
			for(int i=0; i<numj; i++){	
				iswb = 0;
				it =mins[i].q.begin(); 
				//if(it == mins[i].q.end()) continue;
				firstS = *it;	
				nextS = dst[firstS].nextq[t];	//nextS����Ϊ0�������Ҫ���п��� 
				if(nextS == 0) iswb = 1;
				for(int k=0;k<numj;k++){		//��ѯ��һ��״̬���ڷ���t�����״̬���ڵ�״̬�� 
					if(mins[k].q.find(nextS)!=mins[k].q.end()){
						nextS = k;				//nextSΪ��һ��״̬�Է���t�������һ���ż� 
						break;
					}
				}
				it = mins[i].q.begin();it++;
				for(;it != mins[i].q.end();it++){
					if(iswb == 1 && dst[*it].nextq[t] == 0) continue;		//������ޱ�����Ҫ����
					else if(iswb == 1 && dst[*it].nextq[t] != 0){			//һ���ޱ�һ���б� 
						mins[numj].q.insert(*it);
						flag = 1;		//���µ�״̬����������Ҫ�������� 
						continue; 
					} 
					else if(mins[nextS].q.find(dst[*it].nextq[t]) == mins[nextS].q.end() )	//���һ����ͬ˵��Ҫ�����ֵ��µļ��� 
					{
						mins[numj].q.insert(*it);
						flag = 1;continue;
					}			//������ͬ�������� 	
				}
			} 	
		}
		if(flag == 1){ 	//˵�����µļ��ϳ��֣�numj++ 
		//�ٽ�ԭ�������ж��ɾ��
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
	//״̬ת�Ƽ������
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
			if(*it == 1) beginT = i;		//ȷ����ʼ״̬
			if(dst[*it].isjs == 1) mins[i].isjs = 1; 
		}
	} 
} 

void init(){		//�����ļ��е�NFA 
	fin.open("nfatodfa.txt");
	fin >> numq;fin >> numt;
	char zf;
	for(int i = 0;i < numt;i ++){
		fin >> zfj[i]; 
	}
	char x;
	for(int i = 1;i <= numq; i ++){
		for(int j = 0;j < numt;j ++){
			fin >> dst[i].nextq[j] ;				//��һ״̬
		}
		fin>>x;
		if(x == '*') {
			dst[i].isjs = 1;			//��*��˵���ǽ���״̬ 
			mins[1].q.insert(i); 			//���뵽���ܼ� 		
		} 
		else{ 
			dst[i].isjs = 0;					//�����Ƿǽ���״̬ 
			mins[0].q.insert(i);				//����ǽ���״̬�� 
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
	fout<<"��С����dfa״̬��Ϊ��"<< numj<<"\n";
	fout<<"��ʼ״̬Ϊ��"<<beginT+1<<"\n";
	printf("min״̬\t");fout<<"min״̬\t";
	for(int i=0;i<numt;i++){
		printf("%c ",zfj[i]);fout << zfj[i] <<" ";		//dfa�ַ� 
	} 
	printf("dfa״̬��\n");fout<<"dfa״̬��\n";
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
		if(mins[s].nextj[i] == -1) continue;		//�ޱ������� 
		sc[d] = zfj[i];
		jcmin(mins[s].nextj[i],d+1,limit);	
	}
}

int main(){
	init();
	mindfa();	
	print();
	printf("\n�����С��dfa--���ķ�����Ϊ3���ַ�����\n");
	jcmin(beginT,0,3);
}

/*�ο��ĵ���״̬�뼯�ϵı�ʾ��������λ�ķ�ʽ��ʾ״̬������ʾ����
int �Ϳ��Ա�ʾ32��״̬ 
*/
#include <stdio.h>
#include <fstream> 
#include <set>
#include <queue>
using namespace std;
#define maxs 32    //���״̬��Ŀ
#define maxt 20		//���ŵ������Ŀ 
#define maxq 100	//����dfa������Ŀ 
struct NFAState{
	int i;	//��ţ�1��λ->1<<i�� 1-32
	set<int>nexts[maxt];	//��ͬ���ŵ��µ���һ��״̬���ܲ�ͬ 
}nst[maxs];

struct DFAState{
	int Q;		//���Ǳ�ţ��Ǽ���
	int nextq[maxt];	//dfa��������ŵ���һ״̬��ȷ�����ҷ��Ų����� 
	int isjs;		//�Ƿ�Ϊ������ 
}dst[maxq]; 
 
int numt,nums,send;	//�ַ�����nfa״̬�����������ţ�Լ����������ֻ��1������ʼ����Լ��Ϊ1�� 
char zfj[maxt];		//�ַ��� 

set<int> indfa;		//��ʾ״̬���Ѿ���dfa��״̬������ 
int closure[maxs] = {0};		//ÿһ��nfa״̬�Ŀշ��հ�(��������ʾ����) 
set<int>::iterator it;

string sr;		//����������һ״̬���ַ��� 
int numq = 0;		//dfa״̬����Ŀ 

ifstream fin;
ofstream fout;
void srfx(int s,int t){
	nst[s].nexts[t].clear();		//��ʼ��set 
	if(sr.length()==1&&sr[0]=='0'){	//0	(0��ʾ����һ״̬)
		return;
	}	
	int n = 0;
	for(int i = 0;i < sr.length();i ++){	//�����ж��״̬����˽��з��� 
		if(sr[i] == ','){		//����һ��״̬�ɹ� 
			nst[s].nexts[t].insert(n);	//������ǣ�2^n��
			n = 0;				//���� 
		}
		else{
			n *= 10;
			n += sr[i] - '0'; 
		}
	}
	//��󲻻��ж���ģ�����Լ��ټ�������n
	nst[s].nexts[t].insert(n);
} 
void init(){		//�����ļ��е�NFA 
	fin.open("in.txt");
	fin >> numt;
	numt += 1;		//���Ͽշ� 
	char zf;
	for(int i = 0;i < numt-1;i ++){
		fin >> zfj[i]; 
	}
	fin >> nums >> send;			//Լ��״̬�Ǵ�1��ʼ���������� 
	for(int i = 1;i <= nums; i ++){
		nst[i].i = 1 << (i-1); 		//״̬��λ��ʾ��Ӧ2^i 
		for(int j = 0;j < numt;j ++){
			sr = "";
			fin >> sr ;				//��һ״̬�ж��ʱ����,�ָ� 
			srfx(i,j);				//��ȡ���е���һ״̬������ 
		}
	}
	for(int i=0;i<maxq;i++){
		dst[i].Q = 0;dst[i].isjs = 0;
	} 
	indfa.clear();
	fin.close();	
}

void count_closure(int s)		//�շ��հ����㣬����ֱ�Ӽ�������nfa״̬�ıհ������汸�� 
{
	queue<int> cl;
	closure[s] |= nst[s].i;		//�����Լ� 
	for( it= nst[s].nexts[0].begin();it!=nst[s].nexts[0].end();it++ ){	//0���ǿռ��ɴ��״̬ 
		cl.push(*it);		//����ӣ�Ȼ���ٽ��бհ��ļ��� 
	} 
	while(!cl.empty()){
		int nows = cl.front();
		cl.pop();
		if((closure[s]&(1<<(nows-1) )) == 0){	//δ��c�հ����ҵ������ 
			closure[s] |= 1<<(nows-1);
		//	printf("%d\n",closure[s]);
			for(it= nst[nows].nexts[0].begin();it!=nst[nows].nexts[0].end();it++)
				cl.push(*it);	 
		}	
	}
}
void todfa(int ds){			//dsָ����dfa�еļ���״̬ 
	int newq = 0,xh=0;
	for(int t = 1;t < numt ; t++) //ÿһ������ 
	{	newq = 0;
		for(int i=1;i<=nums;i++){		//ÿһ��״̬���ܵ������һ��״̬�� 
			if((dst[ds].Q & (1<<(i-1))) == 0) continue;	//Ϊ0��ʾ������״̬i�����账�� 
			for(it = nst[i].nexts[t].begin();it!=nst[i].nexts[t].end();it++){
				newq |= closure[*it];				
			} 
		}
		if(indfa.find(newq)==indfa.end()&&newq!=0)	//˵�����µ�״̬�� 
		{
			numq++;
			dst[numq].Q = newq;
			dst[ds].nextq[t-1] = numq; 
			indfa.insert(newq);
		}
		else if(newq!=0){	//������״̬��ֱ�Ӽӱ�
			xh = 0;
			for(int i=0;i<=numq;i++){
				if(dst[i].Q == newq){xh = i;break;}
			} 
			dst[ds].nextq[t-1] = xh;
		}
		else{	//newq == 0,�ޱ� 
			dst[ds].nextq[t-1] = -1;	
		} 	
	}
} 
void print(){
	fout.open("nfatodfa.txt");
		fout << numq+1<<"\n";			//dfa״̬�� 
		fout << numt-1<<"\n";		//dfa �ַ��� 
		printf("dfa״̬\t");
		for(int i=0;i<numt-1;i++){
			printf("%c\t",zfj[i]);fout << zfj[i] <<" ";		//dfa�ַ� 
		} printf("nfa״̬��\n");fout<<"\n";
		for(int i=0;i<=numq;i++){
			printf(" %d \t",i+1);	
			for(int j=0;j<numt-1;j++){
				printf("%d\t",dst[i].nextq[j]+1); fout << dst[i].nextq[j]+1 << " ";	//ת�� 
			}printf("{");
			for(int j=1;j<=nums;j++){
				if((dst[i].Q&(1<<(j-1)))!=0) {
					printf("%d ",j);
					if( j == send)	//�ս��
						dst[i].isjs = 1; 
				} 
			}
			printf("}");
			if(dst[i].isjs == 1){
				printf("*");fout<<"*";	//��*�ű�ʾ���ս� 
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
		if(dst[s].nextq[i] == -1) continue;		//�ޱ������� 
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
	dst[numq].Q = closure[1];		//��ʼ��dfa��һ��״̬������ʼ״̬��
	indfa.insert(dst[numq].Q);
	int ds = 0;      
	                                                        
	while(ds<=numq){
		todfa(ds);
		ds++;
	}
	print();
	printf("\n����nfa�����ķ�����Ϊ3���ַ�����\n");
	pathnfa(1,0,3); 
	printf("\n���dfa��ȷ��--���ķ�����Ϊ3���ַ�����\n");
	jcdfa(0,0,3);
} 

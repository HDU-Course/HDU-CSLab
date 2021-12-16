#include<map> 
#include<iostream>
#include<cstdio>
#include<algorithm>
#include<string>
using namespace std;
int fa[50005];
int n,m,k;
double tot=0;
struct edge{
	int x,y;
	double val;
}a[100005];
struct node{
	int num;
	string name;
}b[1000];
int find(int x){
	if(fa[x]!=x)	fa[x]=find(fa[x]);
	return fa[x];
}
void uni(int x,int y){
	int f1=find(x),	f2=find(y);
	if(f1!=f2)	fa[f1]=f2;
}
bool cmp(edge a,edge b){
	return a.val<b.val;
} 
int loc(string nam){
	for(int i=1;i<=n;i++)	if(b[i].name==nam)	return i; 
} 
int main(){
	freopen("data.txt","r",stdin);
	scanf("%d%d",&n,&m);
	string na;
	for(int i=1;i<=n;i++){
		cin>>na;
		b[i].num=i;	b[i].name=na;
	}
	string begin,end;
	for(int i=1;i<=m;i++){
		cin>>begin>>end>>a[i].val;
		a[i].x=loc(begin);
		a[i].y=loc(end);
	}
	
	for(int i=1;i<=n;i++)	fa[i]=i;
	sort(a+1,a+m+1,cmp);
	cout<<"需要修的道路为："<<endl; 
	for(int i=1;i<=m;i++){
		if(find(a[i].x)!=find(a[i].y)){
			uni(a[i].x,a[i].y);
			tot+=a[i].val;	k++;
			cout<<b[a[i].x].name<<""<<b[a[i].y].name<<endl;
		}
		if(k==n-1)break;
	}
	if(k<n-1){
		printf("No spaningtree");
		return 0;
	}
	else
		printf("tot=%lf\n",tot);
	return 0;
}

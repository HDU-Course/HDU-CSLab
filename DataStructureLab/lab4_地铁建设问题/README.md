## 数据结构实验4：地铁建设问题

### （1） 问题描述

某城市要在其各个辖区之间修建地铁来加快经济发展，但由于建设地铁的费用昂贵，因此需要合理安排地铁建设路线，使乘客可以沿地铁到达各个辖区，并使总的建设费用最小。

### （2） 设计要求

1） 从包含各辖区的地图文件中读入辖区名称和各辖区间的直接距离。

2） 根据读入的各辖区的距离信息，计算出应该建设哪些辖区间的地铁路线。

3） 输出应该建设的地铁路线及所需建设的总里程信息。

### （3） 数据结构

  本设计使用的数据结构是无向图，无向图采用邻接矩阵作为存储结构。

### （4） 分析与实现

#### 分析要求：

这个是一个求图的最小生成树的问题，我们需要选择图作为数据结构，并且使用普利姆算法或者克鲁斯卡算法进行实现。

#### 算法设计：

代码将从包含各辖区的地图文件中读入辖区名称和各辖区间的直接距离。并根据读入的各辖区的距离信息，计算出应该建设哪些辖区间的地铁路线。在加入地铁路线时，我采用prim算法，prim算法即树中所有路径之和最小，首先清空生成树，然后任取一个顶点加入生成树，在那些一个端点在生成树里，另一个端点不在生成树里的边中，选取一条权最小的边，将它和另一个端点加进生成树，不停重复步骤2，直到所有的地铁站点都进入了生成树为止，此时的生成树就是最小生成树，此时输出应该建设的地铁路线及所需要建设的总里程信息。

#### 代码：

```C++
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

```

### （5） 运行与测试

#### INPUT

```c++
11
16
怀柔区
昌平区
顺义区
平谷区
海淀区
石景山区
通州区
宣武区
房山区
大兴区
宝坻区
怀柔区 顺义区 19.38
昌平区 顺义区 19.38
顺义区 平谷区 44.43
顺义区 海淀区 33.76
海淀区 石景山区 13.89
海淀区 宣武区 11.96
海淀区 通州区 33.73
石景山区 通州区 49.49
石景山区 宣武区 23.01
石景山区 大兴区 27.70
石景山区 房山区 19.11
房山区 大兴区 19.52
大兴区 宣武区 14.82
宣武区 通州区 28.86
宣武区 宝坻区 82.99
通州区 宝坻区 55.79
```

### OUTPUT

````c++
```
需要修的道路为：
海淀区宣武区
海淀区石景山区
大兴区宣武区
石景山区房山区
怀柔区顺义区
昌平区顺义区
宣武区通州区
顺义区海淀区
顺义区平谷区
通州区宝坻区
tot=261.380000
````

#### 运行截图

  ![image-20211220200115006](C:\Users\76104\AppData\Roaming\Typora\typora-user-images\image-20211220200115006.png)

### （6） 总结与思考

#### 1.选择什么算法实现最小生成树？

Prim和Kruskal算法都可以实现本题要求的最小生成树，前者是将选择结点加入树，后者是选择边加入树，两种算法其实都可以实现。

#### 2.真实情景中的如何使用此算法？

真实情景中，地铁线路的选择不会是一维的路线最短，而是会受到人流量、地铁单位路径造价等等多种因素的制约，可以尝试在多维角度下对最小生成树的构建算法。

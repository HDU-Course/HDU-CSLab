多项式求导：

（1） 问题描述

简单一元多项式的求导问题

（2） 设计要求

  实现一元多项式求导运算,有效字符为变量x，数字0~9，运算符^、+、- 。输出结果也为多项式。例如，输入6x^4-32x，其输出结果为24x^3-32。

（3） 数据结构

  本设计使用顺序表作为实现该问题的数据结构。

（4） 分析与实现

  题目要求使用顺序表实现，顺序表是内存中以数组形式存储的线性表，即用数组实现一元多项式求导。
首先将读入的一元函数表达式进行处理并且在字符数组中存贮，并且对字符数组进行遍历，如果遇到多位数字，则需要通过循环先将数字存储在一个中间变量中，通过此方法系数和幂次都可以存在变量modulus和power中，并且在每一项运算结束后，通过数组下标标记幂次，例如求导后结果为24x^3，则存储将系数在output[3]中，并且进行转换按照幂次从高到低输出即可。

```c++
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
using namespace std;
int main(){
	char input[1000] = {0};
	int output[1000] = {0};
	int temp[1000] = {0};
	char t[100] = {0};
	int modulus = 0;
	int power = 0;
	int flag = 1;
	cin>>input;
	for(int i = 0; i < strlen(input); ++i) {
		if(input[i] == '-') {
			flag = -1;
		}else if(input[i] <= '9' && input[i] >= '0'){
			//将系数存于变量modulus中
			t[0] = '\0';
			for(int j = 0; input[i] <= '9' && input[i] >= '0'; ++i) {
				t[j++] = input[i];
			}
			modulus = atoi(t) * flag;
			flag = 1;
			//根据次数存入对应的ouput数组下标中
			if(input[i] == 'x' && input[i+1] == '^') {
				i += 2;
				for(int j = 0; input[i] <= '9' && input[i] >= '0'; ++i) {
				t[j++] = input[i];
				}
				power = atoi(t);
				i--;
				output[power-1] = modulus * power;
			}else if (input[i] == 'x') {
				power = 1;
				output[power-1] = modulus * power;
			}
		}else if(input[i] == 'x' && input[i+1] == '^') {
			modulus = 1 * flag;
			i += 2;
			for(int j = 0; input[i] <= '9' && input[i] >= '0'; ++i) {
			t[j++] = input[i];
			}
			power = atoi(t);
			i--;
			output[power-1] = modulus * power;
		}else if (input[i] == 'x') {
			modulus = 1 * flag;
			power = 1;
			output[power-1] = modulus * power;
		}
	}
	for(int i=999; i>=0; --i){
		if (output[i] != 0 && i != 0 && i!= 1){
			if(flag) {
				printf("%dx^%d",output[i],i);
				flag = 0;
			}else {
				printf("+%dx^%d",output[i],i);
			}
		}else if (output[i] != 0 && i == 0) {
			printf("%d",output[i]);
		}else if (output[i] != 0 && i == 1) {
			printf("%dx",output[i]);
		}
	}
	return 0;
}
```



（5） 运行与测试

代码能正常通过测试样例.


（6） 总结与思考

此题需要注意以下几点，1如何选择数据结构进行存储数据，2对于零碎细节的处理例如整数，系数正负，幂次为0、1时和首项为正时无符号等都需要注意

  


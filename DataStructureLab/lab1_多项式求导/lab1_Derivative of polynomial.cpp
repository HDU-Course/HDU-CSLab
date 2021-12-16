#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
using namespace std;
char input[1000] = {'\0'};
int output[1000] = {0};
int temp[1000] = {0};
char t[100] = {0};
int flag = 1;

//降幂打印表达式
void print() {
	//首项无符号
	flag = 2;
	for(int i=999; i>=0; --i){
		if (output[i] != 0 && i != 0 && i!= 1){
			if(output[i] <=0 || flag == 2) {
				printf("%dx^%d",output[i],i);
				flag = 1;
			}else {
				printf("+%dx^%d",output[i],i);
			}
		}else if (output[i] != 0 && i == 0) {
			if(output[i] <=0 ) {
				printf("%d",output[i]);
				flag = 1;
			}else {
				printf("+%d",output[i]);
			}
		}else if (output[i] != 0 && i == 1) {
			if(output[i] <=0 ) {
				printf("%dx",output[i]);
				flag = 1;
			}else {
				printf("+%dx",output[i]);
			}
		}
	}
}

int main() {
	cin>>input;
	int modulus = 0;
	int power = 0;
	for(int i = 0; i < strlen(input); ++i) {          
		//任意位置读到符号，将该项的符号用flag变量标记     
		if(input[i] == '-') {
			flag = -1;
		}else if(input[i] <= '9' && input[i] >= '0'){ 
			//将系数存于变量modulus中
			memset(t,0,sizeof(t));
			for(int j = 0; input[i] <= '9' && input[i] >= '0' && i < strlen(input); ++i) {
				t[j++] = input[i];
			}
			modulus = atoi(t) * flag;
			flag = 1;
			//根据次数存入对应的ouput数组下标中
			if(input[i] == 'x' && input[i+1] == '^') {
				memset(t,0,sizeof(t));
				i += 2;
				for(int j = 0; input[i] <= '9' && input[i] >= '0' && i < strlen(input); ++i) {
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
			memset(t,0,sizeof(t));
			for(int j = 0; input[i] <= '9' && input[i] >= '0' && i < strlen(input); ++i) {
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
	print();
	return 0;
}
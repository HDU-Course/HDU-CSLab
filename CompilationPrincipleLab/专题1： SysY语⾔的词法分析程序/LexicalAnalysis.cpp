#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
using namespace std;

//存放处理后的字符串
char tempstr[255] = {};
//空格标志
bool temp = false;
//临时数组
char word[255] = {};

//keyword关键字
string keyword[11] = {
	"begin",
	"end",
	"if",
	"then",
	"else",
	"for",
	"while",
	"do",
	"and",
	"or",
	"not",
};

int keyword_num[11] = {
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
};
//分界符
char symbol[6] = {
	'(',
	')',
	',',
	';',
	'#',
	':'
};
int symbol_num[6] = {
	1,
	2,
	3,
	4,
	5,
	6,
};

struct
{
	int number;
	string str[100];
} identifiers = {0}; //标识符,以下划线或字母开头的字母数字下划线组成的符号串。

int num;
//判断是否为字母
bool IsLetter(char ch)
{
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
		return true;
	return false;
}

//判断是否为数字
bool IsDigit(char ch)
{
	if (ch >= '0' && ch <= '9')
		return true;
	return false;
}

//判断是否为分界符
int IsSymbol(char ch)
{
	for (int i = 0; i < 6; i++)
	{
		if (ch == symbol[i])
			return i;
	}
	return -1;
}

//判断是否为关键字
int IsKeyword(string str)
{
	for (int i = 0; i < 11; i++)
	{
		if (str == keyword[i])
		{
			return i;
		}
	}
	//不是关键字即为标识符
	return 30;
}

//判断标识符是否合法
bool IsRight(char str[])
{
	for (int i = 0; i < strlen(str); i++)
	{
		if (IsDigit(*(str + i)))
			if (IsLetter(*(str + i + 1)) || *(str + i + 1) == '_')
				return false;
	}
	return true;
}

//空格处理
void HandleSpace(char a[])
{
	int j = 0;
	memset(word, 0, 255); //需要清空，不然可能残留上次的字符串
	temp = false;
	for (int i = 0; i < strlen(a); i++)
	{

		if (a[i] != ' ' && a[i] != '\t')
		{
			word[j++] = a[i];
			temp = false;
		}
		else
		{

			if (!temp && a[i] != '\t')
			{
				word[j++] = a[i];
				temp = true;
			}
		}
	}
}

//处理注释
void prePro()
{
	int j = 0;
	memset(tempstr, 0, 255);
	for (int i = 0; i < strlen(word); i++)
	{
		if (word[i] == '#')
		{
			while (i < strlen(word))
			{
				i++;
			}
		}

		else
		{
			tempstr[j++] = word[i];
		}
	}
}

int main()
{
	char instr[255] = {}; //接收输入字符串
	bool flag = false;	  //多行注释标志,false为未处于注释区域
	string Token;		  //存放字符串
	char *str = NULL;	  //存放每行的字符串
	char delims[] = " ";  //分割标志
	int k;
	freopen("test.txt", "r", stdin);
	// freopen("result.txt", "w", stdout);

	while ((gets(instr)) != NULL)
	{
		HandleSpace(instr);
		prePro();
		str = strtok(tempstr, delims); //分割字符串
		while (str != NULL)
		{
			for (int i = 0; i < strlen(str); i++)
			{
				//注释处理: /*,注释区域开始
				if (*(str + i) == '/')
				{
					if (*(str + i + 1) == '*')
					{
						flag = true;
						break;
					}
				}
				//注释处理:,注释区域结束
				if (*(str + i) == '*' && flag)
				{
					if (*(str + i + 1) == '/')
					{
						flag = false;
						i++;
						break;
					}
				}
				//判断标识符是否合法
				if (!IsRight(str))
				{
					printf("  %s为非法输入\n", str);
					break;
				}
				//标识符，关键词
				if ((IsLetter(*(str + i)) || *(str + i) == '_') && !flag)
				{
					while (IsLetter(*(str + i)) || IsDigit(*(str + i)) || *(str + i) == '_')
					{
						Token += *(str + i);
						i++;
					}
					i--;
					if (IsKeyword(Token) != 30)
					{
						printf("( %s ，<关键词%d> )\n", Token.c_str(), keyword_num[IsKeyword(Token)]);
						num = keyword_num[IsKeyword(Token)];
					}

					else
					{
						for (k = 0; k < identifiers.number; k++)
						{
							if (identifiers.str[k] == Token.c_str())
								break;
						}
						if (k == identifiers.number)
						{
							identifiers.str[identifiers.number++] = Token.c_str();
							cout << "( " << Token.c_str() << " ，<标识符," << identifiers.number << "> ）" << endl;
						}
						else
							cout << "( " << Token.c_str() << " ，<标识符," << k + 1 << "> ）" << endl;
					}
					Token = "";
				}
				else if (IsDigit(*(str + i)) && !flag)
				{
					while (IsDigit(*(str + i)))
					{
						Token += *(str + i);
						i++;
					}
					i--;
					printf("( %s ， 数字 )\n", Token.c_str());
					num = 31;
					Token = "";
				}

				//<,<=,<>
				else if (*(str + i) == '<' && !flag)
				{
					// if (*(str + i + 1) == '=')
					// {
					// 	printf("( <= ，<运算符,10> )\n");
					// 	i++;
					// 	num = 21;
					// }
					// else if (*(str + i + 1) == '>')
					// {
					// 	printf("( <> ，<运算符,11> )\n");
					// 	i++;
					// 	num = 20;
					// }
					// else
					{
						printf(" ><为非法标识符\n");
						i+=2;
						num = 19;
					}
				}
				//>,>=
				else if (*(str + i) == '>' && !flag)
				{
					if (*(str + i + 1) == '=')
					{
						// printf("( >= ，<运算符,9> )\n");
						i++;
						num = 23;
					}
					else
					{
						// printf("( > ，<运算符,5> )\n");
						num = 22;
					}
				}
				//:,:=
				else if (*(str + i) == ':' && !flag)
				{
					if (*(str + i + 1) == '=')
					{
						printf("( := ，<运算符,8> )\n");
						i++;
						num = 25;
					}
					else
					{
						printf("( : , ，<分界符,6> )\n");
						num = 24;
					}
				}
				//+,++
				else if (*(str + i) == '+' && !flag)
				{
					if (*(str + i + 1) == '+')
					{
						printf("( ++ ，<运算符,12> )\n");
						i++;
						num = 27;
					}
					else
					{
						printf("( + ，<运算符,1> )\n");
						num = 26;
					}
				}
				//-,--
				else if (*(str + i) == '-' && !flag)
				{
					if (*(str + i + 1) == '-')
					{
						printf("( -- ，<运算符,13> )\n");
						i++;
						num = 29;
					}
					else
					{
						printf("( - ，<运算符,2> )\n");
						num = 28;
					}
				}
				else if (*(str + i) == '*' && !flag)
				{

					printf("( * ，<运算符,3> )\n");
					num = 22;
				}

				else if (*(str + i) == '/' && !flag)
				{

					printf("( / ，<运算符,4> )\n");
					num = 22;
				}
				else if (*(str + i) == '=' && !flag)
				{

					printf("( = ，<运算符,7> )\n");
					num = 22;
				}
				//余下定界符等
				else if (IsSymbol(*(str + i)) != -1 && !flag)
				{
					printf("( %c  ,  <分界符,%d> )\n", *(str + i), symbol_num[IsSymbol(*(str + i))]);
					num = symbol_num[IsSymbol(*(str + i))];
				}
				else if (!flag)
				{
					printf("%s为非法标识符\n", str);
					break;
				}
			}
			str = strtok(NULL, delims);
		}
	}
	freopen("CON", "w", stdout);
	cout << "已完成词法分析:" << endl
		 << endl;
	// freopen("result.txt", "r", stdin);
	while ((gets(instr)) != NULL)
		cout << instr << endl;
	return 0;
}
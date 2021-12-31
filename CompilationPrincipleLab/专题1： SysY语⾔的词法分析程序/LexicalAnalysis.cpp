#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
using namespace std;

//��Ŵ������ַ���
char tempstr[255] = {};
//�ո��־
bool temp = false;
//��ʱ����
char word[255] = {};

//keyword�ؼ���
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
//�ֽ��
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
} identifiers = {0}; //��ʶ��,���»��߻���ĸ��ͷ����ĸ�����»�����ɵķ��Ŵ���

int num;
//�ж��Ƿ�Ϊ��ĸ
bool IsLetter(char ch)
{
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
		return true;
	return false;
}

//�ж��Ƿ�Ϊ����
bool IsDigit(char ch)
{
	if (ch >= '0' && ch <= '9')
		return true;
	return false;
}

//�ж��Ƿ�Ϊ�ֽ��
int IsSymbol(char ch)
{
	for (int i = 0; i < 6; i++)
	{
		if (ch == symbol[i])
			return i;
	}
	return -1;
}

//�ж��Ƿ�Ϊ�ؼ���
int IsKeyword(string str)
{
	for (int i = 0; i < 11; i++)
	{
		if (str == keyword[i])
		{
			return i;
		}
	}
	//���ǹؼ��ּ�Ϊ��ʶ��
	return 30;
}

//�жϱ�ʶ���Ƿ�Ϸ�
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

//�ո���
void HandleSpace(char a[])
{
	int j = 0;
	memset(word, 0, 255); //��Ҫ��գ���Ȼ���ܲ����ϴε��ַ���
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

//����ע��
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
	char instr[255] = {}; //���������ַ���
	bool flag = false;	  //����ע�ͱ�־,falseΪδ����ע������
	string Token;		  //����ַ���
	char *str = NULL;	  //���ÿ�е��ַ���
	char delims[] = " ";  //�ָ��־
	int k;
	freopen("test.txt", "r", stdin);
	// freopen("result.txt", "w", stdout);

	while ((gets(instr)) != NULL)
	{
		HandleSpace(instr);
		prePro();
		str = strtok(tempstr, delims); //�ָ��ַ���
		while (str != NULL)
		{
			for (int i = 0; i < strlen(str); i++)
			{
				//ע�ʹ���: /*,ע������ʼ
				if (*(str + i) == '/')
				{
					if (*(str + i + 1) == '*')
					{
						flag = true;
						break;
					}
				}
				//ע�ʹ���:,ע���������
				if (*(str + i) == '*' && flag)
				{
					if (*(str + i + 1) == '/')
					{
						flag = false;
						i++;
						break;
					}
				}
				//�жϱ�ʶ���Ƿ�Ϸ�
				if (!IsRight(str))
				{
					printf("  %sΪ�Ƿ�����\n", str);
					break;
				}
				//��ʶ�����ؼ���
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
						printf("( %s ��<�ؼ���%d> )\n", Token.c_str(), keyword_num[IsKeyword(Token)]);
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
							cout << "( " << Token.c_str() << " ��<��ʶ��," << identifiers.number << "> ��" << endl;
						}
						else
							cout << "( " << Token.c_str() << " ��<��ʶ��," << k + 1 << "> ��" << endl;
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
					printf("( %s �� ���� )\n", Token.c_str());
					num = 31;
					Token = "";
				}

				//<,<=,<>
				else if (*(str + i) == '<' && !flag)
				{
					// if (*(str + i + 1) == '=')
					// {
					// 	printf("( <= ��<�����,10> )\n");
					// 	i++;
					// 	num = 21;
					// }
					// else if (*(str + i + 1) == '>')
					// {
					// 	printf("( <> ��<�����,11> )\n");
					// 	i++;
					// 	num = 20;
					// }
					// else
					{
						printf(" ><Ϊ�Ƿ���ʶ��\n");
						i+=2;
						num = 19;
					}
				}
				//>,>=
				else if (*(str + i) == '>' && !flag)
				{
					if (*(str + i + 1) == '=')
					{
						// printf("( >= ��<�����,9> )\n");
						i++;
						num = 23;
					}
					else
					{
						// printf("( > ��<�����,5> )\n");
						num = 22;
					}
				}
				//:,:=
				else if (*(str + i) == ':' && !flag)
				{
					if (*(str + i + 1) == '=')
					{
						printf("( := ��<�����,8> )\n");
						i++;
						num = 25;
					}
					else
					{
						printf("( : , ��<�ֽ��,6> )\n");
						num = 24;
					}
				}
				//+,++
				else if (*(str + i) == '+' && !flag)
				{
					if (*(str + i + 1) == '+')
					{
						printf("( ++ ��<�����,12> )\n");
						i++;
						num = 27;
					}
					else
					{
						printf("( + ��<�����,1> )\n");
						num = 26;
					}
				}
				//-,--
				else if (*(str + i) == '-' && !flag)
				{
					if (*(str + i + 1) == '-')
					{
						printf("( -- ��<�����,13> )\n");
						i++;
						num = 29;
					}
					else
					{
						printf("( - ��<�����,2> )\n");
						num = 28;
					}
				}
				else if (*(str + i) == '*' && !flag)
				{

					printf("( * ��<�����,3> )\n");
					num = 22;
				}

				else if (*(str + i) == '/' && !flag)
				{

					printf("( / ��<�����,4> )\n");
					num = 22;
				}
				else if (*(str + i) == '=' && !flag)
				{

					printf("( = ��<�����,7> )\n");
					num = 22;
				}
				//���¶������
				else if (IsSymbol(*(str + i)) != -1 && !flag)
				{
					printf("( %c  ,  <�ֽ��,%d> )\n", *(str + i), symbol_num[IsSymbol(*(str + i))]);
					num = symbol_num[IsSymbol(*(str + i))];
				}
				else if (!flag)
				{
					printf("%sΪ�Ƿ���ʶ��\n", str);
					break;
				}
			}
			str = strtok(NULL, delims);
		}
	}
	freopen("CON", "w", stdout);
	cout << "����ɴʷ�����:" << endl
		 << endl;
	// freopen("result.txt", "r", stdin);
	while ((gets(instr)) != NULL)
		cout << instr << endl;
	return 0;
}
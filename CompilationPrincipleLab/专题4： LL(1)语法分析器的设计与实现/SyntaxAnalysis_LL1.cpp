#include <iostream>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <stack>
#include <fstream>
using namespace std;
int flag = 0;
char LL1[50][50][100] = {
    {
        "::TX",
        "::TX",
        "null",
        "null",
        "null",
        "null",
        "null",
        "null",
    },
    {
        "::FY",
        "::FY",
        "null",
        "null",
        "null",
        "null",
        "null",
        "null",
    },
    {
        "null",
        "null",
        "::~",
        "::ATX",
        "::ATX",
        "null",
        "null",
        "::~",
    },
    {
        "null",
        "null",
        "null",
        "::+",
        "::-",
        "null",
        "null",
        "null",
    },
    {
        "::i",
        "::(E)",
        "null",
        "null",
        "null",
        "null",
        "null",
        "null",
    },
    {
        "null",
        "null",
        "null",
        "null",
        "null",
        "::*",
        "::/",
        "null",
    },
    {
        "null",
        "null",
        "::~",
        "::~",
        "::~",
        "::MFY",
        "::MFY",
        "::~",
    },
};

char H[200] = "ETXAFMY";
char L[200] = "i()+-*/$";
stack<char> cmp;

int findH(char a)
{
    for (int i = 0; i < 7; i++) //找到对应行
    {
        if (a == H[i])
        {
            return i;
        }
    }
    return -1;
}

int findL(char b)
{
    for (int i = 0; i < 8; i++) //找到对应列
    {
        if (b == L[i])
        {
            return i;
        }
    }
    return -1;
}

/*int error(int i, int cnt, int len, char p[], char str[])
{
	cout << cnt << "\t" << p << "\t";
	for (int q = i; q < len; q++)
	{
		cout << str[q];
	}
	cout << "\t报错" << endl;
	return len;
}*/

void analyze(char str[], int len)
{
    int cnt = 1; //输出Step
    int i = 0;
    char p[200] = "$E"; //输出stack
    int pindex = 2;
    cout << "Step\tStack\tString\tRule" << endl;
    while (i < len)
    {
        int x, y;
        char ch = cmp.top();
        if (ch >= 'A' && ch <= 'Z')
        {
            cmp.pop();
            x = findH(ch);
            y = findL(str[i]);
            if (x != -1 && y != -1)
            {
                int len2 = strlen(LL1[x][y]);
                if (strcmp(LL1[x][y], "null") == 0)
                {
                    cout << cnt << "\t" << p << "\t";
                    for (int q = i; q < len; q++)
                    {
                        cout << str[q];
                    }
                    cout << "\t出错,已忽略当前符号" << endl;
                    cnt++;
                    flag = 1;
                    cmp.push(ch);
                    i = i + 1;
                    continue;
                }
                cout << cnt << "\t" << p << "\t";
                if (p[pindex - 1] != '$')
                {
                    p[pindex] = '\0';
                    pindex--;
                }
                if (LL1[x][y][2] != '~')
                {
                    for (int q = len2 - 1; q > 1; q--)
                    {
                        p[pindex++] = LL1[x][y][q];
                        cmp.push(LL1[x][y][q]); //进栈
                    }
                }
                else
                {
                    p[pindex] = '\0';
                    pindex;
                }
                for (int q = i; q < len; q++)
                {
                    cout << str[q]; //输出剩余输入串
                }
                cout << "\t" << ch << LL1[x][y] << endl;
            }
            else
            {
                cout << "\t出错，已忽略当前错误" << endl;
                i = i + 1;
                //i = error(i, cnt, len, p, str);//未找到，报错
                continue;
            }
        }
        else
        {
            if (ch == str[i])
            {
                cmp.pop();
                cout << cnt << "\t" << p << "\t";
                if (ch == '$' && str[i] == '$')
                {
                    cout << "$\t接受" << endl;
                    return;
                }
                for (int q = i; q < len; q++)
                {
                    cout << str[q];
                }
                cout << "\t" << ch << "匹配" << endl;
                pindex--;
                p[pindex] = '\0';
                i++;
            }
            else
            {
                cout << "\t出错，已忽略当前错误" << endl;
                i = i + 1;

                //i = error(i, cnt, len, p, str);//报错
                continue;
            }
        }
        cnt++;
    }
}

int main()
{
    string str1;
    ifstream Prewords("test_right.txt", ios::in);
    // ifstream Prewords("test_wrong.txt", ios::in);  
    getline(Prewords, str1);
    char *str = (char *)str1.c_str();
    int len = strlen(str);
    cmp.push('$');
    cmp.push('E');
    analyze(str, len + 1);
    if (flag == 1)
        cout << "\t当前语句不合法" << endl;
    else
        cout << "\t当前语句合法" << endl;
    system("pause");
    return 0;
}

#include<iostream>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
using namespace std;
#define OK 1
#define ERROR -1
#define Max 200 //�ַ����������ֵ
typedef struct TNode{
    string Wife;//���ӵ�����,��û������ʱΪ��
    string Husband;//�ɷ������,��û���ɷ�ʱΪ��
    struct TNode *parent;//˫��ָ��
    struct TNode *brother;//�ֵ�ָ��
    struct TNode *children;//����ָ��,��ָ�����곤�ĺ���
}tree,*Tree;//��������
int AddChild(Tree &Now,string Husbandname,string Wifename);//��Ӻ��ӵ�˫�׽ڵ�Now
int AddZuXian(Tree &T,string Husbandname,string Wifename);//�������
void CaiDanMain();//���˵�����
int CheckCunZai(Tree T);//�ж�ĳ���ڵ��Ƿ��г�Ա,���򷵻�OK,���򷵻�ERROR
int CheckFile(string file);//�ж��ļ��Ƿ��Ѿ�����,���ڷ���OK�����򷵻�ERROR
int ChangeZuXian(Tree &T,string Husbandname,string Wifename);//�޸ļ��������
int ChangeNow(Tree &Now,string Husbandname,string Wifename);//�޸ĵ�ǰ�ڵ����Ϣ��Ӧ��֤��ǰ�ڵ����
void Findloc(Tree T,string &Husbandname,string &Wifename,Tree &Now);//ͨ���ɷ�����ӵ����ֲ��ҵ���Ӧ�Ľڵ㣬NowΪ��¼�ýڵ��ָ��,�ݹ�ѭ��(��ȱ���)
void PrintTree(Tree T);//���������ĵ�ϵ��Ϣ���
int PrintChild(Tree Now);//��ʾĳ���ڵ�����к���,NowΪ��ǰ�ڵ�
int PrintParent(Tree Now);//��ʾ��ǰ�ڵ��˫����Ϣ,NowΪ��ǰ�ڵ�
int PrintZuXian(Tree Now);//��ʾ����,NowΪ��ǰ�ڵ�
int PrintBrother(Tree Now);//��ʾ�ֵ�,NowΪ��ǰ�ڵ�
int PrintLater(Tree Now);//��ѯ��ǰ�ڵ�����е�ϵ�������Ϣ,��������ǰ�ڵ�,NowΪ��ǰ�ڵ�
int InitTree(Tree &T);//��ʼ��������
int InputFile(Tree T,string file);//�������������ļ���
int UploadFile(Tree &T,string file);//�����е��ļ��н���������Ϣ����

int main()
{
    Tree T;
    InitTree(T);//��ʼ��
    while(1)
    {
        //fox�洢�û������ѡ�����,��ʼֵΪ0
        CaiDanMain();//���˵���ʾ
        int fox=0;//ѡ���ѡ��
        cout<<"��ѡ��: ";
        cin>>fox;
        if(CheckCunZai(T)==ERROR&&(fox>3&&fox<13))//�����Ȳ�����ʱ������
        {
            cout<<"�����������!"<<endl;
            continue;
        }
        switch(fox)
        {
            case 0:return 0;//�˳�ϵͳ
            case 1://ѡ�������ļ����ǵ�ǰ������
                {

                    while(1){
                        //truth1����ֵΪ1ʱ��ʾ�ļ�����ɹ������˳�ѭ��;ֵΪ0ʱ�ļ�����ʧ�ܣ���ʾ��ʾ��Ϣ����ʼֵΪ0
                        string file;
                        cout<<"��������Ҫ����ļ�������: ";
                        cin>>file;
                        int truth1=0;
                        truth1=UploadFile(T,file);
                        if(truth1==OK)//���ļ��е���������ɹ�ʱ
                        {
                            cout<<"����ɹ�!"<<endl;
                            break;
                        }
                        else
                        {
                            //truth2ֵΪ1ʱ����������������ϣ�ֵΪ0ʱ�˳�����ʼֵΪ1
                            cout<<"����ʧ��!�˳���0����������������ϰ�1"<<endl;
                            int truth2=1;
                            cin>>truth2;
                            if(truth2==0) break;
                        }
                    }
                }
                break;
            case 2://����ǰ�����������ļ���
                {
                    //flagֵΪ1ʱ����������������ϣ�ֵΪ0ʱ�˳�����ʼֵΪ1
                    string file;
                    int flag=1;
                    while(1)
                    {
                    cout<<"���������������Ϊ�ļ���: ";
                    cin>>file;
                    if(CheckFile(file)==OK)//�ļ�������ʱ
                    {
                        cout<<"�ü��������Ѵ��ڣ��˳���0����������������ϰ�1"<<endl;
                        cin>>flag;
                        if(flag==0)
                            break;
                        else continue;
                    }
                    if(CheckFile(file)==ERROR) break;
                    }
                    if(flag==0)//�˳�����
                        break;
                    if(CheckFile(file)==ERROR)//�ļ���������ʱ
                    {
                        InputFile(T,file);//�������������ļ�
                        cout<<"�����ɹ�!"<<endl;
                    }
                }
                break;
            case 3://�������
                {
                    if(CheckCunZai(T)==OK)//�������Ѵ���ʱ
                    {
                        cout<<"�����Ѵ���!"<<endl;
                    }
                    else//������ʱ������������Ϣ
                    {
                        string Husbandname,Wifename;
                        cout<<"�游(����): ";
                        cin>>Husbandname;
                        cout<<"��ĸ(����): ";
                        cin>>Wifename;
                        AddZuXian(T,Husbandname,Wifename);
                    }
                }
                break;
            case 4://��Ӽ����³�Ա
                {
                    //flag1ֵΪ0ʱ�˳���Ϊ��0ֵʱ��������Ϣ����ʼֵΪ0
                    int flag1=0;//��ֵΪ0ʱ->�˳��˲���
                    string Fathername,Mothername,Husbandname,Wifename;
                    while(1)//������ĸ��׺�ĸ���ҵ�ʱ�˳�ѭ��
                    {
                        //flag2ֵΪ0ʱ�˳���Ϊ��0ֵʱ��������˫����Ϣ����ʼֵΪ1
                        cout<<"����: ";
                        cin>>Fathername;
                        cout<<"ĸ��: ";
                        cin>>Mothername;
                        Tree truth=NULL;//ΪNULLʱ->��ʾ��ʾ��Ϣ,��ΪNULLʱ->�����Ա��Ϣ
                        Findloc(T,Fathername,Mothername,truth);
                        if(truth==NULL)
                        {
                            int flag2=1;
                            cout<<"δ�ҵ�˫�ף��˳���0����������˫�װ�1"<<endl;
                            cin>>flag2;
                            if(flag2==0) break;
                            else continue;
                        }
                        else
                        {
                            cout<<"���ҳɹ�!�������Ա��Ϣ"<<endl;
                            flag1=1;//���ҳɹ�ʱ��ֵΪ1��ִ�������Ա��Ϣ
                            break;
                        }
                    }
                    if(flag1==0) break;//��Ϊ�˳�ʱ������������
                    Tree Now;//��ʱָ��
                    cout<<"�ɷ�: ";
                    cin>>Husbandname;
                    cout<<"����: ";
                    cin>>Wifename;
                    Findloc(T,Fathername,Mothername,Now);//Ѱ��˫�׵Ľڵ�
                    AddChild(Now,Husbandname,Wifename);//��Ӻ���
                    while(1)
                    {
                        int flag3;//ֵΪ0ʱ�˳�ѭ��
                        cout<<"�������ֵܰ�1���˳�ϵͳ��0"<<endl;
                        cin>>flag3;
                        if(flag3==0)
                            break;
                        cout<<"�ɷ�: ";
                        cin>>Husbandname;
                        cout<<"����: ";
                        cin>>Wifename;
                        AddChild(Now,Husbandname,Wifename);//ֱ��ʹ��˫�׽ڵ�
                    }
                }
                break;
            case 5://�޸ļ�������
                {
                    string Husbandname,Wifename;
                    cout<<"�游(����): ";
                    cin>>Husbandname;
                    cout<<"��ĸ(����): ";
                    cin>>Wifename;
                    ChangeZuXian(T,Husbandname,Wifename);
                    cout<<"�޸ĳɹ���"<<endl;
                }
                break;
            case 6://�޸�ĳ�������Ա
                {
                    //flagֵΪ0ʱ�˳���ֵΪ1ʱ���������Ա��Ϣ�����Ҹó�Ա����ʼֵΪ1
                    string Husbandname,Wifename;
                    int flag=1;
                    while(1){
                        //truthֵΪNULLʱ�����ڸó�Ա����ΪNULLʱ�ɹ��ҵ���Ա��Ϣ,��ʼֵΪNULL
                        cout<<"�ɷ�(��): ";
                        cin>>Husbandname;
                        cout<<"����(��): ";
                        cin>>Wifename;
                        Tree truth=NULL;
                        Findloc(T,Husbandname,Wifename,truth);
                        if(truth)//���ڸó�Աʱ
                        {
                            cout<<"���ҳɹ�!"<<endl;
                            break;
                        }
                        else//�����ڸó�Աʱ
                        {
                            cout<<"����ʧ�ܣ��˳���0���������밴1!"<<endl;
                            cin>>flag;
                            if(flag==0) break;
                        }
                    }
                    if(flag==0)break;//�˳�����
                    Tree Now;//��ʱָ��
                    Findloc(T,Husbandname,Wifename,Now);
                    cout<<"�ɷ�(��): ";
                    cin>>Husbandname;
                    cout<<"����(��): ";
                    cin>>Wifename;
                    ChangeNow(Now,Husbandname,Wifename);
                }
                break;
            case 7://��ʾ���������
                {
                    PrintZuXian(T);
                }
                break;
            case 8://��ʾĳ����Ա��˫��
                {
                    //NowΪNULLʱ���޳�Ա��Ϣ����ΪNULLʱ���г�Ա��Ϣ����ʼֵΪNULL
                    string Husbandname,Wifename;
                    cout<<"�ɷ�: ";
                    cin>>Husbandname;
                    cout<<"����: ";
                    cin>>Wifename;
                    Tree Now=NULL;//��ʱָ��
                    Findloc(T,Husbandname,Wifename,Now);
                    if(Now)
                        PrintParent(Now);
                    else cout<<"�ó�Ա��Ϣ�����ڣ��޷��ҳ�˫��!"<<endl;;
                }
                break;
            case 9://��ʾ�ֵ�
                {
                    while(1){
                        //NowΪNULLʱ���޳�Ա��Ϣ����ΪNULLʱ���г�Ա��Ϣ����ʼֵΪNULL
                        string Husbandname,Wifename;
                        cout<<"�ɷ�: ";
                        cin>>Husbandname;
                        cout<<"����: ";
                        cin>>Wifename;
                        Tree Now=NULL;//��ʱָ��
                        Findloc(T,Husbandname,Wifename,Now);
                        if(Now)//�ҵ���Ϣʱ
                        {
                            PrintBrother(Now);
                            break;
                        }
                        else//δ�ҵ���Ա��Ϣʱ
                        {
                            //truthֵΪ0ʱ�˳�����0ʱ���������Ա��Ϣ����ʼֵΪ1
                            cout<<"�޸ó�Ա��Ϣ���˳���0������������Ϣ��1"<<endl;
                            int truth=1;
                            cin>>truth;
                            if(truth==0)
                                break;
                        }
                    }
                }
                break;
            case 10://��ʾ����
                {
                    while(1)
                        {
                        //NowΪNULLʱ���޳�Ա��Ϣ����ΪNULLʱ���г�Ա��Ϣ����ʼֵΪNULL
                        string Husbandname,Wifename,Sonname,SonWifename;
                        cout<<"�ɷ�: ";
                        cin>>Husbandname;
                        cout<<"����: ";
                        cin>>Wifename;
                        Tree Now=NULL;//��ʱָ��
                        Findloc(T,Husbandname,Wifename,Now);//�ҵ��ڵ�λ��
                        if(Now)//�ҵ��ڵ�ʱ
                        {
                            if(PrintChild(Now)==ERROR)
                            {
                                cout<<"�ó�Ա�޺���!���˳���0������������Ϣ��1"<<endl;
                                int truth=0;
                                cin>>truth;
                                if(truth==0)
                                    break;
                            }
                            else break;
                        }
                        else//δ�ҵ���Ա��Ϣʱ
                        {
                            cout<<"�޸ó�Ա��Ϣ���˳���0������������Ϣ��1"<<endl;
                            int truth=0;
                            cin>>truth;
                            if(truth==0)
                                break;
                        }
                    }
                }
                break;
            case 11://��ʾ���к��
                {
                    while(1)
                    {
                        //NowΪNULLʱ���޳�Ա��Ϣ����ΪNULLʱ���г�Ա��Ϣ����ʼֵΪNULL
                        string Husbandname,Wifename;
                        cout<<"�ɷ�: ";
                        cin>>Husbandname;
                        cout<<"����: ";
                        cin>>Wifename;
                        Tree Now=NULL;
                        Findloc(T,Husbandname,Wifename,Now);
                        if(Now)//����ʱ
                        {
                            PrintLater(Now);
                            break;
                        }
                        else//�����ڸó�Աʱ
                        {
                            cout<<"�޸ó�Ա��Ϣ���˳���0������������Ϣ��1"<<endl;
                            int truth=0;
                            cin>>truth;
                            if(truth==0)
                                break;
                        }
                    }
                }
                break;
            case 12://��ʾ������
                {

                    PrintTree(T);
                }
                break;
            case 13://��������
                {
                    system("cls");
                }
                break;
            default:break;
        }
    }
    return 0;
}
int AddChild(Tree &Now,string Husbandname,string Wifename)//��Ӻ��ӵ�˫�׽ڵ�Now
{
    if(Now->children==NULL)//û�к���ʱ
    {
        Tree p;//�����µĽڵ�
        p=new TNode;
        p->brother=NULL;
        p->children=NULL;
        p->Husband=Husbandname;
        p->Wife=Wifename;
        p->parent=Now;
        Now->children=p;
    }
    else//�к���ʱ
    {
        Tree now;
        now=Now->children;//ָ��������
        while(now->brother!=NULL)//���ֵ��н���ѭ������
            now=now->brother;
        Tree p;//�����µĽڵ�
        p=new TNode;
        p->brother=NULL;
        p->children=NULL;
        p->Husband=Husbandname;
        p->Wife=Wifename;
        p->parent=Now;
        now->brother=p;//now��ʱΪĩλ���ֵܽڵ�
    }
    return OK;
}
int AddZuXian(Tree &T,string Husbandname,string Wifename)//�������
{
    Tree t;
    t=T;
    t->Husband=Husbandname;
    t->Wife=Wifename;
    t->brother=NULL;
    t->parent=NULL;
    t->children=NULL;
    return OK;
}

void CaiDanMain()//���˵�����
{
    cout<<endl;
    cout<<"-----------------------------<���˵�>----------------------------------"<<endl;
    cout<<"|    0 �˳�ϵͳ                                                        |"<<endl;
    cout<<"|    1 ѡ�����м����׸��ǵ�ǰ������                                    |"<<endl;
    cout<<"|    2 ����ϵ�����������ļ�                                            |"<<endl;
    cout<<"|    3 �������                                                        |"<<endl;
    cout<<"|    4 ��Ӽ����³�Ա[���곤���ֵ�Ӧ�������룬������ʱ����д��]        |"<<endl;
    cout<<"|    5 �޸ļ�������                                                    |"<<endl;
    cout<<"|    6 �޸�ĳ�������Ա                                                |"<<endl;
    cout<<"|    7 ��ʾ���������                                                  |"<<endl;
    cout<<"|    8 ��ʾĳ����Ա��˫��                                              |"<<endl;
    cout<<"|    9 ��ʾĳ����Ա���ֵ�                                              |"<<endl;
    cout<<"|   10 ��ʾĳ����Ա�ĺ���                                              |"<<endl;
    cout<<"|   11 ��ʾĳ����Ա�����е�ϵ���                                      |"<<endl;
    cout<<"|   12 ��ʾ��ϵ��������Ϣ                                              |"<<endl;
    cout<<"|   13 ����                                                            |"<<endl;
    cout<<"------------------------------------------------------------------------"<<endl;
    cout<<endl;
}
int CheckCunZai(Tree T)//�ж�ĳ���ڵ��Ƿ��г�Ա,���򷵻�OK,���򷵻�ERROR
{
    Tree t;//��ʱָ��
    t=T;
    if(t->Husband==""&&t->Wife=="")//�Ե�ǰ�ڵ�����ж�
    {
        return ERROR;
    }
    else return OK;
}
int CheckFile(string file)//�ж��ļ��Ƿ��Ѿ�����,���ڷ���OK�����򷵻�ERROR
{
    //flagΪ�ļ������flag=��1.find(��2)
    //flag�ķ���ֵΪstring::nposʱ���ʾ��2��Ϊ��1���Ӵ�
    string::size_type flag;
    flag=file.find(".txt");
    if(flag==string::npos)//�����к�׺.txtʱ,��Ӻ�׺
        file+=".txt";
    FILE*fp;
    char filename[Max];
    strcpy(filename,file.c_str());
    if((fp=fopen(filename,"r"))==NULL)//������ʱ����ERROR
        return ERROR;
    else return OK;
}
int ChangeZuXian(Tree &T,string Husbandname,string Wifename)//�޸ļ��������
{
    Tree t;
    t=T;
    if(t==NULL)
        return ERROR;
    t->Husband=Husbandname;
    t->Wife=Wifename;
    return OK;
}
int ChangeNow(Tree &Now,string Husbandname,string Wifename)//�޸ĵ�ǰ�ڵ����Ϣ��Ӧ��֤��ǰ�ڵ����
{
    Tree now;
    now=Now;
    now->Husband=Husbandname;
    now->Wife=Wifename;
    return OK;
}
void Findloc(Tree T,string &Husbandname,string &Wifename,Tree &Now)//ͨ���ɷ�����ӵ����ֲ��ҵ���Ӧ�Ľڵ㣬NowΪ��¼�ýڵ��ָ��,�ݹ�ѭ��(��ȱ���)
{
    if(T==NULL)
        return;
    else
    {
        if(Husbandname==T->Husband&&Wifename==T->Wife)//����ǰ�ڵ����������Nowָ��ǰ�ڵ�
            Now=T;
        Findloc(T->children,Husbandname,Wifename,Now);//�ȶԺ��ӽ��б���
        Findloc(T->brother,Husbandname,Wifename,Now);//����ֵܽ��б���
    }
}
void PrintTree(Tree T)//���������ĵ�ϵ��Ϣ���
{
    //time��ʾ�ڼ������ӣ���ʼֵΪ1
    Tree childs;//��ʱָ��
    childs=T;
    int time=1;//�ڼ���
    while(childs)//�ں����н���ѭ��
    {
        //cout<<"��"<<time<<"��:"<<endl;
        if(time!=1)
            cout<<"��"<<time<<"��:"<<endl;
        else//���Ƚڵ�ʱ
            cout<<"��1��(����):"<<endl;
        Tree brothers;
        brothers=childs;
       while(brothers)//���ֵ��н���ѭ��
       {
           cout<<brothers->Husband<<"(��) ";
           cout<<brothers->Wife<<"(Ů)"<<endl;
           brothers=brothers->brother;
       }
       time++;
       childs=childs->children;
    }
}
int PrintChild(Tree Now)//��ʾĳ���ڵ�����к���,NowΪ��ǰ�ڵ�
{
    cout<<endl;
    Tree k;
    if(Now->children==NULL)//û�к���ʱ�����ش���
    {
        return ERROR;
    }
    k=Now->children;
    cout<<"[����]"<<endl;
    while(k)//���ֵ��н���ѭ��
    {
        cout<<k->Husband<<"(��) ";
        cout<<k->Wife<<"(Ů)"<<endl;
        k=k->brother;
    }
    return OK;
}
int PrintParent(Tree Now)//��ʾ��ǰ�ڵ��˫����Ϣ,NowΪ��ǰ�ڵ�
{
    cout<<endl;
    Tree now;
    now=Now;
    Tree parents=NULL;//parentsΪָ��ǰ�ڵ㸸ĸ��ָ��
    if(now->parent==NULL)//��nowΪ���Ƚڵ�ʱ�޸��׺�ĸ��
    {
        cout<<"��ǰΪ���ȣ�û��˫��!"<<endl;
        return ERROR;
    }
    else parents=now->parent;
    cout<<"[����] ";
    cout<<parents->Husband<<endl;
    cout<<"[ĸ��] ";
    cout<<parents->Wife<<endl;
    return OK;
}
int PrintZuXian(Tree Now)//��ʾ����,NowΪ��ǰ�ڵ�
{
    Tree k;//��ʱָ��
    k=Now;
    while(k->parent!=NULL)//������ѭ����û��˫�׵Ľ��ʱΪ����
    {
        k=k->parent;
    }
    cout<<"[����] "<<endl;//���ȹ���������
    cout<<k->Husband<<"(��) ";
    cout<<k->Wife<<"(Ů)"<<endl;
    return OK;
}
int PrintBrother(Tree Now)//��ʾ�ֵ�,NowΪ��ǰ�ڵ�
{
    Tree k;//��ʱָ��
    if(Now->parent==NULL)//��Ϊ����ʱ
    {
        cout<<"�������ֵ�!"<<endl;
        return ERROR;
    }
    k=Now->parent->children;//ָ��������
    if(k->brother!=NULL)
    {
        cout<<"[�ֵ�] "<<endl;
    }
    else
        {
            cout<<"���ֵ�!"<<endl;
            return ERROR;
        }
    while(k)//�ڽڵ���ֵ��н���ѭ�����
    {
        if(k->Husband!=Now->Husband&&k->Wife!=Now->Wife)//�������ǰ�ڵ�ĳ�Ա��Ϣ,������ֵܵ���Ϣ
        {
            cout<<k->Husband<<"(��) ";
            cout<<k->Wife<<"(Ů)"<<endl;
        }
        k=k->brother;
    }
    return OK;
}
int PrintLater(Tree Now)//��ѯ��ǰ�ڵ�����е�ϵ�������Ϣ,��������ǰ�ڵ�,NowΪ��ǰ�ڵ�
{
    //time��ʾ�ڼ�������,��ʼֵΪ1
    Tree k;
    if(Now->children==NULL)//�޺���ʱ
    {
        cout<<"�޺����Ϣ"<<endl;
        return ERROR;
    }
    k=Now->children;
    int time=1;//��ʾ�ڼ����ĺ���
    while(k)//������ѭ��
    {
        Tree p;
        p=k;
        cout<<"��"<<time<<"��:"<<endl;
        while(p)//�ֵ���ѭ��
        {
            cout<<p->Husband<<"(��) ";
            cout<<p->Wife<<"(Ů)"<<endl;
            p=p->brother;
        }
        time++;
        k=k->children;
    }
    return OK;
}
int InitTree(Tree &T)//��ʼ��������
{
    T=new TNode;//����ͷ�ڵ�
    T->Husband="";//��ʼ��Ϊ��
    T->Husband="";
    T->children=NULL;
    T->brother=NULL;
    T->parent=NULL;
    return OK;
}
int InputFile(Tree T,string file)//����ϵ�����������ļ���
{
    //flagΪ�ļ������flag=��1.find(��2)
    //����2��Ϊ��1�Ӵ�ʱ��flagֵΪstring::npos
    if(CheckFile(file)==OK)//�ļ��Ѵ���ʱ���ش���
        return ERROR;
    string::size_type flag;
    flag=file.find(".txt");
    if(flag==string::npos)//û�к�׺ʱ��Ӻ�׺
        file+=".txt";
    FILE*fp;
    char filename[Max];
    char Husbandname[Max],Wifename[Max];
    char Fathername[Max],Mothername[Max];
    strcpy(filename,file.c_str());
    fp=fopen(filename,"w");
    Tree k;//����ָ��
    k=T;
    while(k)
    {
        Tree p;//�ֵ�ָ��
        p=k;
        while(p)
        {
            if((p->Husband!=T->Husband)&&(p->Wife!=T->Wife))//��Ϊ���Ƚڵ�ʱ,����ĸ��Ϣ����
            {
                strcpy(Fathername,(p->parent->Husband).c_str());
                strcpy(Mothername,(p->parent->Wife).c_str());
                fprintf(fp,"%s ",Fathername);
                fprintf(fp,"%s\t",Mothername);
            }
            strcpy(Husbandname,(p->Husband).c_str());
            strcpy(Wifename,(p->Wife).c_str());
            fprintf(fp,"%s ",Husbandname);//д�뵱ǰ�ڵ���Ϣ
            fprintf(fp,"%s\n",Wifename);
            p=p->brother;
        }
        k=k->children;
    }
    fprintf(fp,"#\n");//������־
    fclose(fp);//�ر��ļ�
    return OK;
}
int UploadFile(Tree &T,string file)//�����е��ļ��н���������Ϣ����,�����ļ��������򷵻�ERROR
{
    //flagΪ�ļ������flag=��1.find(��2)
    //����2��Ϊ��1�Ӵ�ʱ��flagֵΪstring::npos
    string::size_type flag;//�ж��Ƿ��к�׺.txt
    flag=file.find(".txt");
    if(flag==string::npos)//������׺ʱ��Ӻ�׺.txt
        file+=".txt";
    char filename[Max];
    strcpy(filename,file.c_str());//��file���Ƶ�filename
    FILE*fp=fopen(filename,"r");//�������filenameΪ�ַ���������
    if(fp==NULL)//�ļ�������ʱ
        return ERROR;
    char ZuFu[Max],ZuMu[Max];
    fscanf(fp,"%s",ZuFu);//��������Ϣ����
    fscanf(fp,"%s",ZuMu);
    string ZuFuname(ZuFu);
    string ZuMuname(ZuMu);
    AddZuXian(T,ZuFuname,ZuMuname);
    while(1)
    {
        char Father[Max],Mother[Max];
        char Husband[Max],Wife[Max];
        fscanf(fp,"%s",Father);
        string Fathername(Father);
        if(Fathername=="#")
            break;
        fscanf(fp,"%s",Mother);
        fscanf(fp,"%s",Husband);
        fscanf(fp,"%s",Wife);
        string Mothername(Mother);
        string Husbandname(Husband);
        string Wifename(Wife);
        Tree Now;
        Findloc(T,Fathername,Mothername,Now);//�ҵ�˫�׽ڵ�
        if(Now)//����˫�׽ڵ�ʱ
            AddChild(Now,Husbandname,Wifename);
    }
    fclose(fp);
    return OK;
}
#ifndef OS_FILESYSTEM_H
#define OS_FILESYSTEM_H
#include "global.h"
#include "FCB.h"
#include "memalloc.h"
// static const string TreeFileName = "tree2.txt";
static const string TreeFileName = "tree.txt";

class FileSystem
{
private:
    FCB * root;
    FCB * curFCB;
    string curUser = "rock";
    //������ģ��
    Memalloc* mem_alloc;
public:
    FileSystem();
    ~FileSystem();

    //��ȡĬ��Ŀ¼�ļ�
    void init();
    //�µĶ�ȡĿ¼�ļ�����
    void init2();
    //��ӡĿ¼��Ϣ
    void showTree();

//����ģ��
public:
    void analyse(string s);
    //����ǰ�ļ�Ŀ¼��Ϣ�����ļ�
    void updateTreeFile();
    //�汾2 �洢Ŀ¼�ļ�
    void updateTreeFile2();
    //����ת��Ϊ�ַ��� num > 0
    string num2string(int num);
    //�жϵ�ǰĿ¼�Ƿ����ĳ�ļ�
    bool isExistFile(const string& filename);
    //�жϵ�ǰĿ¼�Ƿ����ĳĿ¼
    bool isExistDir(const string& dirname);
    //�ڵ�ǰĿ¼����ӽڵ�
    void addNode(FCB* curNode, FCB* newNode);
    //�ڵ�ǰĿ¼Ѱ��ĳ�ļ�
    FCB* findFile(FCB* curNode, const string& name);
    //����FCB��
    bool generateFCB(const string& line, FCB*& gfcb);
    //ȷ����Ȩ��
    bool isPermitRead(const string& curUser, FCB* pfcb);
    //ȷ��дȨ��
    bool isPermitWrite(const string& curUser, FCB* pfcb);
    //���� dirTree ����һ��path������ĩβ�Ľڵ�
    TreeNode* matchPath(string path);
    //�������ɾ���ļ�����
    void postOrderDelSubTree(TreeNodePtr &t);
    //��һ���ļ����ļ����ͷ����ݿ�
    int releaseExternalStorage(FCB& fcb);
    //�ַ�����Ƭ
    vector<string> split(const string& str, const string& delim);
    //Tree ����
    void TreeChild(FCB* cur, int depth);
    //��ȡ��ǰ�ڵ����
    int getCurDepth(FCB* cur);
    //��FCB��ת��λline�ַ��� countΪ0���Ϊ����
    string FCB2Line(FCB* cur, int count);
// ����ģ��
public:
    //ʵ��tree���� ������� չʾcur�������
    void Tree(FCB* cur);
    //ls ģ�� ���ʵ��
    void Ls(FCB* cur);
    //չʾ����Ŀ¼
    void Pwd(FCB* cur);
    //�ڵ�ǰĿ¼�����ļ�
    void Touch(const string& fileName,int size);
    //�ڵ�ǰĿ¼����Ŀ¼
    void Mkdir(const string& dirName);
    //ɾ���ļ�
    void Rm(const string& fileName);
    // void Rm(const string path);
    //��Ȩ�޵�ls
    void Ls_l(FCB* cur);
    //�޸�Ȩ������ chmod xxx file
    void Chmod(int LimitNum, string fileName);
    //��ȡ�ļ�ģ��
    void ReadFile(const string& fileName);
    //�л�Ŀ¼
    void Cd(const string& dirName);
    //��дĳ���ļ�
    void ViFile(const string& fileName, const string& toWriteStr);
// ����ģ��
public:
    void preOrder(FCB* cur);
    void testTree(FCB* cur);
    void TestTreeChild(FCB* cur, int depth);
    void test();
    //0428���
    void test2();
    //0429
    void test3();
    //0523
    void test4();
    //0523 ����RM�Ƿ���ȷ
    void test5();
};
#endif //OS_FILESYSTEM_H
#pragma once
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
    //外存分配模块
    Memalloc* mem_alloc;
public:
    FileSystem();
    ~FileSystem();

    //读取默认目录文件
    void init();
    //新的读取目录文件函数
    void init2();
    //打印目录信息
    void showTree();

//辅助模块
public:
    void analyse(string s);
    //将当前文件目录信息存入文件
    void updateTreeFile();
    //版本2 存储目录文件
    void updateTreeFile2();
    //数字转化为字符串 num > 0
    string num2string(int num);
    //判断当前目录是否存在某文件
    bool isExistFile(const string& filename);
    //判断当前目录是否存在某目录
    bool isExistDir(const string& dirname);
    //在当前目录下添加节点
    void addNode(FCB* curNode, FCB* newNode);
    //在当前目录寻找某文件
    FCB* findFile(FCB* curNode, const string& name);
    //构造FCB块
    bool generateFCB(const string& line, FCB*& gfcb);
    //确定读权限
    bool isPermitRead(const string& curUser, FCB* pfcb);
    //确定写权限
    bool isPermitWrite(const string& curUser, FCB* pfcb);
    //在树 dirTree 上找一条path，返回末尾的节点
    TreeNode* matchPath(string path);
    //后序遍历删除文件子树
    void postOrderDelSubTree(TreeNodePtr &t);
    //给一个文件或文件夹释放数据块
    int releaseExternalStorage(FCB& fcb);
    //字符串切片
    vector<string> split(const string& str, const string& delim);
    //Tree 补丁
    void TreeChild(FCB* cur, int depth);
    //获取当前节点深度
    int getCurDepth(FCB* cur);
    //将FCB块转化位line字符串 count为0标记为空行
    string FCB2Line(FCB* cur, int count);
// 功能模块
public:
    //实现tree功能 先序遍历 展示cur树的情况
    void Tree(FCB* cur);
    //ls 模块 最简单实现
    void Ls(FCB* cur);
    //展示当下目录
    void Pwd(FCB* cur);
    //在当前目录创建文件
    void Touch(const string& fileName,int size);
    //在当前目录创建目录
    void Mkdir(const string& dirName);
    //删除文件
    void Rm(const string& fileName);
    // void Rm(const string path);
    //带权限的ls
    void Ls_l(FCB* cur);
    //修改权限命令 chmod xxx file
    void Chmod(int LimitNum, string fileName);
    //读取文件模块
    void ReadFile(const string& fileName);
    //切换目录
    void Cd(const string& dirName);
    //重写某个文件
    void ViFile(const string& fileName, const string& toWriteStr);
// 测试模块
public:
    void preOrder(FCB* cur);
    void testTree(FCB* cur);
    void TestTreeChild(FCB* cur, int depth);
    void test();
    //0428添加
    void test2();
    //0429
    void test3();
    //0523
    void test4();
    //0523 测试RM是否正确
    void test5();
};
#endif //OS_FILESYSTEM_H
#pragma once
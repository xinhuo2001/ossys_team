#include "FCB.h"
#include<fstream>
#include<queue>

static const string TreeFileName = "tree.txt";

class FileSystem
{
private:
    FCB * root;
public:
    FileSystem();
    ~FileSystem();

    //读取默认目录文件
    void init();
    //打印目录信息
    void showTree();
// 功能模块
public:
    //实现tree功能 先序遍历 展示cur树的情况
    void Tree(FCB* cur, int depth);
    //ls 模块 最简单实现
    void Ls(FCB* cur);

// 测试模块
public:
    void preOrder(FCB* cur);
    void test();
};
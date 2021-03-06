#include "FCB.h"
#include<fstream>
#include<queue>
#include<vector>
#include<algorithm>

// static const string TreeFileName = "tree2.txt";
static const string TreeFileName = "tree.txt";

class FileSystem
{
private:
    FCB * root;
    FCB * curFCB;
public:
    FileSystem();
    ~FileSystem();

    //读取默认目录文件
    void init();
    //打印目录信息
    void showTree();

//辅助模块
public:
    //将当前文件目录信息存入文件
    void updateTreeFile();
    //数字转化为字符串 num > 0
    string num2string(int num);
    //判断当前目录是否存在某文件
    bool isExistFile(const string& filename);
    //判断当前目录是否存在某目录
    bool isExistDir(const string& dirname);
    //在当前目录下添加节点
    void addNode(FCB* curNode, FCB* newNode);
 	//在树 dirTree 上找一条path，返回末尾的节点
    TreeNode*& FileSystem::matchPath(string path);
    //后序遍历删除文件子树
    void FileSystem::postOrderDelSubTree(Tree &t);
    //给一个文件或文件夹释放数据块
    int FileSystem::releaseExternalStorage(FCB& fcb);
    //字符串切片
    vector<string> FileSystem::split(const string& str, const string& delim);
    
// 功能模块
public:
    //实现tree功能 先序遍历 展示cur树的情况
    void Tree(FCB* cur, int depth);
    //ls 模块 最简单实现
    void Ls(FCB* cur);
    //展示当下目录
    void Pwd(FCB* cur);
    //在当前目录创建文件
    void Touch(const string& fileName,int size);
    //在当前目录创建目录
    void Mkdir(const string& dirName);
    //删除文件及路径
    void Rm(const string path);

// 测试模块
public:
    void preOrder(FCB* cur);
    void test();
};
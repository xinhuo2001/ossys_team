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
};
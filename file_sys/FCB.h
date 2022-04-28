#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

//定义文件类型
static const char DIR_TYPE = '0';
static const char FILE_TYPE = '1';

class FCB
{
public:
	string name; //名称
	char type; //1为文件夹，2为文件
	
	// 树指针
	FCB* parent;
	FCB* child;
	FCB* sibling;
	
	int size;//占据内存大小

	int locationLen = 0; //存储向量长度
	vector<int> location; //文件的存储位置
	int beginByte = 0; //文件起始块位置
	int endByte = 0; //文件结束块位置

	int rwx; //后期权限控制

	string uname; //文件所属用户
public:
    FCB(char type, const string& name, int size, FCB* parent, FCB* child, FCB* sibling);
	FCB(const string& name);
	//展示模块信息
	void showSelf();
	//详细展示模块
	void showSelf_l();
//set函数
public:
	void setParent(FCB* parent);
	void setChild(FCB* child);
	void setSibling(FCB* sibling);
	void setRWX(int num);
//辅助模块
public:
	string getRWX();
	string num2RWX(int num);
	//分类文件和目录打印名字
	string showName();
//确认类型模块
public:
	bool isFile();
	bool isDir();
};
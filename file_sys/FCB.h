#include<iostream>
#include<vector>
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
	vector<int> location; //文件的存储位置
	bool rwx[3]; //后期权限控制
public:
    FCB(char type, const string& name, int size, FCB* parent, FCB* child, FCB* sibling);
	FCB(const string& name);
	//展示模块信息
	void showSelf();
public:
	void setParent(FCB* parent);
	void setChild(FCB* child);
	void setSibling(FCB* sibling);
//确认类型模块
public:
	bool isFile();
	bool isDir();
};
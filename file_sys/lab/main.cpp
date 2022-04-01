#include <iostream>
#include <io.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#define depth_limit 1   //开启深度限制
#define max_depth 1     //最大深度2层
using namespace std;
//链式二叉树数据结构
typedef struct BiTNode
{
	char data[100];//数据域，存放目录及文件名
	struct BiTNode *lchild, *rchild;//左右孩子
}BiTNode, *BiTree;
int q = 0;
//创建二叉树 规定数据域为"#"，则为空 先序创建
void InitTree(BiTree &T, vector<string> files)
{
	if ("#" == files[q])
	{
		q++;
		T = NULL;
	}
	else {
		T = new BiTNode;
		strcpy_s(T->data,strlen(files[q].c_str())+1,files[q].c_str());
		q++;
		InitTree(T->lchild,files);
		InitTree(T->rchild,files);
	}
}
 
// 二叉树赋值
void TreeAssign(BiTree &T,const char *path)
{
	if ("#" == path) T = NULL;
	else
	{
		T = new BiTNode;
		strcpy_s(T->data,strlen(path)+1,path);
	}
}
 
//二叉树结点计算
int Count(BiTree T)
{
	if (T == NULL)
		return 0;
	else
		return 1 + Count(T->lchild) + Count(T->rchild);
}
 
//先序遍历-递归
void PreOrder(BiTree T,int depth)
{
	if (T != NULL)
	{
		for (int j = 0; j < depth; j++) 
		{
			 if (j == depth - 1)
			{
				cout << "|______";
			}
			else
			cout << "      ";
		}
		cout << T->data<<endl;
		PreOrder(T->lchild,depth+1);//递归先序遍历左右子树
		PreOrder(T->rchild,depth);
	}
}
 
// 读取文件大小
size_t GetFileSize(const std::string& file_name) {
	std::ifstream in(file_name.c_str());
	in.seekg(0, std::ios::end);
	size_t size = in.tellg();
	in.close();
	return size; //单位是：byte
}
 
//读取所有目录及文件 
void getAllFiles(string path, vector<string>& files, unsigned long long &filesize, int depth)
{
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;  //很少用的文件信息读取结构
	string p;  //string类很有意思的一个赋值函数:assign()，有很多重载版本
			   //有文件或目录
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))  //比较文件类型是否是目录
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					files.push_back(p.assign(fileinfo.name));
					if (depth_limit&&depth == max_depth)
					{
						files.push_back("#");
					}
					else
					{
						getAllFiles(p.assign(path).append("/").append(fileinfo.name), files, filesize, depth + 1);
					}
				}
			}
			else
			{
				files.push_back(p.assign(fileinfo.name));
				filesize += GetFileSize(p.assign(path).append("/").append(fileinfo.name));
				//文件没有左孩子
				files.push_back("#");
			}
		} while (_findnext(hFile, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1
		_findclose(hFile);
		//目录遍历结束
		files.push_back("#");
	}
	//没有文件或目录
	else
	{
		files.push_back("#");
	}
}
 
//主函数
int main() {
	char * inPath = "F:/";
	vector<string> files;
	BiTree T;
	files.push_back(inPath);
	unsigned long long filesize = 0;
	getAllFiles(inPath, files, filesize,0);
	files.push_back("#");
	TreeAssign(T, inPath);
	TreeAssign(T->rchild, "#");
	InitTree(T, files);
	cout << "共" << Count(T) << "个目录及文件," << "文件总大小为" << filesize << "字节，目录及文件如下：" << endl;
	PreOrder(T,0);
	return 0;
}
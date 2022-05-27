#ifndef OS_FCB_H
#define OS_FCB_H
#include"global.h"
using namespace std;
//�����ļ�����
static const char DIR_TYPE = '0';
static const char FILE_TYPE = '1';

class FCB
{
public:
	string name; //����
	char type; //1Ϊ�ļ��У�2Ϊ�ļ�
	
	// ��ָ��
	FCB* parent;
	FCB* child;
	FCB* sibling;
	
	int size;//ռ���ڴ��С

	int locationLen = 0; //�洢��������
	vector<int> location; //�ļ��Ĵ洢λ��
	int beginByte = 0; //�ļ���ʼ��λ��
	int endByte = 0; //�ļ�������λ��

	int rwx; //����Ȩ�޿���

	string uname; //�ļ������û�
public:
    FCB(char type, const string& name, int size, FCB* parent, FCB* child, FCB* sibling);
	FCB(const string& name);
	//չʾģ����Ϣ
	void showSelf();
	//��ϸչʾģ��
	void showSelf_l();
//set����
public:
	void setParent(FCB* parent);
	void setChild(FCB* child);
	void setSibling(FCB* sibling);
	void setRWX(int num);
//����ģ��
public:
	string getRWX();
	string num2RWX(int num);
	//�����ļ���Ŀ¼��ӡ����
	string showName();
//ȷ������ģ��
public:
	bool isFile();
	bool isDir();
};

typedef FCB TreeNode;
typedef FCB* TreeNodePtr;
#endif //OS_FCB_H
#pragma once
#ifndef OS_MM_H
#define OS_MM_H
#include"global.h"
using namespace std;

static const int FRAME_PAGE_BEGIN = 20; //��ʼ֡ҳ��
static const int FRAME_PAGE_SIZE = 1000; //ÿһ֡ 1000�ֽ�
static const int FRAME_PAGE_NUM = 50; //����50֡
static const int MAX_MEM_SIZE = FRAME_PAGE_SIZE * FRAME_PAGE_NUM; //Ĭ��50000�ֽڿռ�

/*���п�����ݽṹ*/
struct free_block{
    int size;
    int start_addr;
    struct free_block * next;
    //Ĭ�Ϲ���
    free_block()
     : size(0), start_addr(MAX_MEM_SIZE), next(nullptr) {}
    //�вι���
    free_block(int val, int addr, struct free_block * nptr)
     : size(val), start_addr(addr), next(nptr) {}
};  
typedef struct free_block fbt;
typedef struct free_block* fbtPtr;

class Memalloc
{
private:
    fbtPtr free_block_head;     //���п�ͷָ��
    fbtPtr alloc_block_head;    //æ��ͷָ��
    int bolck_size;             //���ڴ�
// ����ģ��
public:
    //�޲ι���
    Memalloc();
    //�����ʼ��
    Memalloc(int size);
    //չʾ��ǰ�洢�������
    void showMemAllocInfo();
    //չʾfbt�����
    void showfbt(struct free_block* fptr);
    //�����Ӧ�㷨 ʧ�ܷ���-1 �ɹ������׵�ַ
    int worstFit(int msize);
    //����һ��ռ�
    int allocMem(int size);
    //����ӿ� ����ռ� ����vector<int> beginPage/-1 endPage beginByte endByte
    vector<int> allocMemory(int size);
    //�ͷ�һ���ڴ�
    bool freeMem(int startAddr, int size);
    //����ӿ�
    bool freeMemory(vector<int>& memV);
    //����ӿ� ���һ��������
    bool addMemByVec(vector<int>& memV);
    //����ӿ� ����æ������(��ʼ��)���п�
    bool correctFreeBlockList();
//����ģ��
public:
    //���� �ϲ������
    void sortAndMerge(fbtPtr head);
    //�鲢����
    fbtPtr mergeSort(fbtPtr head);
    //��ȡ�е� ����ָ��
    fbtPtr getMidBlock(fbtPtr head);
    //�鲢����
    fbtPtr merge(fbtPtr head1, fbtPtr head2);
    //��ӡ����
    void showList(fbtPtr head);
    //�ϲ��������ӵĿռ�
    void mergeBlock(fbtPtr head);
    //�ж��������ܷ�ϲ�
    bool isMergeBlock(fbtPtr b1, fbtPtr b2);
    //�ж�һ��Block A �ͷŰ��� ��һ��Block B
    bool isContain(fbtPtr A, fbtPtr B);
// ����ģ��
public:
    void test();
};
#endif //OS_PROCESS_H
#pragma once
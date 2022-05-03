#include<iostream>
#include<string>
using namespace std;

static const int MAX_MEM_SIZE = 1024;

/*空闲块的数据结构*/
struct free_block{
    int size;
    int start_addr;
    struct free_block * next;
    //默认构造
    free_block()
     : size(0), start_addr(MAX_MEM_SIZE), next(nullptr) {}
    //有参构造
    free_block(int val, int addr, struct free_block * nptr)
     : size(val), start_addr(addr), next(nptr) {}
};  
typedef struct free_block fbt;
typedef struct free_block* fbtPtr;

class Memalloc
{
private:
    fbtPtr free_block_head;     //空闲块头指针
    fbtPtr alloc_block_head;    //忙块头指针
    int bolck_size;             //总内存
// 功能模块
public:
    //构造初始化
    Memalloc(int size=1024);
    //展示当前存储分配情况
    void showMemAllocInfo();
    //展示fbt块情况
    void showfbt(struct free_block* fptr);
    //最差适应算法 失败返回-1 成功返回首地址
    int worstFit(int msize);
    //申请一块空间
    int allocMem(int size);
    //释放一块内存
    bool freeMem(int startAddr, int size);
//辅助模块
public:
    //重排 合并链表块
    void sortAndMerge(fbtPtr head);
    //归并排序
    fbtPtr mergeSort(fbtPtr head);
    //获取中点 快慢指针
    fbtPtr getMidBlock(fbtPtr head);
    //归并函数
    fbtPtr merge(fbtPtr head1, fbtPtr head2);
    //打印链表
    void showList(fbtPtr head);
    //合并链表连接的空间
    void mergeBlock(fbtPtr head);
    //判断两个块能否合并
    bool isMergeBlock(fbtPtr b1, fbtPtr b2);
// 测试模块
public:
    void test();
};
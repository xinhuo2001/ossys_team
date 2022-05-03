#include "memalloc.h"

Memalloc::Memalloc(int size)
{
    //记录初始化数值
    this->bolck_size = size;
    //空闲块初始化
    this->free_block_head = new fbt(size, 0, nullptr);
    //忙块初始化
    this->alloc_block_head = nullptr;
}

void Memalloc::showMemAllocInfo()
{
    // 展示空闲块情况
    cout << "空闲块展示如下:" << endl;
    auto tem = this->free_block_head;
    while(tem != nullptr) {
        this->showfbt(tem);
        tem = tem->next;
    }

    // 展示忙快信息
    cout << endl << "占用块展示如下:" << endl;
    tem = this->alloc_block_head;
    while(tem != nullptr) {
        this->showfbt(tem);
        tem = tem->next;
    }
}

void Memalloc::showfbt(fbtPtr fptr)
{
    cout << "begin:" << fptr->start_addr << "\t\t"
        << "end:" << fptr->start_addr + fptr->size - 1 << "\t\t"
        << "size:" << fptr->size << endl;
}

int Memalloc::worstFit(int msize)
{
    auto tem = this->free_block_head;   //临时指针
    int maxSize = msize;         //当前的最大空间
    fbtPtr fblock = nullptr;              //找到的最大适应空间
    //寻找最大空间
    while(tem != nullptr) {
        if(tem->size >= maxSize) {
            maxSize = msize;
            fblock = tem;
        }
        tem = tem->next;
    }
    //失败 没有找到合适的空间
    if(fblock == nullptr) {
        return -1;
    }
    //成功 将这块空间迁移到已经申请的模块
    //1.头插法改变忙快链表
    fbtPtr fbusy = new fbt(msize, fblock->start_addr, this->alloc_block_head);
    this->alloc_block_head = fbusy;
    //2.更改fblock的内容
    fblock->start_addr += msize;
    fblock->size -= msize;
    //申请的内存块起始地址
    return fbusy->start_addr;
}

int Memalloc::allocMem(int size)
{
    int ret = this->worstFit(size);
    if(ret == -1) {
        cout << "no memory" << endl;
        return -1;
    }
    return ret;
}

bool Memalloc::freeMem(int startAddr, int size)
{
    // 遍历忙链表
    auto p = this->alloc_block_head;    //匹配节点
    fbtPtr pf = nullptr;                //匹配节点前指针
    bool isFind = false;
    while(p != nullptr) {
        if(p->start_addr == startAddr && p->size == size) {
            isFind = true;
            break;
        }
        pf = p;
        p = p->next;
    }
    if(isFind) {
        //在申请块中删除该记录
        if(pf == nullptr) {
            //删除头节点
            this->alloc_block_head = p->next;
            delete p;
        } else {
            //删除其他节点
            pf->next = p->next;
            delete p;
        }

        //头插
        fbtPtr tem = new fbt(size,startAddr,this->free_block_head);
        this->free_block_head = tem;
        //重排合并
        this->sortAndMerge(this->free_block_head);

    } else {
        cout << "delete error" << endl;
    }
    return isFind;
}

void Memalloc::sortAndMerge(fbtPtr head)
{
    //重排
    this->free_block_head = this->mergeSort(this->free_block_head);
    //合并
    this->mergeBlock(this->free_block_head);
}

fbtPtr Memalloc::mergeSort(fbtPtr head)
{
    if(head == nullptr || head->next == nullptr) {
        return head;
    }
    //获取中点 快慢指针
    fbtPtr head1 = head;
    fbtPtr head2 = this->getMidBlock(head);
    // cout << "p1:";
    // this->showList(head1);
    // cout << "p2:";
    // this->showList(head2);

    head1 = mergeSort(head1);
    head2 = mergeSort(head2);

    return merge(head1, head2);
}

fbtPtr Memalloc::getMidBlock(fbtPtr head)
{
    fbtPtr slow = head->next;   //慢指针中点
    fbtPtr fast = head->next;   //快指针
    fbtPtr prev = head;         //用于分离快慢链表
    while(true) {
        //快指针
        if(fast == nullptr) break;
        fast = fast->next;
        if(fast == nullptr) break;
        fast = fast->next;

        prev = slow;
        //慢指针
        slow = slow->next;
    }
    prev->next = nullptr;
    return slow; 
}

fbtPtr Memalloc::merge(fbtPtr head1, fbtPtr head2)
{
    //头节点
    fbtPtr head3 = new fbt(0,-1,nullptr);
    fbtPtr tail = head3;
    while(head1 && head2) {
        if(head1->start_addr <= head2->start_addr) {
            tail->next = head1;
            head1 = head1->next;
        } else {
            tail->next = head2;
            head2 = head2->next;
        }

        tail = tail->next;
        tail->next = nullptr;
    }
    if(head1) tail->next = head1;
    if(head2) tail->next = head2;
    fbtPtr ret = head3->next;
    delete head3;
    // cout << "merge:";
    // this->showList(ret);
    return ret;
}

void Memalloc::showList(fbtPtr head)
{
    if(head == nullptr) {
        cout << "null" << endl;
        return;
    }
    while(head != nullptr) {
        cout << head->start_addr << '\t'; 
        head = head->next; 
    }
    cout << endl;
}

void Memalloc::mergeBlock(fbtPtr head)
{
    auto tem = head;
    auto temNext = head->next;
    while(tem && temNext) {
        if(isMergeBlock(tem,temNext)) {
            //合并两个块
            tem->size += temNext->size;
            tem->next = temNext->next;
            delete temNext;
            temNext = tem->next;
        } else {
            //不能合并则后移
            tem = temNext;
            temNext = temNext->next;
        }
    }
}

bool Memalloc::isMergeBlock(fbtPtr b1, fbtPtr b2)
{
    if(b1 == nullptr || b2 == nullptr) {
        return false;
    }
    if(b1->start_addr + b1->size == b2->start_addr) {
        return true;
    }
    return false;
}

void Memalloc::test()
{
    string line = "\n-------------------------------------------\n\n";
    this->showMemAllocInfo();
    cout << line;

    this->allocMem(20);
    this->allocMem(30);
    this->allocMem(50);
    this->showMemAllocInfo();
    cout << line;

    this->freeMem(0,20);
    this->freeMem(20,30);
    this->showMemAllocInfo();
    cout << line;
}
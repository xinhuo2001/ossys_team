#include "memalloc.h"

Memalloc::Memalloc()
{
    //��¼��ʼ����ֵ
    this->bolck_size = MAX_MEM_SIZE;
    //���п��ʼ��
    this->free_block_head = new fbt(MAX_MEM_SIZE, 0, nullptr);
    //æ���ʼ��
    this->alloc_block_head = nullptr;
}

Memalloc::Memalloc(int size)
{
    //��¼��ʼ����ֵ
    this->bolck_size = size;
    //���п��ʼ��
    this->free_block_head = new fbt(size, 0, nullptr);
    //æ���ʼ��
    this->alloc_block_head = nullptr;
}

void Memalloc::showMemAllocInfo()
{
    // չʾ���п����
    cout << "���п�չʾ����:" << endl;
    auto tem = this->free_block_head;
    while(tem != nullptr) {
        this->showfbt(tem);
        tem = tem->next;
    }

    // չʾæ����Ϣ
    cout << endl << "ռ�ÿ�չʾ����:" << endl;
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
    auto tem = this->free_block_head;   //��ʱָ��
    int maxSize = msize;         //��ǰ�����ռ�
    fbtPtr fblock = nullptr;              //�ҵ��������Ӧ�ռ�
    //Ѱ�����ռ�
    while(tem != nullptr) {
        if(tem->size >= maxSize) {
            maxSize = msize;
            fblock = tem;
        }
        tem = tem->next;
    }
    //ʧ�� û���ҵ����ʵĿռ�
    if(fblock == nullptr) {
        return -1;
    }
    //�ɹ� �����ռ�Ǩ�Ƶ��Ѿ������ģ��
    //1.ͷ�巨�ı�æ������
    fbtPtr fbusy = new fbt(msize, fblock->start_addr, this->alloc_block_head);
    this->alloc_block_head = fbusy;
    //2.����fblock������
    fblock->start_addr += msize;
    fblock->size -= msize;
    //������ڴ����ʼ��ַ
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

vector<int> Memalloc::allocMemory(int size)
{
    int ret = worstFit(size);
    vector<int> retVector;
    retVector.resize(0);
    //cout << "alloc:" << ret << endl;
    //����ʧ��
    if (ret == -1) {
        retVector.push_back(-1);
    } else {
        int beginFramePage = ret / FRAME_PAGE_SIZE + FRAME_PAGE_BEGIN;
        int endFramePage = (ret + size) / FRAME_PAGE_SIZE + FRAME_PAGE_BEGIN;
        int beginByte = ret % FRAME_PAGE_SIZE;
        int endByte = (ret + size - 1) % FRAME_PAGE_SIZE;
        retVector.push_back(beginFramePage);
        retVector.push_back(endFramePage);
        retVector.push_back(beginByte);
        retVector.push_back(endByte);
    }
    //cout << "ret[0]:" << retVector[0] << endl;
    return retVector;
}

bool Memalloc::freeMem(int startAddr, int size)
{
    // ����æ����
    auto p = this->alloc_block_head;    //ƥ��ڵ�
    fbtPtr pf = nullptr;                //ƥ��ڵ�ǰָ��
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
        //���������ɾ���ü�¼
        if(pf == nullptr) {
            //ɾ��ͷ�ڵ�
            this->alloc_block_head = p->next;
            delete p;
        } else {
            //ɾ�������ڵ�
            pf->next = p->next;
            delete p;
        }

        //ͷ��
        fbtPtr tem = new fbt(size,startAddr,this->free_block_head);
        this->free_block_head = tem;
        //���źϲ�
        this->sortAndMerge(this->free_block_head);

    } else {
        cout << "delete error" << endl;
    }
    return isFind;
}

bool Memalloc::freeMemory(vector<int>& memV)
{
    //������� �˳�
    if (memV.size() < 4) {
        return false;
    }
    int beginFramePage = memV[0];
    int endFramePage = memV[1];
    int beginByte = memV[2];
    int endByte = memV[3];

    //��ȡ��ʼ��ַ��size
    int startAddr = (beginFramePage - FRAME_PAGE_BEGIN) * FRAME_PAGE_SIZE + beginByte;
    //size = �м���ȫҳ�ֽ� + ��ʼҳ + ĩβҳ
    int size = (endFramePage - beginFramePage - 1) * FRAME_PAGE_SIZE + (FRAME_PAGE_SIZE - beginByte) + (endByte + 1);
    return freeMem(startAddr, size);
}

bool Memalloc::addMemByVec(vector<int>& memV)
{
    if (memV.size() < 4) {
        return false;
    }
    int beginFramePage = memV[0];
    int endFramePage = memV[1];
    int beginByte = memV[2];
    int endByte = memV[3];

    //cout << "add Block:" << endl;
    //for (int i = 0; i < 4; i++) {
    //    cout << memV[i] << " ";
    //}
    //cout << endl;
    //��ȡ��ʼ��ַ��size
    int startAddr = (beginFramePage - FRAME_PAGE_BEGIN) * FRAME_PAGE_SIZE + beginByte;
    //size = �м���ȫҳ�ֽ� + ��ʼҳ + ĩβҳ
    int size = (endFramePage - beginFramePage - 1) * FRAME_PAGE_SIZE + (FRAME_PAGE_SIZE - beginByte) + (endByte + 1);
    //ͷ��
    fbtPtr falloc = new fbt(size, startAddr, this->alloc_block_head);
    this->alloc_block_head = falloc;
    //������Ҫ����æ���������п��б�
    return true;
}

bool Memalloc::correctFreeBlockList()
{
    //���ѭ������æ��
    auto temBusy = this->alloc_block_head;
    while (temBusy != nullptr) {
        //�ڴ�ѭ���������п� ��ǰæ��ض���ĳ���п����
        auto temFree = this->free_block_head;
        fbtPtr temFreePro = nullptr; //temFreeǰ��ָ��
        while (temFree != nullptr) {
            //FreeBlock ���� BusyBlock
            if (this->isContain(temFree, temBusy)) {
                //��FreeBlock �ֳ�����
                int front_block_size = temBusy->start_addr - temFree->start_addr;
                int back_block_size = temFree->start_addr + temFree->size - temBusy->start_addr - temBusy->size;
                //��������
                //1.æ�� �� �п� һ��
                if (front_block_size == 0 && back_block_size == 0) {
                    if (temFreePro == nullptr) {
                        //��ǰ�п��ǵ�һ�� �����п�ͷ
                        this->free_block_head = temFree->next;
                    }
                    else {
                        //��ǰ�п鲻�ǵ�һ�� �п���ȥ����ǰ��
                        temFreePro->next = temFree->next;
                    }
                    delete temFree;
                    temFree = nullptr;
                } //2.�п�ֻ������벿��
                else if (front_block_size == 0) {
                    temFree->start_addr = temBusy->start_addr + temBusy->size + 1;
                    temFree->size = back_block_size;
                } //3.�п�ֻ����ǰ�벿��
                else if (back_block_size == 0) {
                    temFree->size = front_block_size;
                } //4.ǰ�����п鱣��
                else {
                    //ǰ��ʹ��temFree 
                    temFree->size = front_block_size;
                    //�����������
                    fbtPtr back_block = new fbt(back_block_size, temBusy->start_addr + temBusy->size, temFree->next);
                    //����ǰ���
                    temFree->next = back_block;
                }
                break;
            }
            temFreePro = temFree;
            temFree = temFree->next;
        }
        temBusy = temBusy->next;
    }
    return true;
}

void Memalloc::sortAndMerge(fbtPtr head)
{
    //����
    this->free_block_head = this->mergeSort(this->free_block_head);
    //�ϲ�
    this->mergeBlock(this->free_block_head);
}

fbtPtr Memalloc::mergeSort(fbtPtr head)
{
    if(head == nullptr || head->next == nullptr) {
        return head;
    }
    //��ȡ�е� ����ָ��
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
    fbtPtr slow = head->next;   //��ָ���е�
    fbtPtr fast = head->next;   //��ָ��
    fbtPtr prev = head;         //���ڷ����������
    while(true) {
        //��ָ��
        if(fast == nullptr) break;
        fast = fast->next;
        if(fast == nullptr) break;
        fast = fast->next;

        prev = slow;
        //��ָ��
        slow = slow->next;
    }
    prev->next = nullptr;
    return slow; 
}

fbtPtr Memalloc::merge(fbtPtr head1, fbtPtr head2)
{
    //ͷ�ڵ�
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
            //�ϲ�������
            tem->size += temNext->size;
            tem->next = temNext->next;
            delete temNext;
            temNext = tem->next;
        } else {
            //���ܺϲ������
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

bool Memalloc::isContain(fbtPtr A, fbtPtr B)
{
    if (A->start_addr <= B->start_addr && A->start_addr + A->size >= B->start_addr + B->size) {
        return true;
    }
    return false;
}

void Memalloc::test()
{
    string line = "\n-------------------------------------------\n\n";
    this->showMemAllocInfo();
    cout << line;

    int ret;

    ret = this->allocMem(20);
    cout << "alloc:" << 20 << " ret:" << ret << endl;
    ret = this->allocMem(30);
    cout << "alloc:" << 20 << " ret:" << ret << endl;
    ret = this->allocMem(50);
    cout << "alloc:" << 20 << " ret:" << ret << endl;
    this->showMemAllocInfo();
    cout << line;

    ret = this->freeMem(0,20);
    ret = this->freeMem(20,30);
    this->showMemAllocInfo();
    cout << line;
}
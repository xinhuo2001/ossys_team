#include "FileSystem.h"

FileSystem::FileSystem()
{
    cout << "init" << endl;
    init();
    cout << "init end" << endl;
}

FileSystem::~FileSystem()
{
    
}

void FileSystem::init()
{
    fstream tree;
    tree.open(TreeFileName);
    if(!tree.is_open()) {
        cout << "open " << TreeFileName << " fail" << endl;
        exit(-1);
    }

    int rnum;
    string rname;
    tree >> rnum >> rname;
    root = new FCB(rname);

    //层序遍历使用队列
    queue<FCB*> myQueue;
    myQueue.push(root);
    while(!tree.eof()) {
        //获取队列头
        if(myQueue.empty()) {
            cout << "queue error" << endl;
            exit(-1);
        }
        auto cur = myQueue.front();
        myQueue.pop();
        //准备cur的儿子和兄弟
        FCB* fcb_child = nullptr;
        FCB* fcb_simbling = nullptr;
        //标识符 0为空
        int num;
        tree >> num;
        //儿子
        if(num != 0) {
            string filename;
            tree >> filename;
            fcb_child = new FCB(filename);
            myQueue.push(fcb_child);
        }
        //兄弟
        tree >> num;
        if(num != 0) {
            string filename;
            tree >> filename;
            fcb_simbling = new FCB(filename);
            myQueue.push(fcb_simbling);
        }
        //入栈
        cur->setChild(fcb_child);
        cur->setSibling(fcb_simbling);
    }
}

void FileSystem::showTree()
{
    auto cur = root;
    queue<FCB*> myQueue;
    myQueue.push(cur);
    int level = 0;
    while(!myQueue.empty()) {
        cout << "当前level:" << level++ << endl;
        //记录下一层所有节点
        queue<FCB*> nextLevel;
        //所有节点出队列
        while(!myQueue.empty()) {
            cur = myQueue.front();
            cout << cur->name << " ";
            myQueue.pop();
            //左子 及 左子的右子全部入栈
            if(cur->child) {
                //左子入队
                nextLevel.push(cur->child);
                //左子的右子入队
                auto tem = cur->child->sibling;
                while(tem != nullptr) {
                    nextLevel.push(tem);
                    tem = tem->sibling;
                }
            }
        }
        cout << endl;
        //交换两个队列
        while(!nextLevel.empty()) {
            myQueue.push(nextLevel.front());
            nextLevel.pop();
        }
    }
}

void FileSystem::Tree(FCB* cur, int depth)
{
    if(cur != nullptr) {
        for(int j = 0; j < depth; j++) {
            if(j == depth - 1) {
                cout << "|_______";
            } else {
                cout << "        ";
            }
        }
        cout << cur->name << endl;
        Tree(cur->child,depth+1);
        Tree(cur->sibling,depth);
    }
}

void FileSystem::Ls(FCB* cur)
{
    //展示cur的左子和左子的右子(递进)
    auto tem = cur->child;
    while(tem != nullptr) {
        tem->showSelf();
        tem = tem->sibling;
    }
}

void FileSystem::preOrder(FCB* cur)
{
    if(cur != nullptr) {
        cout << cur->name << endl;
        preOrder(cur->child);
        preOrder(cur->sibling);
    } else {
        cout << "null" << endl;
    }
}

void FileSystem::test()
{
    cout << "---------------------------" << endl;    
    cout << "tree 根目录" << endl;
    Tree(root,0);
    cout << "---------------------------" << endl;    
    cout << "ls 根目录" << endl;
    Ls(root);
    cout << "---------------------------" << endl;
}
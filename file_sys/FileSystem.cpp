#include "FileSystem.h"

FileSystem::FileSystem()
{
    init();
    this->curFCB = root;
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
    root->setParent(nullptr);
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
            fcb_child->parent = cur;
        }
        //兄弟
        tree >> num;
        if(num != 0) {
            string filename;
            tree >> filename;
            fcb_simbling = new FCB(filename);
            myQueue.push(fcb_simbling);
            fcb_simbling->parent = cur;
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

void FileSystem::updateTreeFile()
{
    string tfname = "a.txt";
    // string tfname = TreeFileName;
    //层序遍历
    fstream fc;
    fc.open(tfname,ios::out | ios::trunc);
    if(!fc.is_open()) {
        cout << "open " << tfname << " error" << endl;
        exit(-1);
    }
    queue<FCB*> myQueue;
    string rootInfo = "";
    int count = 1;
    //写入根节点数据
    rootInfo += this->num2string(count++);
    rootInfo += " ";
    rootInfo += root->name;
    fc << rootInfo << endl;

    myQueue.push(root);
    while(!myQueue.empty()) {
        auto tem = myQueue.front();
        myQueue.pop();
        string temInfo = "";
        //只写入tem 子节点
        //左子孩子
        auto tem_child = tem->child;
        if(tem_child != nullptr) {
            temInfo += num2string(count++);
            temInfo += " ";
            temInfo += tem_child->name;
            myQueue.push(tem_child);
        } else {
            temInfo += "0 ";
        }
        fc << temInfo << endl;
        temInfo.clear();
        //右子兄弟
        auto tem_sibling = tem->sibling;
        if(tem_sibling != nullptr) {
            temInfo += num2string(count++);
            temInfo += " ";
            temInfo += tem_sibling->name;
            myQueue.push(tem_sibling);
        } else {
            temInfo += "0 ";
        }
        fc << temInfo << endl;
    }
}

string FileSystem::num2string(int num)
{
    string ret = "";
    while(num != 0) {
        ret += (char)(num%10 + '0');
        num = num/10;
    }
    reverse(ret.begin(), ret.end());
    // cout << "ret:" << ret << endl;
    return ret;
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

void FileSystem::Pwd(FCB* cur)
{
    vector<string> path;
    auto tem = cur;
    //找到长子 即自己是父亲的左子
    while(tem != nullptr && tem->parent != nullptr) {
        if(tem == tem->parent->child) {
            path.push_back(tem->parent->name);
        }
        tem = tem->parent;
    }
    reverse(path.begin(), path.end());
    for(int i = 0; i < path.size(); i++) {
        cout << path[i] << "/";
    }
    cout << endl;
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
    cout << endl << "---------------------------" << endl;    
    cout << "tree 根目录" << endl;
    Tree(root,0);
    cout << endl << "---------------------------" << endl;    
    cout << "ls 根目录" << endl;
    Ls(root);
    cout << endl << "---------------------------" << endl;    
    cout << "pwd 某节点" << endl;
    auto pwd_fcb = root->child->sibling->child;
    cout << "file:" << pwd_fcb->name << endl;
    Pwd(pwd_fcb);
    cout << endl << "---------------------------" << endl;    
    cout << "写入 目录" << endl;
    updateTreeFile();
    cout << endl << "---------------------------" << endl;    
}
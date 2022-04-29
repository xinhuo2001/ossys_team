#include "FileSystem.h"
#include<vector>
#include <iostream>
#include<fstream>
#include "FCB.h"

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
    // cout << "init begin" << endl;
    fstream tree;
    tree.open(TreeFileName);
    if(!tree.is_open()) {
        cout << "open " << TreeFileName << " fail" << endl;
        exit(-1);
    }

    int rnum;
    string rname;
    int rtype;
    tree >> rnum >> rname >> rtype;
    root = new FCB(rname);
    root->type = char(rtype+'0');
    root->setParent(nullptr);
    cout << "root init" << endl;
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
        // cout << cur->name << " " << cur->type << endl;
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
            //初始化其他信息
            int type;
            tree >> type;
            fcb_child->type = (char)(type+'0');
            fcb_child->parent = cur;

            myQueue.push(fcb_child);
        }
        //兄弟
        tree >> num;
        if(num != 0) {
            string filename;
            tree >> filename;
            fcb_simbling = new FCB(filename);
            //初始化其他信息
            int type;
            tree >> type;
            fcb_simbling->type = (char)(type+'0');
            fcb_simbling->parent = cur;

            myQueue.push(fcb_simbling);
        }
        //入栈
        cur->setChild(fcb_child);
        cur->setSibling(fcb_simbling);
    }
    tree.close();
    // cout << "init ok" << endl;
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

    //关闭文件
    fc.close();
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

bool FileSystem::isExistFile(const string& filename)
{
    auto tem = this->curFCB->child;
    //左孩子右子
    while(tem != nullptr) {
        if(tem->isFile() && filename == tem->name) {
            return true;
        }
        tem = tem->sibling;
    }
    return false;
}

bool FileSystem::isExistDir(const string& dirname)
{
    auto tem = this->curFCB->child;
    //左孩子右子
    while(tem != nullptr) {
        if(tem->isDir() && dirname == tem->name) {
            return true;
        }
        tem = tem->sibling;
    }
    return false;
}

void FileSystem::addNode(FCB* curNode, FCB* newNode)
{
    auto LNode = curNode->child;
    //直接把newNode 放在 curNode的左子上即可
    curNode->child = newNode;
    newNode->parent = curNode;
    newNode->sibling = LNode;
    if(LNode != nullptr) {
        LNode->parent = newNode;
    }

    // auto LNode = curNode->child;
    // //左树为空,直接将newNode放在curNode的左子
    // if(LNode == nullptr) {
    //     curNode->child = newNode;
    //     newNode->parent = curNode;
    //     return;
    // }
    // auto LRNode = LNode->sibling;
    // //左树不为空 LRNode为空 newNode 放在LNode的右子
    // if(LRNode == nullptr) {
    //     LNode->sibling = newNode;
    //     newNode->parent = LNode;
    //     return;
    // }
    // //都不为空 将newNode插入到 LNode 和 LRNode之间
    
}

TreeNode*& FileSystem::matchPath(const string path)//在树 dirTree 上找一条path，返回末尾的节点
{
    vector<string> res = split(path, "/");
    TreeNode* temp = dirTree;
    TreeNode* lastptr = nullptr;

    int i;
    for (i = 0; i < res.size(); i++)
    {
        while (temp != nullptr)
        {
            if (temp->name == res[i] /*&& temp->type == 1*/)//在当前层找到了
            {
                if (i < res.size() - 1)
                {
                    lastptr = temp;
                    temp = temp->child;//进入下一层的匹配
                }
                break;
            }
            else
            {
                lastptr = temp;
                temp = temp->sibling;
            }
        }
        if (temp == nullptr)
            break;
    }

    if (i == res.size())
    {
        cout << "路径匹配成功！" << endl;
        if (lastptr == nullptr)
            return dirTree;
        else if (lastptr->child == temp)
            return lastptr->child;
        else
            return lastptr->sibling;
    }
    else
    {
        cout << "待查找的路径不存在" << endl;
        return temp;
    }
}


void FileSystem::postOrderDelSubTree(Tree &t)//后序遍历删除文件子树
{
    if (t == nullptr)
        return;

    postOrderDelSubTree(t->child);
    postOrderDelSubTree(t->sibling);
    //先删除数据块
    releaseExternalStorage(*t);
    //再删除目录项
    delete t;
    t = nullptr;
}

int FileSystem::releaseExternalStorage(FCB& fcb)//给一个文件或文件夹释放数据块
{
    int size = fcb.size;
    freeSpaceList.insert(freeSpaceList.end(),fcb.location.begin(),fcb.location.end());
    fcb.location.erase(fcb.location.begin(), fcb.location.end());
    utiliRate = 100*(STORAGE_SIZE - freeSpaceList.size()) / STORAGE_SIZE;
    return 0;
}

vector<string> FileSystem::split(const string& str, const string& delim)//字符串切片
{
    vector<string> res;
    if ("" == str) return res;
    //先将要切割的字符串从string类型转换为char*类型  
    int len1 = str.length() + 1;//要加上一个\0
    char* strs = new char[len1]; 
    strcpy_s(strs,len1, str.c_str());

    int len2 = delim.length() + 1;
    char* d = new char[len2];
    strcpy_s(d, len2,delim.c_str());

    char* buf=NULL;
    char* p = strtok_s(strs, d,&buf);

    while (p)
    {
        string s = p; //分割得到的字符串转换为string类型  
        res.push_back(s); //存入结果数组  
        p = strtok_s(NULL, d,&buf);
    }

    delete [] strs;
    delete [] d;

    cout << "\n路径为：" << endl;
    for (int i = 0; i < res.size(); i++)
    {
        std::cout << res[i] << " ";
    }
    //cout << "得到的字段数：" << res.size() << endl;
    return res;
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

void FileSystem::Touch(const string& fileName,int size)
{
    //判断当前目录是否存在该文件
    if(this->isExistFile(fileName)) {
        cout << "touch failed;" << fileName << " Existed" << endl;
        return;
    }
    auto temFcb = this->curFCB;
    //创建新节点
    FCB* tnode = new FCB(fileName);
    tnode->size = size;
    tnode->type = FILE_TYPE;
    //将新节点放在左树的右子
    this->addNode(temFcb,tnode);
}

void FileSystem::Mkdir(const string& dirName)
{
    //判断当前目录是否存在该目录
    if(this->isExistDir(dirName)) {
        cout << "dir:" << dirName << " has existed" << endl;
        return;
    }
    auto temFcb = this->curFCB;
    //创建新节点
    FCB* tnode = new FCB(dirName);
    tnode->type = DIR_TYPE;
    //将新节点放在左树的右子
    this->addNode(temFcb,tnode);
}

void FileSystem::Rm(const string path)
{
 	TreeNode*& pnode = matchPath(path);
    if (pnode == nullptr)
    {
        cout << "删除节点失败，该路径不存在" << endl;
    }
    else
    {
        postOrderDelSubTree(pnode);
        cout << "删除成功！" << endl;
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
    cout << "写入 目录文件" << endl;
    updateTreeFile();
    cout << endl << "---------------------------" << endl; 
    cout << "touch 在 " << curFCB->name << " 添加文件节点" << "hello.cpp" << endl;
    Touch("hello.cpp",5);
    cout << "再次创建hello.cpp" << endl;
    Touch("hello.cpp",5);
    cout << endl << "---------------------------" << endl;   
    cout << "tree 根目录" << endl;
    Tree(root,0);
    cout << endl << "---------------------------" << endl; 
    cout << "mkdir 在 " << curFCB->name << " 添加目录节点" << "user2" << endl;
    Mkdir("user2");
    cout << "再次创建user2" << endl;
    Mkdir("user2");
    cout << endl << "---------------------------" << endl;   
    cout << "tree 根目录" << endl;
    Tree(root,0);
    cout << endl << "---------------------------" << endl; 
    cout << "rm " << "user2" << endl;
    Rm("user2");
    cout << "rm " << "hello.cpp" << endl;
    Rm("hello.cpp");
    cout << endl << "---------------------------" << endl;   
    cout << "tree 根目录" << endl;
    Tree(root,0);
    cout << endl << "---------------------------" << endl;
}
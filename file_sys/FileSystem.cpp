#include "FileSystem.h"

FileSystem::FileSystem()
{
    // init();
    init2();
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

void FileSystem::init2()
{
    // cout << "init begin" << endl;
    fstream tree;
    tree.open(TreeFileName);
    if(!tree.is_open()) {
        cout << "open " << TreeFileName << " fail" << endl;
        exit(-1);
    }

    //初始化根
    string line;
    getline(tree,line);
    this->generateFCB(line,root);

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

        //儿子
        getline(tree,line);
        bool child_ret = this->generateFCB(line,fcb_child);
        if(child_ret) {
            //儿子不为空
            fcb_child->parent = cur;
            myQueue.push(fcb_child);
        }


        //兄弟
        getline(tree,line);
        bool simbling_ret = this->generateFCB(line,fcb_simbling);
        if(simbling_ret) {
            //兄弟不为空
            fcb_simbling->parent = cur;
            myQueue.push(fcb_simbling);
        }

        //入栈
        cur->setChild(fcb_child);
        cur->setSibling(fcb_simbling);
    }
    tree.close();
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

FCB* FileSystem::findFile(FCB* curNode, const string& name)
{
    auto tem = curNode->child;
    while(tem != nullptr) {
        if(tem->name == name) {
            return tem;
        }
        tem = tem->sibling;
    }
    return nullptr;
}

bool FileSystem::generateFCB(const string& line, FCB*& gfcb)
{
    int flag;
    istringstream i_line(line);
    i_line >> flag;
    if(flag == 0) {
        return false;
    }
    //文件的各项参数
    string fname;
    int ftype;
    int flimit;
    string uname;

    //读入文件参数
    i_line >> fname >> ftype >> flimit >> uname;
    gfcb = new FCB(fname);
    gfcb->type = ftype + '0';
    gfcb->rwx = flimit;
    gfcb->uname = uname;

    //读入文件内存部分
    if(gfcb->isFile()) {
        i_line >> gfcb->locationLen;
        //读入内存块向量
        for(int i = 0; i < gfcb->locationLen; i++) {
            int vtem;
            i_line >> vtem;
            gfcb->location.push_back(vtem);
        }
        i_line >> gfcb->beginByte;
        i_line >> gfcb->endByte;
    }
    return true;
}

bool FileSystem::isPermitRead(const string& curUser, FCB* pfcb)
{
    return true;
}

bool FileSystem::isPermitWrite(const string& curUser, FCB* pfcb)
{
    return true;
}

TreeNode* FileSystem::matchPath(const string path)//在树 dirTree 上找一条path，返回末尾的节点
{
    vector<string> res = split(path, "/");
    TreeNode* temp = this->root;
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
            return this->root;
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


void FileSystem::postOrderDelSubTree(TreeNodePtr &t)//后序遍历删除文件子树
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
    // int size = fcb.size;
    // freeSpaceList.insert(freeSpaceList.end(),fcb.location.begin(),fcb.location.end());
    // fcb.location.erase(fcb.location.begin(), fcb.location.end());
    // utiliRate = 100*(STORAGE_SIZE - freeSpaceList.size()) / STORAGE_SIZE;
    // return 0;
}

vector<string> FileSystem::split(const string& str, const string& delim)//字符串切片
{
    vector<string> res;
    if ("" == str) return res;
    //先将要切割的字符串从string类型转换为char*类型  
    int len1 = str.length() + 1;//要加上一个\0
    char* strs = new char[len1]; 
    strcpy(strs,str.c_str());

    int len2 = delim.length() + 1;
    char* d = new char[len2];
    strcpy(d,delim.c_str());

    char* buf=NULL;
    char* p = strtok_r(strs, d,&buf);

    while (p)
    {
        string s = p; //分割得到的字符串转换为string类型  
        res.push_back(s); //存入结果数组  
        p = strtok_r(NULL, d,&buf);
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

void FileSystem::TreeChild(FCB* cur, int depth)
{
    if(cur != nullptr) {
        for(int j = 0; j < depth; j++) {
            if(j == depth - 1) {
                cout << "|_______";
            } else {
                cout << "        ";
            }
        }
        cout << cur->showName() << endl;
        TreeChild(cur->child,depth+1);
        TreeChild(cur->sibling,depth);
    }
}

int FileSystem::getCurDepth(FCB* cur)
{
    int depth = 0;
    while(cur != nullptr && cur->parent != nullptr) {
        //为左子层数+1
        if(cur->parent->child == cur) {
            depth++;
        }
        cur = cur->parent;
    }
    return depth;
}

void FileSystem::Tree(FCB* cur)
{
    cout << cur->showName() << endl;
    // int depth = this->getCurDepth(cur);
    TreeChild(cur->child,1);
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
    path.push_back(cur->name);
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
    tnode->uname = this->curUser;
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
    tnode->uname = this->curUser;
    //将新节点放在左树的右子
    this->addNode(temFcb,tnode);
}

void FileSystem::Rm(const string& fileName)
{
    //判断当前目录是否存在该文件
    if(this->isExistFile(fileName) == false && this->isExistDir(fileName) == false) {
        cout << "no such file:" << fileName << endl;
        return;
    }
    auto tem = this->curFCB;
    //查找待删除文件节点
    auto del_file = this->findFile(tem,fileName);
    //拒绝删除文件目录 后期添加
    if(del_file == nullptr || del_file->type == DIR_TYPE) {
        cout << "del dir error" << endl;
        return;
    }
    auto par_node = del_file->parent;
    auto sib_node = del_file->sibling;
    par_node->sibling = sib_node;
    if(sib_node != nullptr) {
        sib_node->parent = par_node;
    }
    //warn : 在内存删除文件
    if(del_file){
        delete del_file;
        del_file = nullptr;
    }
}

// void FileSystem::Rm(const string path)
// {
//  	TreeNode*& pnode = matchPath(path);
//     if (pnode == nullptr)
//     {
//         cout << "删除节点失败，该路径不存在" << endl;
//     }
//     else
//     {
//         postOrderDelSubTree(pnode);
//         cout << "删除成功！" << endl;
//     }
// }

void FileSystem::Ls_l(FCB* cur)
{
    //展示cur的左子和左子的右子(递进)
    auto tem = cur->child;
    while(tem != nullptr) {
        tem->showSelf_l();
        tem = tem->sibling;
    }
}

void FileSystem::Chmod(int LimitNum, string fileName)
{
    //不存在该文件或目录
    if(this->isExistDir(fileName) == false && this->isExistFile(fileName) == false) {
        return;
    }
    auto ffcb = this->findFile(this->curFCB,fileName);
    ffcb->setRWX(LimitNum);
}

void FileSystem::ReadFile(const string& fileName)
{
    if(this->isExistFile(fileName) == false) {
        cout << "warn:no such file" << endl;
        return;
    }

    //读取文件
    FCB* rfcb = this->findFile(this->curFCB,fileName);
    //无相应权限
    if(this->isPermitRead(fileName,rfcb) == false) {
        cout << "warn:premission denying" << endl;
        return;
    }

    //内存位置
    cout << "准备读入如下内容" << endl;
    cout << "帧块共计:" << rfcb->locationLen << endl;
    cout << "帧块编号如下:" << endl;
    for(int i = 0; i < rfcb->locationLen; i++) {
        cout << rfcb->location[i] << " ";
    }
    cout << endl;
    cout << "起始块起点:" << rfcb->beginByte << endl;
    cout << "结束块终点:" << rfcb->endByte << endl;

    //这里请求对应帧内容 并显示出来

}

void FileSystem::Cd(const string& dirName)
{
    //没有这个目录
    if(this->isExistDir(dirName) == false) {
        cout << "warn:No Such Dir" << endl;
        return;
    }
    //添加权限确定
    FCB* pfcb = this->findFile(curFCB, dirName);
    //无相应权限
    if(this->isPermitRead(dirName,pfcb) == false) {
        cout << "warn:premission denying" << endl;
        return;
    }
    //切换目录
    this->curFCB = pfcb;
}

void FileSystem::ViFile(const string& fileName)
{
    if(this->isExistFile(fileName) == false) {
        cout << "warn:no such file" << endl;
        return;
    }

    //读取文件
    FCB* rfcb = this->findFile(this->curFCB,fileName);
    //无相应权限
    if(this->isPermitWrite(fileName,rfcb) == false) {
        cout << "warn:premission denying" << endl;
        return;
    }
    //准备写入
    //释放相应 内存位置
    cout << "申请内存释放如下内容" << endl;
    cout << "帧块共计:" << rfcb->locationLen << endl;
    cout << "帧块编号如下:" << endl;
    for(int i = 0; i < rfcb->locationLen; i++) {
        cout << rfcb->location[i] << " ";
    }
    cout << endl;
    cout << "起始块起点:" << rfcb->beginByte << endl;
    cout << "结束块终点:" << rfcb->endByte << endl;

    //接收键盘输入

    //申请内存

    //传送相应字符给内存

    //成功写入
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
    Tree(root);
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
    Tree(root);
    cout << endl << "---------------------------" << endl; 
    cout << "mkdir 在 " << curFCB->name << " 添加目录节点" << "user2" << endl;
    Mkdir("user2");
    cout << "再次创建user2" << endl;
    Mkdir("user2");
    cout << endl << "---------------------------" << endl;   
    cout << "tree 根目录" << endl;
    Tree(root);
    cout << endl << "---------------------------" << endl; 
    cout << "rm " << "user2" << endl;
    Rm("user2");
    cout << "rm " << "hello.cpp" << endl;
    Rm("hello.cpp");
    cout << endl << "---------------------------" << endl;   
    cout << "tree 根目录" << endl;
    Tree(root);
    cout << endl << "---------------------------" << endl;
    cout << "touch hello.cpp" << endl;
    Touch("hello.cpp",5);
    cout << "ls -l 命令" << endl;
    Ls_l(root);
    cout << endl << "---------------------------" << endl;
    cout << "chmod 776 hello.cpp" << endl;
    Chmod(776,"hello.cpp");
    cout << endl << "---------------------------" << endl;
    cout << "ls -l 命令" << endl;
    Ls_l(root);
    cout << endl << "---------------------------" << endl;
}

void FileSystem::test2()
{
    this->Ls_l(this->curFCB);   cout << endl;

    this->Cd("bin");    
    
    this->Ls_l(this->curFCB);   cout << endl; 

    this->ReadFile("a.sh");     cout << endl;

    this->ViFile("a.sh");       cout << endl;
}

void FileSystem::test3()
{
    TreeChild(curFCB,0);
    cout << endl;
    Tree(root);
    cout << endl;
    Cd("bin");
    Tree(this->curFCB);
    // Rm("a.sh");
    cout << endl;
    Tree(root);
}
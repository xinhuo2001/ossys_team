#include "FileSystem.h"
#include "process.h"
FileSystem::FileSystem()
{
    // init();
    init2();
    this->curFCB = root;
}

/**
����
���̽ӿ�... 
�����ϲ�����
*/


FileSystem::~FileSystem()
{
    //�ͷ��ڴ����ָ��
    /*if (mem_alloc) {
        delete mem_alloc;
        mem_alloc = nullptr;
    }
    */
    //warn:�ͷ��ļ�ָ��fcb
}

void FileSystem::init()
{
    // cout << "init begin" << endl;
    fstream tree;
    tree.open(TreeFileName);
    if (!tree.is_open()) {
        cout << "open " << TreeFileName << " fail" << endl;
        exit(-1);
    }

    int rnum;
    string rname;
    int rtype;
    tree >> rnum >> rname >> rtype;
    root = new FCB(rname);
    root->type = char(rtype + '0');
    root->setParent(nullptr);
    cout << "root init" << endl;
    //�������ʹ�ö���
    queue<FCB*> myQueue;
    myQueue.push(root);
    while (!tree.eof()) {
        //��ȡ����ͷ
        if (myQueue.empty()) {
            cout << "queue error" << endl;
            exit(-1);
        }
        auto cur = myQueue.front();
        myQueue.pop();
        // cout << cur->name << " " << cur->type << endl;
        //׼��cur�Ķ��Ӻ��ֵ�
        FCB* fcb_child = nullptr;
        FCB* fcb_simbling = nullptr;
        //��ʶ�� 0Ϊ��
        int num;
        tree >> num;
        //����
        if (num != 0) {
            string filename;
            tree >> filename;
            fcb_child = new FCB(filename);
            //��ʼ��������Ϣ
            int type;
            tree >> type;
            fcb_child->type = (char)(type + '0');
            fcb_child->parent = cur;

            myQueue.push(fcb_child);
        }
        //�ֵ�
        tree >> num;
        if (num != 0) {
            string filename;
            tree >> filename;
            fcb_simbling = new FCB(filename);
            //��ʼ��������Ϣ
            int type;
            tree >> type;
            fcb_simbling->type = (char)(type + '0');
            fcb_simbling->parent = cur;

            myQueue.push(fcb_simbling);
        }
        //��ջ
        cur->setChild(fcb_child);
        cur->setSibling(fcb_simbling);
    }
    tree.close();
    // cout << "init ok" << endl;
}

void FileSystem::init2()
{
    // cout << "init begin" << endl;
    mem_alloc = new Memalloc();
    fstream tree;
    tree.open(TreeFileName);
    if (!tree.is_open()) {
        cout << "open " << TreeFileName << " fail" << endl;
        exit(-1);
    }

    //��ʼ����
    string line;
    getline(tree, line);
    this->generateFCB(line, root);

    //�������ʹ�ö���
    queue<FCB*> myQueue;
    myQueue.push(root);
    while (!tree.eof()) {
        //��ȡ����ͷ
        if (myQueue.empty()) {
            cout << "queue error" << endl;
            exit(-1);
        }
        auto cur = myQueue.front();
        myQueue.pop();

        //׼��cur�Ķ��Ӻ��ֵ�
        FCB* fcb_child = nullptr;
        FCB* fcb_simbling = nullptr;

        //����
        getline(tree, line);
        bool child_ret = this->generateFCB(line, fcb_child);
        if (child_ret) {
            //���Ӳ�Ϊ��
            fcb_child->parent = cur;
            myQueue.push(fcb_child);
        }


        //�ֵ�
        getline(tree, line);
        bool simbling_ret = this->generateFCB(line, fcb_simbling);
        if (simbling_ret) {
            //�ֵܲ�Ϊ��
            fcb_simbling->parent = cur;
            myQueue.push(fcb_simbling);
        }

        //��ջ
        cur->setChild(fcb_child);
        cur->setSibling(fcb_simbling);
    }
    mem_alloc->correctFreeBlockList();
    tree.close();
}

void FileSystem::showTree()
{
    auto cur = root;
    queue<FCB*> myQueue;
    myQueue.push(cur);
    int level = 0;
    while (!myQueue.empty()) {
        cout << "��ǰlevel:" << level++ << endl;
        //��¼��һ�����нڵ�
        queue<FCB*> nextLevel;
        //���нڵ������
        while (!myQueue.empty()) {
            cur = myQueue.front();
            cout << cur->name << " ";
            myQueue.pop();
            //���� �� ���ӵ�����ȫ����ջ
            if (cur->child) {
                //�������
                nextLevel.push(cur->child);
                //���ӵ��������
                auto tem = cur->child->sibling;
                while (tem != nullptr) {
                    nextLevel.push(tem);
                    tem = tem->sibling;
                }
            }
        }
        cout << endl;
        //������������
        while (!nextLevel.empty()) {
            myQueue.push(nextLevel.front());
            nextLevel.pop();
        }
    }
}

void FileSystem::updateTreeFile()
{
    string tfname = "tree.txt";
    // string tfname = TreeFileName;
    //�������
    fstream fc;
    fc.open(tfname, ios::out | ios::trunc);
    if (!fc.is_open()) {
        cout << "open " << tfname << " error" << endl;
        exit(-1);
    }
    queue<FCB*> myQueue;
    //ÿһ�е�����
    string rootInfo = "";
    int count = 1;
    //д����ڵ�����
    rootInfo += this->num2string(count++);
    rootInfo += " ";
    rootInfo += root->name;
    fc << rootInfo << endl;

    myQueue.push(root);
    while (!myQueue.empty()) {
        auto tem = myQueue.front();
        myQueue.pop();
        string temInfo = "";
        //ֻд��tem �ӽڵ�
        //���Ӻ���
        auto tem_child = tem->child;
        if (tem_child != nullptr) {
            temInfo += num2string(count++);
            temInfo += " ";
            temInfo += tem_child->name;
            myQueue.push(tem_child);
        }
        else {
            temInfo += "0 ";
        }
        fc << temInfo << endl;
        temInfo.clear();
        //�����ֵ�
        auto tem_sibling = tem->sibling;
        if (tem_sibling != nullptr) {
            temInfo += num2string(count++);
            temInfo += " ";
            temInfo += tem_sibling->name;
            myQueue.push(tem_sibling);
        }
        else {
            temInfo += "0 ";
        }
        fc << temInfo << endl;
    }

    //�ر��ļ�
    fc.close();
}

void FileSystem::updateTreeFile2()
{
    string tfname = "tree.txt";
    // string tfname = TreeFileName;
    //�������
    fstream fc;
    fc.open(tfname, ios::out | ios::trunc);
    if (!fc.is_open()) {
        cout << "open " << tfname << " error" << endl;
        exit(-1);
    }
    queue<FCB*> myQueue;
    int count = 1;
    //��д���
    fc << this->FCB2Line(root, count++);
    myQueue.push(root);
    //�������
    while (!myQueue.empty()) {
        auto tem = myQueue.front();
        myQueue.pop();
        //���Ӻ���
        auto tem_child = tem->child;
        fc << endl;
        fc << this->FCB2Line(tem_child, count++);
        if (tem_child != nullptr) {
            myQueue.push(tem_child);
        }

        //�����ֵ�
        auto tem_sibling = tem->sibling;
        fc << endl;
        fc << this->FCB2Line(tem_sibling, count++);
        if (tem_sibling != nullptr) {
            myQueue.push(tem_sibling);
        }
    }
    //�ر��ļ�
    fc.close();
}


string FileSystem::num2string(int num)
{
    string ret = "";
    while (num != 0) {
        ret += (char)(num % 10 + '0');
        num = num / 10;
    }
    reverse(ret.begin(), ret.end());
    // cout << "ret:" << ret << endl;
    return ret;
}

bool FileSystem::isExistFile(const string& filename)
{
    auto tem = this->curFCB->child;
    //��������
    while (tem != nullptr) {
        if (tem->isFile() && filename == tem->name) {
            return true;
        }
        tem = tem->sibling;
    }
    return false;
}

bool FileSystem::isExistDir(const string& dirname)
{
    auto tem = this->curFCB->child;
    //��������
    while (tem != nullptr) {
        if (tem->isDir() && dirname == tem->name) {
            return true;
        }
        tem = tem->sibling;
    }
    return false;
}

void FileSystem::addNode(FCB* curNode, FCB* newNode)
{
    auto LNode = curNode->child;
    //ֱ�Ӱ�newNode ���� curNode�������ϼ���
    curNode->child = newNode;
    newNode->parent = curNode;
    newNode->sibling = LNode;
    if (LNode != nullptr) {
        LNode->parent = newNode;
    }

    // auto LNode = curNode->child;
    // //����Ϊ��,ֱ�ӽ�newNode����curNode������
    // if(LNode == nullptr) {
    //     curNode->child = newNode;
    //     newNode->parent = curNode;
    //     return;
    // }
    // auto LRNode = LNode->sibling;
    // //������Ϊ�� LRNodeΪ�� newNode ����LNode������
    // if(LRNode == nullptr) {
    //     LNode->sibling = newNode;
    //     newNode->parent = LNode;
    //     return;
    // }
    // //����Ϊ�� ��newNode���뵽 LNode �� LRNode֮��

}

FCB* FileSystem::findFile(FCB* curNode, const string& name)
{
    auto tem = curNode->child;
    while (tem != nullptr) {
        if (tem->name == name) {
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
    if (flag == 0) {
        return false;
    }
    //�ļ��ĸ������
    string fname;
    int ftype;
    int flimit;
    string uname;

    //�����ļ�����
    i_line >> fname >> ftype >> flimit >> uname;
    gfcb = new FCB(fname);
    gfcb->type = ftype + '0';
    gfcb->rwx = flimit;
    gfcb->uname = uname;

    //�����ļ��ڴ沿��
    if (gfcb->isFile()) {
        i_line >> gfcb->locationLen;
        //�����ڴ������
        for (int i = 0; i < gfcb->locationLen; i++) {
            int vtem;
            i_line >> vtem;
            gfcb->location.push_back(vtem);
        }
        i_line >> gfcb->beginByte;
        i_line >> gfcb->endByte;

        //��������ʼ��������Ϣ
        if (gfcb->locationLen > 0) {
            vector<int> f_vec;
            f_vec.push_back(gfcb->location[0]);
            f_vec.push_back(gfcb->location[gfcb->location.size() - 1]);
            f_vec.push_back(gfcb->beginByte);
            f_vec.push_back(gfcb->endByte);
            mem_alloc->addMemByVec(f_vec);
        }
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

TreeNode* FileSystem::matchPath(const string path)//���� dirTree ����һ��path������ĩβ�Ľڵ�
{
    vector<string> res = split(path, "/");
    TreeNode* temp = this->root;
    TreeNode* lastptr = nullptr;

    int i;
    for (i = 0; i < res.size(); i++)
    {
        while (temp != nullptr)
        {
            if (temp->name == res[i] /*&& temp->type == 1*/)//�ڵ�ǰ���ҵ���
            {
                if (i < res.size() - 1)
                {
                    lastptr = temp;
                    temp = temp->child;//������һ���ƥ��
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
        cout << "·��ƥ��ɹ�" << endl;
        if (lastptr == nullptr)
            return this->root;
        else if (lastptr->child == temp)
            return lastptr->child;
        else
            return lastptr->sibling;
    }
    else
    {
        //cout << "�����ҵ�·��������" << endl;
        return temp;
    }
}


void FileSystem::postOrderDelSubTree(TreeNodePtr& t)//�������ɾ���ļ�����
{
    if (t == nullptr)
        return;

    postOrderDelSubTree(t->child);
    postOrderDelSubTree(t->sibling);
    //��ɾ�����ݿ�
    releaseExternalStorage(*t);
    //��ɾ��Ŀ¼��
    delete t;
    t = nullptr;
}

int FileSystem::releaseExternalStorage(FCB& fcb)//��һ���ļ����ļ����ͷ����ݿ�
{
    // int size = fcb.size;
    // freeSpaceList.insert(freeSpaceList.end(),fcb.location.begin(),fcb.location.end());
    // fcb.location.erase(fcb.location.begin(), fcb.location.end());
    // utiliRate = 100*(STORAGE_SIZE - freeSpaceList.size()) / STORAGE_SIZE;
    return 0;
}

vector<string> FileSystem::split(const string& str, const string& delim)//�ַ�����Ƭ
{
    vector<string> res;
    /*if ("" == str) return res;
    //�Ƚ�Ҫ�и���ַ�����string����ת��Ϊchar*����
    int len1 = str.length() + 1;//Ҫ����һ��\0
    char* strs = new char[len1];
    strcpy(strs,str.c_str());

    int len2 = delim.length() + 1;
    char* d = new char[len2];
    strcpy(d,delim.c_str());

    char* buf=NULL;
    char* p = strtok_r(strs, d,&buf);

    while (p)
    {
        string s = p; //�ָ�õ����ַ���ת��Ϊstring����
        res.push_back(s); //����������
        p = strtok_r(NULL, d,&buf);
    }

    delete [] strs;
    delete [] d;

    cout << "\n·��Ϊ��" << endl;
    for (int i = 0; i < res.size(); i++)
    {
        std::cout << res[i] << " ";
    }
    //cout << "�õ����ֶ�����" << res.size() << endl;*/
    return res;
}

void FileSystem::TreeChild(FCB* cur, int depth)
{
    if (cur != nullptr) {
        for (int j = 0; j < depth; j++) {
            if (j == depth - 1) {
                cout << "|_______";
            }
            else {
                cout << "        ";
            }
        }
        cout << cur->showName() << endl;
        TreeChild(cur->child, depth + 1);
        TreeChild(cur->sibling, depth);
    }
}

int FileSystem::getCurDepth(FCB* cur)
{
    int depth = 0;
    while (cur != nullptr && cur->parent != nullptr) {
        //Ϊ���Ӳ���+1
        if (cur->parent->child == cur) {
            depth++;
        }
        cur = cur->parent;
    }
    return depth;
}

string FileSystem::FCB2Line(FCB* cur, int count)
{
    if (count == 0 || cur == nullptr) {
        return "0";
    }
    string ret = "";
    //���ֱ��
    ret += num2string(count);
    ret += " ";
    //�ļ�����
    ret += cur->name;
    ret += " ";
    //�ļ�����
    ret += cur->type;
    ret += " ";
    //�ļ�Ȩ��
    ret += num2string(cur->rwx);
    ret += " ";
    //�����û�
    ret += cur->uname;
    ret += " ";
    //locationLen
    ret += num2string(cur->locationLen);
    ret += " ";

    if (cur->locationLen > 0) {
        //location
        for (int i = 0; i < cur->locationLen; i++) {
            ret += num2string(cur->location[i]);
            ret += " ";
        }
        //beginByte
        ret += num2string(cur->beginByte);
        ret += " ";
        //endByte
        ret += num2string(cur->endByte);
    }
    else {
        ret += "0 0 0";
    }

    return ret;
}


void FileSystem::Tree(FCB* cur)
{
    cout << cur->showName() << endl;
    // int depth = this->getCurDepth(cur);
    TreeChild(cur->child, 1);
}

void FileSystem::Ls(FCB* cur)
{
    //չʾcur�����Ӻ����ӵ�����(�ݽ�)
    auto tem = cur->child;
    while (tem != nullptr) {
        tem->showSelf();
        tem = tem->sibling;
    }
}

void FileSystem::Pwd(FCB* cur)
{
    vector<string> path;
    path.push_back(cur->name);
    auto tem = cur;
    //�ҵ����� ���Լ��Ǹ��׵�����
    while (tem != nullptr && tem->parent != nullptr) {
        if (tem == tem->parent->child) {
            path.push_back(tem->parent->name);
        }
        tem = tem->parent;
    }
    reverse(path.begin(), path.end());
    for (int i = 0; i < path.size(); i++) {
        cout << path[i] << "/";
    }
    cout << endl;
}

void FileSystem::Touch(const string& fileName, int size)
{
    //�жϵ�ǰĿ¼�Ƿ���ڸ��ļ�
    if (this->isExistFile(fileName)) {
        cout << "touch failed;" << fileName << " Existed" << endl;
        return;
    }
    auto temFcb = this->curFCB;
    //�����½ڵ�
    FCB* tnode = new FCB(fileName);
    tnode->size = size;
    tnode->type = FILE_TYPE;
    tnode->uname = this->curUser;
    //���½ڵ��������������
    this->addNode(temFcb, tnode);
}

void FileSystem::Mkdir(const string& dirName)
{
    //�жϵ�ǰĿ¼�Ƿ���ڸ�Ŀ¼
    if (this->isExistDir(dirName)) {
        cout << "dir:" << dirName << " has existed" << endl;
        return;
    }
    auto temFcb = this->curFCB;
    //�����½ڵ�
    FCB* tnode = new FCB(dirName);
    tnode->type = DIR_TYPE;
    tnode->uname = this->curUser;
    //���½ڵ��������������
    this->addNode(temFcb, tnode);
}

void FileSystem::Rm(const string& fileName)
{
    //�жϵ�ǰĿ¼�Ƿ���ڸ��ļ�
    if (this->isExistFile(fileName) == false && this->isExistDir(fileName) == false) {
        cout << "no such file:" << fileName << endl;
        return;
    }
    auto tem = this->curFCB;
    //���Ҵ�ɾ���ļ��ڵ�
    auto del_file = this->findFile(tem, fileName);


    //�ܾ�ɾ���ļ�Ŀ¼ �������
    if (del_file == nullptr || del_file->type == DIR_TYPE) {
        cout << "del dir error" << endl;
        return;
    }
    //��Ϊ ��ɾ���߱ض����ļ� ����del_file û������
    auto par_node = del_file->parent;
    auto sib_node = del_file->sibling;

    cout << par_node->name << endl;
    cout << sib_node->name << endl;


    //del_file�ǵ�һ���ֵ� parent->left = sib_node
    if (par_node->child == del_file) {
        par_node->child = sib_node;
    }//del_file ���ǳ��� �丸������ͬ����
    else if (par_node->sibling == del_file) {
        par_node->sibling = sib_node;
    }

    if (sib_node != nullptr) {
        sib_node->parent = par_node;
    }

    //warn : ���ڴ�ɾ���ļ�
    if (del_file) {
        //������������ͷ����ڴ�
        if (del_file->location.size() > 0) {
            vector<int> free_vec;
            free_vec.push_back(del_file->location[0]);
            free_vec.push_back(del_file->location[del_file->location.size() - 1]);
            free_vec.push_back(del_file->beginByte);
            free_vec.push_back(del_file->endByte);
            bool b_free = mem_alloc->freeMemory(free_vec);
        }
        //�ͷ�malloc�ռ�
        delete del_file;
        del_file = nullptr;
    }
}

// void FileSystem::Rm(const string path)
// {
//  	TreeNode*& pnode = matchPath(path);
//     if (pnode == nullptr)
//     {
//         cout << "ɾ���ڵ�ʧ�ܣ���·��������" << endl;
//     }
//     else
//     {
//         postOrderDelSubTree(pnode);
//         cout << "ɾ���ɹ���" << endl;
//     }
// }

void FileSystem::Ls_l(FCB* cur)
{
    //չʾcur�����Ӻ����ӵ�����(�ݽ�)
    auto tem = cur->child;
    while (tem != nullptr) {
        tem->showSelf_l();
        tem = tem->sibling;
    }
}

void FileSystem::Chmod(int LimitNum, string fileName)
{
    //�����ڸ��ļ���Ŀ¼
    if (this->isExistDir(fileName) == false && this->isExistFile(fileName) == false) {
        return;
    }
    auto ffcb = this->findFile(this->curFCB, fileName);
    ffcb->setRWX(LimitNum);
}

void FileSystem::ReadFile(const string& fileName)
{
    if (this->isExistFile(fileName) == false) {
        cout << "warn:no such file" << endl;
        return;
    }

    //��ȡ�ļ�
    FCB* rfcb = this->findFile(this->curFCB, fileName);
    //����ӦȨ��
    if (this->isPermitRead(fileName, rfcb) == false) {
        cout << "warn:premission denying" << endl;
        return;
    }

    //�ڴ�λ��
    cout << "׼��������������" << endl;
    cout << "֡�鹲��:" << rfcb->locationLen << endl;
    cout << "֡��������:" << endl;
    for (int i = 0; i < rfcb->locationLen; i++) {
        cout << rfcb->location[i] << " ";
    }
    cout << endl;
    cout << "��ʼ�����:" << rfcb->beginByte << endl;
    cout << "�������յ�:" << rfcb->endByte << endl;

    //���������Ӧ֡���� ����ʾ����
    fCreate(rfcb->location, rfcb->beginByte, rfcb->endByte, "", true);
}

void FileSystem::Cd(const string& dirName)
{
    //û�����Ŀ¼
    if (this->isExistDir(dirName) == false) {
        cout << "warn:No Such Dir" << endl;
        return;
    }
    //���Ȩ��ȷ��
    FCB* pfcb = this->findFile(curFCB, dirName);
    //����ӦȨ��
    if (this->isPermitRead(dirName, pfcb) == false) {
        cout << "warn:premission denying" << endl;
        return;
    }
    //�л�Ŀ¼
    this->curFCB = pfcb;
    cout << " cd ok" << endl;
}

void FileSystem::ViFile(const string& fileName, const string& toWriteStr)
{
    if (this->isExistFile(fileName) == false) {
        cout << "warn:no such file" << endl;
        return;
    }
    //��ȡ�ļ�
    FCB* rfcb = this->findFile(this->curFCB, fileName);
    //����ӦȨ��
    if (this->isPermitWrite(fileName, rfcb) == false) {
        cout << "warn:premission denying" << endl;
        return;
    }
    //׼��д��
    int toWriteLen = toWriteStr.length();
    cout << "LEN:" << toWriteLen << endl;
    //�����ڴ�ɹ����ͷ���Ӧ�ڴ�,Ȼ������д��
    //1.�ͷ���Ӧ �ڴ�λ�� ���̽ӿ�...
    cout << "�ͷ���������" << endl;
    cout << "֡�鹲��:" << rfcb->locationLen << endl;
    cout << "֡��������:" << endl;
    for (int i = 0; i < rfcb->locationLen; i++) {
        cout << rfcb->location[i] << " ";
    }
    cout << endl;
    cout << "��ʼ�����:" << rfcb->beginByte << endl;
    cout << "�������յ�:" << rfcb->endByte << endl;
    //1.1��mem_alloc�������ͷ�
    if (rfcb->location.size() > 0) {
        vector<int> free_vec;
        free_vec.push_back(rfcb->location[0]);
        free_vec.push_back(rfcb->location[rfcb->location.size() - 1]);
        free_vec.push_back(rfcb->beginByte);
        free_vec.push_back(rfcb->endByte);
        bool b_free = mem_alloc->freeMemory(free_vec);
        cout << "bfree:" << b_free << endl;
    }
    else {
        cout << "δռ�����,�����ͷ�" << endl;
    }
    //1.2���ڴ��ͷ� (������Բ��� ��ΪֻҪ�Ҳ���,�������ͷŵ�)

    cout << "�ͷź�����������" << endl;
    mem_alloc->showMemAllocInfo();

    //���������ڴ�
    vector<int> disk_vec = this->mem_alloc->allocMemory(toWriteLen);
    //�����ڴ�ʧ�� �˳�
    if (disk_vec[0] == -1 || disk_vec.size() < 4) {
        cout << "no memory" << endl;
        return;
    }

    //2.������Ӧ�ַ����ڴ�
    vector<int> c_location;
    for (int i = disk_vec[0]; i <= disk_vec[1]; i++) {
        c_location.push_back(i);
    }
    int beginByte = disk_vec[2];
    int endByte = disk_vec[3];
    cout << "������������" << endl;
    cout << "֡�鹲��:" << c_location.size() << endl;
    rfcb->locationLen = c_location.size();
    rfcb->location.resize(rfcb->locationLen);
    cout << "֡��������:" << endl;
    for (int i = 0; i < c_location.size(); i++) {
        cout << c_location[i] << " ";
        rfcb->location[i] = c_location[i];
    }
    cout << endl;
    cout << "��ʼ�����:" << beginByte << endl;
    rfcb->beginByte = beginByte;
    cout << "�������յ�:" << endByte << endl;
    rfcb->endByte = endByte;


    cout << "���������������" << endl;
    mem_alloc->showMemAllocInfo();
    //�ɹ�д�� ���̽ӿ�
    fCreate(rfcb->location, rfcb->beginByte, rfcb->endByte, toWriteStr,false);
}

void FileSystem::preOrder(FCB* cur)
{
    if (cur != nullptr) {
        cout << cur->name << endl;
        preOrder(cur->child);
        preOrder(cur->sibling);
    }
    else {
        cout << "null" << endl;
    }
}
void FileSystem::testTree(FCB* cur)
{
    cout << cur->showName() << endl;
    // int depth = this->getCurDepth(cur);
    TestTreeChild(cur->child, 1);
}
void FileSystem::TestTreeChild(FCB* cur, int depth)
{
    if (cur != nullptr) {
        for (int j = 0; j < depth; j++) {
            if (j == depth - 1) {
                cout << "|_______";
            }
            else {
                cout << "        ";
            }
        }
        cout << "I-";
        cout << cur->showName() << endl;
        TreeChild(cur->child, depth + 1);
        TreeChild(cur->sibling, depth);
    }
}
void FileSystem::analyse(string s) {
    string tmp;
    vector<string> words;
    istringstream record(s);
    while (record >> tmp) {
        words.push_back(tmp);
    }
    if (s == "tree") {
        Tree(this->curFCB);
        cout << "tree��Ŀ¼" << endl;
    }
    else if (s == "ls") {
        Ls(this->curFCB);
        cout << "ls Ŀ¼" << endl;
    }
    else if (s == "ls_l") {
        Ls_l(this->curFCB);
        cout << "ls_l Ŀ¼" << endl;
    }
    else if (words[0] == "pwd") {
        Pwd(this->curFCB);
        cout << "file:" << this->curFCB->name << endl;
    }
    else if (words[0] == "touch") {
        Touch(words[1], 0);
        cout << "touch sucesss"<< endl;
    }
    else if (words[0] == "rm") {
        Rm(words[1]);
        cout << "rm sucesss" << endl;
    }
    else if (words[0] == "Mkdir") {
        Mkdir(words[1]);
        cout << "Mkdir sucesss" << endl;
    }
    else if (words[0] == "chmod") {
        Chmod(atoi(words[1].c_str()), words[2]);
        cout << "Chmod sucesss" << endl;
    }
    else if (words[0] == "vi") {
        ViFile(words[1], words[2]);
        cout << "vi end" << endl;
    }
    else if (words[0] == "cat") {
        ReadFile(words[1]);
        cout << "read end" << endl;
    }
    else if (words[0] == "cd") {
        Cd(words[1]);
        cout << "cd end" << endl;
    }
    else {
        cout << "Please enter the instruction in the format" << endl;
    }
}
void FileSystem::test()
{
    cout << endl << "---------------------------" << endl;
    cout << "tree��Ŀ¼" << endl;
    Tree(root);
    cout << endl << "---------------------------" << endl;
    cout << "ls ��Ŀ¼" << endl;
    Ls(root);
    cout << endl << "---------------------------" << endl;
    cout << "pwd ĳ�ڵ�" << endl;
    auto pwd_fcb = root->child->sibling->child;
    cout << "file:" << pwd_fcb->name << endl;
    Pwd(pwd_fcb);
    cout << endl << "---------------------------" << endl;
    cout << "д�� Ŀ¼�ļ�" << endl;
    updateTreeFile();
    cout << endl << "---------------------------" << endl;
    cout << "touch �� " << curFCB->name << " ����ļ��ڵ�" << "hello.cpp" << endl;
    Touch("hello.cpp", 5);
    cout << "�ٴδ���hello.cpp" << endl;
    Touch("hello.cpp", 5);
    cout << endl << "---------------------------" << endl;
    cout << "tree ��Ŀ¼" << endl;
    Tree(root);
    cout << endl << "---------------------------" << endl;
    cout << "mkdir �� " << curFCB->name << " ���Ŀ¼�ڵ�" << "user2" << endl;
    Mkdir("user2");
    cout << "�ٴδ���user2" << endl;
    Mkdir("user2");
    cout << endl << "---------------------------" << endl;
    cout << "tree ��Ŀ¼" << endl;
    Tree(root);
    cout << endl << "---------------------------" << endl;
    cout << "rm " << "user2" << endl;
    Rm("user2");
    cout << "rm " << "hello.cpp" << endl;
    Rm("hello.cpp");
    cout << endl << "---------------------------" << endl;
    cout << "tree ��Ŀ¼" << endl;
    Tree(root);
    cout << endl << endl;
    cout << "touch hello.cpp" << endl;
    Touch("hello.cpp", 5);
    cout << "ls -l ����" << endl;
    Ls_l(root);
    cout << endl << "---------------------------" << endl;
    cout << "chmod 776 hello.cpp" << endl;
    Chmod(776, "hello.cpp");
    cout << endl << "---------------------------" << endl;
    cout << "ls -l ����" << endl;
    Ls_l(root);
    cout << endl << "---------------------------" << endl;
}

void FileSystem::test2()
{
    //this->Ls_l(this->curFCB);   cout << endl;

    //this->Cd("bin");

    //this->Ls_l(this->curFCB);   cout << endl;

    //this->ReadFile("a.sh");     cout << endl;

    //this->ViFile("a.sh");       cout << endl;
}

void FileSystem::test3()
{
    TreeChild(curFCB, 0);
    cout << endl;
    Tree(root);
    cout << endl;
    Cd("bin");
    Tree(this->curFCB);
    // Rm("a.sh");
    cout << endl;
    Tree(root);
}

void FileSystem::test4()
{
    this->Tree(this->curFCB);
    mem_alloc->showMemAllocInfo();
    cout << "\n\n\n\n" << endl;
    this->Cd("bin");
    this->Tree(this->curFCB);
    this->Ls_l(this->curFCB);
    this->ViFile("a.sh", "I am a.sh");
    cout << "\n\n\n\n" << endl;
    this->ReadFile("a.sh");
}

void FileSystem::test5()
{
    this->updateTreeFile2();
    //this->Tree(this->curFCB);

    //Touch("a.txt", 5);

    //this->Tree(this->curFCB);

    //Rm("a.txt");

    //this->Tree(this->curFCB);

    //Cd("bin");

    //this->Tree(this->curFCB);

    //Touch("a.txt", 5);

    //this->Tree(this->curFCB);

    //Rm("a.sh");

    //this->Tree(this->curFCB);



    //Cd("bin");
    //Touch("a.txt", 5);

    //Tree(this->curFCB);
    //Rm("a.sh");

    //auto tem = this->curFCB->child;
    //cout << tem->name << endl;
    //tem = tem->sibling;
    //cout << tem->name << endl;
    //this->Tree(this->curFCB);
}


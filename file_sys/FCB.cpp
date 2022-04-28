#include "FCB.h"

FCB::FCB(char type, const string& name, int size, FCB* parent, FCB* child, FCB* sibling)
{
    this->type = type;
    this->name = name;
    this->size = size;
    this->parent = parent;
    this->child = child;
    this->sibling = sibling;
}

FCB::FCB(const string& name)
{
    this->name = name;
    this->child = nullptr;
    this->sibling = nullptr;
    this->parent = nullptr;
    //权限默认控制rwxrwxrwx 即777
    this->rwx = 777;
}

void FCB::showSelf()
{
    string ret = "";
    ret += showName();
    cout << ret << "\t";
}

void FCB::showSelf_l()
{
    string ret = "";
    if(this->type == FILE_TYPE) {
        //文件
        ret += '-';
    } else if(this->type == DIR_TYPE){
        //目录
        ret += 'd';
    } else {
        ret += '+';
    }
    ret += this->getRWX();
    ret += '\t';
    ret += this->uname;
    ret += '\t';
    ret += showName();

    cout << ret << endl;
}

void FCB::setParent(FCB* parent)
{
    this->parent = parent;
}

void FCB::setChild(FCB* child)
{
    this->child = child;
}

void FCB::setSibling(FCB* sibling)
{
    this->sibling = sibling;
}

void FCB::setRWX(int num)
{
    if(num < 0 || num > 777) {
        cout << "limit num error" << endl;
        return;
    }
    this->rwx = num;
}

string FCB::getRWX()
{
    //owner、group、others
    string owner = num2RWX(this->rwx / 100);
    string group = num2RWX((this->rwx % 100) / 10);
    string others = num2RWX(this->rwx % 10);
    string ret = owner + group + others;
    return ret;
}

string FCB::num2RWX(int num)
{
    if(num < 0) {
        cout << "error FCB:num2RWX" << endl;
        return "";
    }
    string srwx = "xwr";
    string ret = "";
    for(int i = 0; i < 3; i++) {
        if(num & 1) {
            ret += srwx[i];
        } else {
            ret += '-';
        }
        num = num >> 1;
    }
    reverse(ret.begin(), ret.end());
    return ret;
}

string FCB::showName()
{
    string ret = "";
    if(this->type == DIR_TYPE) {
        ret += '[';
    }
    ret += this->name;
    if(this->type == DIR_TYPE) {
        ret += ']';
    }
    return ret;
}

bool FCB::isFile()
{
    return this->type == FILE_TYPE;
}
bool FCB::isDir()
{
    return this->type == DIR_TYPE;
}
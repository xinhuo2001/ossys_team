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
}

void FCB::showSelf()
{
    string ret = "";
    ret += this->name;
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

bool FCB::isFile()
{
    return this->type == FILE_TYPE;
}
bool FCB::isDir()
{
    return this->type == DIR_TYPE;
}
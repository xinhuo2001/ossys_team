//#include"memory.cpp"
#include "process.h"
#include "FileSystem.h"
#include"memory.h"
using namespace std;

clock_t Timer, start_time;
int pidNum = 0;
//int getOccupied_size() = 100; //内存块数 
double time_slice = 1000;
int debuglevel = 1;
list<PCB>readyList1;//就绪队列1
list<PCB>readyList2;//就绪队列2
list<PCB>readyList3;//就绪队列3
list<PCB>blockList;//阻塞队列
list<PCB>externalPCBList;//挂起进程队列
string state[6] = { "Creating",   "Ready",    "Running", "Block","End","SUSPEND" };/* NOLINT */

void PrintInfo() {
    Timer = clock();
    cout << "时间:" << (double)(Timer - start_time) << endl;
    cout << "剩余内存:" << getOccupied_size() << endl;
    cout << "进程数:" << pidNum << endl<<endl;
    //printMemoryInfo();
    cout << "就绪队列3进程数:" << readyList3.size() << endl;
    cout << "进程信息:" << endl;
    cout << "pid" << "  " << "state" << "  " << "useMemory" << "  " << "start_time" << "    " <<"remain_time"<<"    "<< "priority" << endl;
    for (auto iter : readyList3) {
        cout << iter.pid << "    " << state[iter.state] << "    "
            << iter.Mem_info->task_size << "          " << iter.start_time << "            " << iter.remain_time << "                " << iter.priority << endl;
    }
    cout << endl;
    cout << "就绪队列2进程数:" << readyList2.size() << endl;
    cout << "进程信息:" << endl;
    cout << "pid" << "  " << "state" << "  " << "useMemory" << "  " << "start_time" << "    " << "remain_time" << "    " << "priority" << endl;
    for (auto iter : readyList2) {
        cout << iter.pid << "    " << state[iter.state] << "    "
            << iter.Mem_info->task_size << "          " << iter.start_time << "            " << iter.remain_time << "               " << iter.priority << endl;
    }
    cout << endl;
    cout << "就绪队列1进程数:" << readyList1.size() << endl;
    cout << "进程信息:" << endl;
    cout << "pid" << "  " << "state" << "  " << "useMemory" << "  " << "start_time" << "    " << "remain_time" << "    " << "priority" << endl;
    for (auto iter : readyList1) {
        cout << iter.pid << "    " << state[iter.state] << "    "
            << iter.Mem_info->task_size << "          " << iter.start_time << "             " << iter.remain_time << "               " << iter.priority << endl;
    }
    cout << endl;
    cout << "阻塞队列进程数" << blockList.size() << endl;
    cout << "进程信息:" << endl;
    cout << "pid" << "  " << "state" << "  " << "useMemory" << "  " << "start_time" << "    " << "remain_time" << "    " << "priority" << endl;
    for (auto iter : blockList) {
        cout << iter.pid << "    " << state[iter.state] << "    "
            << iter.Mem_info->task_size << "          " << iter.start_time << "            " << iter.remain_time << "                " << iter.priority << endl;
    }
    cout << "外部队列进程数" << externalPCBList.size() << endl;
    cout << "进程信息:" << endl;
    cout << "pid" << "  " << "state" << "  " << "useMemory" << "  " << "start_time" << "    " << "priority" << endl;
    for (auto iter = externalPCBList.begin();iter != externalPCBList.end(); ++iter) {
        cout << iter->pid << "    " << state[iter->state] << "    "
             << iter->Mem_info->task_size << "          " << iter->start_time << "           " << iter->priority << endl;
    }
}
void Block(PCB& p)//进入阻塞队列
{
    p.state = 3;
    blockList.push_back(p);
    if (debuglevel >= 2) {
        cout << "进程" << p.pid << "进入阻塞队列" << endl;
    }
}
bool Ready(PCB& p,bool blocked)//进入就绪队列
{
    if (applyForMemory(p))
    {
        p.state = 1;
        switch (p.priority) {
        case 1:readyList1.push_back(p); break;
        case 2:readyList2.push_back(p); break;
        case 3:readyList3.push_back(p); break;
        }
        if (debuglevel >= 2) {
            cout << "进程" << p.pid << "进入就绪态队列" << p.priority << endl;
        }
        return true;
    }
    else {
        if (!blocked) {
            Block(p);
        }
        return false;
    }
}
//创建进程
PCB Create(int task_size, int priority)
{
    int i;
    PCB p;
    p.pid = pidNum + 1;//Sum为进程总数
    pidNum++;
    p.state = 0;//创建状态
    p.CPUstate = 0;//CPU处于用户态
    p.priority = priority;
    p.serve_time = rand() % 8000 + 1;
    p.remain_time = p.serve_time;
    p.Mem_info = (struct Memory_struct*)malloc(sizeof(Memory_struct));
    p.Mem_info->task_size = task_size;
    p.Mem_info->is_apply = false;
    p.start_time = Timer;
    for (int i = 0; i < TABLE_LEN; i++) {
        p.disc_offset[i] = i;
    }
    for (int i = 0; i < SEQ_LEN; i++) {
        int j = rand() % 7 + 1;
        p.sequence[i] = j;
    }
    if (debuglevel >= 2) {
        cout << "创建进程" << p.pid << endl;
    }
    return p;
}

void Stop(PCB& p, list<PCB> lists)//结束进程
{
    p.state = 4;
    list<PCB>::iterator e;
    if (debuglevel >= 2) {
        cout << "进程" << p.pid << "结束" << endl;
    }
    for (e = lists.begin(); e != lists.end();) {
        if (e->pid == p.pid) {
            cout << "进程" << e->pid << "结束" << endl;
            releaseMemory(e->pid);
            e = lists.erase(e);
            if (e != lists.end()) {
                e++;
            }
            else {
                break;
            }
        }
        else
        {
            e++;
        }
    }
}
//周期性回收和处理阻塞队列
void Retrieve()
{
    list<PCB>::iterator e;
    if (!readyList3.empty()) {
        for (e = readyList3.begin(); e != readyList3.end();) {
            if (e->state == 4) {
                if (debuglevel >= 2) {
                    cout << "进程" << e->pid << "结束" << endl;
                }
                releaseMemory(e->pid);
                e = readyList3.erase(e);
                if (e != readyList3.end()) {
                    e++;
                }
            }
            else
            {
                e++;
            }
        }
    }
    if (!readyList2.empty()) {
        for (e = readyList2.begin(); e != readyList2.end();) {
            if (e->state == 4) {
                if (debuglevel >= 2) {
                    cout << "进程" << e->pid << "结束" << endl;
                }
                releaseMemory(e->pid);
                e = readyList2.erase(e);
                if (e != readyList2.end()) {
                    e++;
                }
            }
            else
            {
                e++;
            }
        }
    }
    if (!readyList1.empty()) {
        for (e = readyList1.begin(); e != readyList1.end();) {
            if (e->state == 4) {
                if (debuglevel >= 2) {
                    cout << "进程" << e->pid << "结束" << endl;
                }
                releaseMemory(e->pid);
                e = readyList1.erase(e);
                if (e != readyList1.end()) {
                    e++;
                }
            }
            else
            {
                e++;
            }
        }
    }
    if (!blockList.empty()) {
        for (e = blockList.begin(); e != blockList.end();) {
            if (e->state == 4) {
                if (debuglevel >= 2) {
                    cout << "进程" << e->pid << "结束" << endl;
                }
                e = blockList.erase(e);
                if (e != blockList.end()) {
                    e++;
                }
            }
            else {
                if (Ready(*e,true)) {
                    e = blockList.erase(e);
                    if (e != blockList.end()) {
                        e++;
                    }
                }
                else
                {
                    if (e != blockList.end()) {
                        e++;
                    }
                }
            }
        }
    }
    cout << "Retrieve success" << endl;
}
//多级反馈队列调度算法
void CPUScheduling() {
    int site,time = 0;
    char c;
    string s;
    PCB e;
    if (!readyList3.empty()) {
        e = readyList3.front();
        if (e.remain_time > time_slice) {
            for (int i = 0; i < SEQ_LEN; i++) {
                site=handle_process(e.pid);
            }
            Sleep(time_slice);
            e.remain_time -= time_slice;
            e.priority = 2;
            readyList2.push_back(e);
            readyList3.pop_front();
        }
        else {
            Sleep(e.remain_time);
            e.state = 4;
            if (debuglevel >= 2) {
                cout << "进程" << e.pid << "结束" << endl;
            }
            releaseMemory(e.pid);
            readyList3.pop_front();
        }
    }
    if (!readyList2.empty() && readyList3.empty()) {
        e = readyList2.front(); 
        if (e.remain_time > time_slice) {
            for (int i = 0; i < SEQ_LEN; i++) {
                handle_process(e.pid);
            }
            Sleep(time_slice);
            e.remain_time -= time_slice;
            e.priority = 1;
            readyList1.push_back(e);
            readyList2.pop_front();
        }
        else {
             Sleep(e.remain_time);
             e.state = 4;
             if (debuglevel >= 2) {
                 cout << "进程" << e.pid << "结束" << endl;
             }
             releaseMemory(e.pid);
             readyList2.pop_front();
         }
    }
    if (!readyList1.empty() && readyList3.empty() && readyList2.empty()) {
        e = readyList1.front();
        if (e.remain_time > time_slice) {
            for (int i = 0; i < SEQ_LEN; i++) {
                handle_process(e.pid);
            }
              Sleep(time_slice);
              e.remain_time -= time_slice;
              readyList1.push_back(e);
              readyList1.pop_front();
        }
        else {
              Sleep(e.remain_time);
              e.state = 4;
              if (debuglevel >= 2) {
                  cout << "进程" << e.pid << "结束" << endl;
              }
              releaseMemory(e.pid);
              readyList1.pop_front();
         }
     }
}
void LongTermScheduling(string filename)
{
    //长期调度,将外存中的程序调度内存并创建进程
    //打开新加入的程序文件
    ifstream fp;
    fp.open(filename.c_str(), ios::in);
    if (fp)
    {
        list<string> other;
        string line;
        int task_size = 0; //占用的内存块数
        int prority = 1; //优先级
        while (getline(fp, line))
        {
            //处理各种在创建进程时相关的指令
            if (line[0] == 'M') //内存相关指令，提取出需要占用的内存块数
            {
                string tmp = line.substr(2, line.size());
                task_size = atoi(tmp.c_str());
            }
            else if (line[0] == 'Y') //优先级相关指令 提取出优先级
            {
                string tmp = line.substr(2, line.size());
                prority = atoi(tmp.c_str());
            }
            PCB p = Create(task_size, prority);
            p.state = 3;
            blockList.push_back(p);
        }
        if (debuglevel == 3) {
            cout << "将程序" << filename << "调入内存" << endl;
        }
    }
    fp.close();
}
int MidTermScheduling(int inOrOut)
{
    if (inOrOut == OUT) //调出 
    {
        if (blockList.size() == 0)
        {
            if (debuglevel == 3) {
                cout << "阻塞队列为空，中期调度失败" << endl;
            }
            return 0;
        }
        for (int i = 0; i < blockList.size(); ++i)
        {
            pair<int, int> pcbPair = { INT_MAX, -1 };
            for (list<PCB>::iterator it = blockList.begin(); it != blockList.end(); ++it)  //找到优先级最低的进程
            {
                if (it->priority < pcbPair.first && it->Mem_info->is_apply)
                {
                    pcbPair.first = it->priority;
                    pcbPair.second = it->pid;
                }
            }
            if (pcbPair.second != -1) //找到了 
            {
                for (list<PCB>::iterator it = blockList.begin(); it != blockList.end(); ++it)
                {
                    if (it->pid == pcbPair.second)    //找到并换出 
                    {
                        it->state = PROCESS_SUSPEND_BLOCK;   //进程挂起 
                        releaseMemory(it->pid);
                        it->Mem_info->is_apply = 0;
                        blockList.erase(it);
                        externalPCBList.push_back(*it);
                        if (debuglevel == 3) {
                            cout << "调出进程pid=" << it->pid << endl;
                        }
                        break;
                    }
                }
            }
            else return 0;   //没找到，调度失败 
            /*if (getOccupied_size() >= needed)
            {
                break;
            }*/
        }
    }
    else if (inOrOut == IN)
    {
        pair<int, int> pcbPair = { INT_MIN, -1 };
        for (list<PCB>::iterator it = externalPCBList.begin(); it != externalPCBList.end(); ++it) //找到优先级最高的进程
        {
            if (it->priority > pcbPair.first&& it->Mem_info->task_size < (PHYSICAL_SIZE-getOccupied_size()))
            {
                pcbPair.first = it->priority;
                pcbPair.second = it->pid;
            }
        }
        if (pcbPair.second != -1)
        {
            for (list<PCB>::iterator it = externalPCBList.begin(); it != externalPCBList.end(); ++it)
            {
                if (it->pid == pcbPair.second)   //找到并换入 
                {
                    applyForMemory(*it);
                    it->Mem_info->is_apply = 1;
                    externalPCBList.erase(it);
                    blockList.push_back(*it);
                    break;
                }
            }
        }
    }
    return 1;
}
int MidTerm_OUT()
{
        if (readyList1.size() != 0)
        {
            list<PCB>::iterator it = readyList1.begin();
            it->state = PROCESS_SUSPEND_BLOCK;   //进程挂起 
            Stop(*it, readyList1);
            externalPCBList.push_back(*it);
            if (debuglevel == 3) {
                cout << "调出进程pid=" << it->pid << endl;
            }
            return 0;
        }
            if (readyList1.size() == 0 && readyList2.size() != 0)
            {
                list<PCB>::iterator it = readyList2.begin();
                it->state = PROCESS_SUSPEND_BLOCK;   //进程挂起 
                Stop(*it, readyList2);
                externalPCBList.push_back(*it);
                if (debuglevel == 3) {
                    cout << "调出进程pid=" << it->pid << endl;
                }
                    return 0;
            }
            if (readyList1.size() == 0 && readyList2.size() == 0 && readyList3.size() != 0)
            {
                list<PCB>::iterator it = readyList3.begin();
                it->state = PROCESS_SUSPEND_BLOCK;   //进程挂起 
                Stop(*it, readyList3);
                externalPCBList.push_back(*it);
                if (debuglevel == 3) {
                    cout << "调出进程pid=" << it->pid << endl;
                }
                return 0;
            }
    return 0;
}
void Blockdelete(PCB& p)
{
    for (auto i = blockList.begin(); i != blockList.end();)
    {
        if (i->pid == p.pid)
        {
            blockList.erase(i++);
        }
        else
        {
            i++;
        }
    }
};
void Externaldelete(PCB& p)
{
    for (auto i = externalPCBList.begin(); i != externalPCBList.end();)
    {
        if (i->pid == p.pid)
        {
            externalPCBList.erase(i++);
        }
        else
        {
            i++;
        }
    }
};
void handle(string s, FileSystem &fs) {
    string tmp;
    vector<string> words;
    istringstream record(s);
    int n;
    PCB p;
    while (record >> tmp) {
        words.push_back(tmp);
    }
    if (words[0] == "debuglevel") {
        if (atoi(words[1].c_str()) == 1 || atoi(words[1].c_str()) == 2 || atoi(words[1].c_str()) == 3) {
            debuglevel = atoi(words[1].c_str());
            cout << endl <<">debuglevel turn to " << words[1] << endl;
        }
        else
        {
            cout << "command error" << endl;
        }
    }
    else if (words[0] == "start") {
        n = rand() % 3 + 1;
        p = Create(n * 2, n);
        Ready(p, false);
    }
    else if (words[0] == "meminfo") {
        showMemInfo();
    }
    else
    {
        fs.analyse(s);
    }
}
//根据文件模块信息创建进程
void fCreate(vector<int> location, int start, int end, string s, bool read)
{
    cout << start << "  " << end << "  " << s << endl;
    if (location.size() == 1 && s.length()!=0 &&s.length() - 1 > end - start) {
        cout << "too long to handle" << endl;
    }
    else
    {
        PCB p;
        p.pid = pidNum + 1;//Sum为进程总数
        pidNum++;
        p.state = 0;//创建状态
        p.CPUstate = 0;//CPU处于用户态
        p.priority = 3;
        p.serve_time = rand() % 10 + 1;
        p.remain_time = p.serve_time;
        p.Mem_info = (struct Memory_struct*)malloc(sizeof(Memory_struct));
        p.Mem_info->task_size = location.size();
        p.Mem_info->is_apply = false;
        p.start_time = Timer;
        if ((location[0] - 20) * 1000 + start <= 0) {
            cout << "offset error" << endl;
        }
        else
        {
            for (int i = 0; i < TABLE_LEN; i++) {
                p.disc_offset[i] = (location[0] - 20) * 1000 + start;
            }
            for (int i = 0; i < SEQ_LEN; i++) {
                int j = rand() % 7 + 1;
                p.sequence[i] = j;
            }
            if (debuglevel >= 2) {
                cout << "创建进程" << p.pid << endl;
            }
            while (!Ready(p, false))
            {
                MidTerm_OUT();
            }
            int offset;
            string result;
            for (int i = 0; i < SEQ_LEN; i++) {
                offset = handle_process(p.pid);
                if (i == 3 && read == false) {
                    writeMem(offset, (char*)s.data());
                    isChanged(p.pid, offset);
                }
                if (i == 3 && read == true) {
                    result = readMem(p.disc_offset[i]);
                    cout << result << endl;
                }
            }
            Stop(p, readyList3);
        }
        }
}
void run(FileSystem &fs) {
    int n, m, times = 0;
    PCB p;
    string s;
    char c;
    for (int i = 0; i < 15; i++) {
        m = rand() % 3 + 1;
        p = Create(m * 2, m);
        Ready(p, false);
    }
    if (debuglevel == 3) {
        PrintInfo();
    }
    while (1) {
        CPUScheduling();
        n = rand() % 100 + 1;
        if (n > 70) {
            m = rand() % 3 + 1;
            p = Create(m, m);
            Ready(p, false);
        }
        if (n > 90) {
            if (getOccupied_size() < 10) {
                MidTermScheduling(IN);
            }
            else
            {
                MidTermScheduling(OUT);
            }
        }
        if (n > 98) {
            LongTermScheduling("example.txt");
        }
        times++;
        if (times % 10 == 0) {
            Retrieve();
            if (debuglevel == 3) {
                PrintInfo();
            }
        }
        if (_kbhit()) {
            cout << endl;
            c = _getch();//读取用户输入
            cout << c;
            s += c;;
            while (1) {
                if (_kbhit()) {
                    c = _getch();
                    cout << c;
                    if (c == 27) { break; }//按下ESC时退出循环
                    s += c;;
                }
            }
        }
        if (s != "") {
            if (s == "end") break;
            cout <<" " <<endl;
            cout << "||";
            handle(s,fs);
            s = "";
        }
    }
}
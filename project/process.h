//
// Created by 79069 on 2022/4/5.
//
#ifndef OS_PROCESS_H
#define OS_PROCESS_H
#include "global.h"
#include"FileSystem.h"
using namespace::std;

struct Memory_struct
{
    unsigned long task_size;//占用内存块数
    unsigned long total_vm, locked_vm, shared_vm, exec_vm;
    //用户态堆栈的页数，
    unsigned long stack_vm, reserved_vm, def_flags, nr_ptes;
    //维护代码段和数据段
    unsigned long start_code, end_code, start_data, end_data;
    //维护堆和栈
    unsigned long start_brk, brk, start_stack;
    bool is_apply;//是否分配了内存
};

typedef struct task_struct {
    unsigned short UID;//用户标识
    int pid;//进程标识
    int state;//状态号
    int CPUstate;//特权状态号 用户/内核
    int priority; //进程优先级
    clock_t start_time;//进程创建时间
    double serve_time;//进程需要服务时间
    double remain_time;//进程仍要服务时间
    clock_t CPU_time;//进程使用CPU计算时间
    struct Memory_struct* Mem_info;//内存进程管理信息
    int disc_offset[TABLE_LEN];//磁盘偏移量
    int sequence[SEQ_LEN];//访问序列
}PCB;
void fCreate(vector<int> location, int start, int end, string s, bool read);//根据文件模块信息创建进程
void run(FileSystem &fs);//运行
void PrintInfo();//打印信息
void Block(PCB& p);//进入阻塞队列
void Blockdelete(PCB& p);//从阻塞队列移除
void Externaldelete(PCB& p);//从挂起队列移除
bool Ready(PCB& p, bool blocked);//进入就绪队列
void Stop(PCB& p, list<PCB> lists);//结束进程
PCB Create(int task_size, int priority);//根据优先级和内存创建进程
void Retrieve();//周期性回收和处理阻塞队列
void CPUScheduling();//短期调度，使用多级反馈队列调度算法
void LongTermScheduling(string filename);//长期调度
int MidTermScheduling(int inOrOut);//中期调度
int MidTerm_OUT();//中期调度（换出就绪队列进程）
void handle(string s, FileSystem &fs);//处理用户输入
#endif //OS_PROCESS_H
#pragma once

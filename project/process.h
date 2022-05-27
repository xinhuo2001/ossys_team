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
    unsigned long task_size;//ռ���ڴ����
    unsigned long total_vm, locked_vm, shared_vm, exec_vm;
    //�û�̬��ջ��ҳ����
    unsigned long stack_vm, reserved_vm, def_flags, nr_ptes;
    //ά������κ����ݶ�
    unsigned long start_code, end_code, start_data, end_data;
    //ά���Ѻ�ջ
    unsigned long start_brk, brk, start_stack;
    bool is_apply;//�Ƿ�������ڴ�
};

typedef struct task_struct {
    unsigned short UID;//�û���ʶ
    int pid;//���̱�ʶ
    int state;//״̬��
    int CPUstate;//��Ȩ״̬�� �û�/�ں�
    int priority; //�������ȼ�
    clock_t start_time;//���̴���ʱ��
    double serve_time;//������Ҫ����ʱ��
    double remain_time;//������Ҫ����ʱ��
    clock_t CPU_time;//����ʹ��CPU����ʱ��
    struct Memory_struct* Mem_info;//�ڴ���̹�����Ϣ
    int disc_offset[TABLE_LEN];//����ƫ����
    int sequence[SEQ_LEN];//��������
}PCB;
void fCreate(vector<int> location, int start, int end, string s, bool read);//�����ļ�ģ����Ϣ��������
void run(FileSystem &fs);//����
void PrintInfo();//��ӡ��Ϣ
void Block(PCB& p);//������������
void Blockdelete(PCB& p);//�����������Ƴ�
void Externaldelete(PCB& p);//�ӹ�������Ƴ�
bool Ready(PCB& p, bool blocked);//�����������
void Stop(PCB& p, list<PCB> lists);//��������
PCB Create(int task_size, int priority);//�������ȼ����ڴ洴������
void Retrieve();//�����Ի��պʹ�����������
void CPUScheduling();//���ڵ��ȣ�ʹ�ö༶�������е����㷨
void LongTermScheduling(string filename);//���ڵ���
int MidTermScheduling(int inOrOut);//���ڵ���
int MidTerm_OUT();//���ڵ��ȣ������������н��̣�
void handle(string s, FileSystem &fs);//�����û�����
#endif //OS_PROCESS_H
#pragma once

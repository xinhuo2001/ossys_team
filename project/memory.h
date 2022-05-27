#ifndef OS_MEMORY_H
#define OS_MEMORY_H
void initMemory();    //ϵͳ����ʱ��ʼ���ڴ�
bool applyForMemory(PCB& p);
int  handle_process(int pid);
void releaseMemory(int pid);
int  replace(struct process_info& p);
void swapToDisc(int offset, int site);
void swapToMemory(int offset, int site);
void isChanged(int pid, int frameNo);
void initDisc();
int getOccupied_size();
void writeMem(int offset, char data[]);
char * readMem(int offset);
void showMemInfo();
const string DISC = "disc.txt";
const int PAGE_SIZE = 1024; //ҳ��С
const int PAGE_NUM = 1024;
const int PHYSICAL_SIZE = 32; //�����ڴ��С����λ��ҳ�� 
const int FRAMES_PER_PROCESS = 3; //ÿ�����̵�פ����
const int PROCESS_NUM = PHYSICAL_SIZE / FRAMES_PER_PROCESS; //�ڴ�����������ڵĽ����������=�ڴ��ܴ�С/ÿ�����̷���������ڴ��С
struct table_unit	//ҳ��Ԫ
{
	int frame_no;
	int valid;

};
struct frame_unit	//֡��Ԫ
{
	int pid;
	int isUsed;
};
struct page_unit
{
	char elem[PAGE_SIZE];
};
struct process_info
{
	int pid;
	int size; //�ܹ�ҳ��Ŀ������棩
	struct table_unit page_table[TABLE_LEN];
	int sequence[SEQ_LEN];				   //����֡�ķ�������	
	int current_pointer;                   //��������ָ��
	int occupied_frames;                   //�ǿ���frame����
	int frames_number[FRAMES_PER_PROCESS]; //���������frame���
	int lru[FRAMES_PER_PROCESS];
	int alive; //�Ƿ����ڴ��� 0 ������ 1 ����
	bool modifiedBit[FRAMES_PER_PROCESS];
	int disc_offset[TABLE_LEN];
	int logic_number[FRAMES_PER_PROCESS];	//��ǰ������פ�����е��߼�ҳ��
	int page_miss;							//ȱҳ����
};
#endif //OS_MEMORY_H
#pragma once

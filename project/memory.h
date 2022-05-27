#ifndef OS_MEMORY_H
#define OS_MEMORY_H
void initMemory();    //系统运行时初始化内存
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
const int PAGE_SIZE = 1024; //页大小
const int PAGE_NUM = 1024;
const int PHYSICAL_SIZE = 32; //物理内存大小（单位：页） 
const int FRAMES_PER_PROCESS = 3; //每个进程的驻留集
const int PROCESS_NUM = PHYSICAL_SIZE / FRAMES_PER_PROCESS; //内存内能允许存在的进程最大数量=内存总大小/每个进程分配的物理内存大小
struct table_unit	//页表单元
{
	int frame_no;
	int valid;

};
struct frame_unit	//帧单元
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
	int size; //总共页数目（内外存）
	struct table_unit page_table[TABLE_LEN];
	int sequence[SEQ_LEN];				   //物理帧的访问序列	
	int current_pointer;                   //访问序列指针
	int occupied_frames;                   //非空闲frame数量
	int frames_number[FRAMES_PER_PROCESS]; //分配的所有frame编号
	int lru[FRAMES_PER_PROCESS];
	int alive; //是否还在内存中 0 不存在 1 存在
	bool modifiedBit[FRAMES_PER_PROCESS];
	int disc_offset[TABLE_LEN];
	int logic_number[FRAMES_PER_PROCESS];	//当前存在于驻留集中的逻辑页号
	int page_miss;							//缺页次数
};
#endif //OS_MEMORY_H
#pragma once

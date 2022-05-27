#include"global.h"
#include"process.h"
#include"memory.h"
using namespace::std;
page_unit memory[1024];
int occupied_size; //内存中被占用帧的数目
process_info process_list[PROCESS_NUM]; //内存模块维护的进程信息表 
frame_unit frame[PHYSICAL_SIZE]; //模拟物理内存的帧结构数组
bool memInfo = false;
/*int main(void)
{
	//initDisc();
	PCB aPcb[10];
	for (int i = 0; i < 10; i++)
	{
		aPcb[i].pid = i;
		aPcb[i].Mem_info = (struct Memory_struct*)malloc(sizeof(struct Memory_struct));
		aPcb[i].Mem_info->task_size = 20;
	}
	initMemory();
	int seq[SEQ_LEN] = {1,2,3,4,2,1,5,6,2,1,2,3,7,6,3,2,1,2,3,6};
	int seq2[SEQ_LEN] = {1,2,3,4,1,2,5,1,2,3,4,5,1,2,3,4,1,2,5,1 };
	PCB p = aPcb[0];
	PCB p2 = aPcb[1];
	for (int i = 0; i < TABLE_LEN; i++) 
	{
		p.disc_offset[i] = i;
		p2.disc_offset[i] = i;
	}
	for (int i = 0; i < SEQ_LEN; i++) {
		p.sequence[i] = seq[i];
	}
	for (int i = 0; i < SEQ_LEN; i++) {
		p2.sequence[i] = seq2[i];
	}
	applyForMemory(p);
	int site;
	for (int i = 0; i < SEQ_LEN; i++) {
		site = handle_process(p.pid);
		if (i == 4) {
			memory[site].elem[0] = 'z';
			memory[site].elem[1] = 'z';
			isChanged(p.pid, site);
		}
	}
	applyForMemory(p2);
	for (int i = 0; i < SEQ_LEN; i++) {
		site = handle_process(p2.pid);
		if (i == 7) {
			memory[site].elem[0] = 't';
			memory[site].elem[1] = 't';
			isChanged(p2.pid, site);
		}
	}
	releaseMemory(p.pid);
	releaseMemory(p2.pid);
/*
	int site;
	//将逻辑3，物理5行调入，更改，说明已更改  到0
	site = handle_process(p.pid);
	memory[0].elem[0] = 'a';
	isChanged(p.pid, site);

	cout << memory[0].elem << endl;
	
	//将逻辑2，物理4调入         到1
	site = handle_process(p.pid);
	cout << memory[1].elem << endl;
	//将逻辑1，物理3调入         到2
	site = handle_process(p.pid);
	cout << memory[2].elem << endl;
	
	cout << memory[0].elem << endl;
	//将逻辑0，物理2调入，        到0
	site = handle_process(p.pid);
	cout << memory[0].elem << endl;
	return 0;
}*/
int getOccupied_size()
{
	return occupied_size;
}
void writeMem(int offset, char data[])
{
	strcpy_s(memory[offset].elem,strlen(data)+1,data);
}
char* readMem(int offset)
{
	ifstream fin(DISC, ios::in);
	fin.seekg(offset, ios::cur);
	fin.getline(memory[offset/1000].elem, PAGE_SIZE);
	return memory[offset/1000].elem;
}
void initDisc()
{
	ofstream out("disc.txt", ios::out);
	char data[PAGE_SIZE + 1] = { '\0' };
	for (int i = 0; i < PAGE_SIZE; i++) {
		data[i] = '0';
	}
	if (out.is_open()) {
		for (int i = 0; i < PAGE_SIZE; i++) {
			out << data << endl;
		}
		out.close();
	}
	else {
		cout << "Error in opening file!" << endl;
		exit(1);
	}
}

void swapToDisc(int offset, int site)
{
	fstream out(DISC, ios::binary | ios::in | ios::out);
	if (out.is_open()) {
		out.seekp(offset, ios::beg);
		out.write(memory[site].elem, PAGE_SIZE);
		out.close();
	}
	else {
		cout << "Error in opening file!" << endl;
		exit(1);
	}
}

void swapToMemory(int offset, int site)
{
	ifstream fin(DISC, ios::in);
	char temp[PAGE_SIZE];
	if (!fin)//文件打开失败
	{
		cout << "Error in opening file!" << endl;
		exit(1);
	}
	//将前面的过掉
	fin.seekg(offset, ios::cur);
	//将目标行写到对应内存中
	fin.getline(memory[site].elem, PAGE_SIZE);

	return;
}

void initMemory() //初始化内存，可以加在系统执行的init中
{
	srand(time(NULL));
	for (int i = 0; i < PROCESS_NUM; ++i)
	{
		process_list[i].pid = -1;
		process_list[i].alive = 0;
		for (int j = 0; j < FRAMES_PER_PROCESS; j++)
		{
			process_list[i].modifiedBit[j] = false;
			process_list[i].logic_number[j] = -1;
		}
		for (int j = 0; j < TABLE_LEN; j++)
		{
			process_list[i].disc_offset[j] = -1;
		}
	}
	for (int i = 0; i < PHYSICAL_SIZE; ++i)
	{
		frame[i].pid = -1;
		frame[i].isUsed = 0;
	}
	occupied_size = 0;
}

int replace(struct process_info &p)
{
	if (memInfo == true) {
		cout << "current lru:";
		for (int i = 0; i < p.occupied_frames; i++) {
			cout << p.lru[i] << " ";
		}
		cout << endl;
		cout << "current frame:";
		for (int i = 0; i < p.occupied_frames; i++) {
			cout << p.logic_number[i] << " ";
		}
		cout << endl;
	}
	if (p.occupied_frames < FRAMES_PER_PROCESS) //有空闲frame直接调入并更新lru数组
	{
		
		int offset = p.disc_offset[p.sequence[p.current_pointer]];
		int frameNo = p.frames_number[p.occupied_frames];
		if (offset < 0) {
			cout << "bad offset!" << endl;
			exit(1);
		}
		swapToMemory(offset, frameNo);
		p.lru[p.occupied_frames] = -1;
		p.page_table[p.sequence[p.current_pointer]].frame_no = p.frames_number[p.occupied_frames];
		p.page_table[p.sequence[p.current_pointer]].valid = 1;

		p.logic_number[p.occupied_frames] = p.sequence[p.current_pointer];
		if (memInfo == true) {
			cout << p.sequence[p.current_pointer] << " --- > memory " << "[" << frameNo << "]" << endl;
		}
		p.occupied_frames++;
		for (int k = 0; k < p.occupied_frames; k++)
		{
			p.lru[k]++;
		}
		return frameNo;
	}
	else //没有空闲frame
	{
		int max = -9999;
		int loc_max;
		for (int k = 0; k < p.occupied_frames; k++) //找到当前lru数组中值最大的元素即最少使用
		{
			if (p.lru[k] > max)
			{
				max = p.lru[k];
				loc_max = k;
			}
		}
		for (int i = 0; i < p.size; i++)
		{
			if (p.page_table[i].frame_no == p.frames_number[loc_max] && p.page_table[i].valid == 1)
			{
				p.page_table[i].valid = -1;		//表示换出内存
			}
		}
		//判断该物理帧是否被修改若被修改则需写回磁盘
		int site = p.frames_number[loc_max];
		if (p.modifiedBit[site] == true)
		{
			//获取外存偏移量offset
			//获取物理物理帧号site
			int index = p.logic_number[loc_max];
			int offset = p.disc_offset[index];
			if (offset < 0) {
				cout << "bad offset!" << endl;
				exit(1);
			}
			swapToDisc(offset, site);
			cout << index << " --- > disk" << "[" << offset << "]     ";
			p.modifiedBit[site] = false;
		}

		p.page_table[p.sequence[p.current_pointer]].frame_no = p.frames_number[loc_max];	//物理帧号
		p.page_table[p.sequence[p.current_pointer]].valid = 1;
		p.logic_number[loc_max] = p.sequence[p.current_pointer];
		p.lru[loc_max] = -1;
		
		int offset = p.disc_offset[p.sequence[p.current_pointer]];
		if (memInfo == true) {
			cout << p.sequence[p.current_pointer] << " --- > memory " << "[" << site << "]" << endl;
		}
		swapToMemory(offset, site);

		for (int k = 0; k < p.occupied_frames; k++)
		{
			p.lru[k]++;
		}
		return site;
	}
}


bool applyForMemory(PCB &p)				   //进程内存接口 传一个pcb过来,返回是否分配内存成功
{										   // pid 进程pid， size 进程所需页数
	if (occupied_size > PHYSICAL_SIZE - FRAMES_PER_PROCESS) //如果没有足够的的帧
		return false;
	//帧足够，分配内存
	for (int i = 0; i < PROCESS_NUM; ++i) //是否达到进程上限
	{
		if (process_list[i].alive != 0)
			continue;
		//初始化内存中进程信息表
		//pid size 信息
		process_info process;
		process.pid = p.pid;
		process.size = p.Mem_info->task_size;  
		process.page_miss = 0;
		//初始化进程页表
		for (int j = 0; j < TABLE_LEN; ++j)
		{
			process.page_table[j].frame_no = -1;
			process.page_table[j].valid = -1;
		}
		if (memInfo == true) {
			cout << "sequence:";
		}
		for (int j = 0; j < SEQ_LEN; j++)
		{
			process.sequence[j] = p.sequence[j];
			if (memInfo == true) {
				cout << process.sequence[j] << " ";
			}
		}

		//cout << "disc: ";
		for (int j = 0; j < TABLE_LEN; j++)
		{
			process.disc_offset[j] = p.disc_offset[j];
			//cout << process.disc_offset[j] << " ";
		}
		cout << endl;
		process.current_pointer = 0;
		// 初始化内存调度 和 生存信息
		process.occupied_frames = 0;
		process.alive = 1;
		for (int j = 0; j < PAGE_NUM; ++j)
		{
			if (frame[j].isUsed == 0)
			{
				for (int k = j, l = 0; k < j + FRAMES_PER_PROCESS; ++k, ++l)
				{
					frame[k].pid = process.pid;
					frame[k].isUsed = 1;
					process.frames_number[l] = k;
					//cout << k << " ";
				}
				break;
			}
		}
		for (int j = 0; j < FRAMES_PER_PROCESS; ++j)
		{
			process.lru[j] = 0;
		}
		process_list[i] = process;
		occupied_size += FRAMES_PER_PROCESS;
		if (memInfo == true) {
			cout << "process_list:" << i << " framenumber:";
			for (int j = 0; j < FRAMES_PER_PROCESS; ++j)
			{
				cout << process_list[i].frames_number[j] << " ";
			}
			cout << endl;
		}
		return true;

	}
	return false; //进程数目达到上限，分配失败

}

int handle_process(int pid) //处理调页过程，每个PC执行一次,判断当前所处理进程的调页
{
	int no=1;
	//找到与进程pid相等的process_info
	for (int i = 0; i < PROCESS_NUM; i++)
		if (process_list[i].alive == 1 && process_list[i].pid == pid)
		{
			no = i;
			break;
		}

	struct process_info &p = process_list[no];
	if (memInfo == true) {
		cout << p.current_pointer << endl;
		cout << "pid:";
		cout << p.pid << endl;
		cout << "p.sequenceNO:" << p.sequence[p.current_pointer]<< endl;
		cout << "page_table_frameNO:" << p.page_table[p.sequence[p.current_pointer]].frame_no << endl;
		cout << "page_table_valid:" << p.page_table[p.sequence[p.current_pointer]].valid << endl;
	}
	//bool pageFault = false;
	int frameNo;
	if (p.page_table[p.sequence[p.current_pointer]].valid == -1) //如果进程所需页不在frame中，调页
	{
		//pageFault = true;
		frameNo = replace(p);
		if (memInfo == true) {
			cout << "frameNo" << frameNo << endl;
		}
		p.page_miss++;

	}
	else
	{
		frameNo = p.page_table[p.sequence[p.current_pointer]].frame_no;
		for (int i = 0; i < FRAMES_PER_PROCESS; ++i)
		{
			if (frameNo == p.frames_number[i])
			{
				p.lru[i] = 0;
			}
			else {
				p.lru[i]++;
			}
		}
	}
	if (memInfo == true) {
		cout << "\n--------------------------------------------------------------- " << endl;
	}
	p.current_pointer++;
	if (p.current_pointer == 20) {
		p.current_pointer = 0;
	}
	return frameNo;
}

void isChanged(int pid, int frameNo)
{
	int no;
	//找到与进程pid相等的process_info
	for (int i = 0; i < PROCESS_NUM; i++)
		if (process_list[i].alive == 1 && process_list[i].pid == pid)
		{
			no = i;
			break;
		}

	struct process_info &p = process_list[no];
	for(int i = 0; i < FRAMES_PER_PROCESS; i++)
		if (p.frames_number[i] == frameNo)
		{
			p.modifiedBit[frameNo] = true;
		}
	return;
}


void releaseMemory(int pid) // 进程内存接口传一个pcb过来，将该进程在内存中释放
{
	if (memInfo == true) {
		cout << "release success:pid = " << pid << endl;
	}
	for (int i = 0; i < PROCESS_NUM; ++i)
	{
		if (process_list[i].pid == pid)
		{
			process_list[i].alive = 0;
			if (memInfo == true) {
				cout << "[i]:" << i << endl;
			}
			for (int j = 0; j < FRAMES_PER_PROCESS; ++j)
			{
				if (memInfo == true) {
					cout << "process_list[i].frames_number[j]:" << process_list[i].frames_number[j] << endl;
				}
				frame[process_list[i].frames_number[j]].isUsed = 0;
			}
			occupied_size -= FRAMES_PER_PROCESS;
			if (memInfo == true) {
				cout << "PID:" << pid << " ----------- page miss rate: %" << (float)process_list[i].page_miss / (float)SEQ_LEN * 100.0 << endl;
			}

		}
	}
}
void showMemInfo()
{
	memInfo = !memInfo;
}
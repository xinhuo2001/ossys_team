//https://blog.csdn.net/GreyBtfly/article/details/84646981

#include<bits/stdc++.h>
using namespace std;
/*进程分配内存块链表的首指针*/
struct allocated_block *allocated_block_head = NULL;
#define PROCESS_NAME_LEN 32   /*进程名长度*/
#define MIN_SLICE    10             /*最小碎片的大小*/
#define DEFAULT_MEM_SIZE 1024     /*内存大小*/
#define DEFAULT_MEM_START 0       /*起始位置*/
/* 内存分配算法 */
#define MA_FF 1	//first fit
#define MA_BF 2
#define MA_WF 3
#define Buddy 4 //伙伴算法 
/*描述每一个空闲块的数据结构*/
struct free_block_type{
    int size;
    int start_addr;
    struct free_block_type *next;
};  
typedef struct free_block_type FB;
/*指向内存中空闲块链表的首指针*/
struct free_block_type *free_block;//此处尽量按内存地址顺序排列 ，
/*每个进程分配到的内存块的描述*/
struct allocated_block{
    int pid;    int size;
    int start_addr;
    char process_name[PROCESS_NAME_LEN];
    struct allocated_block *next;
};
typedef struct allocated_block allocated_block_type;
//buddy
typedef struct b_free_block_type
{
	int size;
	free_block_type *list;
	b_free_block_type *next;
}b_free_block_type;

b_free_block_type *b_free_block=NULL;//空的时候要设置为NULL 
//end of buddy 
 
typedef struct allocated_block AB;
 
int mem_size=DEFAULT_MEM_SIZE; /*内存大小*/
int ma_algorithm = Buddy;           /*当前分配算法*/ //------------------------>>>>>>>>> 
static int pid = 0;                                      /*初始pid*/
int flag = 0;                             /*设置内存大小标志*/
 
 
//init_free_block(int mem_size);
int display_mem_usage();
int b_creat_free_blocks(free_block_type *ab);
int rearrange_Buddy();
int rearrange(int algorithm);
int allocate_mem(struct allocated_block *ab); 
int free_mem(struct allocated_block *ab);
int dispose(struct allocated_block *free_ab);
int disfree(FB *free_ab);
void free_f();
void free_b();
/*初始化空闲块，默认为一块，可以指定大小及起始地址*/
struct free_block_type* init_free_block(int mem_size)
{
	free_f();
	free_b();
    struct free_block_type *fb;
    fb=(struct free_block_type *)malloc(sizeof(struct free_block_type));
    if(fb==NULL){
        printf("Error.\n");
        getchar();
        return NULL;
	}
    fb->size = mem_size;
    fb->start_addr = DEFAULT_MEM_START;
    fb->next = NULL;
    
    free_block=(struct free_block_type *)malloc(sizeof(struct free_block_type));
    *free_block=*fb;	//free_block供rearrange_Buddy使用，会被销毁 
    rearrange_Buddy();//初始化buddy算法 
    return fb;	//会在main中重新赋值free_block 
}
/*显示菜单*/
void display_menu(){
    printf("\n");
    printf("1 - Set memory size (default=%d)\n", DEFAULT_MEM_SIZE);
    printf("2 - Select memory allocation algorithm\n");
    printf("3 - New process \n");
    printf("4 - Terminate a process \n");
    printf("5 - Display memory usage \n");
    printf("0 - Exit\n");
}
/*设置内存的大小*/
int set_mem_size(){	//只能设置一次, 清除现有所有链表，重新分配 
    int size;
    if(flag!=0){  //防止重复设置
        printf("Cannot set memory size again\n");
        return 0;
    }
    printf("Total memory size =");
    scanf("%d", &size);
    if(size>0) {
        mem_size = size;
       // free_block->size = mem_size;
        init_free_block(mem_size);
	    flag=1;   
		return 1;
    }
    cout<<"输入不合法"<<endl;
    return 0;
    
}
 
/* 设置当前的分配算法 */
int set_algorithm(){
    int algorithm;
    printf("\t1 - First Fit\n");
    printf("\t2 - Best Fit \n");
    printf("\t3 - Worst Fit \n");
    printf("\t4 - Buddy \n"); 
    scanf("%d", &algorithm);
	//按指定算法重新排列空闲区链表
	if(algorithm==Buddy) 
   	{
   		if(ma_algorithm!=Buddy)
   			rearrange(algorithm); 	
	}
	else
	{
		if(ma_algorithm==Buddy)
			rearrange(algorithm); 	
	}
    if(algorithm>=1 && algorithm <=4)  
    {
    	ma_algorithm=algorithm;
        pid=0; 
	}
    else
    	cout<<"输入错误!!!"<<endl;
    display_mem_usage();
}
/*按FF算法重新整理内存空闲块链表*/
int rearrange_FF(){   
	//请自行补充
	//将buddy的二级制表示法展开，按地址排序 
	b_free_block_type *p=b_free_block;
	free_block_type *work,*twork;
	allocated_block_type *t=(allocated_block_type*)malloc(sizeof(allocated_block_type));	//最后要销毁此临时块 
	//不排除其他函数调用此函数来完成内存列表的切换，需要暂时改变算法，结束之前再进行还原。
	int ma_algorithm_temp=ma_algorithm;//备份 
	ma_algorithm=MA_FF; //保证free_mem()的按照FF的方式工作 
	free_f();
	for(p=b_free_block;p!=NULL;p=p->next)
	{
		for(work=p->list;work!=NULL;work=work->next)//在f系列列表里增加内存而不销毁b算法列表 
		{
			t->size=work->size;
			t->start_addr=work->start_addr;
			free_mem(t);	//不会删除work
		}
	}
	//还原算法
	ma_algorithm=ma_algorithm_temp; 
	//不销毁buddy.free内存列表 
	free(t);
}
/*按BF算法重新整理内存空闲块链表*/
int rearrange_BF(){
	//请自行补充
	/*
	如果按地址排，则分配时复杂，free时方便
	如果按内存大小排，则分配时方便，free复杂
	综合考虑，按地址排序还可以重用代码，本程序选择按地址排序 ——wbt 
	*/
	return rearrange_FF();
}
/*按WF算法重新整理内存空闲块链表*/
int rearrange_WF(){
    //请自行补充
    return rearrange_FF();
}
 
int rearrange_Buddy()
{
	//尽量将ff，bf，wf的内存表示方式转换（不足2的幂的补足，若无法补足返回0表示无法切换）
	//算了，太麻烦了还是按照只能开机设置设定吧。
 
	int i;
	free_b();
	b_free_block_type *p,*tp;//工作指针 
	p=b_free_block;
	while(p!=NULL) //释放掉之前的 
	{
		tp=p->next;
		free(p);
		p=tp;
	}
	
	//将最大内存mem_size建立头链表
	int size=1;
	while(size<mem_size)
		size<<=1;
	b_free_block=NULL;//头插法，按从小到大排序 
	while(size)
	{
		p=(b_free_block_type*)malloc(sizeof(b_free_block_type));
		p->next=b_free_block;
		b_free_block=p;
		p->size=size;
		p->list=NULL;
		size>>=1;
	}
	//将free_block链表变换成b_free_block链表
	free_block_type *work,*twork;
	while(free_block!=NULL)	//边转换边删除 
	{
		work=free_block;
		free_block=free_block->next; 
		b_creat_free_blocks(work);	//新建一个 并插入 销毁原来的free_block 
	}
	return 1;
}
//将free_block_type ab展开成buddy形式 销毁原来的free_block 并进行合并检查和操作 
int b_creat_free_blocks(free_block_type *ab)	//wbt class 3 grade 2016 
{
	b_free_block_type *p=b_free_block;
	free_block_type *work,*fwork,*twork;
	int i=0;
	while(p!=NULL && ab->size)
	{
		if(ab->size>>i&1)
		{
			if(p->list==NULL)//如果还没有节点，则添加一个新节点 
			{
				work=p->list=(free_block_type*)malloc(sizeof(free_block_type));
				work->size=p->size;
				ab->size-=work->size;
				work->start_addr=ab->start_addr;
				ab->start_addr+=work->size;
				
				work->next=NULL;
				continue;
			}
			//从列表里选取合适的位置插入并上寻找伙伴合并（保证列表按地址排序） 
			//两个块由一个块分裂而来的伙伴条件：判断地址是不是2的幂次就可以
			fwork=NULL;
			for(work=p->list;/*work!=NULL,此处要考虑挂在尾巴上*/;work=work->next)
			{
				
				if(work==NULL || work->start_addr > ab->start_addr)//把一部分(p->size)插在work和fwork之间 
				{	//找到了位置，准备插入节点
					
					twork=(free_block_type*)malloc(sizeof(free_block_type));//新增的块不用free 
					twork->size = p->size;
					twork->start_addr=ab->start_addr;
					ab->start_addr+=p->size; //留下低地址的部分,带着高地址部分继续寻找插入点 
					ab->size-=p->size;
					if(work==NULL)//挂在尾巴上  twork插在fwork和NULL(wrok)之间 
					{//特点是work=NULL，fwork!=NULL
						fwork->next=twork;
						twork->next=NULL;
						//看看能不能和前面的合并 
						work=twork;//合并统一性要求 （因为work==NULL，所以将twork替换work，即相当于对next之后的不会改动） 
 
					}
					else if(work==p->list)//第一个（没有前驱） 
					{//特点是fwork==NULL ,work==p->list不等于NULL 
						twork->next=p->list;
						p->list=twork;//头插法直接插在头部 
						//只检查后面的能不能合并
					}
					else//有前驱和后继  插在fwork之后，work之前 
					{//特点是fwork和work都不为NULL 
						twork->next=work;
						fwork->next=twork;
						//和前面合并 
					}
					//两个块由一个块分裂而来的伙伴条件：判断地址是不是2的幂次就可以
 
					//和前面合并 (如果有前驱的话)  合并fwork 和 twork 其中twork->next= work  fwork的地址必须是size*2的倍数 
					if(fwork!=NULL && fwork->size + fwork->start_addr==twork->start_addr && fwork->start_addr%(p->size<<1)==0)
					{
						fwork->next=twork->next;
						fwork->size+=twork->size;
						//删除fwork,因为合并一定能向上冒泡
						if(p->list==fwork)//如果fwork为第一个 
						{
							p->list=fwork->next;//删除第一个 
						}
						else
						{
							//twork初值为第一个 
							for(work=p->list;work->next!=fwork;work=twork->next)
								;
							//此时twork为fwork的前驱
							work->next=fwork->next;
						}
						b_creat_free_blocks(fwork);//递归调用 
						free(twork); 
					}
					//检查后面的能不能合并(如果有后继的话)
					if(twork->next!=NULL && twork->start_addr+twork->size == twork->next->start_addr && twork->start_addr%(p->size<<1)==0)						
					{
						work=twork->next;
						twork->next=work->next;
						twork->size+=work->size;
						//twork为新块  现在要删掉twork向上冒泡 
						if(twork==p->list)//如果twork是第一个，即没有前驱
						{
							p->list=twork->next;
						}
						else
						{
							fwork->next=twork->next;
						}
						b_creat_free_blocks(twork);
						free(work);	//和后面的二合一  
					}
				//	free(twork);
					break;//break for
				}
				else//设置前驱，继续寻找 
				{
					fwork=work;
				}
			}//end of for 
		}//end of if
		p=p->next;
		i++;
	}//end of while
	free(ab);
	ab=NULL;
	return 1;
}
 
/*创建新的进程，主要是获取内存的申请数量*/
int new_process(){
    struct allocated_block *ab;
    int size;    int ret;
    ab=(struct allocated_block *)malloc(sizeof(struct allocated_block));
    if(!ab) exit(-5);
    ab->next = NULL;
    pid++;
    sprintf(ab->process_name, "PROCESS-%02d", pid);
    ab->pid = pid;    
    printf("Memory for %s:", ab->process_name);
    scanf("%d", &size);
    if(size>0) ab->size=size;
    ret = allocate_mem(ab);  /* 从空闲区分配内存，ret==1表示分配ok*/
 
 /*如果此时allocated_block_head尚未赋值，则赋值*/
    if((ret==1) &&(allocated_block_head == NULL)){ 
        allocated_block_head=ab;
        return 1;        }
    /*分配成功，将该已分配块的描述插入已分配链表*/
    else if (ret==1) {
        ab->next=allocated_block_head;//头插法 
        allocated_block_head=ab;
        return 2;        }
    else if(ret==-1){ /*分配不成功*/
        printf("Allocation fail\n");
        free(ab);
        return -1;       
     }
    return 3;
    }
/*分配内存模块*/
int allocate_mem(struct allocated_block *ab){	//ff bf
    struct free_block_type *fbt, *pre;
    int request_size=ab->size;
    fbt = pre = free_block;
    //根据当前算法在空闲分区链表中搜索合适空闲分区进行分配，分配时注意以下情况：
    // 1. 找到可满足空闲分区且分配后剩余空间足够大，则分割
    // 2. 找到可满足空闲分区且但分配后剩余空间比较小，则一起分配
    // 3. 找不可满足需要的空闲分区但空闲分区之和能满足需要，则采用内存紧缩技术，进行空闲分区的合并，然后再分配
    // 4. 在成功分配内存后，应保持空闲分区按照相应算法有序
    // 5. 分配成功则返回1，否则返回-1
	// 请自行补充。。。。。
	if(ma_algorithm==MA_FF)	//FF	从低地址往高地址分配 
	{
		for(FB* p=free_block;p!=NULL;p=p->next)
		{
			if(p->size >= request_size)//遇到即分配  FF
			{
				ab->start_addr=p->start_addr;
				p->start_addr+=request_size;
				p->size-=request_size;
				if(p->size<MIN_SLICE)
				{
					ab->size+=p->size;
					//删除free block 
					disfree(p); 
				}
				/*		从高往低 
				ab->start_addr=p->start_addr;
				p->size-=request_size;
				*/
				return 1;
			}
		}
		printf("FF装载失败\n");
		return 0;
	}
	else if(ma_algorithm==MA_BF)//free block按地址排 
	{
		FB *p=free_block;
		FB *mini;
		int minv;
		int fg=0; 
		for(;p!=NULL;p=p->next)//遍历找到差距最小能装载的free block    从低地址开始装载 
		{
			if(p->size>=request_size)
			{
				if(fg==0)
				{
					minv=p->size - request_size;
					mini=p; 
					fg=1;
				}
				else
				{
					if(p->size-request_size<minv)
					{
						minv=p->size-request_size;
						mini=p;
					}
				}
			}
			
		}
		if(fg)
		{
			//ab 为要分配的
			ab->start_addr=mini->start_addr;
			mini->start_addr+=request_size;
			mini->size-=request_size;
			if(mini->size<MIN_SLICE)
			{
				ab->size+=mini->size;
				//删除free block 
				disfree(mini); 
			}
			return 1;
		}
		printf("BF装载失败\n");
		return 0;
	}
	else if(ma_algorithm==MA_WF) //free block按地址排 
	{
		FB *p=free_block;
		FB *maxi;
		int maxv;
		int fg=0; 
		for(;p!=NULL;p=p->next)//遍历找到差距最小能装载的free block    从低地址开始装载 
		{
			if(p->size>=request_size)
			{
				if(fg==0)
				{
					maxv=p->size - request_size;
					maxi=p; 
					fg=1;
				}
				else
				{
					if(p->size-request_size>maxv)
					{
						maxv=p->size-request_size;
						maxi=p;
					}
				}
			}
		}
		if(fg)
		{
			//ab 为要分配的
			ab->start_addr=maxi->start_addr;
			maxi->start_addr+=request_size;
			maxi->size-=request_size;
			if(maxi->size<MIN_SLICE)
			{
				ab->size+=maxi->size;
				//删除free block 
				disfree(maxi);
			}
			return 1;
		}
		printf("WF装载失败\n");
		return 0;
	}
	else if(ma_algorithm==Buddy)
	{
		if((ab->size)&(ab->size-1))
		{
			long long i=1;
			//不是2的幂次  将ab->size向上寻找一个2的幂次 
			for(;i<ab->size;i<<=1)
				;
			ab->size=i;
		}
		//从b_free_block中从小到大找到一个能装下的2的次方
		b_free_block_type *p=b_free_block;
		while(p!=NULL)
		{
			if(p->size>=ab->size && p->list!=NULL)
			{
				break;
			}
			p=p->next;
		}
		
		if(p!=NULL)//找到了可用块
		{
			//list按照地址排序的，现在按小地址优先的原则分配
			free_block_type *work=p->list;
			work->size-=ab->size;
			ab->start_addr=work->start_addr;
			work->start_addr+=ab->size;
		
			p->list=work->next;
			b_creat_free_blocks(work);	//将剩余的分割挂在链表合适位置 
			return 1;
		}
		else
		{
			printf("Buddy装载失败\n"); 
			return 0;
		}
	}//end of buddy
//	return 1;
}
 
struct allocated_block* find_process(int pid)
{
	int i;
	struct allocated_block *p=allocated_block_head;
	for(;p!=NULL;p=p->next)
	{
		if(p->pid==pid)
			return p;
	}
	return NULL;
}
/*删除进程，归还分配的存储空间，并删除描述该进程内存分配的节点*/
int kill_process(){
    struct allocated_block *ab;
    int pid;
    printf("Kill Process, pid=");
    scanf("%d", &pid);
    ab=find_process(pid);
    if(ab!=NULL){
        free_mem(ab); /*释放ab所表示的分配区*/
        dispose(ab);  /*释放ab数据结构节点*/
    }
    else
    {
    	cout<<"不存在"<<endl; 
	}
}
/*将ab所表示的已分配区归还，并进行可能的合并*/
int free_mem(struct allocated_block *ab){	//不会删除ab 
    struct free_block_type *fbt=NULL, *pre, *work;
  //  fbt=(struct free_block_type*) malloc(sizeof(struct free_block_type));
  //  if(!fbt) return -1;
    // 进行可能的合并，基本策略如下
    // 1. 将新释放的结点插入到空闲分区队列末尾
    // 2. 对空闲链表按照地址有序排列
    // 3. 检查并合并相邻的空闲分区
    // 4. 将空闲链表重新按照当前算法排序   /->  本程序不做次操作 
    //请自行补充……
    
    if(ma_algorithm==MA_FF || ma_algorithm== MA_BF || ma_algorithm== MA_WF)//free链表都是按地址排序,所以可以用同一种方式free 
    {
    	FB *fp=NULL,*p=NULL;
    	for(p=free_block;p!=NULL && p->start_addr < ab->start_addr;p=p->next)//寻找block属于哪一个free block 
    	{
    		fp=p;
    			//前一个小于，后一个大于 
		}
		//fp为前一个free_block  ab为当前要free的block  p为下一个 
		if(fp!=NULL && fp->start_addr+fp->size==ab->start_addr)
		{	//当前面还有东西，就往前合并。。。，//???但如果，已经是最低地址的块了 就不往前找了 我好像说了个废话 ???
			fp->size+=ab->size;
			//fp为前面的块，p为后面的块 
			printf("和前一个合并\n");//不新增free_blocks
			if(p!=NULL && p->start_addr == fp->start_addr + fp->size)//用前面的free block尝试往后合并 
			{
			//	fp->start_addr = p->start_addr;
				fp->size += p->size;
				fp->next=p->next; 
				free(p);//前后合并，删除后面的free block 
				printf("和后一个合并"); 
				return 1;
			}
			return 1;
		}
		else if(p!=NULL && p->start_addr == ab->start_addr + ab->size)//尝试往后合并	p为后面的 
		{
			p->start_addr = ab->start_addr;
			p->size += ab->size;
			printf("和后一个合并");
			return 1;
		}
		//无法合并，直接释放，free链表插入节点 fp为前一个，p为后一个 fbt为新的free block 
	//	display_mem_usage();
		fbt=(struct free_block_type*) malloc(sizeof(struct free_block_type));
		fbt->size=ab->size;
		fbt->start_addr=ab->start_addr;
		fbt->next=p;
		if(fp==NULL)
		{
			fbt->next=free_block;//头插法 
			free_block=fbt;
		}
		else
		{
			fp->next=fbt;
		}
		printf("没法合并,直接释放");
	}
	else if(ma_algorithm==Buddy)
	{
		fbt=(struct free_block_type*) malloc(sizeof(struct free_block_type));
  		if(!fbt) return -1;
  		fbt->size=ab->size;
  		fbt->start_addr=ab->start_addr;
  		return b_creat_free_blocks(fbt);//会销毁fbt 
	}
    return 1;
}
/*释放ab数据结构节点*/
int dispose(struct allocated_block *free_ab){
    struct allocated_block *pre, *ab;
   if(free_ab == allocated_block_head) { /*如果要释放第一个节点*/
     allocated_block_head = allocated_block_head->next;
        free(free_ab);
        return 1;
        }
    pre = allocated_block_head;  
    ab = allocated_block_head->next;
    while(ab!=free_ab){ pre = ab;  ab = ab->next; }
    pre->next = ab->next;
    free(ab);
    return 2;
   }
int disfree(FB *free_ab){
    FB *pre, *ab;
   if(free_ab == free_block) { /*如果要释放第一个节点*/
     free_block = free_block->next;
        free(free_ab);
        return 1;
        }
    pre = free_block;  
    ab = free_block->next;
    while(ab!=free_ab){ pre = ab;  ab = ab->next; }
    pre->next = ab->next;
    free(ab);
    return 2;
   }
/* 显示当前内存的使用情况，包括空闲区的情况和已经分配的情况 */
int display_mem_usage(){
	if(ma_algorithm==Buddy)
	{
		cout<<"free blocks:"<<endl;
		b_free_block_type *p=b_free_block;
		while(p!=NULL)
		{
			printf("list:%d-----------<\n",p->size);
			free_block_type *work=p->list;
			while(work!=NULL)
			{
				printf("·start_adress:%-3d  size:%-3d\n",work->start_addr,work->size);
				work=work->next;
			}
			p=p->next;
			
		}
		 struct free_block_type *fbt=free_block;
		    struct allocated_block *ab=allocated_block_head;
			printf("\nUsed Memory:\n");
		    printf("%10s %20s %10s %10s\n", "PID", "ProcessName", "start_addr", " size");
		    while(ab!=NULL){
	        printf("%10d %20s %10d %10d\n", ab->pid, ab->process_name, ab->start_addr, ab->size);
	        ab=ab->next;
	        }
	}
    else 
    {
    	struct free_block_type *fbt=free_block;
	    struct allocated_block *ab=allocated_block_head;
	    if(fbt==NULL) return(-1);
	    printf("\tw.%s.t.\n","b"); 
	    printf("----------------------------------------------------------\n");
	
	    /* 显示空闲区 */
	    printf("Free Memory:\n");
	    printf("%20s %20s\n", "      start_addr", "       size");
	    while(fbt!=NULL){
	        printf("%20d %20d\n", fbt->start_addr, fbt->size);
	        fbt=fbt->next;
	        }    
	/* 显示已分配区 */
	    printf("\nUsed Memory:\n");
	    printf("%10s %20s %10s %10s\n", "PID", "ProcessName", "start_addr", " size");
	    while(ab!=NULL){
	        printf("%10d %20s %10d %10d\n", ab->pid, ab->process_name, ab->start_addr, ab->size);
	        ab=ab->next;
	        }
	    printf("----------------------------------------------------------\n");
	    return 0;
	}
}
 
	/*按指定的算法整理内存空闲块链表*/
int rearrange(int algorithm){
    switch(algorithm){
        case MA_FF:  rearrange_FF(); break;
        case MA_BF:  rearrange_BF(); break;
        case MA_WF: rearrange_WF(); break;
        case Buddy:rearrange_Buddy();break;
		}
}
void free_f()
{
	struct allocated_block *f,*p;
	f=allocated_block_head;
	for(;f!=NULL;f=p)
	{
		p=f->next;
		free(f);
	}
	allocated_block_head=NULL;
	struct free_block_type *fn,*fp;
	fp=free_block;
	for(;fp!=NULL;fp=fn)
	{
		fn=fp->next;
		free(fp);
	}
	free_block=NULL;
}
void free_b()
{
	b_free_block_type *p,*tp;
	free_block_type *work,*t;
	p=b_free_block;
	while(p!=NULL)
	{
		tp=p;
		p=p->next;
		work=tp->list;
		while(work!=NULL)
		{
			t=work;
			work=work->next;
			free(t);
		}
		free(tp);
	}
	b_free_block=NULL;
}
void do_exit()
{
	free_f();
	free_b();
}
 
main(){
    char choice;      pid=0;
    free_block = init_free_block(mem_size); //初始化空闲区
    while(1) {
	    display_menu();	//显示菜单
	    cout<<"当前算法编号:"<<ma_algorithm<<endl;
	    fflush(stdin);
	    choice=getchar();	//获取用户输入
	    switch(choice)
		{
	        case '1': set_mem_size(); break; 	//设置内存大小   //开机只能设置一次，且只能在分配之前设置 
	        case '2': set_algorithm();flag=1; break;//设置算法
	        case '3': new_process(); flag=1; break;//创建新进程
	        case '4': kill_process(); flag=1;   break;//删除进程
	        case '5': display_mem_usage();    flag=1; break;	//显示内存使用
	        case '0': do_exit(); exit(0);	//释放链表并退出
	        default: break;      
		}
	} 
}
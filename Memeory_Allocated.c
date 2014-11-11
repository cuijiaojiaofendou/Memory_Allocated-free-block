// =====================================================================================
// 
//       Filename:  Memeory_Allocated.c
//
//    Description:  简单的内存分配实现
//
//        Version:  1.0
//        Created:  2014年11月11日 10时15分08秒
//       Revision:  none
//       Compiler:  gcc
//
//         Author:  Striver (CuiJiaojiao), cuijiaojiaofendou@gmail.com
//        Company:  Class 1204 of Software Engineering
// 
// =====================================================================================

#include <stdio.h>
#include <stdlib.h>

#define PROCESS_NAME_LEN 32   /*进程名长度*/
#define MIN_SLICE    10             /*最小碎片的大小*/
#define DEFAULT_MEM_SIZE 1024     /*内存大小*/
#define DEFAULT_MEM_START 0       /*起始位置*/

/* 内存分配算法 */
#define MA_FF 1
#define MA_BF 2
#define MA_WF 3

///所有的函数
struct free_block_type *init_free_block(int mem_size);
void daiplay_menu();
int set_mem_size();
void set_algorithm();
void rearrange(int algorithm);
void rearrange_FF();
void rearrange_BF();
void rearrange_WF();
int new_process();
//int allocate_mem(struct allocated_block *ab);
void kill_process();
//int free_mem(struct allocated_block *ab);
//int dispose(struct allocated_block *free_ab);
void daiplay_mem_usage();

int mem_size=DEFAULT_MEM_SIZE; /*内存大小*/
int ma_algorithm = MA_FF;           /*当前分配算法*/
static int pid = 0;                                      /*初始pid*/
int flag = 0;                             /*设置内存大小标志*/

/*描述每一个空闲块的数据结构*/
struct free_block_type{
    int size;
    int start_addr;
    struct free_block_type *next;
};  

/*指向内存中空闲块链表的首指针*/
struct free_block_type *free_block;

/*每个进程分配到的内存块的描述*/
struct allocated_block{
    int pid;    
    int size;
    int start_addr;
    char process_name[PROCESS_NAME_LEN];
    struct allocated_block *next;
    };

/*进程分配内存块链表的首指针*/
struct allocated_block *allocated_block_head = NULL;

/*初始化空闲块，默认一块，可以指定大小及初始地址*/
struct free_block_type* init_free_block(int mem_size){
	struct free_block_type *fb;

	fb = (struct free_block_type *)malloc(sizeof(struct free_block_type));
	if (fb == NULL){
		printf("No mem\n");
		return NULL;
	}

	fb->size = mem_size;
	fb->start_addr = DEFAULT_MEM_START;
	fb->next = NULL;
	return fb;
}

/*显示菜单信息*/
void display_menu(){
	printf("\n");
	printf("1.Set memory size(default = %d)\n", DEFAULT_MEM_SIZE);
	printf("2.Select memory allocation algorithm\n");
	printf("3.New process\n");
	printf("4.Terminate a process\n");
	printf("5.Display memory usage\n");
	printf("0.exit\n");
}

/*设置内存的大小*/
int set_mem_size(){
	int size;
	if (flag != 0){   ///防止重复设置
		printf("Can't set memory size again\n");
		return 0;
	}

	printf("Total memory size = ");
	scanf("%d", &size);

	if (size > 0){
		mem_size = size;
		free_block->size = mem_size;
	}
	flag = 1;
	return 1;
}

/*设置当前的分配算法*/
void set_algorithm(){
	int algorithm;
	printf("\t1.First Fit\n");
	printf("\t2.Best Fit\n");
	printf("\t3.Wordt Fit\n");

	scanf("%d", &algorithm);

	if (algorithm >= 1 && algorithm <= 3){
		ma_algorithm = algorithm;	
	}

	rearrange(ma_algorithm);
}

void rearrange(int algorithm)
{
	switch(algorithm){
		case MA_FF:rearrange_FF();
		break;
		case MA_BF:rearrange_BF();
		break;
		case MA_WF:rearrange_WF();
		break;
	}
}

/*实现两个数的交换*/
void my_swap(int *a, int *b)
{
	int temp;

	temp = *a;
	*a = *b;
	*b = temp; 
}
/*按FF算法重新整理内存空闲块链表,按照地址递增的方式处理*/
void rearrange_FF()
{
	struct free_block_type *head, *rear;
	
	printf("Rearrange ---------- FF\n");

	head = free_block;

	while (head->next != NULL){
		rear = head->next;
		while (rear != NULL){
			///按地址递增进行排序
			if (head->start_addr > rear->start_addr){
				my_swap(&head->start_addr, &rear->start_addr);
				my_swap(&head->size, &rear->size);
			}

			rear = rear->next;
		}

		head = head->next;
	}
}

/*按BF算法重新整理内存空闲块链表, 按容量递增有序*/
void rearrange_BF()
{
	struct free_block_type *head, *rear;

	printf("Rearrange ------------ BF\n");
	head = free_block;

	while (head->next != NULL){
		rear = head->next;
		while (rear != NULL){
			if (head->size > rear->size){
				my_swap(&head->start_addr, &rear->start_addr);
				my_swap(&head->size, &rear->size);
			}

			rear = rear->next;
		}

		head = head->next;
	}
}

/*按WF算法重新整理内存空闲块链表, 按容量递减有序*/
void rearrange_WF()
{
	struct free_block_type *head, *rear;

	printf("Rearrange ------------ WF\n");
	
	head = free_block;

	while (head->next != NULL){
		rear = head->next;
		while (rear != NULL){
			if (head->size < rear->size){
				my_swap(&head->start_addr, &rear->start_addr);
				my_swap(&head->size, &rear->size);
			}
			rear = rear->next;
		}

		head = head->next;
	}
}

/*创建新的进程，主要是获取内存的申请数量*/
int new_process()
{
	struct allocated_block *ab;
	int size;
	int ret;

	ab = (struct allocated_block*)malloc(sizeof(struct allocated_block));

	if (!ab){
		exit(-5);
	}

	ab->next = NULL;
	pid ++;

	//进程名的赋值函数，讲PROCESS-N0.pid的进程名写入process_name字符串中
	sprintf(ab->process_name, "PROCESS-%02d", pid);

	ab->pid = pid;

	printf("Memory for %s:", ab->process_name);
	scanf("%d", &size);
	if (size > 0){
		ab->size = size;
	}

	ret = allocate_mem(ab);
	
	printf("allalala    ret = %d\n", ret);
	///如果此时allocated_block_head尚未赋值
	if (ret == 1 && (allocated_block_head == NULL)){
		allocated_block_head = ab;
		return 1;
	} else if (ret == 1){
		///分配成功，将该分配块的描述插入已分配链表
		ab->next = allocated_block_head;
		allocated_block_head = ab;
		return 2;
	} else if (ret == -1){
		///分配不成功
		printf("Allocation fail.\n");
		free(ab);
		return -1;
	}

	return 3;
}

//合并空闲分区
void compress_free_block()
{
	struct free_block_type *head, *rear;

	head = free_block;

	while (head->next != NULL){
		rear = head->next;
		while (rear != NULL){
			if (head->start_addr + head->size == rear->start_addr){ 
				//空闲分区地址间的连续
				head->size += rear->size;
				head->next = rear->next;

				free(rear);

				continue;
			}
			rear = rear->next;
		}

		head = head->next;
	}

}
/*分配内存模块*/
int allocate_mem(struct allocated_block *ab)
{
	struct free_block_type *fbt, *pre;
	int request_size = ab->size;

	fbt = pre = free_block;

	///需要补充
	//按当前分配算法进行内存组织与分配
	rearrange(ma_algorithm);

	while (fbt != NULL){
		if (fbt->size >= request_size){
			//存在一个空闲分区，分配后还剩余空间足够大
			if (fbt->size - request_size >= MIN_SLICE){
				//从总的空闲块中减掉
				mem_size -= request_size;
				//从free_block中减掉
				fbt->size -= request_size;
				//按地址递增的方式分配内存
				ab->start_addr = fbt->start_addr;
				//指针向后移动request_size个长度大小
				fbt->start_addr += request_size;
				
				return 1;
			} else if ((fbt->size - request_size < MIN_SLICE) 
						&& (fbt->size > request_size)){
				///存在一个空闲分区，但是分配后的剩余空间小于MIN_SLICE，则一起分配
				//将剩余的一小部分也分给当前进程所申请的空间
				mem_size -= fbt->size;
				//指针指向下一个练习的空间
				pre = fbt->next;
				ab->start_addr = fbt->start_addr;
				fbt->start_addr += fbt->size;
				free(fbt);

				return 1;
			} else {
				///没有找到合适大小的空闲分区,则进行紧缩
				compress_free_block();

				fbt = free_block;
				break;
			}

			//按照当前分配算法组织
			rearrange(ma_algorithm);
			return 1;
		}
			pre = fbt;
			fbt = fbt->next;
	}
	///没有申请到，则返回-1
	return -1;
}

//查找进程
struct allocated_block *find_process(int pid)
{
	struct allocated_block *head;
	
	head = allocated_block_head;
	while (head != NULL){
		if (head->pid == pid){
			printf("find that pid = %d, process_name :%s, allocated :%d", head->pid, head->process_name, head->size);
			return head;
		}
		head = head->next;
	}

	//没有找到相应的进程
	if (head == NULL){
		printf("NO exist the pid is %10d process \n", pid);
		return NULL;
	}
}
/*删除进程，归还分配的存储空间，并删除描述该进程内存分配的节点*/
void kill_process()
{
	struct allocated_block *ab;
	int pid;

	printf("Kill Process, pid = ");
	scanf("%d", &pid);

	ab = find_process(pid);

	if (ab != NULL){
		//释放ab所表示的分配区
		free_mem(ab);
		//释放ab数据结构节点
		dispose(ab);
	}
	printf("hehhehh\n");
}

/*将ab所表示的已分配区归还，并进行可能的合并*/
int free_mem(struct allocated_block *ab)
{
	int algorithm = ma_algorithm;
	struct free_block_type *fbt, *pre, *work;
	fbt = (struct free_block_type *)malloc(sizeof(struct free_block_type));
	
	if (!fbt){
		return -1;	
	}

	///需要补充的部分
	fbt->size = ab->size;
	fbt->start_addr = ab->start_addr;

	//将准备释放的节点插入到空闲链表的首部
	fbt->next = free_block;
	free_block = fbt;
	rearrange(algorithm);

	compress_free_block();

	rearrange(algorithm);
	return 1;
}

/*释放ab数据结构节点*/
int dispose(struct allocated_block *free_ab)
{
	struct allocated_block *pre, *ab;
	if (free_ab == allocated_block_head){
		///如果要释放第一个节点
		allocated_block_head = allocated_block_head->next;
		free(free_ab);
		return 1;
	}

	pre = allocated_block_head;
	ab = allocated_block_head->next;

	while (ab != free_ab){
		pre = ab;
		ab = ab->next;
	}
	pre->next = ab->next;
	free(ab);
	return 2;
}

/*显示当前的内存使用情况，包括空闲区的情况和已经分配的情况*/
int display_mem_usage()
{
	struct free_block_type *fbt = free_block;
	struct allocated_block *ab = allocated_block_head;

	if (fbt == NULL){
		return -1;
	}
	printf("--------------------------------------------------\n");

	/*显示空闲区*/
	printf("Free Memory:\n");
	printf("%20s %20s\n", "    start_addr", "    size");

	while(fbt != NULL){
		printf("%20d %20d\n", fbt->start_addr, fbt->size);
		fbt = fbt->next;
	}

	/*显示已分配区*/
	printf("\nUsed Memory:\n");
	printf("%10s %20s %10s %10s\n", "PID", "ProcessName", "start_addr", " size");

	while(ab != NULL) {
		printf("%10d %20s %10d %10d\n", ab->pid, ab->process_name, ab->start_addr, ab->size);
		ab = ab->next;
	}

	printf("-----------------------------------------------------\n");
	return 0;
}

int main(int argc, char **argv)
{
	char choice;
	pid = 0;
	free_block = init_free_block(mem_size);
	
	while (1){

		display_menu();  //显示菜单
		fflush(stdin);
		choice = getchar(); //获取用户信息
		switch(choice){
			case '1':set_mem_size();   //设置内存大小
			break;
			case '2':set_algorithm();
				 flag = 1;         //设置算法
			break;
			case '3':new_process();
				 flag = 1;      //创建新的进程
			break;
			case '4':kill_process();
				 flag = 1;      //删除进程
			break;
			case '5':display_mem_usage();
				 flag = 1;	//显示内存使用
			break;
			case '0'://do_exit();
				 exit(0);          //释放链表并退出
			default: break;
		}
	}
		return 0;
}



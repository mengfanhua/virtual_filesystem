#ifndef VIRTUAL_SYSTEM_H
#define VIRTUAL_SYSTEM_H

#include<stdio.h>

#define BLOCK_SIZE 512      //每块大小
#define SUPER_BLOCK 8       //超级块区所占块个数
#define INODE_BLOCK 32      //i节点区所占块数，第一块为主引导部分，不包括在i节点部分
#define INODE_SIZE 64       //每个i节点的大小，经过测试，正好为64字节，无浪费
#define FILE_BLOCK 512      //数据区所占块数
#define MAX_INODE_NUM 256   //计算得出i节点的最大个数
#define MAX_DATA_NUM 512    //数据区的最大个数，逻辑上与所占块数相同
#define MAX_FILE_NUM 10     //每个i节点最多能够连接的数据区块的个数

#define DIR_BLOCK 8         //目录部分所占块数
#define DIR_SIZE 16         //单个目录文件所占字节
#define MAX_DIR_NUM 256     //最大目录数

#define HASH_SIZE 64        //哈希双向链表个数

//此处定义了各部分在文件系统中的基础偏移，inode特殊在留出一个位置给了主引导部分，超级块前留出了引导区
#define SUPER_START BLOCK_SIZE
#define INODE_START SUPER_BLOCK * BLOCK_SIZE
#define DIR_START (SUPER_BLOCK + INODE_BLOCK) * BLOCK_SIZE
#define DATA_START (SUPER_BLOCK + INODE_BLOCK + DIR_BLOCK) * BLOCK_SIZE

#define MAX_SYSTEM_OPEN 50   //系统打开表


struct superblock {
	//**********inode控制部分**********
	unsigned short inode_num;
	unsigned short free_inode_num;
	char free_inode[MAX_INODE_NUM];
	unsigned int inode_point[MAX_INODE_NUM];
	unsigned short max_free_index;

	//**********数据区控制部分**********
	unsigned int data_num;
	unsigned int free_data_num;
	char data_show[MAX_DATA_NUM];
	unsigned int data_point[MAX_DATA_NUM];
	char dirty;

	//**********用户控制部分***********
	unsigned short usernum;
	char uid[8][6];
	char passwd[8][6];
};

struct dir {
	//读取目录数据的结构
	char name[14];
	unsigned short index;
};

struct dinode {
	unsigned int connect_num;
	unsigned short rw_mode;//1为读，2为写，3为执行
	char uid[6];
	unsigned short filesize;
	char file_type[MAX_FILE_NUM];//1为目录，0为文件
	unsigned int block_index[MAX_FILE_NUM];
};

struct inode {
	struct dinode disk_block;
	struct inode *front;
	struct inode *next;
	char dirty;
};

struct hinode {
	struct inode *next;
};

struct system_open {
	unsigned short mode;
	unsigned int count;
	struct inode *inode;
};

int format();
int init();
extern FILE *fp;
extern struct superblock superblock;
extern struct dir dir[MAX_DIR_NUM];
extern struct hinode hash_head[HASH_SIZE];
extern struct system_open sys_open[MAX_SYSTEM_OPEN];
extern char uid[6];//用来登陆后记录当前用户的uid的

#endif // VIRTUAL_SYSTEM_H

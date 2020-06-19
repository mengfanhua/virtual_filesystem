#ifndef VIRTUAL_SYSTEM_H
#define VIRTUAL_SYSTEM_H

#ifdef __cplusplus
extern "C"{
#endif

#include<stdio.h>

#define BLOCK_SIZE 512      //每块大小
#define SUPER_BLOCK 8       //超级块区所占块个数
#define INODE_BLOCK 32      //i节点区所占块数，第一块为主引导部分
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
#define INODE_START (SUPER_BLOCK+1) * BLOCK_SIZE
#define DIR_START (SUPER_BLOCK + INODE_BLOCK+1) * BLOCK_SIZE
#define DATA_START (SUPER_BLOCK + INODE_BLOCK + DIR_BLOCK+1) * BLOCK_SIZE

#define MAX_SYSTEM_OPEN 40   //系统打开表最大值
#define MAX_USER_OPEN 10     //用户打开表最大值
#define USER_NUM 8           //最大用户数
#define MAX_USER_EXIST_NUM 5 //最多同时登录用户数
#define MAX_DIR_NAME_SIZE 11


struct superblock_ {
	//**********inode控制部分**********
	unsigned short inode_num;
	unsigned short free_inode_num;
	char free_inode[MAX_INODE_NUM];
	unsigned int inode_point[MAX_INODE_NUM];

	//**********数据区控制部分**********
	unsigned int data_num;
	unsigned int free_data_num;
	char data_show[MAX_DATA_NUM];
	unsigned int data_point[MAX_DATA_NUM];
	char dirty;

	//**********用户控制部分***********
	unsigned short usernum;
	char uid[USER_NUM][6];
	char passwd[USER_NUM][6];
};

struct dir_ {
	//读取目录数据的结构
	char name[12];
	unsigned short index;
	short front;
};

struct dinode {
	unsigned int rw_mode;//每四位为1个用户的读写权限，001，010，100为读写完全控制，用户顺序从低位到高位
	unsigned short connect_num;
	char uid[6];
	unsigned short filesize;
	char file_type[MAX_FILE_NUM];//1为目录，0为文件
	unsigned int block_index[MAX_FILE_NUM];
};

struct inode {
	struct dinode disk_block;
	unsigned int index;
	struct inode *front;
	struct inode *next;
	char dirty;
};

struct hinode {
	struct inode *next;
};

struct system_open {
	unsigned int count;
	struct inode *inode;
};

int format();
int init();
void exit_sys();
FILE *fp;
struct superblock_ superblock;
struct dir_ dir[MAX_DIR_NUM];
struct hinode hash_head[HASH_SIZE];
struct system_open sys_open[MAX_SYSTEM_OPEN];
char uid[6];//用来登陆后记录当前用户的uid的

//**********log**********
struct user_open {
	char uid[6];
	unsigned short opensize;//已打开个数
	unsigned int useropen[MAX_USER_OPEN];//保存系统打开表的索引
	struct user_open *next;
};

struct cur_path {
	struct cur_path *front;
	struct cur_path *next;
	int inum;//用于记录i节点值
};

struct user_head {
	unsigned short num;
	struct user_open *next;
};
struct user_head uhead;
struct cur_path curpath;
char id[6];

int login(char* uid, char* passwd);//登录
void logout();//登出，会关闭已打开得出文件
int signup(char* uid, char* passwd);//注册用户
void exchange_admin();//切换账户，不会关闭当前用户已打开的文件

//***********文件操作方法**********
void close(int sysopen_index);
char* open(int inode_index);
int save(int inode, char * content);
char* _open(int inode_index);

//**********文件操作与底层接口函数**********
char* read(int dnode_index);
void write(int dnode_index, char* content);
struct inode* iget(int inode_index);
void iput(int inode_index);
int ialloc();
void ifree(int inode_index);
int balloc();
void bfree(int data_index);

//**********文件夹及文件操作方法**********
int mkdir(int inode_index, char* dirname);
int exist_dir(int inode_index, char *name);
int create(int inode_index, char* dirname);
int del(int inode_index, int index);
int share(int inode_index, int new_inode_index);
int access(unsigned int allmode, int mode);


int new_index;

#ifdef __cplusplus
}
#endif

#endif // VIRTUAL_SYSTEM_H

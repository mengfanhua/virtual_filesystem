#ifndef VIRTUAL_SYSTEM_H
#define VIRTUAL_SYSTEM_H

#ifdef __cplusplus
extern "C"{
#endif

#include<stdio.h>

#define BLOCK_SIZE 512      //ÿ���С
#define SUPER_BLOCK 8       //����������ռ�����
#define INODE_BLOCK 32      //i�ڵ�����ռ��������һ��Ϊ����������
#define INODE_SIZE 64       //ÿ��i�ڵ�Ĵ�С���������ԣ�����Ϊ64�ֽڣ����˷�
#define FILE_BLOCK 512      //��������ռ����
#define MAX_INODE_NUM 256   //����ó�i�ڵ��������
#define MAX_DATA_NUM 512    //�����������������߼�������ռ������ͬ
#define MAX_FILE_NUM 10     //ÿ��i�ڵ�����ܹ����ӵ���������ĸ���

#define DIR_BLOCK 8         //Ŀ¼������ռ����
#define DIR_SIZE 16         //����Ŀ¼�ļ���ռ�ֽ�
#define MAX_DIR_NUM 256     //���Ŀ¼��

#define HASH_SIZE 64        //��ϣ˫���������

//�˴������˸��������ļ�ϵͳ�еĻ���ƫ�ƣ�inode����������һ��λ�ø������������֣�������ǰ������������
#define SUPER_START BLOCK_SIZE
#define INODE_START (SUPER_BLOCK+1) * BLOCK_SIZE
#define DIR_START (SUPER_BLOCK + INODE_BLOCK+1) * BLOCK_SIZE
#define DATA_START (SUPER_BLOCK + INODE_BLOCK + DIR_BLOCK+1) * BLOCK_SIZE

#define MAX_SYSTEM_OPEN 40   //ϵͳ�򿪱����ֵ
#define MAX_USER_OPEN 10     //�û��򿪱����ֵ
#define USER_NUM 8           //����û���
#define MAX_USER_EXIST_NUM 5 //���ͬʱ��¼�û���
#define MAX_DIR_NAME_SIZE 11


struct superblock_ {
	//**********inode���Ʋ���**********
	unsigned short inode_num;
	unsigned short free_inode_num;
	char free_inode[MAX_INODE_NUM];
	unsigned int inode_point[MAX_INODE_NUM];

	//**********���������Ʋ���**********
	unsigned int data_num;
	unsigned int free_data_num;
	char data_show[MAX_DATA_NUM];
	unsigned int data_point[MAX_DATA_NUM];
	char dirty;

	//**********�û����Ʋ���***********
	unsigned short usernum;
	char uid[USER_NUM][6];
	char passwd[USER_NUM][6];
};

struct dir_ {
	//��ȡĿ¼���ݵĽṹ
	char name[12];
	unsigned short index;
	short front;
};

struct dinode {
	unsigned int rw_mode;//ÿ��λΪ1���û��Ķ�дȨ�ޣ�001��010��100Ϊ��д��ȫ���ƣ��û�˳��ӵ�λ����λ
	unsigned short connect_num;
	char uid[6];
	unsigned short filesize;
	char file_type[MAX_FILE_NUM];//1ΪĿ¼��0Ϊ�ļ�
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
char uid[6];//������½���¼��ǰ�û���uid��

//**********log**********
struct user_open {
	char uid[6];
	unsigned short opensize;//�Ѵ򿪸���
	unsigned int useropen[MAX_USER_OPEN];//����ϵͳ�򿪱������
	struct user_open *next;
};

struct cur_path {
	struct cur_path *front;
	struct cur_path *next;
	int inum;//���ڼ�¼i�ڵ�ֵ
};

struct user_head {
	unsigned short num;
	struct user_open *next;
};
struct user_head uhead;
struct cur_path curpath;
char id[6];

int login(char* uid, char* passwd);//��¼
void logout();//�ǳ�����ر��Ѵ򿪵ó��ļ�
int signup(char* uid, char* passwd);//ע���û�
void exchange_admin();//�л��˻�������رյ�ǰ�û��Ѵ򿪵��ļ�

//***********�ļ���������**********
void close(int sysopen_index);
char* open(int inode_index);
int save(int inode, char * content);
char* _open(int inode_index);

//**********�ļ�������ײ�ӿں���**********
char* read(int dnode_index);
void write(int dnode_index, char* content);
struct inode* iget(int inode_index);
void iput(int inode_index);
int ialloc();
void ifree(int inode_index);
int balloc();
void bfree(int data_index);

//**********�ļ��м��ļ���������**********
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

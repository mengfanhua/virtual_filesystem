#ifndef VIRTUAL_SYSTEM_H
#define VIRTUAL_SYSTEM_H

#include<stdio.h>

#define BLOCK_SIZE 512      //ÿ���С
#define SUPER_BLOCK 8       //����������ռ�����
#define INODE_BLOCK 32      //i�ڵ�����ռ��������һ��Ϊ���������֣���������i�ڵ㲿��
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
#define INODE_START SUPER_BLOCK * BLOCK_SIZE
#define DIR_START (SUPER_BLOCK + INODE_BLOCK) * BLOCK_SIZE
#define DATA_START (SUPER_BLOCK + INODE_BLOCK + DIR_BLOCK) * BLOCK_SIZE

#define MAX_SYSTEM_OPEN 50   //ϵͳ�򿪱�


struct superblock {
	//**********inode���Ʋ���**********
	unsigned short inode_num;
	unsigned short free_inode_num;
	char free_inode[MAX_INODE_NUM];
	unsigned int inode_point[MAX_INODE_NUM];
	unsigned short max_free_index;

	//**********���������Ʋ���**********
	unsigned int data_num;
	unsigned int free_data_num;
	char data_show[MAX_DATA_NUM];
	unsigned int data_point[MAX_DATA_NUM];
	char dirty;

	//**********�û����Ʋ���***********
	unsigned short usernum;
	char uid[8][6];
	char passwd[8][6];
};

struct dir {
	//��ȡĿ¼���ݵĽṹ
	char name[14];
	unsigned short index;
};

struct dinode {
	unsigned int connect_num;
	unsigned short rw_mode;//1Ϊ����2Ϊд��3Ϊִ��
	char uid[6];
	unsigned short filesize;
	char file_type[MAX_FILE_NUM];//1ΪĿ¼��0Ϊ�ļ�
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
extern char uid[6];//������½���¼��ǰ�û���uid��

#endif // VIRTUAL_SYSTEM_H

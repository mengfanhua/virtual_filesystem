#ifndef VIRTUAL_SYSTEM_H
#define VIRTUAL_SYSTEM_H

#include<stdio.h>

#define BLOCK_SIZE 512      //ÿ���С
#define SUPER_BLOCK 6       //����������ռ�����
#define INODE_BLOCK 33      //i�ڵ�����ռ��������һ��Ϊ���������֣���������i�ڵ㲿��
#define INODE_SIZE 64       //ÿ��i�ڵ�Ĵ�С���������ԣ�����Ϊ64�ֽڣ����˷�
#define FILE_BLOCK 512      //��������ռ����
#define MAX_INODE_NUM 256   //����ó�i�ڵ��������
#define MAX_DATA_NUM 512    //�����������������߼�������ռ������ͬ
#define MAX_FILE_NUM 10     //ÿ��i�ڵ�����ܹ����ӵ���������ĸ���

//�˴������˸��������ļ�ϵͳ�еĻ���ƫ�ƣ�inode����������һ��λ�ø������������֣�������ǰ������������
#define SUPER_START BLOCK_SIZE
#define ROOTDIR_START SUPER_BLOCK * BLOCK_SIZE
#define INODE_START (SUPER_BLOCK + 1) * BLOCK_SIZE
#define DATA_START (SUPER_BLOCK + INODE_BLOCK) * BLOCK_SIZE


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
	//���ã����ļ�ϵͳ��Ŀ¼�����ṹ����ȡĿ¼���ݵĽṹ
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
	unsigned short block_num;
	struct dinode disk_block;
	struct inode *front;
	struct inode *next;
	char dirty;
};

int format();
void init();
extern FILE *fp;
extern struct superblock superblock;

#endif // VIRTUAL_SYSTEM_H

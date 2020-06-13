#include "virtual_system.h"
#include <malloc.h>
#include <string.h>

FILE *fp = NULL;
struct superblock superblock;
struct dir dir[MAX_DIR_NUM];
struct hinode hash_head[HASH_SIZE];
struct system_open sys_open[MAX_SYSTEM_OPEN];
struct user_head uhead;
struct cur_path curpath;
char id[6];

int format() {
	int flag = 0;
	char * buf;
	if (fp != NULL) {
		fclose(fp);//���ִ�й�������Ҫ��ʽ��
	}
	fp = fopen("filesystem", "wb+");
	buf = (char *)malloc((INODE_BLOCK + FILE_BLOCK + SUPER_BLOCK + DIR_BLOCK + 1) * BLOCK_SIZE * sizeof(char));
	if (buf == NULL)
	{
		return flag;
	}
	fseek(fp, 0, SEEK_SET);
	fwrite(buf, 1, (INODE_BLOCK + FILE_BLOCK + SUPER_BLOCK + DIR_BLOCK + 1) * BLOCK_SIZE * sizeof(char), fp);
	//**********��ʼ���쳬����**********
	superblock.inode_num = MAX_INODE_NUM;
	superblock.free_inode_num = MAX_INODE_NUM - 1;
	int i;
	for (i = MAX_INODE_NUM - 1; i >= 0; i--) {
		superblock.free_inode[i] = 0;
		superblock.inode_point[i] = MAX_INODE_NUM - i - 1;
	}
	superblock.free_inode[MAX_INODE_NUM - 1] = 1;
	superblock.max_free_index = MAX_INODE_NUM - 2;

	superblock.data_num = MAX_DATA_NUM;
	superblock.free_data_num = MAX_DATA_NUM - 1;
	for (i = 0; i < MAX_DATA_NUM; i++) {
		superblock.data_show[i] = 0;
		superblock.data_point[i] = i;
	}

	superblock.dirty = 0;

	superblock.usernum = 1;
	strcpy(superblock.uid[0], "admin");
	strcpy(superblock.passwd[0], "admin");
	fseek(fp, SUPER_START, SEEK_SET);
	fwrite(&superblock, 1, sizeof(struct superblock), fp);
	//**********��ʼд��Ŀ¼i�ڵ�**********
	struct dinode dinode;
	dinode.connect_num = 1;
	dinode.rw_mode = 0x777;//�����û�ȫ������Ȩ��
	strcpy(dinode.uid, superblock.uid[0]);
	dinode.filesize = 0;
	dinode.file_type[0] = 1;
	fseek(fp, INODE_START, SEEK_SET);
	fwrite(&dinode, 1, sizeof(struct dinode), fp);
	//**********��ʼд�������׿��ļ���Ŀ¼����**********
	strcpy(dir[0].name, "root");
	dir[0].index = 0;//ע�⣺�ô�ΪĿ¼��ָ��i�ڵ���������ָʾ��һ��i�ڵ��λ��
	fseek(fp, DIR_START, SEEK_SET);
	fwrite(&dir[0], 1, sizeof(struct dir), fp);
	flag = 1;
	return flag;
}

int init() {
	int flag = 0;
	fp = fopen("filesystem", "rb+");
	if (fp == NULL) {
		//�ļ�ϵͳ������
		if (format() == 0) {
			//��ʽ��ʧ��
			return flag;
		}
	}
	else {
		//**********��ȡ������**********
		fseek(fp, SUPER_START, SEEK_SET);
		fread(&superblock, 1, sizeof(struct superblock), fp);
		//**********��ȡȫ��Ŀ¼**********
		int j;
		for (j = 0; j < MAX_DIR_NUM; j++) {
			fseek(fp, DIR_START + DIR_SIZE*j, SEEK_SET);
			fread(&dir[j], 1, sizeof(struct dir), fp);
		}
	}
	//**********��ʼ��hash����**********
	int i;
	for (i = 0; i < HASH_SIZE; i++) {
		hash_head[i].next = NULL;
	}
	//**********��ʼ��ϵͳ�ļ��򿪱�**********
	for (i = 0; i < MAX_SYSTEM_OPEN; i++) {
		sys_open[i].count = 0;
		sys_open[i].inode = NULL;
	}
	//**********��ʼ���û��򿪱�����ͷ**********
	uhead.next = NULL;
	uhead.num = 0;
	//**********��ʼ����ǰ·��**********
	curpath.inum = -1;
	curpath.front = &curpath;
	curpath.next = &curpath;
	//**********��ʼ����ǰ�û���**********
	strcpy(id, "");
	flag = 1;
	return flag;
}
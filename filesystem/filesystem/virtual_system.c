#include "virtual_system.h"
#include <malloc.h>
#include <string.h>

FILE *fp;
struct superblock superblock;
struct dir dir[MAX_DIR_NUM];
struct hinode hash_head[HASH_SIZE];
struct system_open sys_open[MAX_SYSTEM_OPEN];

int format() {
	int flag = 0;
	char * buf;
	if (fp != NULL) {
		fclose(fp);//如果执行过程中想要格式化
	}
	fp = fopen("filesystem", "wb+");
	buf = (char *)malloc((INODE_BLOCK + FILE_BLOCK + SUPER_BLOCK + DIR_BLOCK + 1) * BLOCK_SIZE * sizeof(char));
	if (buf == NULL)
	{
		return flag;
	}
	fseek(fp, 0, SEEK_SET);
	fwrite(buf, 1, (INODE_BLOCK + FILE_BLOCK + SUPER_BLOCK + DIR_BLOCK + 1) * BLOCK_SIZE * sizeof(char), fp);
	//**********开始构造超级块**********
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
	//**********开始写根目录i节点**********
	struct dinode dinode;
	dinode.connect_num = 1;
	dinode.rw_mode = 1;
	strcpy(dinode.uid, superblock.uid[0]);
	dinode.filesize = 0;
	dinode.file_type[0] = 1;
	fseek(fp, INODE_START, SEEK_SET);
	fwrite(&dinode, 1, sizeof(struct dinode), fp);
	//**********开始写数据区首块文件根目录名称**********
	strcpy(dir[0].name, "root");
	dir[0].index = 0;//注意：该处为目录区指向i节点区，用来指示下一个i节点的位置
	fseek(fp, DIR_START, SEEK_SET);
	fwrite(&dir[0], 1, sizeof(struct dir), fp);
	flag = 1;
	return flag;
}

int init() {
	int flag = 0;
	fp = fopen("filesystem", "rb+");
	if (fp == NULL) {
		//文件系统不存在
		if (format() == 0) {
			//格式化失败
			return flag;
		}
	}
	else {
		//**********读取超级块**********
		fseek(fp, SUPER_START, SEEK_SET);
		fread(&superblock, 1, sizeof(struct superblock), fp);
		//**********读取全部目录**********
		int j;
		for (j = 0; j < MAX_DIR_NUM; j++) {
			fseek(fp, DIR_START + DIR_SIZE*j, SEEK_SET);
			fread(&dir[j], 1, sizeof(struct dir), fp);
		}
	}
	//**********初始化hash链表**********
	int i;
	for (i = 0; i < HASH_SIZE; i++) {
		hash_head[i].next = NULL;
	}
	//**********初始化系统文件打开表**********
	for (i = 0; i < MAX_SYSTEM_OPEN; i++) {
		sys_open[i].count = 0;
		sys_open[i].mode = 1;
		sys_open[i].inode = NULL;
	}
	flag = 1;
	return flag;
}

#include "virtual_system.h"
#include <malloc.h>
#include <string.h>
FILE *fp;
struct superblock superblock;

int format() {
	int flag = 0;
	char * buf;
	if (fp != NULL) {
		fclose(fp);//如果执行过程中想要格式化
	}
	fp = fopen("filesystem", "wb+");
	buf = (char *)malloc((INODE_BLOCK + FILE_BLOCK + SUPER_BLOCK + 1) * BLOCK_SIZE * sizeof(char));
	if (buf == NULL)
	{
		return flag;
	}
	fseek(fp, 0, SEEK_SET);
	fwrite(buf, 1, (INODE_BLOCK + FILE_BLOCK + SUPER_BLOCK + 1) * BLOCK_SIZE * sizeof(char), fp);
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
	superblock.data_show[0] = 1;
	superblock.dirty = 0;
	superblock.usernum = 1;
	strcpy(superblock.uid[0], "admin");
	strcpy(superblock.passwd[0], "admin");
	fseek(fp, SUPER_START, SEEK_SET);
	fwrite(&superblock, 1, sizeof(struct superblock), fp);
	//**********开始写主文件目录**********
	struct dir rootdir;
	strcpy(rootdir.name, "/");
	rootdir.index = 0;//注意：此处用来做主文件目录，该index指向数据区首位置，与下方的数据区指出不同
	fseek(fp, ROOTDIR_START, SEEK_SET);
	fwrite(&rootdir, 1, sizeof(struct dir), fp);
	//**********开始写根目录i节点**********
	struct dinode dinode;
	dinode.connect_num = 0;
	dinode.rw_mode = 1;
	strcpy(dinode.uid, superblock.uid[0]);
	dinode.filesize = 0;
	dinode.file_type[0] = 1;
	fseek(fp, INODE_START, SEEK_SET);
	fwrite(&dinode, 1, sizeof(struct dinode), fp);
	//**********开始写数据区首块文件根目录名称**********
	struct dir subdir;
	strcpy(subdir.name, "root");
	subdir.index = 0;//注意：该处为数据区指向i节点区，用来指示下一个i节点的位置
	fseek(fp, DATA_START, SEEK_SET);
	fwrite(&subdir, 1, sizeof(struct dir), fp);
	flag = 1;
	return flag;
}

void init() {

}

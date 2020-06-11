#include "virtual_system.h"
#include <malloc.h>
#include <string.h>
FILE *fp;
struct superblock superblock;

int format() {
	int flag = 0;
	char * buf;
	if (fp != NULL) {
		fclose(fp);//���ִ�й�������Ҫ��ʽ��
	}
	fp = fopen("filesystem", "wb+");
	buf = (char *)malloc((INODE_BLOCK + FILE_BLOCK + SUPER_BLOCK + 1) * BLOCK_SIZE * sizeof(char));
	if (buf == NULL)
	{
		return flag;
	}
	fseek(fp, 0, SEEK_SET);
	fwrite(buf, 1, (INODE_BLOCK + FILE_BLOCK + SUPER_BLOCK + 1) * BLOCK_SIZE * sizeof(char), fp);
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
	superblock.data_show[0] = 1;
	superblock.dirty = 0;
	superblock.usernum = 1;
	strcpy(superblock.uid[0], "admin");
	strcpy(superblock.passwd[0], "admin");
	fseek(fp, SUPER_START, SEEK_SET);
	fwrite(&superblock, 1, sizeof(struct superblock), fp);
	//**********��ʼд���ļ�Ŀ¼**********
	struct dir rootdir;
	strcpy(rootdir.name, "/");
	rootdir.index = 0;//ע�⣺�˴����������ļ�Ŀ¼����indexָ����������λ�ã����·���������ָ����ͬ
	fseek(fp, ROOTDIR_START, SEEK_SET);
	fwrite(&rootdir, 1, sizeof(struct dir), fp);
	//**********��ʼд��Ŀ¼i�ڵ�**********
	struct dinode dinode;
	dinode.connect_num = 0;
	dinode.rw_mode = 1;
	strcpy(dinode.uid, superblock.uid[0]);
	dinode.filesize = 0;
	dinode.file_type[0] = 1;
	fseek(fp, INODE_START, SEEK_SET);
	fwrite(&dinode, 1, sizeof(struct dinode), fp);
	//**********��ʼд�������׿��ļ���Ŀ¼����**********
	struct dir subdir;
	strcpy(subdir.name, "root");
	subdir.index = 0;//ע�⣺�ô�Ϊ������ָ��i�ڵ���������ָʾ��һ��i�ڵ��λ��
	fseek(fp, DATA_START, SEEK_SET);
	fwrite(&subdir, 1, sizeof(struct dir), fp);
	flag = 1;
	return flag;
}

void init() {

}

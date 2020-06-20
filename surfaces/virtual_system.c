#include "virtual_system.h"
#include <malloc.h>
#include <string.h>

struct superblock_ superblock;
struct dir_ dir[MAX_DIR_NUM];
struct system_open sys_open[MAX_SYSTEM_OPEN];
FILE *fp = NULL;
struct hinode hash_head[HASH_SIZE];
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
	fwrite(&superblock, 1, sizeof(struct superblock_), fp);
	//**********��ʼд��Ŀ¼i�ڵ�**********
	struct dinode dinode;
	dinode.connect_num = 1;
	dinode.rw_mode = 0x77777777;//�����û�ȫ������Ȩ��
	strcpy(dinode.uid, superblock.uid[0]);
	dinode.filesize = 0;
	dinode.file_type[0] = 1;
	fseek(fp, INODE_START, SEEK_SET);
	fwrite(&dinode, 1, sizeof(struct dinode), fp);
	//**********��ʼд�������׿��ļ���Ŀ¼����**********
	strcpy(dir[0].name, "root");
	dir[0].index = 0;//ע�⣺�ô�ΪĿ¼��ָ��i�ڵ���������ָʾ��һ��i�ڵ��λ��
    for(i=0;i<MAX_DIR_NUM;i++){
        dir[i].front = -1;
    }
	fseek(fp, DIR_START, SEEK_SET);
	fwrite(&dir[0], 1, sizeof(struct dir_), fp);
	flag = 1;
    fclose(fp);
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
        else{
            return init();
        }
	}
	else {
		//**********��ȡ������**********
		fseek(fp, SUPER_START, SEEK_SET);
		fread(&superblock, 1, sizeof(struct superblock_), fp);
		//**********��ȡȫ��Ŀ¼**********
		int j;
		for (j = 0; j < MAX_DIR_NUM; j++) {
			fseek(fp, DIR_START + DIR_SIZE*j, SEEK_SET);
			fread(&dir[j], 1, sizeof(struct dir_), fp);
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
    curpath.inum = 0;
	curpath.front = &curpath;
	curpath.next = &curpath;
	//**********��ʼ����ǰ�û���**********
	strcpy(id, "");
	flag = 1;
	return flag;
}

void exit_sys() {
	if (superblock.dirty == 1) {//�������й�����
		superblock.dirty = 0;
		fseek(fp, SUPER_START, SEEK_SET);
		fwrite(&superblock, 1, sizeof(struct superblock_), fp);
	}
	int i;
	struct inode *p, *t;
	for (i = 0; i < HASH_SIZE; i++) {
		p = hash_head[i].next;
		while (p != NULL) {
			if (p->dirty == 1) {//��ÿ���޸Ĺ���i�ڵ���д洢
				fseek(fp, INODE_START+INODE_SIZE*p->index, SEEK_SET);
				fwrite(&p->disk_block, 1, sizeof(struct dinode), fp);
                iput(p->index);
			}
			t = p;
			p = p->next;
		}
	}
	int j;
	for (j = 0; j < MAX_DIR_NUM; j++) {//��Ŀ¼���д洢
		fseek(fp, DIR_START + DIR_SIZE*j, SEEK_SET);
		fwrite(&dir[j], 1, sizeof(struct dir_), fp);
	}

    for(i=0;i<MAX_SYSTEM_OPEN;i++){
        sys_open[i].count = 0;
        sys_open[i].inode = NULL;
    }
    struct user_open *s;
    while(uhead.num != 0){
        s = uhead.next;
        uhead.next = s->next;
        uhead.num-=1;
        free(s);
    }
    s=NULL;
	p = NULL;
	t = NULL;
    fclose(fp);
    fp = NULL;
}

#include "virtual_system.h"
#include<malloc.h>
#include<string.h>

struct system_open sys_open[MAX_SYSTEM_OPEN];
struct superblock superblock;

void close(int sysopen_index) {
	//�û����ļ������������ִ���
	sys_open[sysopen_index].count -= 1;
	if (sys_open[sysopen_index].count == 0) {
		sys_open[sysopen_index].inode = NULL;
	}
}

char* _open(int inode_index) {
	//�˲�������Ѿ����ڵĴ򿪵��ļ����д���
	struct inode *p;
	p = iget(inode_index);
	char *t, *r;
	t = (char *)malloc(sizeof(char)*p->disk_block.filesize);
	t[0] = '\0';
	int i;
	for (i = 0; i < p->disk_block.filesize; i++) {
		r = read(p->disk_block.block_index[i]);
		if (i == 0) {
			strcpy(t, r);
		}
		else {
			strcat(t, r);
		}
		free(r);
	}
	p = NULL;
	r = NULL;
	return t;
}

char* open(int inode_index) {
	//����Ϊ�ļ���i�ڵ㣬����Ϊ���ݿ�Ŀ��,�û����ļ������������ִ���
	//�˲��������δ�򿪹����ļ����û�ʹ��
	int j, flag = 0;
	char *s;
	for (j = 0; j < MAX_SYSTEM_OPEN; j++) {
		if (sys_open[j].count == 0) {
			flag = 1;//��־�п���λ��
		}
		else if (sys_open[j].inode->index == inode_index) {
			break;//�ҵ���Ӧ���ļ�
		}
		else {
			continue;
		}
	}
	if (j != MAX_SYSTEM_OPEN) {//���ҵ��ļ�
		sys_open[j].count += 1;
		s = _open(inode_index);

	}
	else if (flag == 1) {//û���ҵ��ļ������п�λ�������
		for (j = 0; j < MAX_SYSTEM_OPEN; j++) {
			if (sys_open[j].count == 0) {
				break;
			}
			else {
				continue;
			}
		}
		sys_open[j].count += 1;
		sys_open[j].inode = iget(inode_index);//����i�ڵ�
		s = _open(inode_index);
	}
	else {
		s = NULL;//��null�жϲ���ʧ��
	}
	return s;
}

char * _split_char(char* content, int start, int size) {//�����ַ����ָ��㷨
	char *p;
	p = (char *)malloc(sizeof(char)*size+1);
	int i;
	for (i = 0; i < size; i++) {
		p[i] = content[start + i];
	}
	p[size] = '\0';//�˴�������һλ����ֹ��д���ļ�ʱ���ڷô��������Խ������
	return p;
}

int save(int inode, char* content) {
	int flag = 0;
	int size = strlen(content);
	int block;
	//�ܷ�ռ����������
	if (size%BLOCK_SIZE != 0) {
		block = (int)(size / BLOCK_SIZE) + 1;
	}
	else {
		block = (int)(size / BLOCK_SIZE);
	}
	struct inode *p;
	p = iget(inode);//��ȡ��Ӧi�ڵ�
	if (block > MAX_FILE_NUM) {}//�ļ�������������
	else if (p->disk_block.filesize < block) {
		//��ǰ�����ļ��鲻���Դ洢�޸ĺ���ַ���
		if (superblock.free_data_num >= block - p->disk_block.filesize) {
			//�����㹻�Ŀ��п�
			int i;
			for (i = p->disk_block.filesize; i < block; i++) {
				p->disk_block.block_index[i] = balloc();//��ȡ���п�����
				p->disk_block.filesize += 1;
				p->disk_block.file_type[i] = 2;//�����ʶ���ļ�������
			}
			char *t;
			for (i = 0; i < block; i++) {
				if (i + 1 == block && size%BLOCK_SIZE != 0) {
					//�˴��ж��Ƿ�����ȫ�洢���÷�֧����
					t = _split_char(content, i*BLOCK_SIZE, size - i*BLOCK_SIZE);
				}
				else {
					t = _split_char(content, i*BLOCK_SIZE, BLOCK_SIZE);
				}
				write(p->disk_block.block_index[i], t);
				free(t);
			}
			t = NULL;
			flag = 1;
		}
	}
	else if (p->disk_block.filesize > block) {
		//��ǰռ�п���
		int i, j = 0;
		for (i = block; i < p->disk_block.filesize; i++) {
			bfree(p->disk_block.block_index[i]);//�ͷŶ����
			j += 1;
		}
		p->disk_block.filesize -= j;
		char *t;
		for (i = 0; i < block; i++) {
			if (i + 1 == block && size%BLOCK_SIZE != 0) {
				t = _split_char(content, i*BLOCK_SIZE, size - i*BLOCK_SIZE);
			}
			else {
				t = _split_char(content, i*BLOCK_SIZE, BLOCK_SIZE);
			}
			write(p->disk_block.block_index[i], t);
			free(t);
		}
		t = NULL;
		flag = 1;
	}
	else {
		int i;
		char *t;
		for (i = 0; i < block; i++) {
			if (i + 1 == block && size%BLOCK_SIZE != 0) {
				t = _split_char(content, i*BLOCK_SIZE, size - i*BLOCK_SIZE);
			}
			else {
				t = _split_char(content, i*BLOCK_SIZE, BLOCK_SIZE);
			}
			write(p->disk_block.block_index[i], t);
			free(t);
		}
		t = NULL;
		flag = 1;
	}
	p = NULL;
	return flag;
}
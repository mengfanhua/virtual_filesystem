#include "virtual_system.h"
#include<string.h>

struct superblock superblock;
struct dir dir[MAX_DIR_NUM];
char uid[6];
struct system_open sys_open[MAX_SYSTEM_OPEN];

unsigned int _cul_mode(int type) {//����Ĭ��Ȩ��,0Ϊ�ļ���1Ϊ�ļ��У�����Ĭ��Ȩ�޲�ͬ
	int i, j;
	for (i = 0; i < superblock.usernum; i++) {
		if (strcmp(superblock.uid[i], uid) == 0) {//�ҵ���ǰ�û�������id
			break;
		}
		else {
			continue;
		}
	}
	unsigned int flag;
	if (type == 1) {//�ļ���Ĭ�ϸ���дȨ��
		flag = 0x33333333;
	}
	else {//�ļ�Ĭ��ֻ����Ȩ��
		flag = 0x11111111;
	}
	unsigned int te = 0x7;
	for (j = 0; j < i; j++) {
		te = te * 16;//�û�ƫ��Ϊ4λ�����Գ�16
	}
	flag = flag | te;//�����㽫ӵ��������Ϊ����Ȩ��
	return flag;
}

int mkdir(int inode_index, char* dirname) {//�����ļ���
	int flag = 0;
	struct inode *p;
	p = iget(inode_index);//��ȡ��Ŀ¼�ڵ�id
	if (p->disk_block.filesize == MAX_FILE_NUM) {}//���ļ����ѵ����ֵ
	else {
		int i = 0, j = 0;
		while (i < p->disk_block.filesize) {
			//ע���˴�����Լ�����������б�ɾ���Ľڵ���Ҫ��index��ΪMAX_INODE_NUM�������޷�ִ��
			if (p->disk_block.block_index[j] != MAX_INODE_NUM) {//����λ����ָ��
				if (strcmp(dir[p->disk_block.block_index[j]].name, dirname) == 0) {//�ж�����
					break;
				}
				else {
					i += 1;//�ڴ���ָ��ʱ������ֵ��һ
				}
			}
			j += 1;//����ȫ����ѭ��
		}
		if (i == p->disk_block.filesize) {//֤������������������Ĭ��flag=0
			i = 0;
			while (i < p->disk_block.filesize) {//Ѱ����С�Ŀհ���
				if (p->disk_block.block_index[i] == MAX_INODE_NUM) {//�˴�������delete��������λ�������޷�ִ��
					break;
				}
				else {
					i += 1;
				}
			}
			int index = ialloc();//��ȡi�ڵ�
			if (index != -1) {//����i�ڵ����
				strcpy(dir[index].name, dirname);//��Ŀ¼��Ӧλ
				dir[index].index = index;//��Ŀ¼��Ӧָ��
				dir[index].front = p->index;
				p->dirty = 1;//���ø�Ŀ¼Ϊ�࣬ʹ�ùر�ϵͳʱд�ش���
				p->disk_block.filesize += 1;
				p->disk_block.block_index[i] = index;
				p->disk_block.file_type[i] = 1;//���Ϊ�ļ��У��ļ������ļ��򿪷�ʽ��һ��
				p = iget(index);//��ȡ��Ŀ¼
				p->dirty = 1;//����λ

				//��ʼ���µ�i�ڵ�
				p->disk_block.filesize = 0;
				p->disk_block.rw_mode = _cul_mode(1);//����Ȩ��ֵ��Ĭ��ֻ��ӵ����ȫ��Ȩ��
				strcpy(p->disk_block.uid, uid);//���ӵ����
				p->disk_block.connect_num = 1;
				flag = 1;
			}
			else {//�޿���i�ڵ�
				flag = -1;
			}
		}
	}
	p = NULL;
	return flag;
}

int create(int inode_index, char* dirname) {
	int flag = 0;
	struct inode *p;
	p = iget(inode_index);//��ȡ��Ŀ¼�ڵ�id
	if (p->disk_block.filesize == MAX_FILE_NUM) {}//���ļ����ѵ����ֵ
	else {
		int i = 0, j = 0;
		while (i < p->disk_block.filesize) {
			//ע���˴�����Լ�����������б�ɾ���Ľڵ���Ҫ��index��ΪMAX_INODE_NUM�������޷�ִ��
			if (p->disk_block.block_index[j] != MAX_INODE_NUM) {//����λ����ָ��
				if (strcmp(dir[p->disk_block.block_index[j]].name, dirname) == 0) {//�ж�����
					break;
				}
				else {
					i += 1;//�ڴ���ָ��ʱ������ֵ��һ
				}
			}
			j += 1;//����ȫ����ѭ��
		}
		if (i == p->disk_block.filesize) {//֤������������������Ĭ��flag=0
			i = 0;
			while (i < p->disk_block.filesize) {//Ѱ����С�Ŀհ���
				if (p->disk_block.block_index[i] == MAX_INODE_NUM) {//�˴�������delete��������λ�������޷�ִ��
					break;
				}
				else {
					i += 1;
				}
			}
			int index = ialloc();//��ȡi�ڵ�
			if (index != -1) {//����i�ڵ����
				strcpy(dir[index].name, dirname);//��Ŀ¼��Ӧλ
				dir[index].index = index;//��Ŀ¼��Ӧָ��
				dir[index].front = p->index;
				p->dirty = 1;//���ø�Ŀ¼Ϊ�࣬ʹ�ùر�ϵͳʱд�ش���
				p->disk_block.filesize += 1;
				p->disk_block.block_index[i] = index;
				p->disk_block.file_type[i] = 0;//���Ϊ�ļ����ļ������ļ��򿪷�ʽ��һ��
				p = iget(index);//��ȡ��Ŀ¼
				p->dirty = 1;//����λ

							 //��ʼ���µ�i�ڵ�
				p->disk_block.filesize = 0;
				p->disk_block.rw_mode = _cul_mode(0);//����Ȩ��ֵ��Ĭ��ֻ��ӵ����ȫ��Ȩ��
				strcpy(p->disk_block.uid, uid);//���ӵ����
				p->disk_block.connect_num = 1;
				flag = 1;
			}
			else {//�޿���i�ڵ�
				flag = -1;
			}
		}
	}
	p = NULL;
	return flag;
}
int _open_system_sys(int inode_index) {
	int flag = 0;
	int i = 0, j = 0;
	for (i = 0; i < MAX_SYSTEM_OPEN; i++) {
		if (sys_open[i].count == 0) {
			continue;
		}
		else if (sys_open[i].inode->index != inode_index) {
			continue;
		}
		else {
			flag = 1;
			break;
		}
	}
	return flag;
}

int del(int inode_index, int index) {//ɾ���ļ��е���ɾ�������Ȩ�޲�����ɾ��,��indexΪǰindex�����
	struct inode *p,*q;
	int i = 0, j = 0;
	int flag = 1;
	p = iget(inode_index);
	q = iget(dir[p->disk_block.block_index[index]].index);
	if (p->disk_block.file_type[index] == 0) {//�жϵ�Ҫɾ�������ļ�
		if (_open_system_sys(q->index) != 0 || access(q->disk_block.rw_mode, 3) == 0) {
			//   !=   �ļ�û�򿪲��Ҹ��û���Ȩ��ɾ��
			flag = 0;
		}
		else {
			for (i = 0; i < q->disk_block.filesize; i++) {
				bfree(q->disk_block.block_index[i]);//�ͷ�������
			}
			ifree(q->index);//�ͷ�i�ڵ�
			iput(q->index);//�ͷ��ڴ�i�ڵ�
			p->disk_block.filesize -= 1;
			p->disk_block.block_index[index] = MAX_INODE_NUM;//�Կհ���������λ
			p->dirty = 1;
		}
	}
	else if (p->disk_block.file_type[index] == 3) {//ɾ����ݷ�ʽ
		p->disk_block.filesize -= 1;
		p->disk_block.block_index[index] = MAX_INODE_NUM;//�Կհ���������λ
		q->disk_block.connect_num -= 1;
		q->dirty = 1;
	}
	else {
		while (i < q->disk_block.filesize) {
			if (q->disk_block.block_index[j] == MAX_INODE_NUM) {}//����֮ǰɾ���Ŀ�λ
			else {
				flag = flag & del(q->index, j);//����ɾ�������¼����ɾ��ֵ
				i += 1;
			}
			j += 1;
		}
		if (access(q->disk_block.rw_mode, 3) == 0 || flag == 0) {//Ȩ�޲�����ļ���������û��ȫ��ɾ���ɾ�
			flag = 0;
		}
		else {
			ifree(q->index);//�ͷ�i�ڵ�
			iput(q->index);//�ͷ��ڴ�i�ڵ�
			p->disk_block.filesize -= 1;
			p->disk_block.block_index[index] = MAX_INODE_NUM;//�Կհ���������λ
			p->dirty = 1;
		}
	}
	return flag;
}

int share(int inode_index, int new_inode_index) {//��һ��������Ҫ�����ӵĽڵ㣬��һ��������Ŀ�Ľڵ㣬�����->ǰ
	//���жϻ᲻�ṹ�ɻ�·��ע���˴���������ֱ����ӵ����ļ�
	int flag = 0;
	int m = new_inode_index;
	while (m != -1) {
		if (m != inode_index) {
			m = dir[m].front;
			continue;//��ǰ�ж�
		}
		else {
			break;//�жϵ���
		}
	}
	if (m == -1) {
		//û�в�ѯ������û�бջ�
		struct inode *p, *t;
		p = iget(new_inode_index);
		t = iget(inode_index);
		if (p->disk_block.filesize == MAX_FILE_NUM) { }//��ǰ�ļ����Ѿ�����
		else {
			int i = 0, j = 0;
			while (i < p->disk_block.filesize) {
				if (p->disk_block.block_index[j] != MAX_INODE_NUM) {//����λ����ָ��
					if (strcmp(dir[p->disk_block.block_index[j]].name, dir[inode_index].name) == 0) {//�ж�����
						break;
					}
					else {
						i += 1;//�ڴ���ָ��ʱ������ֵ��һ
					}
				}
				j += 1;//����ȫ����ѭ��
			}
			if (i == p->disk_block.filesize) {//������
				i = 0;
				while (i < p->disk_block.filesize) {//Ѱ����С�Ŀհ���
					if (p->disk_block.block_index[i] == MAX_INODE_NUM) {//�˴�������delete��������λ�������޷�ִ��
						break;
					}
					else {
						i += 1;
					}
				}
				p->disk_block.block_index[i] = inode_index;
				p->disk_block.filesize += 1;
				p->disk_block.file_type[i] = 3;//3��ʾ��ݷ�ʽ
				p->dirty = 1;
				t->disk_block.connect_num += 1;
				t->dirty = 1;

				flag = 1;
			}
		}
		p = NULL;
		t = NULL;
	}
	else {
		flag = -1;
	}
	return flag;
}

int access(unsigned int allmode, int mode) {
	int i, j, f;
	unsigned int flag;
	for (i = 0; i < superblock.usernum; i++) {
		if (strcmp(superblock.uid[i], uid) == 0) {//�ҵ���ǰ�û�������id
			break;
		}
		else {
			continue;
		}
	}
	unsigned int te = 0x7;
	for (j = 0; j < i; j++) {
		te = te * 16;//�û�ƫ��Ϊ4λ�����Գ�16
	}
	flag = allmode & te;//��ȡ��ǰ�û��ļ�Ȩ��
	for (j = 0; j < i; j++) {
		flag = flag / 16;//���������ʹȨ��ָʾ�������λ
	}
	if ((flag & mode) == 0) {//�ж϶�Ӧλ�Ƿ�Ϊ1
		f = 0;
	}
	else {
		f = 1;
	}
	return f;
}

int exist_dir(int inode_index, char *name) {
	int flag = 0;
	struct inode *p;
	p = iget(inode_index);
	int i = 0, j = 0;
	while (i < p->disk_block.filesize) {
		if (p->disk_block.block_index[j] != MAX_FILE_NUM) {
			i += 1;
			if (strcmp(dir[p->disk_block.block_index[j]].name, name) == 0) {
				flag = 1;
				break;
			}
			else {
				j += 1;
			}
		}
	}
	return flag;
}

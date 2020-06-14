#include "virtual_system.h"

struct superblock superblock;
struct dir dir[MAX_DIR_NUM];
char uid[6];

int _cul_mode(int type) {//����Ĭ��Ȩ��,0Ϊ�ļ���1Ϊ�ļ��У�����Ĭ��Ȩ�޲�ͬ

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

int del(int inode_index, int index) {//ɾ���ļ��е���ɾ�������Ȩ�޲�����ɾ��
	struct inode *p,*q;
	int i = 0, j = 0;
	int flag = 1;
	p = iget(inode_index);
	q = iget(dir[p->disk_block.block_index[index]].index);
	if (p->disk_block.file_type[index] == 0) {//�жϵ�Ҫɾ�������ļ�
		if (q->disk_block.connect_num != 0 || access(q->disk_block.rw_mode, 3) == 0) {
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
		}
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
		}
	}
	return flag;
}

int share(int inode_index, int new_inode_index) {

}

int access(int allmode, int mode) {
	
}
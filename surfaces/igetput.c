#include "virtual_system.h"
#include<malloc.h>
#include<string.h>

struct inode* iget(int inode_index) {
	int m = inode_index % HASH_SIZE;//����hashֵ
	struct inode *p, *t = NULL;//t������¼p��ǰһ�ڵ�
	for (p = hash_head[m].next; p != NULL; p = p->next) {
		if (p->index == inode_index) {//���ڸýڵ�
			break;
		}
		else {
			t = p;//��¼��ǰ�ڵ�
			continue;
		}
	}
	if (p == NULL) {//û�в�ѯ���ýڵ�
		//���ļ��е���
		p = (struct inode *)malloc(sizeof(struct inode));
		fseek(fp,INODE_START+inode_index*INODE_SIZE,SEEK_SET);
		fread(&p->disk_block, 1, INODE_SIZE, fp);
		//tΪNULL����hash_head��nextΪNULL������ǰû�нڵ�
		if (t == NULL) {
			hash_head[m].next = p;
			p->front = NULL;
		}
		else {
			t->next = p;
			p->front = t;
		}
		p->dirty = 0;
		p->index = inode_index;
        p->next = NULL;
	}
	//������ڽڵ�ֱ�ӷ��ؾͿ�
	t = NULL;
	return p;
}

void iput(int inode_index) {
	int m = inode_index % HASH_SIZE;//����hashֵ
	struct inode *p, *t = NULL;//t������¼p��ǰһ�ڵ�
	for (p = hash_head[m].next; p != NULL; p = p->next) {
		if (p->index == inode_index) {//���ڸýڵ�
			break;
		}
		else {
			t = p;//��¼��ǰ�ڵ�
			continue;
		}
	}
	if (p != NULL) {//�ҵ��ýڵ�
		if (p->dirty == 1) {//�ýڵ㱻�޸Ĺ�
			//���޸�д���ļ�
			fseek(fp, INODE_START + inode_index*INODE_SIZE, SEEK_SET);
			fwrite(&p->disk_block, 1, INODE_SIZE, fp);
		}
		if (t == NULL) {//pΪhash�����ϵ�һ���ڵ�
			hash_head[m].next = p->next;//ǰ��������p�ų�����
            if(p->next!=NULL){
                p->next->front = NULL;
            }
		}
		else {//p�������ϵ�һ���ڵ�
			t->next = p->next;
            if(p->next!=NULL){
                p->next->front = t;
            }
		}
		p->front = NULL;//���ָ���ָ��
		p->next = NULL;
		free(p);
	}
    p = NULL;
	t = NULL;
}

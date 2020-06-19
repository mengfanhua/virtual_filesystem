#include "virtual_system.h"
#include<malloc.h>
#include<string.h>

struct inode* iget(int inode_index) {
	int m = inode_index % HASH_SIZE;//计算hash值
	struct inode *p, *t = NULL;//t用来记录p的前一节点
	for (p = hash_head[m].next; p != NULL; p = p->next) {
		if (p->index == inode_index) {//存在该节点
			break;
		}
		else {
			t = p;//记录当前节点
			continue;
		}
	}
	if (p == NULL) {//没有查询到该节点
		//从文件中调入
		p = (struct inode *)malloc(sizeof(struct inode));
		fseek(fp,INODE_START+inode_index*INODE_SIZE,SEEK_SET);
		fread(&p->disk_block, 1, INODE_SIZE, fp);
		//t为NULL代表hash_head的next为NULL，即当前没有节点
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
	//如果存在节点直接返回就可
	t = NULL;
	return p;
}

void iput(int inode_index) {
	int m = inode_index % HASH_SIZE;//计算hash值
	struct inode *p, *t = NULL;//t用来记录p的前一节点
	for (p = hash_head[m].next; p != NULL; p = p->next) {
		if (p->index == inode_index) {//存在该节点
			break;
		}
		else {
			t = p;//记录当前节点
			continue;
		}
	}
	if (p != NULL) {//找到该节点
		if (p->dirty == 1) {//该节点被修改过
			//将修改写回文件
			fseek(fp, INODE_START + inode_index*INODE_SIZE, SEEK_SET);
			fwrite(&p->disk_block, 1, INODE_SIZE, fp);
		}
		if (t == NULL) {//p为hash链表上第一个节点
			hash_head[m].next = p->next;//前后互连，将p排除在外
            if(p->next!=NULL){
                p->next->front = NULL;
            }
		}
		else {//p不是链上第一个节点
			t->next = p->next;
            if(p->next!=NULL){
                p->next->front = t;
            }
		}
		p->front = NULL;//清除指针的指向
		p->next = NULL;
		free(p);
	}
    p = NULL;
	t = NULL;
}

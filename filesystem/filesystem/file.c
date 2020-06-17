#include "virtual_system.h"
#include<malloc.h>
#include<string.h>

struct system_open sys_open[MAX_SYSTEM_OPEN];
struct superblock superblock;

void close(int sysopen_index) {
	//用户打开文件表在其他部分处理
	sys_open[sysopen_index].count -= 1;
	if (sys_open[sysopen_index].count == 0) {
		sys_open[sysopen_index].inode = NULL;
	}
}

char* _open(int inode_index) {
	//此部分针对已经存在的打开的文件进行处理
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
	//参数为文件的i节点，内容为数据块的块号,用户打开文件表在其他部分处理
	//此部分针对尚未打开过此文件的用户使用
	int j, flag = 0;
	char *s;
	for (j = 0; j < MAX_SYSTEM_OPEN; j++) {
		if (sys_open[j].count == 0) {
			flag = 1;//标志有空闲位置
		}
		else if (sys_open[j].inode->index == inode_index) {
			break;//找到对应打开文件
		}
		else {
			continue;
		}
	}
	if (j != MAX_SYSTEM_OPEN) {//即找到文件
		sys_open[j].count += 1;
		s = _open(inode_index);

	}
	else if (flag == 1) {//没有找到文件，但有空位置允许打开
		for (j = 0; j < MAX_SYSTEM_OPEN; j++) {
			if (sys_open[j].count == 0) {
				break;
			}
			else {
				continue;
			}
		}
		sys_open[j].count += 1;
		sys_open[j].inode = iget(inode_index);//连接i节点
		s = _open(inode_index);
	}
	else {
		s = NULL;//以null判断操作失败
	}
	return s;
}

char * _split_char(char* content, int start, int size) {//简易字符串分割算法
	char *p;
	p = (char *)malloc(sizeof(char)*size+1);
	int i;
	for (i = 0; i < size; i++) {
		p[i] = content[start + i];
	}
	p[size] = '\0';//此处多留出一位，防止在写入文件时由于访存问题产生越界问题
	return p;
}

int save(int inode, char* content) {
	int flag = 0;
	int size = strlen(content);
	int block;
	//能否占满整数个块
	if (size%BLOCK_SIZE != 0) {
		block = (int)(size / BLOCK_SIZE) + 1;
	}
	else {
		block = (int)(size / BLOCK_SIZE);
	}
	struct inode *p;
	p = iget(inode);//获取对应i节点
	if (block > MAX_FILE_NUM) {}//文件超出最大块限制
	else if (p->disk_block.filesize < block) {
		//当前已有文件块不足以存储修改后的字符串
		if (superblock.free_data_num >= block - p->disk_block.filesize) {
			//还有足够的空闲块
			int i;
			for (i = p->disk_block.filesize; i < block; i++) {
				p->disk_block.block_index[i] = balloc();//获取空闲块索引
				p->disk_block.filesize += 1;
				p->disk_block.file_type[i] = 2;//特殊标识，文件块特有
			}
			char *t;
			for (i = 0; i < block; i++) {
				if (i + 1 == block && size%BLOCK_SIZE != 0) {
					//此处判断是否能完全存储，该分支不能
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
		//当前占有块多出
		int i, j = 0;
		for (i = block; i < p->disk_block.filesize; i++) {
			bfree(p->disk_block.block_index[i]);//释放多余块
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
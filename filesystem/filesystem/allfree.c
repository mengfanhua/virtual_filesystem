#include "virtual_system.h"

struct superblock superblock;

int ialloc() {
	int i = MAX_INODE_NUM - 1, flag = -1;
	while (superblock.free_inode[i] == 1 && i >= 0) {
		//寻找空闲i节点
		i -= 1;
	}
	if (i == -1) { }
	else {
		superblock.free_inode[i] = 1;
		superblock.dirty = 1;
		superblock.free_inode_num -= 1;
		flag = superblock.inode_point[i];
	}
	return flag;
}

void ifree(int inode_index) {
	int i = MAX_INODE_NUM - 1;
	while (superblock.inode_point[i] != inode_index) {
		//寻找空闲i节点
		i -= 1;
	}
	superblock.dirty = 1;
	superblock.free_inode[i] == 0;
	superblock.free_inode_num += 1;
}

int balloc() {
	int i = 0, flag = -1;
	while (superblock.data_show[i] == 1 && i < MAX_DATA_NUM) {
		//寻找空闲数据区
		i += 1;
	}
	if (i == MAX_DATA_NUM) {}
	else {
		superblock.data_show[i] = 1;
		superblock.dirty = 1;
		flag = superblock.data_point[i];
		superblock.free_data_num -= 1;
	}
	return flag;
}

void bfree(int data_index) {
	//释放数据区，由于数据区序号与索引相对应，直接简化赋值
	superblock.dirty = 1;
	superblock.data_show[data_index] = 0;
	superblock.free_data_num += 1;
}
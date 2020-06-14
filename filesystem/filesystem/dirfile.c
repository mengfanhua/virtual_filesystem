#include "virtual_system.h"

struct superblock superblock;
struct dir dir[MAX_DIR_NUM];
char uid[6];

int _cul_mode(int type) {//计算默认权限,0为文件，1为文件夹，二者默认权限不同

}

int mkdir(int inode_index, char* dirname) {//创建文件夹
	int flag = 0;
	struct inode *p;
	p = iget(inode_index);//获取父目录节点id
	if (p->disk_block.filesize == MAX_FILE_NUM) {}//该文件夹已到最大值
	else {
		int i = 0, j = 0;
		while (i < p->disk_block.filesize) {
			//注：此处有制约条件，即所有被删除的节点需要将index置为MAX_INODE_NUM，否则无法执行
			if (p->disk_block.block_index[j] != MAX_INODE_NUM) {//即该位存在指向
				if (strcmp(dir[p->disk_block.block_index[j]].name, dirname) == 0) {//判断重名
					break;
				}
				else {
					i += 1;//在存在指向时，计数值加一
				}
			}
			j += 1;//用于全部的循环
		}
		if (i == p->disk_block.filesize) {//证明无重名，有重名会默认flag=0
			i = 0;
			while (i < p->disk_block.filesize) {//寻找最小的空白区
				if (p->disk_block.block_index[i] == MAX_INODE_NUM) {//此处利用了delete函数会置位，否则无法执行
					break;
				}
				else {
					i += 1;
				}
			}
			int index = ialloc();//获取i节点
			if (index != -1) {//空闲i节点存在
				strcpy(dir[index].name, dirname);//置目录对应位
				dir[index].index = index;//置目录对应指向
				p->dirty = 1;//设置父目录为脏，使得关闭系统时写回磁盘
				p->disk_block.filesize += 1;
				p->disk_block.block_index[i] = index;
				p->disk_block.file_type[i] = 1;//标记为文件夹，文件夹与文件打开方式不一样
				p = iget(index);//获取子目录
				p->dirty = 1;//置脏位

				//初始化新的i节点
				p->disk_block.filesize = 0;
				p->disk_block.rw_mode = _cul_mode(1);//计算权限值，默认只有拥有者全部权限
				strcpy(p->disk_block.uid, uid);//标记拥有者
				flag = 1;
			}
			else {//无空闲i节点
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
	p = iget(inode_index);//获取父目录节点id
	if (p->disk_block.filesize == MAX_FILE_NUM) {}//该文件夹已到最大值
	else {
		int i = 0, j = 0;
		while (i < p->disk_block.filesize) {
			//注：此处有制约条件，即所有被删除的节点需要将index置为MAX_INODE_NUM，否则无法执行
			if (p->disk_block.block_index[j] != MAX_INODE_NUM) {//即该位存在指向
				if (strcmp(dir[p->disk_block.block_index[j]].name, dirname) == 0) {//判断重名
					break;
				}
				else {
					i += 1;//在存在指向时，计数值加一
				}
			}
			j += 1;//用于全部的循环
		}
		if (i == p->disk_block.filesize) {//证明无重名，有重名会默认flag=0
			i = 0;
			while (i < p->disk_block.filesize) {//寻找最小的空白区
				if (p->disk_block.block_index[i] == MAX_INODE_NUM) {//此处利用了delete函数会置位，否则无法执行
					break;
				}
				else {
					i += 1;
				}
			}
			int index = ialloc();//获取i节点
			if (index != -1) {//空闲i节点存在
				strcpy(dir[index].name, dirname);//置目录对应位
				dir[index].index = index;//置目录对应指向
				p->dirty = 1;//设置父目录为脏，使得关闭系统时写回磁盘
				p->disk_block.filesize += 1;
				p->disk_block.block_index[i] = index;
				p->disk_block.file_type[i] = 0;//标记为文件，文件夹与文件打开方式不一样
				p = iget(index);//获取子目录
				p->dirty = 1;//置脏位

							 //初始化新的i节点
				p->disk_block.filesize = 0;
				p->disk_block.rw_mode = _cul_mode(0);//计算权限值，默认只有拥有者全部权限
				strcpy(p->disk_block.uid, uid);//标记拥有者
				flag = 1;
			}
			else {//无空闲i节点
				flag = -1;
			}
		}
	}
	p = NULL;
	return flag;
}

int del(int inode_index, int index) {//删除文件夹迭代删除，如果权限不足则不删除
	struct inode *p,*q;
	int i = 0, j = 0;
	int flag = 1;
	p = iget(inode_index);
	q = iget(dir[p->disk_block.block_index[index]].index);
	if (p->disk_block.file_type[index] == 0) {//判断到要删除的是文件
		if (q->disk_block.connect_num != 0 || access(q->disk_block.rw_mode, 3) == 0) {
			//   !=   文件没打开并且该用户有权限删除
			flag = 0;
		}
		else {
			for (i = 0; i < q->disk_block.filesize; i++) {
				bfree(q->disk_block.block_index[i]);//释放数据区
			}
			ifree(q->index);//释放i节点
			iput(q->index);//释放内存i节点
			p->disk_block.filesize -= 1;
			p->disk_block.block_index[index] = MAX_INODE_NUM;//对空白区进行置位
		}
	}
	else {
		while (i < q->disk_block.filesize) {
			if (q->disk_block.block_index[j] == MAX_INODE_NUM) {}//存在之前删除的空位
			else {
				flag = flag & del(q->index, j);//迭代删除且重新计算可删除值
				i += 1;
			}
			j += 1;
		}
		if (access(q->disk_block.rw_mode, 3) == 0 || flag == 0) {//权限不足或文件夹内内容没有全部删除干净
			flag = 0;
		}
		else {
			ifree(q->index);//释放i节点
			iput(q->index);//释放内存i节点
			p->disk_block.filesize -= 1;
			p->disk_block.block_index[index] = MAX_INODE_NUM;//对空白区进行置位
		}
	}
	return flag;
}

int share(int inode_index, int new_inode_index) {

}

int access(int allmode, int mode) {
	
}
#include "virtual_system.h"
#include<string.h>
#include<malloc.h>

struct superblock superblock;
struct user_head uhead;
char id[6];
struct system_open sys_open[MAX_SYSTEM_OPEN];

struct user_open* _create_user(char* uid) {//内部创建函数，创建一个已登录账户的链表项
	struct user_open *p;
	p = (struct user_open *)malloc(sizeof(struct user_open));
	strcpy(p->uid, uid);
	p->next = NULL;
	p->opensize = 0;
	int i;
	for (i = 0; i < MAX_USER_OPEN; i++) {
		p->useropen[i] = MAX_SYSTEM_OPEN;
	}
	return p;
}

int login(char* uid, char* passwd) {
	//登录
	int flag = 0;
	int i;
	for (i = 0; i < superblock.usernum; i++) {
		if (strcmp(superblock.uid[i], uid) == 0) {
			if (strcmp(superblock.passwd[i], passwd) == 0) {
				//找到了对应账户
				if (uhead.num == 0) {//当前已登录用户为0，证明该用户未曾登陆过
					uhead.next = _create_user(uid);
					uhead.num += 1; strcpy(id, uid); flag = 1;
				}
				else {//当前已存在登录的用户
					struct user_open *s,*t=NULL;
					s = uhead.next;
					while (s != NULL) {
						t = s;//为避免两次循环，使用另一指针记录上一节点
						if (strcmp(s->uid, uid) == 0) {//此账户曾登陆过账户，因切换而挂起
							flag = 1; strcpy(id, uid); break;//保持原有账户打开文件表
						}
						else {
							s = s->next;
						}
					}
					if (s == NULL) {//没找到当前用户的登录信息
						if(uhead.num==MAX_USER_EXIST_NUM){
							//当前登陆数已达最大
							flag = -1;
						}
						else {//创建该用户登录信息，创建打开表
							t->next = _create_user(uid);
							uhead.num += 1; flag = 1; strcpy(id, uid);
						}
					}
					s = NULL;
					t = NULL;
				}
			}
			break;
		}
		else {
			continue;
		}
	}
	return flag;
}

void logout() {
	//登出，会关闭已打开得出文件
	struct user_open *p, *t = NULL;
	p = uhead.next;
	while (p != NULL) {
		//此处一定可以搜索到
		if (strcmp(id, p->uid) == 0) {
			break;
		}
		else {
			t = p;
			p = p->next;
		}
	}
	//将p节点从链表中去除
	if (t == NULL) {//此处为当p为第一个节点时
		uhead.next = p->next;
	}
	else {
		t->next = p->next;
	}
	uhead.num -= 1;
	int i = 0, j = 0;//i用来计数，j用来循环
	while (i != p->opensize) {
		if (p->useropen[j] != MAX_SYSTEM_OPEN) {//如果为MAX_SYSTEM_OPEN，则证明这个点未使用
			close(p->useropen[j]);//close会处理系统打开表和节点指针
			i += 1;
		}
		j += 1;
	}
	free(p);//释放p节点，即清除当前用户的登录信息
	strcpy(id, "");
	p = NULL;
	t = NULL;
}

int signup(char* uid, char* passwd) {
	//注册用户
	int flag = 0;
	if (superblock.usernum == USER_NUM) {
		//当前已达到最大用户数，无法继续注册
		flag = -1;
	}
	else {
		int i;
		for (i = 0; i < superblock.usernum; i++) {
			if (strcmp(uid, superblock.uid[i]) == 0) {
				//当前注册uid与已有冲突
				break;
			}
			else {
				continue;
			}
		}
		if (i == superblock.usernum) {
			//未查询到冲突uid则添加新账户信息
			strcpy(superblock.uid[superblock.usernum], uid);
			strcpy(superblock.passwd[superblock.usernum], passwd);
			superblock.usernum += 1;
			superblock.dirty = 1;
			flag = 1;
		}
	}
	return flag;
}

void exchange_admin() {
	//切换账户，不会关闭当前用户已打开的文件
	strcpy(id, "");//仅清除当前用户id即可，其他不需改变，以此完成多用户登录
}
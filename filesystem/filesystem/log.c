#include "virtual_system.h"
#include<string.h>
#include<malloc.h>

struct superblock superblock;
struct user_head uhead;
char id[6];
struct system_open sys_open[MAX_SYSTEM_OPEN];

struct user_open* _create_user(char* uid) {//�ڲ���������������һ���ѵ�¼�˻���������
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
	//��¼
	int flag = 0;
	int i;
	for (i = 0; i < superblock.usernum; i++) {
		if (strcmp(superblock.uid[i], uid) == 0) {
			if (strcmp(superblock.passwd[i], passwd) == 0) {
				//�ҵ��˶�Ӧ�˻�
				if (uhead.num == 0) {//��ǰ�ѵ�¼�û�Ϊ0��֤�����û�δ����½��
					uhead.next = _create_user(uid);
					uhead.num += 1; strcpy(id, uid); flag = 1;
				}
				else {//��ǰ�Ѵ��ڵ�¼���û�
					struct user_open *s,*t=NULL;
					s = uhead.next;
					while (s != NULL) {
						t = s;//Ϊ��������ѭ����ʹ����һָ���¼��һ�ڵ�
						if (strcmp(s->uid, uid) == 0) {//���˻�����½���˻������л�������
							flag = 1; strcpy(id, uid); break;//����ԭ���˻����ļ���
						}
						else {
							s = s->next;
						}
					}
					if (s == NULL) {//û�ҵ���ǰ�û��ĵ�¼��Ϣ
						if(uhead.num==MAX_USER_EXIST_NUM){
							//��ǰ��½���Ѵ����
							flag = -1;
						}
						else {//�������û���¼��Ϣ�������򿪱�
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
	//�ǳ�����ر��Ѵ򿪵ó��ļ�
	struct user_open *p, *t = NULL;
	p = uhead.next;
	while (p != NULL) {
		//�˴�һ������������
		if (strcmp(id, p->uid) == 0) {
			break;
		}
		else {
			t = p;
			p = p->next;
		}
	}
	//��p�ڵ��������ȥ��
	if (t == NULL) {//�˴�Ϊ��pΪ��һ���ڵ�ʱ
		uhead.next = p->next;
	}
	else {
		t->next = p->next;
	}
	uhead.num -= 1;
	int i = 0, j = 0;//i����������j����ѭ��
	while (i != p->opensize) {
		if (p->useropen[j] != MAX_SYSTEM_OPEN) {//���ΪMAX_SYSTEM_OPEN����֤�������δʹ��
			close(p->useropen[j]);//close�ᴦ��ϵͳ�򿪱�ͽڵ�ָ��
			i += 1;
		}
		j += 1;
	}
	free(p);//�ͷ�p�ڵ㣬�������ǰ�û��ĵ�¼��Ϣ
	strcpy(id, "");
	p = NULL;
	t = NULL;
}

int signup(char* uid, char* passwd) {
	//ע���û�
	int flag = 0;
	if (superblock.usernum == USER_NUM) {
		//��ǰ�Ѵﵽ����û������޷�����ע��
		flag = -1;
	}
	else {
		int i;
		for (i = 0; i < superblock.usernum; i++) {
			if (strcmp(uid, superblock.uid[i]) == 0) {
				//��ǰע��uid�����г�ͻ
				break;
			}
			else {
				continue;
			}
		}
		if (i == superblock.usernum) {
			//δ��ѯ����ͻuid��������˻���Ϣ
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
	//�л��˻�������رյ�ǰ�û��Ѵ򿪵��ļ�
	strcpy(id, "");//�������ǰ�û�id���ɣ���������ı䣬�Դ���ɶ��û���¼
}
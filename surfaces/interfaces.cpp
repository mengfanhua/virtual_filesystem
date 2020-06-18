#include "interfaces.h"
extern "C" {
#include "virtual_system.h"
}
struct cur_path curpath;
int new_index;
struct dir_ dir[MAX_DIR_NUM];


int _check_access(QString str) {
	//����ļ��������ļ����Ƿ����Ҫ��
	int flag = 1;
	int i;
	for (i = 0; i<str.size(); i++) {
		char s = str.at(i).toLatin1();//QString��char��ת������
		if ((s >= 48 && s <= 57) || (s >= 65 && s <= 90) || (s >= 97 && s <= 122) || s == 95 || s == 46) {
			//A-Z, a-z, 0-9, _, .
			continue;
		}
		else {
			flag = 0;
			break;
		}
	}
	return flag;
}

int _mkdir(QStringList& param) {
	//�����ļ��е�Ԥ����
	int flag = 0;
	QStringList path = param.at(1).split("/");
	//���༶Ŀ¼���ֿ�
	int i;
	for (i = 0; i < path.size(); i++) {
        if (path.at(i).size() <= MAX_DIR_NAME_SIZE) {
			//�ļ��������㳤������
			continue;
		}
		else {
			break;
		}
	}
	if (i == path.size()) {//�����ļ����������㳤������
		for (i = 0; i<path.size(); i++) {
			if (_check_access(path.at(i)) == 0) {
				//�ļ������зǷ��ַ�
				break;
			}
			else if (path.at(i) == "." || path.at(i) == "..") {
				//�ļ�����ռ�ñ��������ַ�
				break;
			}
			else {
				continue;
			}
		}
		if (i == path.size()) {//�ļ������ϸ�
			int m = curpath.front->inum;//��¼��ǰĿ¼��i�ڵ�ֵ
			int i;
			for (i = 0; i < path.size(); i++) {
				int d = mkdir(m, path.at(i).toUtf8().data());
				if (d == -1 || d == 0) {
					//����i�ڵ���������������
					flag = d - 1;//Ϊ����������flagֵ������
					break;
				}
				m = new_index;//�˴�Ϊmkdir�����ɹ�ʱ��������i�ڵ��ֵ����Ϊȫ�ֱ������Ա�˴�����
			}
            if (i == path.size()) {//���в�����޴���
				flag = 1;
			}
		}
	}
	else {
		//���Ȳ���������
        flag = -3;
	}
	return flag;
}

int _chdir(QStringList& param) {
	//�ı��ļ��У���ʵ���ǳ��õ�cdָ��
	int flag = 0;
	QStringList path = param.at(1).split("/");
	//����·��
	int i;
	for (i = 0; i<path.size(); i++) {
		if (_check_access(path.at(i)) == 0) {
			//���Ƿ��ַ�
			break;
		}
		else if ((path.at(i) == "." && i != 0) || (path.at(i) == ".." && i != 0)) {
			//�ж��Ƿ�ռ�ñ����֣���λ����
			break;
		}
		else {
			continue;
		}
	}
	if (i == path.size()) {//�ַ��������
		int m;
		if (path.at(0) == ".") {//�ӵ�ǰĿ¼��ʼѰ��
			m = curpath.front->inum;
		}
		else if (path.at(0) == "..") {//�ӵ�ǰĿ¼����һ��Ŀ¼��ʼ
			m = curpath.front->front->inum;
		}
		else if (path.at(0) == "") {//�Ӹ�Ŀ¼��ʼ
			m = 0;
		}
		else {//�ӵ�ǰĿ¼����Ŀ¼��ʼ
			m = exist_dir(curpath.front->inum, path.at(0).toUtf8().data());
		}
		int i, n = m;
		for (i = 1; i < path.size(); i++) {
			//�ж��Ƿ���ڸ�·��
			if (n != -1) {
				n = exist_dir(n, path.at(i).toUtf8().data());
			}
			else {
				break;
			}
		}
		if (i == path.size() && n != -1) {//ȷʵ���ڸ�·��
			struct cur_path *p;
			if (path.at(0) == "..") {//ȥ����ǰ·��
				p = curpath.front;
				if (p != &curpath) {
					//�жϵ�ǰ�Ƿ�Ϊ��Ŀ¼�������ٽ��з�����һ��
					p->front->next = p->next;//����˫������ָ��
					curpath.front = p->front;
					free(p);
				}
			}
			else if (path.at(0) == "") {//ȥ������Ŀ¼������·��
				p = curpath.front;
				while (p != &curpath) {
					//����Ŀ¼ָ���Լ�ʱ��Ϊ����
					p->front->next = p->next;
					curpath.front = p->front;
					free(p);
					p = curpath.front;
				}
			}
			else if (path.at(0) != ".") {//����Ŀ¼��ʼѰ�ҵ���Ҫ�����һ���ڵ�
				p = (struct cur_path *)malloc(sizeof(struct cur_path));
				p->inum = m;
				p->front = curpath.front;
				curpath.front->next = p;
				p->next = &curpath;
				curpath.front = p;
			}
			for (i = 1; i < path.size(); i++) {
				//ѭ�������ṹ������
				p = (struct cur_path *)malloc(sizeof(struct cur_path));
				m = exist_dir(m, path.at(i).toUtf8().data());
				p->inum = m;
				p->front = curpath.front;
				curpath.front->next = p;
				p->next = &curpath;
				curpath.front = p;
			}
			flag = 1;
			p = NULL;
		}
		else {
			//�����ڸ�·��
			flag = -1;
		}
	}
	return flag;
}

QString _dir() {
	//��ʽ������ļ�Ŀ¼������,�������Ϳ��������⣬��Ҫ����
	QString temp = "file         | type\n-------------------\n";
	QString type1 = "| file";
	QString type2 = "| dir";
	QString type3 = "| fast";
	struct inode *p;
	p = iget(curpath.front->inum);//��ȡ��ǰ�ڵ�
	int i = 0, j = 0, k;
	while (i < p->disk_block.filesize) {
		if (p->disk_block.block_index[j] != MAX_FILE_NUM) {//��ռλ����ɾ�������
			QString te = QString(QLatin1String(dir[p->disk_block.block_index[j]].name));//char * תQString
			for (k = te.size(); k < 13; k++) {
				te = te + " ";//��ʽ��
			}
			temp = temp + te;//����ǰ�벿��
			switch (p->disk_block.file_type[j]) {
				//�ж��ļ���ʽ���ļ����ļ��У���ݷ�ʽ
			case 0:temp = temp + type1 + "\n"; break;
			case 1:temp = temp + type2 + "\n"; break;
			case 3:temp = temp + type3 + "\n"; break;
			default:break;
			}
			i += 1;
		}
		j += 1;
	}
	temp = temp + "\n";//β������һ������
	p = NULL;
	return temp;
}

int _create(QStringList& param) {
	int flag = 0;
    if (param.at(1).size() > MAX_DIR_NAME_SIZE) {
		//�ļ�����������
		flag = -3;
	}
	else if (_check_access(param.at(1)) == 0) {
		//�ļ����зǷ��ַ�
		flag = -2;
	}
	else {
		//���ú�̨create����
		flag = create(curpath.front->inum, param.at(1).toUtf8().data());
	}
	return flag;
}

int _del(QStringList& param, int x) {//ɾ���ļ������ɾ���ļ���
	int flag = 0;
	int i = 0, j = 0;
	struct inode *p;
	p = iget(curpath.front->inum);
	while (i < p->disk_block.filesize) {
		if (p->disk_block.block_index[j] != MAX_FILE_NUM) {//��ռλ����ɾ�������
            if (strcmp(dir[p->disk_block.block_index[j]].name, param.at(1).toUtf8().data()) == 0) {
				if (p->disk_block.file_type[j] == x) {//�ж������Ƿ���ȷ
					flag = del(curpath.front->inum, j);
				}
				else if (x == 0 && p->disk_block.file_type[j] == 3) {//�ļ����⴦����Ϊ�п�ݷ�ʽ
					flag = del(curpath.front->inum, j);
				}
				else {
					flag = -1;//�����жϴ���
				}
				break;
			}
			i += 1;
		}
		j += 1;
	}
	p = NULL;
	return flag;
}

int _delete(QStringList& param) {
	return _del(param, 0);
}

int _rm(QStringList& param) {
	return _del(param, 1);
}

int _open(QStringList& param) {
	int flag = 0;
	int i = 0, j = 0;
	struct inode *p;
	p = iget(curpath.front->inum);
	while (i < p->disk_block.filesize) {
		if (p->disk_block.block_index[j] != MAX_FILE_NUM) {//��ռλ����ɾ�������
            if (strcmp(dir[p->disk_block.block_index[j]].name, param.at(1).toUtf8().data()) == 0) {
				if (p->disk_block.file_type[j] == 0) {//�ж������Ƿ���ȷ
					flag = dir[p->disk_block.block_index[j]].index;
				}
				else if (p->disk_block.file_type[j] == 3) {//�ļ����⴦����Ϊ�п�ݷ�ʽ
					flag = dir[p->disk_block.block_index[j]].index;
				}
				else {
					flag = -1;//�����жϴ���
				}
				break;
			}
			i += 1;
		}
		j += 1;
	}
	p = NULL;
	return flag;
}

int _ls(QStringList& param) {
	int flag = 0, flag1 = 0;
	int i = 0, j = 0;
	struct inode *p;
	p = iget(curpath.front->inum);
	while (i < p->disk_block.filesize) {
		if (p->disk_block.block_index[j] != MAX_FILE_NUM) {//��ռλ����ɾ�������
            if (strcmp(dir[p->disk_block.block_index[j]].name, param.at(1).toUtf8().data()) == 0) {
				if (p->disk_block.file_type[j] == 0) {//�ж������Ƿ���ȷ
					flag = dir[p->disk_block.block_index[j]].index;
				}
				else if (p->disk_block.file_type[j] == 3) {//�ļ����⴦����Ϊ�п�ݷ�ʽ
					flag = dir[p->disk_block.block_index[j]].index;
				}
				else {
					flag = -1;//�����жϴ���
				}
				break;
			}
			i += 1;
		}
		j += 1;
	}
	if (flag != -1 && flag != 0) {//�ļ�����
		QStringList path = param.at(2).split("/");
		//����·��
		for (i = 0; i<path.size(); i++) {
			if (_check_access(path.at(i)) == 0) {
				//���Ƿ��ַ�
				break;
			}
			else if ((path.at(i) == "." && i != 0) || (path.at(i) == ".." && i != 0)) {
				//�ж��Ƿ�ռ�ñ����֣���λ����
				break;
			}
			else {
				continue;
			}
		}
		if (i == path.size()) {//�ַ��������
			int m;
			if (path.at(0) == ".") {//�ӵ�ǰĿ¼��ʼѰ��
				m = curpath.front->inum;
			}
			else if (path.at(0) == "..") {//�ӵ�ǰĿ¼����һ��Ŀ¼��ʼ
				m = curpath.front->front->inum;
			}
			else if (path.at(0) == "") {//�Ӹ�Ŀ¼��ʼ
				m = 0;
			}
			else {//�ӵ�ǰĿ¼����Ŀ¼��ʼ
				m = exist_dir(curpath.front->inum, path.at(0).toUtf8().data());
			}
			int i, n = m;
			for (i = 1; i < path.size(); i++) {
				//�ж��Ƿ���ڸ�·��
				if (n != -1) {
					n = exist_dir(n, path.at(i).toUtf8().data());
				}
				else {
					break;
				}
			}
			if (i == path.size() && n != -1) {//·������
				flag1 = n;//��¼·����i�ڵ�ֵ
			}
		}
		if (flag1 != 0) {
			flag = share(flag, flag1) + 2;//���Ϸ�����flag���ֿ���
		}
	}
	return flag;
}

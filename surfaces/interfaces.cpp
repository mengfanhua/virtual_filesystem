#include "interfaces.h"
#include "virtual_system.h"



int _check_access(QString str) {
	//检测文件夹名或文件名是否符合要求
	int flag = 1;
	int i;
	for (i = 0; i<str.size(); i++) {
		char s = str.at(i).toLatin1();//QString到char的转换函数
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
	//创建文件夹的预处理
	int flag = 0;
	QStringList path = param.at(1).split("/");
	//将多级目录划分开
	int i;
	for (i = 0; i < path.size(); i++) {
        if (path.at(i).size() <= MAX_DIR_NAME_SIZE) {
			//文件夹名满足长度限制
			continue;
		}
		else {
			break;
		}
	}
	if (i == path.size()) {//所有文件夹名都满足长度限制
		for (i = 0; i<path.size(); i++) {
			if (_check_access(path.at(i)) == 0) {
				//文件夹名有非法字符
				break;
			}
			else if (path.at(i) == "." || path.at(i) == "..") {
				//文件夹名占用保留处理字符
				break;
			}
			else {
				continue;
			}
		}
		if (i == path.size()) {//文件夹名合格
			int m = curpath.front->inum;//记录当前目录的i节点值
			int i;
			for (i = 0; i < path.size(); i++) {
				int d = mkdir(m, path.at(i).toUtf8().data());
				if (d == -1 || d == 0) {
					//分配i节点错误或有重名问题
					flag = d - 1;//为了与其他的flag值区分来
					break;
				}
				m = new_index;//此处为mkdir函数成功时将创建的i节点的值保留为全局变量，以便此处调用
			}
            if (i == path.size()) {//所有步骤均无错误
				flag = 1;
			}
		}
	}
	else {
		//长度不满足限制
        flag = -3;
	}
	return flag;
}

int _chdir(QStringList& param) {
	//改变文件夹，其实就是常用的cd指令
	int flag = 0;
	QStringList path = param.at(1).split("/");
	//划分路径
	int i;
	for (i = 0; i<path.size(); i++) {
		if (_check_access(path.at(i)) == 0) {
			//检查非法字符
			break;
		}
		else if ((path.at(i) == "." && i != 0) || (path.at(i) == ".." && i != 0)) {
			//判断是否占用保留字，首位除外
			break;
		}
		else {
			continue;
		}
	}
	if (i == path.size()) {//字符检验完成
		int m;
		if (path.at(0) == ".") {//从当前目录开始寻找
			m = curpath.front->inum;
		}
		else if (path.at(0) == "..") {//从当前目录的上一级目录开始
			m = curpath.front->front->inum;
		}
		else if (path.at(0) == "") {//从根目录开始
			m = 0;
		}
		else {//从当前目录的子目录开始
			m = exist_dir(curpath.front->inum, path.at(0).toUtf8().data());
		}
		int i, n = m;
		for (i = 1; i < path.size(); i++) {
			//判断是否存在该路径
			if (n != -1) {
				n = exist_dir(n, path.at(i).toUtf8().data());
			}
			else {
				break;
			}
		}
		if (i == path.size() && n != -1) {//确实存在该路径
			struct cur_path *p;
			if (path.at(0) == "..") {//去掉当前路径
				p = curpath.front;
				if (p != &curpath) {
					//判断当前是否为根目录，是则不再进行返回上一级
					p->front->next = p->next;//处理双向链表指针
					curpath.front = p->front;
					free(p);
				}
			}
			else if (path.at(0) == "") {//去掉除根目录外所有路径
				p = curpath.front;
				while (p != &curpath) {
					//当根目录指向自己时即为结束
					p->front->next = p->next;
					curpath.front = p->front;
					free(p);
					p = curpath.front;
				}
			}
			else if (path.at(0) != ".") {//从子目录开始寻找的需要先添加一个节点
				p = (struct cur_path *)malloc(sizeof(struct cur_path));
				p->inum = m;
				p->front = curpath.front;
				curpath.front->next = p;
				p->next = &curpath;
				curpath.front = p;
			}
			for (i = 1; i < path.size(); i++) {
				//循环创建结构并连接
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
			//不存在该路径
			flag = -1;
		}
	}
	return flag;
}

QString _dir() {
	//格式化输出文件目录的内容,返回类型可能有问题，需要测试
	QString temp = "file         | type\n-------------------\n";
	QString type1 = "| file";
	QString type2 = "| dir";
	QString type3 = "| fast";
	struct inode *p;
	p = iget(curpath.front->inum);//获取当前节点
	int i = 0, j = 0, k;
	while (i < p->disk_block.filesize) {
		if (p->disk_block.block_index[j] != MAX_FILE_NUM) {//非占位数，删除引起的
			QString te = QString(QLatin1String(dir[p->disk_block.block_index[j]].name));//char * 转QString
			for (k = te.size(); k < 13; k++) {
				te = te + " ";//格式化
			}
			temp = temp + te;//连接前半部分
			switch (p->disk_block.file_type[j]) {
				//判断文件格式，文件，文件夹，快捷方式
			case 0:temp = temp + type1 + "\n"; break;
			case 1:temp = temp + type2 + "\n"; break;
			case 3:temp = temp + type3 + "\n"; break;
			default:break;
			}
			i += 1;
		}
		j += 1;
	}
	temp = temp + "\n";//尾部多留一个换行
	p = NULL;
	return temp;
}

int _create(QStringList& param) {
	int flag = 0;
    if (param.at(1).size() > MAX_DIR_NAME_SIZE) {
		//文件名超出限制
		flag = -3;
	}
	else if (_check_access(param.at(1)) == 0) {
		//文件名有非法字符
		flag = -2;
	}
	else {
		//调用后台create函数
        QByteArray s = param.at(1).toUtf8();
        char *ss = s.data();
        flag = create(curpath.front->inum, ss);
	}
	return flag;
}

int _del(QStringList& param, int x) {//删除文件或迭代删除文件夹
	int flag = 0;
	int i = 0, j = 0;
	struct inode *p;
	p = iget(curpath.front->inum);
	while (i < p->disk_block.filesize) {
		if (p->disk_block.block_index[j] != MAX_FILE_NUM) {//非占位数，删除引起的
            if (strcmp(dir[p->disk_block.block_index[j]].name, param.at(1).toUtf8().data()) == 0) {
				if (p->disk_block.file_type[j] == x) {//判断类型是否正确
					flag = del(curpath.front->inum, j);
				}
				else if (x == 0 && p->disk_block.file_type[j] == 3) {//文件特殊处理，因为有快捷方式
					flag = del(curpath.front->inum, j);
				}
				else {
					flag = -1;//类型判断错误
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

int _open_file(QStringList& param) {
	int flag = 0;
	int i = 0, j = 0;
	struct inode *p;
	p = iget(curpath.front->inum);
	while (i < p->disk_block.filesize) {
		if (p->disk_block.block_index[j] != MAX_FILE_NUM) {//非占位数，删除引起的
            if (strcmp(dir[p->disk_block.block_index[j]].name, param.at(1).toUtf8().data()) == 0) {
				if (p->disk_block.file_type[j] == 0) {//判断类型是否正确
					flag = dir[p->disk_block.block_index[j]].index;
				}
				else if (p->disk_block.file_type[j] == 3) {//文件特殊处理，因为有快捷方式
					flag = dir[p->disk_block.block_index[j]].index;
				}
				else {
					flag = -1;//类型判断错误
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
		if (p->disk_block.block_index[j] != MAX_FILE_NUM) {//非占位数，删除引起的
            if (strcmp(dir[p->disk_block.block_index[j]].name, param.at(1).toUtf8().data()) == 0) {
				if (p->disk_block.file_type[j] == 0) {//判断类型是否正确
					flag = dir[p->disk_block.block_index[j]].index;
				}
				else if (p->disk_block.file_type[j] == 3) {//文件特殊处理，因为有快捷方式
					flag = dir[p->disk_block.block_index[j]].index;
				}
				else {
					flag = -1;//类型判断错误
				}
				break;
			}
			i += 1;
		}
		j += 1;
	}
	if (flag != -1 && flag != 0) {//文件存在
		QStringList path = param.at(2).split("/");
		//划分路径
		for (i = 0; i<path.size(); i++) {
			if (_check_access(path.at(i)) == 0) {
				//检查非法字符
				break;
			}
			else if ((path.at(i) == "." && i != 0) || (path.at(i) == ".." && i != 0)) {
				//判断是否占用保留字，首位除外
				break;
			}
			else {
				continue;
			}
		}
		if (i == path.size()) {//字符检验完成
			int m;
			if (path.at(0) == ".") {//从当前目录开始寻找
				m = curpath.front->inum;
			}
			else if (path.at(0) == "..") {//从当前目录的上一级目录开始
				m = curpath.front->front->inum;
			}
			else if (path.at(0) == "") {//从根目录开始
				m = 0;
			}
			else {//从当前目录的子目录开始
				m = exist_dir(curpath.front->inum, path.at(0).toUtf8().data());
			}
			int i, n = m;
			for (i = 1; i < path.size(); i++) {
				//判断是否存在该路径
				if (n != -1) {
					n = exist_dir(n, path.at(i).toUtf8().data());
				}
				else {
					break;
				}
			}
			if (i == path.size() && n != -1) {//路径存在
				flag1 = n;//记录路径的i节点值
			}
		}
		if (flag1 != 0) {
			flag = share(flag, flag1) + 2;//区上方两个flag区分开来
		}
	}
	return flag;
}

int _init(){
    return init();
}

void _exit_sys(){
    exit_sys();
}

void _clear_cur_path(){
    //清除除根节点外所有的路径节点
    struct cur_path *p;
    p = curpath.front;
    while(curpath.next != &curpath){
        p->front->next = &curpath;
        curpath.front = p->front;
        p->front = NULL;
        p->next = NULL;
        free(p);
        p = curpath.front;
    }
    p = NULL;
}

int _login(QString u,QString p){
    int flag = login(u.toUtf8().data(),p.toUtf8().data());
    if(flag == 1){
        //登录后从根节点开始
        _clear_cur_path();
    }
    return flag;
}

int _signup(QString u,QString p){
    return signup(u.toUtf8().data(),p.toUtf8().data());
}

void _logout(){
    logout();
}
void _exchange_admin(){
    exchange_admin();
}

QString get_cur_path(){
    //获取当前路径
    QString s;
    s = s + QString(QLatin1String(id))+" @ "+QString(QLatin1String(dir[curpath.inum].name));
    struct cur_path *p;
    p = curpath.next;
    while(p!=&curpath){
        s = s + "/" + QString(QLatin1String(dir[p->inum].name));
        p=p->next;
    }
    s = s+ " $";
    return s;
}

int get_rwmode(int i){
    struct inode *p;
    int rw = 0;
    //判定文件权限
    p = iget(i);
    if(access(p->disk_block.rw_mode,1)!=0){
        rw = rw + 1;
        if(access(p->disk_block.rw_mode,2)!=0){
            rw = rw + 2;
        }
    }
    return rw;
}

char* _open_file_text(int a){
    char* s;
    struct user_open *p;
    p = uhead.next;
    while(p!=NULL){
        if(strcmp(p->uid, id)==0){
            break;
        }
        else{
            p=p->next;
        }
    }
    if(p->opensize == MAX_USER_OPEN){
        s = NULL;
        return s;
    }
    int i=0,j=0;
    while(i<p->opensize){
        if(p->useropen[j]!=MAX_SYSTEM_OPEN){
            if(sys_open[p->useropen[j]].inode->index == a){
                break;
            }
            else{
                i+=1;
            }
        }
        j+=1;
    }
    char *k;
    if(i == p->opensize){
        k = open(a);
        if(k==NULL){
            return k;
        }
        i=0;
        j=0;
        while(i<p->opensize){
            if(p->useropen[j]==MAX_SYSTEM_OPEN){
                break;
            }
            else{
                i+=1;
            }
            j+=1;
        }
        p->opensize+=1;
        p->useropen[i]=new_index;
        return k;
    }
    else{
        k = _open(a);
        return k;
    }
}

void _close(int a){
    struct user_open *p;
    p = uhead.next;
    while(p!=NULL){
        if(strcmp(p->uid, id)==0){
            break;
        }
        else{
            p=p->next;
        }
    }
    int i=0,j=0;
    while(i<p->opensize){
        if(p->useropen[j]!=MAX_SYSTEM_OPEN){
            if(sys_open[p->useropen[j]].inode->index == a){
                break;
            }
            else{
                i+=1;
            }
        }
        j+=1;
    }
    close(p->useropen[j]);
    p->useropen[j] = MAX_SYSTEM_OPEN;
    p->opensize-=1;
}

int _save(int a, QString content){
    char *s;
    QByteArray t = content.toLatin1();
    s = t.data();
    int flag = save(a, s);
    return flag;
}

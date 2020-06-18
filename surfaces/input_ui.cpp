#include "input_ui.h"
#include <QScrollBar>
#include <QStringList>
#include <QString>
#include <QLayoutItem>
#include "virtual_system.h"
#include <malloc.h>

struct cur_path curpath;
int new_index;

input_ui::input_ui(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    this->setFixedSize(700, 700);
    one =new QGridLayout;
    this->setLayout(one);

    bottom = new QScrollArea;
    bottomContent = new QWidget;
    contentLayout = new QVBoxLayout;
    bottom->setWidget(bottomContent);
    bottom->setWidgetResizable(true);
    bottomContent->setLayout(contentLayout);
    contentLayout->setAlignment(Qt::AlignTop);

    one->addWidget(bottom,0,0);

    path = new QLabel;
    input = new QLineEdit;
    input->setFixedWidth(650);
    contentLayout->addWidget(path);
    contentLayout->addWidget(input);
    connect(input,SIGNAL(returnPressed()),this,SLOT(solve_input()));
    connect(bottom->verticalScrollBar(), &QAbstractSlider::rangeChanged, this, &input_ui::slider);
    showResult = NULL;
}

input_ui::~input_ui()
{
}

int _check_access(QString str){
    int flag = 1;
    int i;
    for(i=0;i<str.size();i++){
        char s = str.at(i).toLatin1();
        if((s>=48&&s<=57)||(s>=65&&s<=90)||(s>=97&&s<=122)||s==95||s==46){
            continue;
        }
        else{
            flag=0;
            break;
        }
    }
    return flag;
}

int _mkdir(QStringList& param){
    int flag = 0;
    QStringList path = param.at(1).split("/");
    int i;
	for (i = 0; i < path.size(); i++) {
		if (path.at(i).size <= MAX_DIR_NAME_SIZE) {
			continue;
		}
		else {
			break;
		}
	}
	if (i == path.size()) {
		for (i = 0; i<path.size(); i++) {
			if (_check_access(path.at(i)) == 0) {
				break;
			}
			else if (path.at(i) == "." || path.at(i) == "..") {
				break;
			}
			else {
				continue;
			}
		}
		if (i == path.size()) {
			int m = curpath.front->inum;
			int i;
			for (i = 0; i < path.size(); i++) {
				int d = mkdir(m, path.at(i).toUtf8().data());
				if (d == -1 || d == 0) {
					flag = d - 1;
					break;
				}
				m = new_index;
			}
			if (i == path.size) {
				flag = 1;
			}
		}
	}
	else {
		flag == -3;
	}
    return flag;
}

int _chdir(QStringList& param) {
	int flag = 0;
	QStringList path = param.at(1).split("/");
	int i;
	for (i = 0; i<path.size(); i++) {
		if (_check_access(path.at(i)) == 0) {
			break;
		}
		else if ((path.at(i) == "." && i != 0) || (path.at(i) == ".." && i != 0)) {
			break;
		}
		else {
			continue;
		}
	}
	if (i == path.size()) {
		int m;
		if (path.at(0) == ".") {
			m = curpath.front->inum;
		}
		else if (path.at(0) == "..") {
			m = curpath.front->front->inum;
		}
		else {
			m = exist_dir(curpath.front->inum, path.at(0).toUtf8().data());
		}
		int i,n = m;
		for (i = 1; i < path.size(); i++) {
			if (n != -1) {
				n = exist_dir(n, path.at(i).toUtf8().data());
			}
			else {
				break;
			}
		}
		if (i == path.size()) {
			struct cur_path *p;
			if (path.at(0) == "..") {
				p = curpath.front;
				if (p != &curpath) {
					p->front->next = p->next;
					curpath.front = p->front;
					free(p);
				}
			}
			else if (path.at(0) != ".") {
				p = (struct cur_path *)malloc(sizeof(struct cur_path));
				p->inum = m;
				p->front = curpath.front;
				curpath.front->next = p;
				p->next = &curpath;
				curpath.front = p;
			}
			for (i = 1; i < path.size(); i++) {
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
			flag = -1;
		}
	}
	return flag;
}

QString _dir() {
	QString temp = "file         | type\n-------------------\n";
	QString type1 = "| file";
	QString type2 = "| dir";
	QString type3 = "| fast";
	struct inode *p;
	p = iget(curpath.front->inum);
	int i = 0, j = 0, k;
	while (i < p->disk_block.filesize) {
		if (p->disk_block.block_index[j] != MAX_FILE_NUM) {
			QString te = QString(QLatin1String(dir[p->disk_block.block_index[j]].name));
			for (k = te.size(); k < 13; k++) {
				te = te + " ";
			}
			temp = temp + te;
			switch (p->disk_block.file_type[j]) {
			case 0:temp = temp + type1 + "\n"; break;
			case 1:temp = temp + type2 + "\n"; break;
			case 3:temp = temp + type3 + "\n"; break;
			default:break;
			}
			i += 1;
		}
		j += 1;
	}
	temp = temp + "\n";
	p = NULL;
	return temp;
}

int _create(QStringList& param) {
	int flag = 0;
	if (param.at(1).size > MAX_DIR_NAME_SIZE) {
		flag = -3;
	}
	else if (_check_access(param.at(1)) == 0) {
		flag = -2;
	}
	else {
		flag = create(curpath.front->inum, param.at(1).toUtf8().data());
	}
	return flag;
}

void input_ui::solve_input(){
    QString instr = input->text();
    QStringList param = instr.split(" ");
    int i=param.size()-1;
    for(;i>=0;i--){
        if(param.at(i) == ""){
            param.removeAt(i);
        }
    }
    QString response = "";
    if(param.size() == 0){ }
    else if(param.at(0) == "login" && param.size() == 1){
        emit log(0);
    }
    else if(param.at(0) == "logout" && param.size() == 1){
        emit log(0);
    }
    else if(param.at(0) == "signup" && param.size() == 1){
        emit log(1);
    }
    else if(param.at(0) == "chuser" && param.size() == 1){
        emit log(0);
    }
    else if(param.at(0) == "mkdir" && param.size() == 2){
        int flag = _mkdir(param);
		if (flag == 0) {
			response = "ERROR: The path contains illegal characters.\n\n";
		}
		else if (flag == -1) {
			response = "ERROR: The path already exists.\n\n";
		}
		else if (flag == -3) {
			response = "ERROR: Directory length exceeds limit.\n\n";
		}
		else if (flag == -2) {
			response = "ERROR: Inodes are insufficient.\n\n";
		}
		else {
			response = "Done.\n\n";
		}
    }
    else if(param.at(0) == "chdir" && param.size() == 2){
		int flag = _chdir(param);
		if (flag == 0) {
			response = "ERROR: The path contains illegal characters.\n\n";
		}
		else if (flag == -1) {
			response = "ERROR: The path does not exist.\n\n";
		}
		else {
			response = "Done.\n\n";
		}
    }
    else if(param.at(0) == "dir" && param.size() == 1){
		response = _dir();
    }
    else if(param.at(0) == "create" && param.size() == 2){
		int flag = _create(param);
		if (flag == -2) {
			response = "ERROR: The path contains illegal characters.\n\n";
		}
		else if (flag == 0) {
			response = "ERROR: The path already exists.\n\n";
		}
		else if (flag == -3) {
			response = "ERROR: Directory length exceeds limit.\n\n";
		}
		else if (flag == -1) {
			response = "ERROR: Inodes are insufficient.\n\n";
		}
		else {
			response = "Done.\n\n";
		}
    }
    else if(param.at(0) == "open" && param.size() == 2){

    }
    else if(param.at(0) == "delete" && param.size() == 2){

    }
    else if(param.at(0) == "rm" && param.size() == 2){

    }
    else if(param.at(0) == "exit" && param.size() == 1){
        this->close();
    }
    else if(param.at(0) == "cls" && param.size() == 1){
        QLayoutItem *child;
         while ((child = contentLayout->takeAt(0)) != 0){
            if(child->widget()){
                child->widget()->setParent(NULL);
            }

            delete child;
         }
		 response = "Done.\n\n";
    }
	else if (param.at(0) == "ls" && param.size() == 3) {

	}
    else{
        response = "ERROR: Unrecognized instructions.\n\n";
    }
    path = new QLabel;
    input->setEnabled(false);
    showResult = new QLabel;
    showResult->setEnabled(false);
    showResult->setText(response);
    input = new QLineEdit;
    input->setFixedWidth(650);
    showResult->setFixedWidth(650);
    contentLayout->addWidget(showResult);
    contentLayout->addWidget(path);
    contentLayout->addWidget(input);
    connect(input,SIGNAL(returnPressed()),this,SLOT(solve_input()));
    input->setFocus();
}

void input_ui::slider(){
    bottom->verticalScrollBar()->setValue(bottom->verticalScrollBar()->maximum());
}

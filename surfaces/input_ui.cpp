#include "input_ui.h"
#include <QScrollBar>
#include <QStringList>
#include <QString>
#include <QLayoutItem>
#include "interfaces.h"

input_ui::input_ui(QWidget *parent)
    : QWidget(parent)
{
	//禁用关闭键，使得可以自己设置关闭方法
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    this->setFixedSize(700, 700);
	//主界面布局，如果需要添加button就在此更改
    one =new QGridLayout;
    this->setLayout(one);

	//设置滚动条区，用于动态显示下一个命令提示符输入区
    bottom = new QScrollArea;
	//滚动条区需要内置Widget
    bottomContent = new QWidget;
    contentLayout = new QVBoxLayout;
    bottom->setWidget(bottomContent);
    bottom->setWidgetResizable(true);//设置内部Widget可拉伸，否则动态修改将失效
    bottomContent->setLayout(contentLayout);
    contentLayout->setAlignment(Qt::AlignTop);//设置纵向布局的起点是上部，由上至下拓展

    one->addWidget(bottom,0,0);//后面的参数是行，列

	//此处的代码其实可以省略，然后在该界面被触发时，利用槽函数初始化
    path = NULL;//用于显示当前路径
    input = NULL;//用于输入命令
	//滚动条自动向下槽函数
    connect(bottom->verticalScrollBar(), &QAbstractSlider::rangeChanged, this, &input_ui::slider);
    showResult = NULL;
}

input_ui::~input_ui()
{
}

void input_ui::solve_input(){//换行触发的槽函数
    QString instr = input->text();//获取当前内容
    QStringList param = instr.split(" ");//空格划分
    int i=param.size()-1;//此处注意，要倒序删除空串，以免出现越界情况，且i为int型，不要用对应的size_t类型，无负数会越界
    for(;i>=0;i--){
        if(param.at(i) == ""){
			//循环去除空串
            param.removeAt(i);
        }
    }
    QString response = "";//用于显示结果
    if(param.size() == 0){ }//用户仅输入了回车或仅有空格
    else if(param.at(0) == "logout" && param.size() == 1){
		//登出，会关闭所有该用户打开的文件
        _logout();
        this->hide();
        emit log(0);
    }
    else if(param.at(0) == "signup" && param.size() == 1){
		//注册
        this->hide();
        response = "Done.\n\n";
        up(response);
        emit log(1);
    }
    else if(param.at(0) == "chuser" && param.size() == 1){
		//切换账户，不会关闭当前用户打开的文件，以此实现同步共享
        _exchange_admin();
        this->hide();
        emit log(0);
    }
    else if(param.at(0) == "mkdir" && param.size() == 2){
		//创建文件夹，或多层文件夹，但不能使用/, ./, ../的方式，只能为: 新文件夹名/
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
        up(response);
    }
    else if(param.at(0) == "chdir" && param.size() == 2){
		//切换路径，可多级目录，支持 ./   ../  .  ..  文件夹名/    文件夹
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
        up(response);
    }
    else if(param.at(0) == "dir" && param.size() == 1){
		response = _dir();
        up(response);
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
        up(response);
    }
    else if(param.at(0) == "open" && param.size() == 2){
        int flag = _open_file(param);
		if (flag == -1) {
			response = "ERROR: Incorrect file type.\n\n";
            up(response);
		}
		else if (flag == 0) {
			response = "ERROR: File does not exist.\n\n";
            up(response);
		}
		else {
			response = "Done.\n\n";
            up(response);
			this->hide();
			emit fileopen(flag);
		}
    }
    else if(param.at(0) == "delete" && param.size() == 2){
		int flag = _delete(param);
		if (flag == -1) {
			response = "ERROR: Incorrect file type.\n\n";
		}
		else if (flag == 0) {
			response = "ERROR: No operation authority or file does not exist.\n\n";
		}
		else {
			response = "Done.\n\n";
		}
        up(response);
    }
    else if(param.at(0) == "rm" && param.size() == 2){
		int flag = _rm(param);
		if (flag == -1) {
			response = "ERROR: Incorrect dir type.\n\n";
		}
		else if (flag == 0) {
			response = "DONE.\nWARNING: Some files are kept without permission or dir does not exist.\n\n";
		}
		else {
			response = "Done.\n\n";
		}
        up(response);
    }
    else if(param.at(0) == "exit" && param.size() == 1){
        _exit_sys();
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
         up(response);
    }
	else if (param.at(0) == "ls" && param.size() == 3) {
		int flag = _ls(param);
		if (flag == -1) {
			response = "ERROR: Incorrect file type.\n\n";
		}
		else if (flag == 0) {
			response = "ERROR: file does not exist.\n\n";
		}
		else if (flag == 1) {
			response = "ERROR: Insufficient number of inodes.\n\n";
		}
		else if (flag == 2) {
			response = "ERROR: Closed loop.\n\n";
		}
		else {
			response = "Done.\n\n";
		}
        up(response);
	}
    else{
        response = "ERROR: Unrecognized instructions.\n\n";
        up(response);
    }
}

void input_ui::slider(){
    bottom->verticalScrollBar()->setValue(bottom->verticalScrollBar()->maximum());
}

void input_ui::back(){
    this->show();
}

void input_ui::comein(){
    //登录槽函数
    QLayoutItem *child;
     while ((child = contentLayout->takeAt(0)) != 0){
        if(child->widget()){
            child->widget()->setParent(NULL);
        }

        delete child;
     }
     path = new QLabel;
     QString s = get_cur_path();
     path->setText(s);
     input = new QLineEdit;
     input->setFixedWidth(650);
     contentLayout->addWidget(path);
     contentLayout->addWidget(input);
     connect(input,SIGNAL(returnPressed()),this,SLOT(solve_input()));
     input->setFocus();
     this->show();
}
void input_ui::up(QString response){
    path = new QLabel;
    QString s = get_cur_path();
    path->setText(s);
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

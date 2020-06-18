#include "input_ui.h"
#include <QScrollBar>
#include <QStringList>
#include <QString>
#include <QLayoutItem>
#include "virtual_system.h"
#include <malloc.h>
#include "interfaces.h"

input_ui::input_ui(QWidget *parent)
    : QWidget(parent)
{
	//���ùرռ���ʹ�ÿ����Լ����ùرշ���
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    this->setFixedSize(700, 700);
	//�����沼�֣������Ҫ���button���ڴ˸���
    one =new QGridLayout;
    this->setLayout(one);

	//���ù������������ڶ�̬��ʾ��һ��������ʾ��������
    bottom = new QScrollArea;
	//����������Ҫ����Widget
    bottomContent = new QWidget;
    contentLayout = new QVBoxLayout;
    bottom->setWidget(bottomContent);
    bottom->setWidgetResizable(true);//�����ڲ�Widget�����죬����̬�޸Ľ�ʧЧ
    bottomContent->setLayout(contentLayout);
    contentLayout->setAlignment(Qt::AlignTop);//�������򲼾ֵ�������ϲ�������������չ

    one->addWidget(bottom,0,0);//����Ĳ������У���

	//�˴��Ĵ�����ʵ����ʡ�ԣ�Ȼ���ڸý��汻����ʱ�����òۺ�����ʼ��
    path = new QLabel;//������ʾ��ǰ·��
    input = new QLineEdit;//������������
    input->setFixedWidth(650);
    contentLayout->addWidget(path);
    contentLayout->addWidget(input);
    connect(input,SIGNAL(returnPressed()),this,SLOT(solve_input()));//���������»س������ۺ���
	//�������Զ����²ۺ���
    connect(bottom->verticalScrollBar(), &QAbstractSlider::rangeChanged, this, &input_ui::slider);
    showResult = NULL;
}

input_ui::~input_ui()
{
}

void input_ui::solve_input(){//���д����Ĳۺ���
    QString instr = input->text();//��ȡ��ǰ����
    QStringList param = instr.split(" ");//�ո񻮷�
    int i=param.size()-1;//�˴�ע�⣬Ҫ����ɾ���մ����������Խ���������iΪint�ͣ���Ҫ�ö�Ӧ��size_t���ͣ��޸�����Խ��
    for(;i>=0;i--){
        if(param.at(i) == ""){
			//ѭ��ȥ���մ�
            param.removeAt(i);
        }
    }
    QString response = "";//������ʾ���
    if(param.size() == 0){ }//�û��������˻س�����пո�
    else if(param.at(0) == "login" && param.size() == 1){
		//��¼
        emit log(0);
    }
    else if(param.at(0) == "logout" && param.size() == 1){
		//�ǳ�����ر����и��û��򿪵��ļ�
        emit log(0);
    }
    else if(param.at(0) == "signup" && param.size() == 1){
		//ע��
        emit log(1);
    }
    else if(param.at(0) == "chuser" && param.size() == 1){
		//�л��˻�������رյ�ǰ�û��򿪵��ļ����Դ�ʵ��ͬ������
        emit log(0);
    }
    else if(param.at(0) == "mkdir" && param.size() == 2){
		//�����ļ��У������ļ��У�������ʹ��/, ./, ../�ķ�ʽ��ֻ��Ϊ: ���ļ�����/
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
		//�л�·�����ɶ༶Ŀ¼��֧�� ./   ../  .  ..  �ļ�����/    �ļ���
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
		int flag = _open(param);
		if (flag == -1) {
			response = "ERROR: Incorrect file type.\n\n";
		}
		else if (flag == 0) {
			response = "ERROR: File does not exist.\n\n";
		}
		else {
			response = "Done.\n\n";
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

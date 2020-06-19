#include "file_open.h"

file_open::file_open(QWidget *parent) : QWidget(parent)
{
    inode_index = 0;
    dirty = 0;
    mainLayout = new QVBoxLayout;
    top = new QWidget;
    topLayout = new QHBoxLayout;
    smallButton = new QPushButton;
    saveButton = new QPushButton;
    closeButton = new QPushButton;
    textEdit = new QPlainTextEdit;
    this->setLayout(mainLayout);
    mainLayout->addWidget(top);
    mainLayout->addWidget(textEdit);
    top->setLayout(topLayout);
    topLayout->addWidget(smallButton);
    topLayout->addWidget(saveButton);
    topLayout->addWidget(closeButton);
    smallButton->setText("smallest");
    saveButton->setText("save");
    closeButton->setText("close");
    this->setFixedSize(300,300);
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    connect(smallButton, SIGNAL(clicked()), this, SLOT(smallclick()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveclick()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(closeclick()));
    connect(textEdit, SIGNAL(textChanged()),this, SLOT(dirtyChange()));
}

file_open::~file_open(){
}

void file_open::smallclick(){
    this->hide();
    emit instruct();
}
void file_open::saveclick(){

}
void file_open::closeclick(){
    this->hide();
    emit instruct();
}

void file_open::openfile(int i){
    this->inode_index = i;
    this->show();
}

void file_open::dirtyChange(){
    dirty = 1;
    saveButton->setEnabled(true);
}

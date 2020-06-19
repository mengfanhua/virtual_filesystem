#include "file_open.h"
#include "interfaces.h"
#include <QMessageBox>
#include <QDialog>

file_open::file_open(QWidget *parent) : QWidget(parent)
{
    inode_index = 0;
    dirty = 0;
    success = 0;
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
    textEdit->setPlainText("");
}

file_open::~file_open(){
}

void file_open::smallclick(){
    if(dirty==1){
        if(QMessageBox::Yes == QMessageBox::question(this,"EXIT","Save your change?",
                                                     QMessageBox::Yes | QMessageBox:: No)){
            QString z = textEdit->toPlainText();
            int flag = _save(inode_index, z);
            if(flag == 1){
                this->hide();
                emit instruct();
            }
            else{
                QMessageBox::information(NULL,"FAIL","ERROR to save.",true);
            }
        }
        else{
            this->hide();
            emit instruct();
        }
    }
    this->hide();
    emit instruct();
}
void file_open::saveclick(){
    QString z = textEdit->toPlainText();
    int flag = _save(inode_index, z);
    if(flag == 1){
        QMessageBox::information(NULL,"OK","Successful.",true);
        dirty = 0;
        saveButton->setEnabled(false);
    }
    else{
        QMessageBox::information(NULL,"FAIL","ERROR to save.",true);
    }
}
void file_open::closeclick(){
    if(success != 0 && dirty == 0){
        _close(inode_index);
        this->hide();
        emit instruct();
    }
    else{
        if(dirty==1){
            if(QMessageBox::Yes == QMessageBox::question(this,"EXIT","Save your change?",
                                                         QMessageBox::Yes | QMessageBox:: No)) {
                QString z = textEdit->toPlainText();
                int flag = _save(inode_index, z);
                if(flag == 1){
                    _close(inode_index);
                    this->hide();
                    emit instruct();
                }
                else{
                    QMessageBox::information(NULL,"FAIL","ERROR to save.",true);
                }
            }
            else{
                _close(inode_index);
                this->hide();
                emit instruct();
            }
        }
    }
}

void file_open::openfile(int i){
    this->show();
    this->inode_index = i;
    int rw = get_rwmode(i);
    if(rw/2 != 0){
        textEdit->setEnabled(true);
    }
    else{
        textEdit->setEnabled(false);
    }
    if(rw==0){
        success = 0;
        textEdit->setPlainText("No permission to view.");
        smallButton->setEnabled(false);
    }
    else{
        char* s = _open_file_text(i);
        if(s==NULL){
            success = 0;
            textEdit->setPlainText("File failed to open.");
            textEdit->setEnabled(false);
            smallButton->setEnabled(false);
        }
        else{
            success = 1;
            QString r = QString(QLatin1String(s));
            textEdit->setPlainText(r);
            smallButton->setEnabled(true);
        }
    }
    saveButton->setEnabled(false);
    dirty = 0;
}

void file_open::dirtyChange(){
    dirty = 1;
    saveButton->setEnabled(true);
}

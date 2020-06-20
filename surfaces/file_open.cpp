#include "file_open.h"
#include "interfaces.h"

file_open::file_open(QWidget *parent) : QWidget(parent)
{
    this->setFixedSize(650,300);
    inode_index = 0;
    dirty = 0;
    success = 0;
    mainLayout = new QVBoxLayout;
    top = new QWidget;
    topLayout = new QHBoxLayout;
    smallButton = new QPushButton;
    saveButton = new QPushButton;
    closeButton = new QPushButton;
    textEdit = new QTextEdit;
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
    connect(smallButton, SIGNAL(clicked()), this, SLOT(smallclick()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveclick()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(closeclick()));
    connect(textEdit, SIGNAL(textChanged()),this, SLOT(dirtyChange()));
    r = new QString;
    *r = "";
    textEdit->setText(*r);
    message = new QMessageBox;
}

file_open::~file_open(){
    delete r;
    delete message;
    delete textEdit;
    delete closeButton;
    delete smallButton;
    delete saveButton;
    delete topLayout;
    delete top;
    delete mainLayout;
    mainLayout=NULL;
    top=NULL;
    topLayout=NULL;
    smallButton=NULL;
    saveButton=NULL;
    closeButton=NULL;

    textEdit=NULL;
    message=NULL;
    r=NULL;
}

void file_open::smallclick(){
    if(dirty==1){
        if(QMessageBox::Yes == message->question(this,"EXIT","Save your change?",
                                                     QMessageBox::Yes | QMessageBox:: No)){
            *r = textEdit->toPlainText();
            int flag = _save(inode_index, *r);
            if(flag == 1){
                this->setEnabled(false);
                emit instruct("Done.\n\n");
            }
            else{
                message->information(this,"FAIL","ERROR to save.",true);
            }
        }
        else{
            this->setEnabled(false);
            emit instruct("Done.\n\n");
        }
    }
    this->setEnabled(false);
    emit instruct("Done.\n\n");
}
void file_open::saveclick(){
    *r = textEdit->toPlainText();
    int flag = _save(inode_index, *r);
    if(flag == 1){
        message->information(this,"OK","Successful.",true);
        dirty = 0;
        saveButton->setEnabled(false);
    }
    else{
        message->information(this,"FAIL","ERROR to save.",true);
    }
}
void file_open::closeclick(){
    if(success != 0 && dirty == 0){
        _close(inode_index);
        this->setEnabled(false);
        emit instruct("Done.\n\n");
    }
    else{
        if(dirty==1){
            if(QMessageBox::Yes == message->question(this,"EXIT","Save your change?",
                                                         QMessageBox::Yes | QMessageBox:: No)) {
                *r = textEdit->toPlainText();
                int flag = _save(inode_index, *r);
                if(flag == 1){
                    _close(inode_index);
                    this->setEnabled(false);
                    emit instruct("Done.\n\n");
                }
                else{
                    message->information(this,"FAIL","ERROR to save.",true);
                }
            }
            else{
                _close(inode_index);
                this->setEnabled(false);
                emit instruct("Done.\n\n");
            }
        }
    }
}

void file_open::openfile(int i){
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
            *r = QString(QLatin1String(s));
            textEdit->setPlainText(*r);
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

#include "logi.h"
#include "interfaces.h"

logi::logi(QWidget *parent) : QWidget(parent)
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    sig = 0;
    user = new QLineEdit;
    password = new QLineEdit;
    confirm = new QPushButton;
    exit_s = new QPushButton;
    mainLayout = new QVBoxLayout;
    buttonLayout = new QHBoxLayout;
    bottomWidget = new QWidget;
    error = new QLabel;
    error->setText("password ERROR.");
    error->setVisible(false);
    this->setLayout(mainLayout);
    mainLayout->addWidget(user);
    mainLayout->addWidget(password);
    mainLayout->addWidget(error);
    mainLayout->addWidget(bottomWidget);
    bottomWidget->setLayout(buttonLayout);
    buttonLayout->addWidget(confirm);
    buttonLayout->addWidget(exit_s);
    user->setMaxLength(5);
    password->setMaxLength(5);
    password->setEchoMode(QLineEdit::Password);
    confirm->setText("login");
    exit_s->setText("exit");
    connect(confirm,SIGNAL(clicked()),this, SLOT(judge()));
    connect(exit_s,SIGNAL(clicked()),this, SLOT(closeornot()));
    this->setFixedSize(300,150);
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
}

logi::~logi(){
	
    delete user;
    delete password;
    delete confirm;
    delete exit_s;
    delete error;
    delete buttonLayout;
    delete bottomWidget;
    delete mainLayout;
    user=NULL;
    password=NULL;
    confirm=NULL;
    exit_s=NULL;
    mainLayout=NULL;
    buttonLayout=NULL;
    bottomWidget=NULL;
    error=NULL;
}

void logi::judge(){
    QString u = user->text();
    QString p = password->text();
    if(u==""||p==""){
        error->setText("No one can be empty.");
        error->setVisible(true);
    }
    else{
        int flag;
        if(sig==0){
            flag = _login(u, p);
            if(flag == 0){
                error->setText("Wrong password.");
                error->setVisible(true);
            }
            else if(flag == -1){
                error->setText("The maximum number of people logged in.");
                error->setVisible(true);
            }
            else{
                error->setVisible(false);
                this->hide();
                emit instr();
            }
        }
        else{
            flag = _signup(u, p);
            if(flag == 0){
                error->setText("Duplicate users.");//已存在的用户名
                error->setVisible(true);
            }
            else if(flag == -1){
                error->setText("Maximum number of registered users.");
                error->setVisible(true);
            }
            else{
                error->setVisible(false);
                this->hide();
                emit instr_s();
            }
        }

    }
}

void logi::closeornot(){
    if(sig==0){
        _clear_cur_path();
        _exit_sys();
        this->deleteLater();
    }
    else{
        error->setVisible(false);
        this->hide();
        emit instr_s();
    }
}

void logi::rec(int i){
    sig = i;
    if(sig == 0){
        confirm->setText("login");
        exit_s->setText("exit");
    }
    else{
        confirm->setText("signup");
        exit_s->setText("back");
    }
    this->show();
}

#include "input_ui.h"
#include "file_open.h"
#include "interfaces.h"
#include "logi.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    if(_init() == 0){
        return 0;
    }
    QApplication a(argc, argv);
    input_ui w;
    file_open e;
    logi p;
    QObject::connect(&w,SIGNAL(fileopen(int)), &e, SLOT(openfile(int)));
    QObject::connect(&e,SIGNAL(instruct()), &w, SLOT(back()));
    QObject::connect(&p,SIGNAL(instr()), &w, SLOT(comein()));
    QObject::connect(&p,SIGNAL(instr_s()), &w, SLOT(back()));
    QObject::connect(&w,SIGNAL(log(int)), &p, SLOT(rec(int)));
    p.show();
    return a.exec();
}

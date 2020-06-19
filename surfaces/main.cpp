#include "input_ui.h"
#include "file_open.h"
#include "interfaces.h"
#include "logi.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    _init();
    QApplication a(argc, argv);
    input_ui w;
    file_open e;
    logi p;
    QObject::connect(&w,SIGNAL(fileopen(int)), &e, SLOT(openfile(int)), Qt :: QueuedConnection);
    QObject::connect(&e,SIGNAL(instruct()), &w, SLOT(back()), Qt :: QueuedConnection);
    QObject::connect(&p,SIGNAL(instr()), &w, SLOT(comein()), Qt :: QueuedConnection);
    QObject::connect(&p,SIGNAL(instr_s()), &w, SLOT(back()), Qt :: QueuedConnection);
    QObject::connect(&w,SIGNAL(log(int)), &p, SLOT(rec(int)), Qt :: QueuedConnection);
    p.show();
    return a.exec();
}

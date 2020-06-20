#include "input_ui.h"
#include "interfaces.h"
#include "logi.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    _init();
    QApplication a(argc, argv);
    logi p;
    input_ui w;
    QObject::connect(&p,SIGNAL(instr()), &w, SLOT(comein()));
    QObject::connect(&p,SIGNAL(instr_s()), &w, SLOT(back()));
    QObject::connect(&w,SIGNAL(log(int)), &p, SLOT(rec(int)));
    p.show();
    return a.exec();
}

#ifndef INTERFACES_H
#define INTERFACES_H
#include <QString>
#include <QStringList>

int _mkdir(QStringList& param);
int _chdir(QStringList& param);
QString _dir();
int _create(QStringList& param);
int _delete(QStringList& param);
int _rm(QStringList& param);
int _open_file(QStringList& param);
int _ls(QStringList& param);
int _init();
void _exit_sys();

int _login(QString u,QString p);
int _signup(QString u,QString p);
void _logout();
void _exchange_admin();

QString get_cur_path();

int get_rwmode(int i);
char* _open_file_text(int a);
void _close(int a);
int _save(int a, QString content);

#endif // INTERFACES_H

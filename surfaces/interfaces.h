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
int _open(QStringList& param);
int _ls(QStringList& param);

#endif // INTERFACES_H

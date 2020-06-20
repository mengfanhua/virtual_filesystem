#ifndef FILE_OPEN_H
#define FILE_OPEN_H

#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QMessageBox>

class file_open : public QWidget
{
    Q_OBJECT
public:
    file_open(QWidget *parent = nullptr);
    ~file_open();

    int inode_index;
    int dirty;
    int success;
    QVBoxLayout *mainLayout;
    QWidget *top;
    QHBoxLayout *topLayout;
    QPushButton *smallButton;
    QPushButton *saveButton;
    QPushButton *closeButton;

    QTextEdit *textEdit;
    QMessageBox *message;
    QString *r;
    void openfile(int i);
private slots:
    void smallclick();
    void saveclick();
    void closeclick();
    void dirtyChange();

signals:
    void instruct(QString s);
};

#endif // FILE_OPEN_H

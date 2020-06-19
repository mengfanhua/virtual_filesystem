#ifndef FILE_OPEN_H
#define FILE_OPEN_H

#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <QPlainTextEdit>

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
    QHBoxLayout * topLayout;
    QPushButton *smallButton;
    QPushButton *saveButton;
    QPushButton *closeButton;

    QPlainTextEdit *textEdit;


private slots:
    void smallclick();
    void saveclick();
    void closeclick();
    void openfile(int i);
    void dirtyChange();

signals:
    void instruct();
};

#endif // FILE_OPEN_H

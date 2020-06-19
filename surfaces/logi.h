#ifndef LOGI_H
#define LOGI_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QLabel>

class logi : public QWidget
{
    Q_OBJECT
public:
    logi(QWidget *parent = nullptr);
    ~logi();
    int sig;
    QLineEdit *user;
    QLineEdit *password;
    QPushButton *confirm;
    QPushButton *exit_s;
    QVBoxLayout *mainLayout;
    QHBoxLayout *buttonLayout;
    QWidget *bottomWidget;
    QLabel *error;

private slots:
    void judge();
    void closeornot();
    void rec(int i);

signals:
    void instr();
    void instr_s();
};

#endif // LOGI_H

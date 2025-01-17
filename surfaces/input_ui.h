#ifndef INPUT_UI_H
#define INPUT_UI_H

#include <QWidget>
#include <QLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include "file_open.h"

class input_ui : public QWidget
{
    Q_OBJECT

public:
    input_ui(QWidget *parent = nullptr);
    ~input_ui();
    QGridLayout *one;

    QScrollArea *bottom;
    QWidget *bottomContent;
    QVBoxLayout *contentLayout;

    QLabel *path;
    QLineEdit *input;
    QLabel *showResult;
    file_open *file;

private slots:
    void solve_input();
    void slider();
    void back();
    void comein();
    void up(QString response);

signals:
    void log(int i);
	void fileopen(int i);
};
#endif // INPUT_UI_H

#ifndef INPUT_UI_H
#define INPUT_UI_H

#include <QWidget>
#include <QLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

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

private slots:
    void solve_input();
    void slider();

signals:
    void log(int i);
};
#endif // INPUT_UI_H
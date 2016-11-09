#ifndef FORM3_H
#define FORM3_H

#include <QWidget>
#include "basicwindow.h"
namespace Ui {
    class Form3;
}

class Form3 : public BasicWindow
{
    Q_OBJECT

public:
    explicit Form3(BasicWindow *parent = 0);
    ~Form3();

private slots:
    void on_QpbtnMax_clicked();

    void on_QpbtnMin_clicked();

    void on_QpbtnClose_clicked();

private:
    Ui::Form3 *ui;
    void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif // FORM3_H

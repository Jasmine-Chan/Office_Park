#ifndef PSEUDOMONAS_H
#define PSEUDOMONAS_H

#include <QWidget>
#include <QSqlQuery>
#include <qmessagebox.h>
#include <QDate>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <dropshadowwidget.h>
#include <struct.h>

namespace Ui {
    class Pseudomonas;
}

class Pseudomonas : public QWidget
{
    Q_OBJECT

public:
    explicit Pseudomonas(QWidget *parent = 0);
    ~Pseudomonas();

private slots:
    void on_QpbtnOK_clicked();
    void on_QpbtnClose_clicked();
    void SlotLeaveDate(QDate nLeaveDate, SStaff staff);

signals:
    void SigAddSuccess();

private:
    Ui::Pseudomonas *ui;
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    int CardType;
    QString number;
    QPoint move_point; //移动的距离
    bool mouse_press; //按下鼠标左键
    QPixmap background;
    QDate LeaveDate;
};

#endif // PSEUDOMONAS_H

#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QPushButton>
#include <QMouseEvent>
#include <QPainter>
class QMyPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit QMyPushButton(QWidget *parent = 0);
    ~QMyPushButton();
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void setMousePress(bool mouse_press);
signals:

public:
    bool mouse_over; //����Ƿ��ƹ�
    bool mouse_press; //����Ƿ���
    void painterInfo(int top_color, int middle_color, int bottom_color);

};

#endif // MYPUSHBUTTON_H

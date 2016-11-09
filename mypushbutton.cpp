#include "mypushbutton.h"

QMyPushButton::QMyPushButton(QWidget *parent) :
    QPushButton(parent)
{
    mouse_over = false;
    mouse_press = false;
}
QMyPushButton::~QMyPushButton()
{

}

void QMyPushButton::enterEvent(QEvent *)
{
        mouse_over = true;
}

void QMyPushButton::leaveEvent(QEvent *)
{
        mouse_over = false;
}

void QMyPushButton::mousePressEvent(QMouseEvent *event)
{
        if(event->button() == Qt::LeftButton)
        {
                emit clicked();
        }
}

void QMyPushButton::setMousePress(bool mouse_press)
{
        this->mouse_press = mouse_press;
        update();
}

void QMyPushButton::paintEvent(QPaintEvent *event)
{
        if(mouse_over)
        {
                //绘制鼠标移到按钮上的按钮效果
                painterInfo(0, 100, 150);
        }
        else
        {
                if(mouse_press)
                {
                        painterInfo(0, 100, 150);
                }
        }

        QPushButton::paintEvent(event);
}

void QMyPushButton::painterInfo(int top_color, int middle_color, int bottom_color)
{

        QPainter painter(this);
        QPen pen(Qt::NoBrush, 1);
        painter.setPen(pen);
        QLinearGradient linear(rect().topLeft(), rect().bottomLeft());
        linear.setColorAt(0, QColor(230, 230, 230, top_color));
        linear.setColorAt(0.5, QColor(230, 230, 230, middle_color));
        linear.setColorAt(1, QColor(230, 230, 230, bottom_color));
        painter.setBrush(linear);
        painter.drawRect(rect());
}

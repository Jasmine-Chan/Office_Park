#include "leavestaff.h"
#include "ui_leavestaff.h"
#include <QDebug>
#include <QMessageBox>
CLEAVESTAFF::CLEAVESTAFF(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CLEAVESTAFF)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    ui->Addr_label->setText("人事注销");
    ui->Addr_label->setAlignment(Qt::AlignCenter);
    setWindowModality(Qt::ApplicationModal);
    ui->QcomSex->addItem("男");
    ui->QcomSex->addItem("女");
    ui->QcomSex->setEnabled(false);
    ui->QdateEnter->setEnabled(false);
    ui->QlineIdCard->setEnabled(false);
    ui->QlineName->setEnabled(false);
    ui->QlineNo->setEnabled(false);
    ui->QlinePost->setEnabled(false);
}

CLEAVESTAFF::~CLEAVESTAFF()
{
    delete ui;
}

void CLEAVESTAFF::_Update(SStaff sStaff)
{
    m_sStaff = sStaff;
    ui->QlineIdCard->setText(sStaff.strIdCard);
    ui->QlineName->setText(sStaff.strName);
    ui->QlineNo->setText(sStaff.strNo);
    ui->QdateEnter->setDate(QDate::fromString(sStaff.strEnter,"yyyy-MM-dd"));
    ui->QlinePost->setText(sStaff.strPost);
    ui->QcomSex->setCurrentIndex(sStaff.nSex);
    ui->QlineDeal->clear();
    ui->QtextDeal->clear();
}

void CLEAVESTAFF::on_QpbtnOK_clicked()
{
    if(ui->QlineDeal->text() == "")
    {
        QMessageBox::information(this,"提示","处理人不能为空");
    }
    else
    {
        CMYSQL CMysql;
        if(CMysql._LeaveStaff(m_sStaff,ui->QlineDeal->text(),ui->QtextDeal->toPlainText()))
        {
            QMessageBox::information(this,"提示","处理成功");
            emit SigLeaveStaff(ui->QlineNo->text());
            close();
        }
        else
        {
            QMessageBox::information(this,"提示","处理失败");
        }
    }
}

void CLEAVESTAFF::on_QpbtnClose_clicked()
{
    close();
}

//重写鼠标时间，实现窗体拖拽
void CLEAVESTAFF::mousePressEvent(QMouseEvent *event)
{
    //只能是鼠标左键移动和改变大小
        if(event->button() == Qt::LeftButton)
        {
            mouse_press = true;
        }

        //窗口移动距离
        move_point = event->globalPos() - pos();
}

void CLEAVESTAFF::mouseReleaseEvent(QMouseEvent *event)
{
     mouse_press = false;
}

void CLEAVESTAFF::mouseMoveEvent(QMouseEvent *event)
{
    if(mouse_press)
    {
        QPoint move_pos = event->globalPos();
        move(move_pos - move_point);
    }
}

void CLEAVESTAFF::paintEvent(QPaintEvent *event)
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRect(10, 10, this->width()-20, this->height()-20);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(Qt::white));

    QColor color(0, 0, 0, 50);
    for(int i=0; i<10; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(10-i, 10-i, this->width()-(10-i)*2, this->height()-(10-i)*2);
        color.setAlpha(150 - qSqrt(i)*50);
        painter.setPen(color);
        painter.drawPath(path);
    }
}

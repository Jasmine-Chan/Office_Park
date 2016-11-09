#include "find.h"
#include "ui_find.h"

CFIND::CFIND(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CFIND)
{
    ui->setupUi(this);
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::FramelessWindowHint);
    ui->QlineFind->setMaxLength(24);
    ui->Addr_label->setAlignment(Qt::AlignCenter);
    mouse_press = false;
}

CFIND::~CFIND()
{
    delete ui;
}

//日志更新
void CFIND::_Update(int nPage)
{
    m_nPage = nPage;
    m_nIndex = 0;
    ui->QlineFind->clear();
    ui->QchkFind->clear();
    switch(m_nPage)
    {
        case 1:
            ui->Addr_label->setText("报警记录-查找");
            ui->QchkFind->addItem(QWidget::tr("地址"));
            ui->QchkFind->addItem(QWidget::tr("IP地址"));
            ui->QchkFind->addItem(QWidget::tr("未处理"));
            break;

        case 3:
            ui->Addr_label->setText("系统日志-查找");
            ui->QchkFind->addItem(QWidget::tr("用户"));
            ui->QchkFind->addItem(QWidget::tr("姓名"));
            ui->QchkFind->addItem(QWidget::tr("操作类型"));
            break;
        case 2:
            ui->Addr_label->setText("设备日志-查找");
            ui->QchkFind->addItem(QWidget::tr("地址"));
            ui->QchkFind->addItem(QWidget::tr("IP地址"));
            ui->QchkFind->addItem(QWidget::tr("状态"));
            break;

         case 4:
            ui->Addr_label->setText("考勤记录-查找"); //???操作按钮在哪？
            ui->QchkFind->addItem(QWidget::tr("工号"));
            ui->QchkFind->addItem(QWidget::tr("姓名"));

        default :
            break;
    }

    QDateTime dateTime=QDateTime::currentDateTime();    //获取当前时间
    ui->QdateETime->setDateTime(dateTime);//setDate(dateTime.date());
    dateTime = dateTime.addDays(-5);    //当前日期减5天
    ui->QdateSTime->setDateTime(dateTime);//setDate(dateTime.date());
    ui->QchkFind->setCurrentIndex(m_nIndex);
    ui->QlineFind->show();
    ui->QlineFind->setFocus();
    m_nTime = 0;
    ui->QrbtnTime->setChecked(false);
    ui->QdateETime->setEnabled(false);
    ui->QdateSTime->setEnabled(false);
}

void CFIND::on_QpbtnFind_clicked()
{

    if(ui->QdateETime->dateTime() >= ui->QdateSTime->dateTime())
    {
        emit SigFind(m_nIndex,m_nTime,ui->QlineFind->text(),ui->QdateSTime->text(),ui->QdateETime->text());
        close();
    }
    else
    {
        QMessageBox::information(this,tr("提示"),tr("开始时间不能大于结束时间"));
        ui->QdateSTime->setFocus();
    }
}

void CFIND::on_QpbtnClose_clicked()
{
    close();
}

//下拉列表
void CFIND::on_QchkFind_activated(int index)
{
    m_nIndex = index;
    if((m_nPage == 1 || m_nPage == 4)&&m_nIndex == 2)
    {
        ui->QlineFind->hide();
    }
    else
        ui->QlineFind->show();
}

void CFIND::on_QrbtnTime_clicked()
{
    if(ui->QrbtnTime->isChecked())
    {
        m_nTime = 1;
        ui->QdateETime->setEnabled(true);
        ui->QdateSTime->setEnabled(true);
        ui->QdateSTime->setFocus();
    }
    else
    {
        m_nTime = 0;
        ui->QdateETime->setEnabled(false);
        ui->QdateSTime->setEnabled(false);
    }
}

void CFIND::mousePressEvent(QMouseEvent *event)
{
    //只能是鼠标左键移动和改变大小
    if(event->button() == Qt::LeftButton)
    {
         mouse_press = true;
    }

    //窗口移动距离
    move_point = event->globalPos() - pos();
}

void CFIND::mouseReleaseEvent(QMouseEvent *event)
{
    mouse_press = false;
}

void CFIND::mouseMoveEvent(QMouseEvent *event)
{
    if(mouse_press)
    {
        QPoint move_pos = event->globalPos();
        move(move_pos - move_point);
    }
}

//重绘外边框
void CFIND::paintEvent(QPaintEvent *event)
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

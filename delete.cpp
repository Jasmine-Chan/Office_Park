#include "delete.h"
#include "ui_delete.h"
#include "mysql.h"
#include "QMessageBox"

CDELETE::CDELETE(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDELETE)
{
    ui->setupUi(this);
    setWindowModality(Qt::ApplicationModal);//该窗口是模态的应用程序和块输入到所有的窗口
    setWindowFlags(Qt::FramelessWindowHint);
    ui->Addr_label->setAlignment(Qt::AlignCenter);
    m_nPage = 0;
    ui->QlineAddr->setMaxLength(25);//室内机地址长度超过20
}

CDELETE::~CDELETE()
{
    delete ui;
}

/*****更新函数*****/
void CDELETE::_Update(int nPage, SUser sUser)
{
    m_nPage = nPage;
    m_sUser = sUser;
    ui->QlineAddr->clear();
    switch(m_nPage)
    {
        case 3:
            ui->Addr_label->setText("系统日志-删除");
            ui->label_3->setText("用户名：");
            break;
        case 1:
            ui->Addr_label->setText("报警记录-删除");
            ui->label_3->setText("地址：");
            break;
        case 4:
            ui->Addr_label->setText("报修记录-删除");
            ui->label_3->setText("地址：");
            break;
        case 2:
            ui->Addr_label->setText("设备日志-删除");
            ui->label_3->setText("地址：");
            break;
        case 9:
            ui->Addr_label->setText("刷卡记录-删除");
            ui->label_3->setText("地址：");
            break;
        default :
            break;
    }
    ui->QlineAddr->setFocus();
    QDateTime dateTime = QDateTime::currentDateTime();
    ui->QdateEtime->setDateTime(dateTime);  //setDate(dateTime.date());
    dateTime = dateTime.addDays(-5);
    ui->QdateStime->setDateTime(dateTime);  //setDate(dateTime.date());
}

/*****信号函数：“删除”按钮*****/
void CDELETE::on_QpbtnDelete_clicked()
{
    //先判断是否有权限执行删除操作
    QString SQL = "SELECT *FROM user WHERE User_User = '"+ui->QlineAddr->text()+"' ORDER BY User_Grade ASC";
    QSqlQuery query;
    if(query.exec(SQL))
    {
        while(query.next())
        {
            //用户只能删除自己，以及比自己权限低的用户，操作员不拥有删除权限
            if(m_sUser.nGrade > query.value(4).toInt() || query.value(4).toInt() == 3)
            {
                QMessageBox::critical(this, tr(""), tr("权限限制，无法删除"));
                this->close();
                return;
            }
        }
    }

    CMYSQL CMysql;
    QDateTime DateSTime = ui->QdateStime->dateTime();
    QDateTime DateETime = ui->QdateEtime->dateTime();
    if(DateSTime < DateETime)
    {
        QString str;
        str.clear();
        if(ui->QlineAddr->text() != NULL)
        {
            str = "删除从'"+ui->QdateStime->text()+"'到'"+ui->QdateEtime->text()+"'时间段里'"+ui->QlineAddr->text()+"'设备的所有记录";
        }
        else
        {
            str = "删除从'"+ui->QdateStime->text()+"'到'"+ui->QdateEtime->text()+"'时间段里所有记录";
        }
        switch( QMessageBox::question( this, tr("提示"),str,tr("Yes"), tr("No"),0, 1 ) )
        {
            case 0:
                switch(m_nPage)
                {
                    case 1:     //报警记录-删除
                        if(CMysql._DeleteAlarmRecord(ui->QlineAddr->text(),ui->QdateStime->text(),ui->QdateEtime->text()))
                        {
                            QMessageBox::information(this,tr("提示"),tr("删除成功"));
                            emit SigDelete();   //触发officepark中的SlotDeleteSuc函数，重新查询数据库
                            this->close();
                            return;
                        }
                        break;
                    case 2:     //设备日志-删除
                        if(CMysql._DeleteDeviceRecord(ui->QlineAddr->text(),ui->QdateStime->text(),ui->QdateEtime->text()))
                        {
                            QMessageBox::information(this,tr("提示"),tr("删除成功"));
                            emit SigDelete();
                            close();
                            return;
                        }
                        break;
                    case 3:     //系统日志-删除
                        if(CMysql._DeleteSystemRecord(ui->QlineAddr->text(),ui->QdateStime->text(),ui->QdateEtime->text()))
                        {
                            QMessageBox::information(this,tr("提示"),tr("删除成功"));
                            emit SigDelete();
                            close();
                            return;
                        }
                        break;
                    case 8:
        //              if(CMysql._DeleteRepairsRecord(ui->QlineAddr->text(),ui->QdateStime->text(),ui->QdateEtime->text()))
                        {
                            QMessageBox::information(this,tr("提示"),tr("删除成功"));
                            emit SigDelete();
                            close();
                            return;
                        }
                        break;
                     case 9:    //刷卡记录-删除
        //              if(CMysql._DeleteCardRecord(ui->QlineAddr->text(),ui->QdateStime->text(),ui->QdateEtime->text()))
                        {
                            QMessageBox::information(this,tr("提示"),tr("删除成功"));
                            emit SigDelete();
                            close();
                            return;
                        }
                        break;
                     default :
                        break;
                }
                break;
            case 1:

            default:
                break;
       }
    }
    else
    {
        QMessageBox::information(this,tr("提示"),tr("开始时间不能大于结束时间"));
        ui->QdateStime->setFocus();
    }
}

/*****信号函数：“取消”按钮*****/
void CDELETE::on_QpbtnClose_clicked()
{
    close();
}

/*****函数：鼠标按下事件*****/
void CDELETE::mousePressEvent(QMouseEvent *event)
{
    //只能是鼠标左键移动和改变大小
    if(event->button() == Qt::LeftButton)
    {
        mouse_press = true;
    }

    //窗口移动距离
    move_point = event->globalPos() - pos();
}

/*****函数：鼠标释放事件*****/
void CDELETE::mouseReleaseEvent(QMouseEvent *event)
{
     mouse_press = false;
}

/*****函数：鼠标移动事件*****/
void CDELETE::mouseMoveEvent(QMouseEvent *event)
{
    if(mouse_press)
    {
        QPoint move_pos = event->globalPos();
        move(move_pos - move_point);
    }
}

/*****函数：重绘事件*****/
void CDELETE::paintEvent(QPaintEvent *event)
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

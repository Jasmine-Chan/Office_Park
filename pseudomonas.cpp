#include "pseudomonas.h"
#include "ui_pseudomonas.h"
#include <QPainter>
#include <QMouseEvent>
Pseudomonas::Pseudomonas(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Pseudomonas)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);//设置主窗口无边框
    setWindowModality(Qt::ApplicationModal);
    background.load(":/images/OfficeParks22_03.png");
    this->setWindowTitle(tr("请假单"));
//    QGraphicsDropShadowEffect *shadow_line = new QGraphicsDropShadowEffect(this);
//    shadow_line->setOffset(0, 10);
//    shadow_line->setColor(Qt::gray);
//    shadow_line->setBlurRadius(8);
//    ui->linePart->setGraphicsEffect(shadow_line);

    LeaveDate.setDate(0,0,0);
    ui->STime->setCalendarPopup(true);
    ui->ETime->setCalendarPopup(true);

    ui->STime->setTime(QTime::currentTime());
    ui->ETime->setTime(QTime(18,0,0,0));
    ui->Number->clear();
}

Pseudomonas::~Pseudomonas()
{
    delete ui;
}

void Pseudomonas::on_QpbtnOK_clicked()
{
   CardType = 0;
   if(ui->checkBox_1->checkState())
       CardType = CardType*10+1;
   if(ui->checkBox_2->checkState())
       CardType = CardType*10+2;
   if(ui->checkBox_3->checkState())
       CardType  =CardType*10+3;
   if(ui->checkBox_4->checkState())
       CardType = CardType*10+4;
   if(ui->checkBox_5->checkState())
       CardType = CardType*10+5;
   if(ui->checkBox_6->checkState())
       CardType = CardType*10+6;

   if(CardType == 0)
   {
        QMessageBox::information(this,"提示","请选择请假类型",QMessageBox::Yes);
   }
   else
   {
       number = ui->Number->text();
       switch(number.length())
       {
           case 1: number = "0000"+number;
                   break;
           case 2: number = "000"+number;
              break;
           case 3: number = "00"+number;
              break;
           case 4: number = "0"+number;
              break;
           case 5:
              break;
           default: QMessageBox::information(this,"提示","请正确输入你的工号",QMessageBox::Yes);
              return;
       }
       QString DepartmentSQL= "SELECT a.Department_No FROM department a WHERE a.Department_Name = '"+ui->departmentName->text()+"'";
       QString DepartmentId = "";
       QSqlQuery Departmentquery;
       if(Departmentquery.exec(DepartmentSQL))
       {
           if(Departmentquery.next())
               DepartmentId = Departmentquery.value(0).toString();
       }
       QString StaffSQL = "SELECT a.* FROM staff a WHERE a.Staff_No = '"+number+"'";
       QSqlQuery Staffquery;
       if(Staffquery.exec(StaffSQL))
       {
           if(Staffquery.next())
           {
               if(ui->Name->text() == Staffquery.value(2).toString())
               {
                   if(DepartmentId == Staffquery.value(5).toString())
                   {
                       if(ui->position->text() == Staffquery.value(6).toString())
                       {
                           if(!ui->Manhour->text().isEmpty())
                           {
                               if(!ui->Account->text().isEmpty())
                               {
                                   QString SQL = "insert into `leave` (Leave_Date,Leave_Staff_No,Leave_SDateTime,Leave_EDateTime,Leave_Type,Leave_ManHour,Leave_Cause) VALUES "
                                           "('"+LeaveDate.toString("yyyy-MM-dd")+"','"+number+"','"+ui->STime->dateTime().toString("yyyy/MM/dd hh:mm:ss")+"','"+ui->ETime->dateTime().toString("yyyy/MM/dd hh:mm:ss")+"',"
                                           "'"+QString::number(CardType)+"','"+ui->Manhour->text()+"','"+ui->Account->text()+"')";
                                   QSqlQuery query;
                                   if(query.exec(SQL))
                                   {
                                       QMessageBox::information(this,"提示","添加成功！",QMessageBox::Yes);
                                       emit SigAddSuccess();
                                       close();

                                     //  _addcardRecord(number,ui->STime->dateTime(),ui->ETime->dateTime());
                                   }
                                   else
                                       QMessageBox::information(this,"提示","添加失败！",QMessageBox::Yes);
                               }
                               else
                                   QMessageBox::information(this,"提示","请输入请假事由",QMessageBox::Yes);
                           }
                           else
                               QMessageBox::information(this,"提示","请输入请假工时",QMessageBox::Yes);
                       }
                       else
                           QMessageBox::information(this,"提示","请正确输入你的职位",QMessageBox::Yes);
                   }
                   else
                       QMessageBox::information(this,"提示","请正确输入你的部门",QMessageBox::Yes);
               }
               else
                   QMessageBox::information(this,"提示","请正确输入你的姓名",QMessageBox::Yes);
           }
           else
               QMessageBox::information(this,"输入错误","此工号不存在",QMessageBox::Yes);
       }
    }
}

void Pseudomonas::on_QpbtnClose_clicked()
{
    close();
}

void Pseudomonas::SlotLeaveDate(QDate nLeaveDate, SStaff staff)
{
    LeaveDate.setDate(nLeaveDate.year(),nLeaveDate.month(),nLeaveDate.day());
    ui->STime->setDate(LeaveDate);
    ui->ETime->setDate(LeaveDate);

    ui->Name->setText(staff.strName);
    ui->Number->setText(staff.strNo);
    ui->position->setText(staff.strPost);
    ui->departmentName->setText(staff.strDepartment);
}

void Pseudomonas::paintEvent(QPaintEvent *event)
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

void Pseudomonas::mousePressEvent(QMouseEvent *event)
{
    //只能是鼠标左键移动和改变大小
        if(event->button() == Qt::LeftButton)
        {
                mouse_press = true;
        }

        //窗口移动距离
        move_point = event->globalPos() - pos();
}

void Pseudomonas::mouseReleaseEvent(QMouseEvent *event)
{
     mouse_press = false;
}

void Pseudomonas::mouseMoveEvent(QMouseEvent *event)
{
    if(mouse_press)
    {
            QPoint move_pos = event->globalPos();
            move(move_pos - move_point);
    }
}

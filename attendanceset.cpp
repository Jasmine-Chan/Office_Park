#include "attendanceset.h"
#include "ui_attendanceset.h"

CATTENDANCESET::CATTENDANCESET(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CATTENDANCESET)
{
    ui->setupUi(this);
    NomalFalg = 0;
    RestFalg = 0;
//    ui->calendarWidget->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
//    int weekNum = WEEKNUM(TODAY(),2)
    ui->twin_rest->setCheckState(Qt::Checked);
    ui->workTime_normal->setEnabled(false);
    ui->offworkTime_normal->setEnabled(false);
    ui->workTime_rest->setEnabled(false);
    ui->offworkTime_rest->setEnabled(false);
    ui->workTime_weekday->setEnabled(false);
    ui->offworkTime_weekday->setEnabled(false);
    ui->QpbtnOK->setEnabled(false);
    ui->normalLable->setEnabled(false);
    ui->restLable->setEnabled(false);
    ui->weekdayLable->setEnabled(false);
    ui->single_rest->setEnabled(false);
    ui->twin_rest->setEnabled(false);
    ui->select_rest->setEnabled(false);
    ui->single_weekday->setEnabled(false);
    ui->twin_weekday->setEnabled(false);
    _UpdateWorkTime();
}

CATTENDANCESET::~CATTENDANCESET()
{
    delete ui;
}

void CATTENDANCESET::on_QpbtnOK_clicked()
{
    if(ui->workTime_normal->time()> ui->offworkTime_normal->time())
        QMessageBox::information(this,"提示","下班时间不能小于上班时间1");
    else
        if(ui->workTime_rest->time()>ui->offworkTime_rest->time())
            QMessageBox::information(this,"提示","下班时间不能小于上班时间2");
        else
            if(ui->workTime_weekday->time()>ui->offworkTime_weekday->time())
                QMessageBox::information(this,"提示","下班时间不能小于上班时间3");
            else
            {
                ui->QpbtnOK->setEnabled(false);
                ui->normalLable->setEnabled(false);
                ui->restLable->setEnabled(false);
                ui->weekdayLable->setEnabled(false);
                ui->workTime_normal->setEnabled(false);
                ui->offworkTime_normal->setEnabled(false);
                ui->workTime_rest->setEnabled(false);
                ui->offworkTime_rest->setEnabled(false);
                ui->workTime_weekday->setEnabled(false);
                ui->offworkTime_weekday->setEnabled(false);
                ui->single_rest->setEnabled(false);
                ui->twin_rest->setEnabled(false);
                ui->select_rest->setEnabled(false);
                ui->single_weekday->setEnabled(false);
                ui->twin_weekday->setEnabled(false);
                ui->normalLable->setCheckState(Qt::Unchecked);
                ui->restLable->setCheckState(Qt::Unchecked);
                ui->weekdayLable->setCheckState(Qt::Unchecked);
                _UpdateSQLWorkTime();
                _UpdateWorkTime();
                emit SigMonthlyrestType(QString(restType));
            }
}

/*****工作日 选中*****/
void CATTENDANCESET::on_normalLable_clicked()
{
    if(ui->normalLable->checkState())
    {
        ui->workTime_normal->setEnabled(true);
        ui->offworkTime_normal->setEnabled(true);
        NomalFalg=1;
    }
    else
    {
        ui->workTime_normal->setEnabled(false);
        ui->offworkTime_normal->setEnabled(false);
        NomalFalg=0;
    }
}

/*****周六 选中*****/
void CATTENDANCESET::on_restLable_clicked()
{
    if(ui->restLable->checkState())
    {
        ui->workTime_rest->setEnabled(true);
        ui->offworkTime_rest->setEnabled(true);
        RestFalg=1;
    }
    else
    {
        ui->workTime_rest->setEnabled(false);
        ui->offworkTime_rest->setEnabled(false);
        RestFalg=0;
    }
}

/*****周日 选中*****/
void CATTENDANCESET::on_weekdayLable_clicked()
{
    if(ui->weekdayLable->checkState())
    {
        ui->workTime_weekday->setEnabled(true);
        ui->offworkTime_weekday->setEnabled(true);
    }
    else
    {
        ui->workTime_weekday->setEnabled(false);
        ui->offworkTime_weekday->setEnabled(false);
    }
}

void CATTENDANCESET::on_pbtnInstall_clicked()
{
    ui->QpbtnOK->setEnabled(true);
    ui->normalLable->setEnabled(true);
    ui->restLable->setEnabled(true);
    ui->weekdayLable->setEnabled(true);
    ui->single_rest->setEnabled(true);
    ui->twin_rest->setEnabled(true);
    ui->select_rest->setEnabled(true);
}

/*****更新函数：上下班工作时间*****/
void CATTENDANCESET::_UpdateWorkTime()
{
    QString SQL ="SELECT a.SetAttendance_Attendance_Time,a.SetAttendance_Closeing_Time,a.SetAttendance_Saturday_Attendance_Time,a.SetAttendance_Saturday_Closeing_Time,SetAttendance_Sunday_Attendance_Time,SetAttendance_Sunday_Closeing_Time FROM setattendance a";
    QSqlQuery query;

    qDebug()<<"query"<<query.exec(SQL)<<endl;
    if(query.exec(SQL))
    {
        while(query.next())
        {
            QTime N_worktime;
            QString S_worktime = query.value(0).toString();
            N_worktime.setHMS(S_worktime.left(2).right(2).toInt(),S_worktime.left(5).right(2).toInt(),S_worktime.left(8).right(2).toInt());
            ui->workTime_normal->setTime(N_worktime);

            QTime N_offworktime;
            QString S_offworktime = query.value(1).toString();
            N_offworktime.setHMS(S_offworktime.left(2).right(2).toInt(),S_offworktime.left(5).right(2).toInt(),S_offworktime.left(8).right(2).toInt());
            ui->offworkTime_normal->setTime(N_offworktime);

            QTime R_worktime;
            QString RS_worktime = query.value(2).toString();
            R_worktime.setHMS(RS_worktime.left(2).right(2).toInt(),RS_worktime.left(5).right(2).toInt(),RS_worktime.left(8).right(2).toInt());
            ui->workTime_rest->setTime(R_worktime);

            QTime R_offworktime;
            QString RS_offworktime = query.value(3).toString();
            R_offworktime.setHMS(RS_offworktime.left(2).right(2).toInt(),RS_offworktime.left(5).right(2).toInt(),RS_offworktime.left(8).right(2).toInt());
            ui->offworkTime_rest->setTime(R_offworktime);

            QTime W_worktime;
            QString WS_worktime = query.value(4).toString();
            W_worktime.setHMS(WS_worktime.left(2).right(2).toInt(),WS_worktime.left(5).right(2).toInt(),WS_worktime.left(8).right(2).toInt());
            ui->workTime_weekday->setTime(W_worktime);

            QTime W_offworktime;
            QString WS_offworktime = query.value(5).toString();
            W_offworktime.setHMS(WS_offworktime.left(2).right(2).toInt(),WS_offworktime.left(5).right(2).toInt(),WS_offworktime.left(8).right(2).toInt());
            ui->offworkTime_weekday->setTime(W_offworktime);
        }
    }
}

/*****信号槽函数：设置对月报表数据造成的影响*****/
void CATTENDANCESET::SlotMonthlycardDate(int year,int month,int day)
{
    m_date.setDate(0,0,0);
    restType = "0";
    m_date.setDate(year,month,day);
    _UpdateSQLWorkTime();
    _UpdateWorkTime();
    emit SigMonthlyrestType(restType);
}

/*****信号槽函数：设置对日报表数据造成的影响*****/
void CATTENDANCESET::SlotDaycardDate(int year, int month, int day)
{
    m_date.setDate(0,0,0);
    restType = "0";
    m_date.setDate(year,month,day);
    _UpdateSQLWorkTime();
    _UpdateWorkTime();
    emit SigDayrestType(restType);
}

/*****数据库更新函数：系统上下班时间设置*****/
void CATTENDANCESET::_UpdateSQLWorkTime()
{
    QString SQL;
    SQL = "UPDATE setattendance SET SetAttendance_Attendance_Time = '"+ui->workTime_normal->text()+"',"
           "SetAttendance_Closeing_Time = '"+ui->offworkTime_normal->text()+"',SetAttendance_Saturday_Attendance_Time ="
           "'"+ui->workTime_rest->text()+"',SetAttendance_Saturday_Closeing_Time = '"+ui->offworkTime_rest->text()+"',"
            "SetAttendance_Sunday_Attendance_Time = '"+ui->workTime_weekday->text()+"',SetAttendance_Sunday_Closeing_Time = '"+ui->offworkTime_weekday->text()+"'";
    if(ui->single_rest->checkState())
    {
        restType="1";
    }
    else if(ui->select_rest->checkState())
    {

        restType="3";
        if(ui->single_weekday->checkState())
        {
            if(m_date.weekNumber()%2==1)
            {
                restType="3";
            }
            else if(m_date.weekNumber()%2==0)
            {
                restType="4";
            }
        }
        else if(ui->twin_weekday->checkState())
        {
            if(m_date.weekNumber()%2==0)
            {
                restType="6";
            }
            else if(m_date.weekNumber()%2==1)
            {
                restType="5";
            }
        }
    }
    else if(ui->twin_rest->checkState())
    {
        restType="2";
    }
    QSqlQuery query;
    query.exec(SQL);
}

/*****信号槽函数：单双休 以及奇偶周休对本月补卡天数的影响*****/
void CATTENDANCESET::SlotFillcardDate(QDate fillcardDate)
{
    if(ui->single_rest->checkState())
        fillCardRestType = "1";
    else if(ui->select_rest->checkState())
    {
        if(ui->single_weekday->checkState())
        {
            if(fillcardDate.weekNumber()%2==1)
                fillCardRestType = "3";
            else if(m_date.weekNumber()%2==0)
                fillCardRestType = "4";
        }
        else if(ui->twin_weekday->checkState())
        {
            if(m_date.weekNumber()%2==0)
                fillCardRestType = "6";
            else if(m_date.weekNumber()%2==1)
                fillCardRestType = "5";
        }
    }
    else if(ui->twin_rest->checkState())
        fillCardRestType = "2";
    emit SigFillcardRestType(fillCardRestType);
}

/*****选择  双休*****/
void CATTENDANCESET::on_twin_rest_clicked()
{
    if(ui->twin_rest->checkState()==Qt::Checked)
    {
        ui->single_rest->setCheckState(Qt::Unchecked);
        ui->select_rest->setCheckState(Qt::Unchecked);
        ui->twin_weekday->setCheckState(Qt::Unchecked);
        ui->single_weekday->setCheckState(Qt::Unchecked);
        ui->twin_weekday->setEnabled(false);
        ui->single_weekday->setEnabled(false);
    }
}

/*****选择单休*****/
void CATTENDANCESET::on_single_rest_clicked()
{
    if(ui->single_rest->checkState()==Qt::Checked)
    {
        ui->twin_rest->setCheckState(Qt::Unchecked);
        ui->select_rest->setCheckState(Qt::Unchecked);
        ui->twin_weekday->setCheckState(Qt::Unchecked);
        ui->single_weekday->setCheckState(Qt::Unchecked);
        ui->twin_weekday->setEnabled(false);
        ui->single_weekday->setEnabled(false);
    }
}

/*****选择单数周或偶数周休  单休*****/
void CATTENDANCESET::on_select_rest_clicked()
{
    if(ui->select_rest->checkState()==Qt::Checked)
    {
        ui->single_rest->setCheckState(Qt::Unchecked);
        ui->twin_rest->setCheckState(Qt::Unchecked);

        ui->single_weekday->setEnabled(true);
        ui->twin_weekday->setEnabled(true);
    }
    else
    {
        ui->single_weekday->setEnabled(false);
        ui->twin_weekday->setEnabled(false);
        ui->single_weekday->setCheckState(Qt::Unchecked);
        ui->twin_weekday->setCheckState(Qt::Unchecked);
    }
}

/*****单数周 单休*****/
void CATTENDANCESET::on_single_weekday_clicked()
{
    if(ui->single_weekday->checkState()==Qt::Checked)
        ui->twin_weekday->setCheckState(Qt::Unchecked);
}

/*****偶数周 单休*****/
void CATTENDANCESET::on_twin_weekday_clicked()
{
    if(ui->twin_weekday->checkState()==Qt::Checked)
        ui->single_weekday->setCheckState(Qt::Unchecked);
}

void CATTENDANCESET::SlotFocusOut()
{
    ui->SetCalendarWidget->setSelectedDate(QDate::currentDate());
}

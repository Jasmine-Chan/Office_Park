#include "day.h"
#include "ui_day.h"

CDAY::CDAY(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDAY)
{
    ui->setupUi(this);
    _InitDay();
    _InitTree();
    isholiday = false;
    isTakeWorks = false;
    ui->Button_Year->setText(QString::number(QDate::currentDate().year()));
    ui->Button_Month->setText(QString::number(QDate::currentDate().month()));

    isYear = true;  //默认是变动年份

    ui->QpbtnBack->setToolTip(tr("返回"));
    ui->QpbtnLead->setToolTip(tr("导出报表"));
}

CDAY::~CDAY()
{
    delete ui;
}

void CDAY::_Update()
{
    _InitTree();
    ui->DaytableWidget->setRowCount(0);
    FindName.clear();
    ui->SearchData->clear();
}

/*****初始化函数：日报表数据*****/
void CDAY::_InitDay()
{
    ui->DaytableWidget->setColumnCount(13);
    ui->DaytableWidget->setShowGrid(true);
    QStringList headers;
    headers<<"编号"<<"姓名"<<"刷卡日期"<<"星期"<<"班次名称"<<"应出勤天数"<<"实出勤天数"<<"缺勤天数"<<"少打卡次数"<<"工作时间/小时"<<"迟到/分钟"<<"早退/分钟"<<"加班时间/小时";
    ui->DaytableWidget->setHorizontalHeaderLabels(headers);
    ui->DaytableWidget->resizeColumnsToContents();
    ui->DaytableWidget->setFont(QFont("Helvetica"));
    ui->DaytableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->DaytableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->DaytableWidget->setColumnWidth(0,60);
    ui->DaytableWidget->setColumnWidth(1,80);
    ui->DaytableWidget->setColumnWidth(2,80);
    ui->DaytableWidget->setColumnWidth(3,80);
    ui->DaytableWidget->setColumnWidth(4,100);
    ui->DaytableWidget->setColumnWidth(5,100);
    ui->DaytableWidget->setColumnWidth(6,100);
    ui->DaytableWidget->setColumnWidth(7,100);
    ui->DaytableWidget->setColumnWidth(8,100);
    ui->DaytableWidget->setColumnWidth(9,120);
    ui->DaytableWidget->setColumnWidth(10,80);
    ui->DaytableWidget->setColumnWidth(11,80);
    ui->DaytableWidget->setColumnWidth(12,120);

}

/*****初始化函数：日报表员工树*****/
void CDAY::_InitTree()
{
    m_List.clear();
    ui->Day_treeWidget->clear();
    ui->Day_treeWidget->setHeaderHidden(true);
    QString SQL = "SELECT * from department ORDER BY Department_No DESC";
    QSqlQuery query;
    if(query.exec(SQL))
    {
        int i = 1;
        while(query.next())
        {
            SList data;
            data.Id = query.value(0).toInt();
            data.strNo = query.value(1).toString();
            data.strName = query.value(2).toString();
            m_List.append(data);
            i++;
        }
    }

    QTreeWidgetItem* B1 = new QTreeWidgetItem(QStringList()<<"在职");
    ui->Day_treeWidget->addTopLevelItem(B1);

    QTreeWidgetItem* C = new QTreeWidgetItem(QStringList()<<"未分配");
    B1->addChild(C);
    QString unabsorbedSQL;
    QSqlQuery unabsorbedquery;
    unabsorbedSQL = "SELECT a.Staff_Name,a.Staff_No FROM `staff` a WHERE a.Staff_No NOT IN (SELECT b.Staff_No FROM staff b,department c WHERE b.Staff_Department_No = c.Department_No) ORDER BY a.Staff_No ASC";
    if(unabsorbedquery.exec(unabsorbedSQL))
    {
        while(unabsorbedquery.next())
        {
            QTreeWidgetItem *CB = new QTreeWidgetItem(QStringList()<<unabsorbedquery.value(0).toString());
            C->addChild(CB);
        }
    }

    QTreeWidgetItem* B[m_List.size()];
    for(int i = 0;i < m_List.size();i++)
    {
        B[i] = new QTreeWidgetItem(QStringList()<<m_List.at(i).strName);
        B1->addChild(B[i]);
        QString StaffSQL;
        QSqlQuery Staffquery;
        StaffSQL = "SELECT a.Staff_Name FROM `staff` a where  a.Staff_Department_No = '"+m_List.at(i).strNo+"' ORDER BY a.Staff_No ASC";
        if(Staffquery.exec(StaffSQL))
        {
            while(Staffquery.next())
            {
                QTreeWidgetItem *BiB = new QTreeWidgetItem(QStringList()<<Staffquery.value(0).toString());
                B[i]->addChild(BiB);
            }
        }

    }

    QTreeWidgetItem* B2 = new QTreeWidgetItem(QStringList()<<"离职");
    ui->Day_treeWidget->addTopLevelItem(B2);
    QString LeaveSQL;
    LeaveSQL = "SELECT a.Leave_Staff_Name FROM `leave_staff` a ORDER BY a.Leave_Staff_Id ASC";
    QSqlQuery Leavequery;
    if(Leavequery.exec(LeaveSQL))
    {
        while(Leavequery.next())
        {
            QTreeWidgetItem *B2B = new QTreeWidgetItem(QStringList()<<Leavequery.value(0).toString());
            B2->addChild(B2B);
        }
    }
}

/*****更新函数：日报表数据*****/
void CDAY::_UpdateDetail(QString strNo, int nFlag, QString strName)
{
    QDate StartDate;
    StartDate.setYMD(ui->Button_Year->text().toInt(),ui->Button_Month->text().toInt(),1);
    QDate EndDate;
    EndDate.setYMD(ui->Button_Year->text().toInt(),ui->Button_Month->text().toInt(),StartDate.daysInMonth());
    QString strSTime = StartDate.toString("yyyy-MM-dd");
    QString strETime = EndDate.toString("yyyy-MM-dd");
    int n_WorkHour =0;
    int n_WorkMimute =0;
    int n_OFFSpaceMinute=0;
    int n_ONSpaceMinute=0;
    int n_overTime=0;
    int n_earlierTime=0;
    QString SQL,Holid_SQL;
    if(nFlag == 0)
    {//离职

        SQL = "SELECT a.Leave_Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,a.Leave_Staff_No,b.Card_Record_Type,c.SetAttendance_Attendance_Time,"
                "c.SetAttendance_Closeing_Time,c.SetAttendance_Saturday_Attendance_Time,c.SetAttendance_Saturday_Closeing_Time,c.SetAttendance_Sunday_Attendance_Time,c.SetAttendance_Sunday_Closeing_Time FROM `leave_staff` a,card_record b,setattendance c WHERE "
                "a.Leave_Staff_No = b.Card_Record_Staff_No and b.Card_Record_Date >= a.Leave_Staff_Enter_Day  and b.Card_Record_Date <= a.Leave_Deal_Day "
                " and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"'  ORDER BY a.Leave_Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
    }
    else if(nFlag == 1)
    {//部门

        SQL = "SELECT a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,a.Staff_No,b.Card_Record_Type,c.SetAttendance_Attendance_Time,"
                "c.SetAttendance_Closeing_Time,c.SetAttendance_Saturday_Attendance_Time,c.SetAttendance_Saturday_Closeing_Time,c.SetAttendance_Sunday_Attendance_Time,c.SetAttendance_Sunday_Closeing_Time FROM `staff` a,card_record b,setattendance c WHERE "
                "a.Staff_No = b.Card_Record_Staff_No and a.Staff_Department_No = '"+strNo+"' and b.Card_Record_Date >= a.Staff_Enter_Day  "
                " and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
    }
    else if(nFlag == 2)
    {//在职

        SQL = "SELECT a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,a.Staff_No,b.Card_Record_Type,"
                "c.SetAttendance_Attendance_Time,c.SetAttendance_Closeing_Time,c.SetAttendance_Saturday_Attendance_Time,c.SetAttendance_Saturday_Closeing_Time,c.SetAttendance_Sunday_Attendance_Time,c.SetAttendance_Sunday_Closeing_Time FROM `staff` a,card_record b,setattendance c WHERE "
                "b.Card_Record_Date >= a.Staff_Enter_Day and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' "
                "and  a.Staff_No = b.Card_Record_Staff_No ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
    }
    else if(nFlag == 3)
    {//姓名

        if(m_nFlag)
        {
            if(strName == "")
            {
                SQL =" SELECT a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,a.Staff_No,b.Card_Record_Type,"
                        "c.SetAttendance_Attendance_Time,c.SetAttendance_Closeing_Time,c.SetAttendance_Saturday_Attendance_Time,c.SetAttendance_Saturday_Closeing_Time,c.SetAttendance_Sunday_Attendance_Time,c.SetAttendance_Sunday_Closeing_Time FROM"
                        "`staff` a,card_record b,setattendance c WHERE a.Staff_No = b.Card_Record_Staff_No and a.Staff_Name is NULL and "
                        "b.Card_Record_Date >= a.Staff_Enter_Day and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' "
                        "ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
            }
            else
            {
                SQL =" SELECT a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,a.Staff_No,b.Card_Record_Type,"
                        "c.SetAttendance_Attendance_Time,c.SetAttendance_Closeing_Time,c.SetAttendance_Saturday_Attendance_Time,c.SetAttendance_Saturday_Closeing_Time,c.SetAttendance_Sunday_Attendance_Time,c.SetAttendance_Sunday_Closeing_Time FROM"
                        "`staff` a,card_record b,setattendance c WHERE a.Staff_No = b.Card_Record_Staff_No and a.Staff_Name = '"+strName+"' and "
                        "b.Card_Record_Date >= a.Staff_Enter_Day and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' "
                        "ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
            }
        }
        else
        {
            SQL =" SELECT a.Leave_Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,a.Leave_Staff_No,b.Card_Record_Type,"
                    "c.SetAttendance_Attendance_Time,c.SetAttendance_Closeing_Time,c.SetAttendance_Saturday_Attendance_Time,c.SetAttendance_Saturday_Closeing_Time,c.SetAttendance_Sunday_Attendance_Time,c.SetAttendance_Sunday_Closeing_Time FROM"
                    "`leave_staff` a,card_record b,setattendance c WHERE a.Leave_Staff_No = b.Card_Record_Staff_No and a.Leave_Staff_Name = '"+strName+"' "
                    "and b.Card_Record_Date >= a.Leave_Staff_Enter_Day  and b.Card_Record_Date <= a.Leave_Deal_Day "
                    " and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' "
                    "ORDER BY a.Leave_Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
        }
    }
    else if(nFlag == 4)
    {//部门不存在
        SQL = "SELECT a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,a.Staff_No,b.Card_Record_Type,c.SetAttendance_Attendance_Time,"
                "c.SetAttendance_Closeing_Time,c.SetAttendance_Saturday_Attendance_Time,c.SetAttendance_Saturday_Closeing_Time,c.SetAttendance_Sunday_Attendance_Time,c.SetAttendance_Sunday_Closeing_Time FROM (SELECT d.Staff_Name,d.Staff_No,d.Staff_Enter_Day,d.Staff_Department_No "
                "FROM (SELECT f.Staff_No,f.Staff_Name,f.Staff_Enter_Day,f.Staff_Department_No FROM `staff`"
                " f WHERE f.Staff_No NOT IN (SELECT e.Staff_No FROM staff e,department g WHERE e.Staff_Department_No = g.Department_No)) d) a,card_record b,setattendance c WHERE "
                "a.Staff_No = b.Card_Record_Staff_No  and b.Card_Record_Date >= a.Staff_Enter_Day  "
                " and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
    }

    Holid_SQL = "SELECT * FROM holiday ORDER BY Holiday_StartDay";
    QSqlQuery qeruy,Holid_query;

    bool bSQL = qeruy.exec(SQL);

    QDate OldDate(0,0,0);
    int OldType=0;
    QString strNO1 = "";

    if(bSQL)
    {
        int ti=0;
        int num;
        QTime onCardTime;
        QTime offCardTime;
        ui->DaytableWidget->setRowCount(ti);
        while(qeruy.next())
        {
            if( strNO1 != QString(qeruy.value(4).toByteArray().data()) || !(OldDate == qeruy.value(1).toDate()&&qeruy.value(5).toInt()==OldType))
            {
                if(strNO1 != QString(qeruy.value(4).toByteArray().data())||OldDate != qeruy.value(1).toDate())
                {
                    num = 0;
                    n_WorkHour =0;
                    n_WorkMimute =0;
                    n_OFFSpaceMinute=0;
                    n_ONSpaceMinute=0;
                    n_overTime=0;
                    n_earlierTime=0;
                    isTakeWorks = false;
                    onCardTime.setHMS(0,0,0);
                    offCardTime.setHMS(0,0,0);
                }
                QTime NormalONTime;
                QTime NormalOFFTime;
                int n_NormalSpaceMinute;
                emit SigcardDate(qeruy.value(1).toDate().year(),qeruy.value(1).toDate().month(),qeruy.value(1).toDate().day());
                switch(qeruy.value(1).toDate().dayOfWeek())
                {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    {
                        NormalONTime=qeruy.value(6).toTime();
                        NormalOFFTime = qeruy.value(7).toTime();
                        break;
                    }
                    case 6:
                    {
                        NormalONTime=qeruy.value(8).toTime();
                        NormalOFFTime = qeruy.value(9).toTime();
                        break;
                    }
                    case 7:
                    {
                        NormalONTime=qeruy.value(10).toTime();
                        NormalOFFTime = qeruy.value(11).toTime();
                        break;
                    }
                    default:
                        break;

                }

                if(Holid_query.exec(Holid_SQL))
                {
                    isholiday = false;
                    while(Holid_query.next())
                    {
                        for(int i =0;i<Holid_query.value(4).toInt();i++)
                        {
                            if(Holid_query.value(3).toDate().addDays(i).toString("yyyy-MM-dd")==qeruy.value(1).toDate().toString("yyyy-MM-dd"))
                            {
                                 isholiday = true;
                                 break;
                            }

                        }
                    }
                }

                if(Holid_query.exec(Holid_SQL))
                {
                    while(Holid_query.next())
                    {

                            switch(Holid_query.value(1).toInt())
                            {
                                case 0:
                                case 2:
                                case 3:
                                    break;
                                case 4:
                                case 5:
                                {
                                    n_List.append(Holid_query.value(3).toDate().addDays(3));
                                }
                                    break;
                                case 1:
                                case 6:
                                {
                                    n_List.append(Holid_query.value(3).toDate().addDays(-1));
                                    n_List.append(Holid_query.value(3).toDate().addDays(7));
                                }
                                    break;
                                default:
                                    break;
                        }
                    }
                }

                for(int i=0;i<n_List.length();i++)
                {
                    if(n_List.at(i).toString("yyyy-MM-dd")==qeruy.value(1).toDate().toString("yyyy-MM-dd"))
                    {
                        isTakeWorks = true;
                        break;
                    }
                }

                n_NormalSpaceMinute=(NormalONTime.secsTo(NormalOFFTime)-5400)/60;
                if(OldDate == qeruy.value(1).toDate())//如果本条数据跟上条数据同日期,不换行
                    ti--;
                ui->DaytableWidget->setRowCount(ti+1);
                if(qeruy.value(5).toInt()==1)//上班卡
                {
                    num++;
                    onCardTime = qeruy.value(2).toTime();
                    int n_SpaceSec = NormalONTime.secsTo(onCardTime);
                    n_ONSpaceMinute = n_SpaceSec/60;
                    n_ONSpaceMinute=n_ONSpaceMinute<240?n_ONSpaceMinute:240;
                    n_ONSpaceMinute=n_ONSpaceMinute>0?n_ONSpaceMinute:0;
                    n_WorkMimute+=((n_NormalSpaceMinute/2)-n_ONSpaceMinute)%60;
                    n_WorkHour+=((n_NormalSpaceMinute/2)-n_ONSpaceMinute)/60+n_WorkMimute/60;
                    n_WorkMimute=n_WorkMimute%60;
                 }
                 else if(qeruy.value(5).toInt()==2)//下班卡
                 {
                    num++;
                    QTime offCardTime = qeruy.value(2).toTime();
                    int n_SpaceSec = offCardTime.secsTo(NormalOFFTime);
                    n_OFFSpaceMinute = n_SpaceSec/60;
                    n_OFFSpaceMinute=n_OFFSpaceMinute<240?n_OFFSpaceMinute:240;
                    n_WorkMimute+=((n_NormalSpaceMinute/2)-n_OFFSpaceMinute)%60;
                    n_WorkHour+=((n_NormalSpaceMinute/2)-n_OFFSpaceMinute)/60+n_WorkMimute/60;
                    n_WorkMimute=n_WorkMimute%60;
                    if(n_OFFSpaceMinute<0)//加班
                    {
                        n_overTime=(~n_OFFSpaceMinute);
                    }
                    else if(n_OFFSpaceMinute>0)//早退
                    {
                        n_earlierTime=n_OFFSpaceMinute;
                    }

                }
                QTableWidgetItem *tableItem0 = new QTableWidgetItem(QString(qeruy.value(4).toByteArray().data()));
                tableItem0->setTextAlignment(Qt::AlignCenter);
                ui->DaytableWidget->setItem(ti,0,tableItem0);

                QTableWidgetItem *tableItem1 = new QTableWidgetItem(QString(qeruy.value(0).toByteArray().data()));
                tableItem1->setTextAlignment(Qt::AlignCenter);
                ui->DaytableWidget->setItem(ti,1,tableItem1);

                QTableWidgetItem *tableItem2 = new QTableWidgetItem(QString(qeruy.value(1).toDate().toString("yyyy-MM-dd")));
                tableItem2->setTextAlignment(Qt::AlignCenter);
                ui->DaytableWidget->setItem(ti,2,tableItem2);

                QString strWeek;
                switch(qeruy.value(1).toDate().dayOfWeek())
                {
                    case 1: strWeek="一";
                            break;
                    case 2: strWeek="二";
                            break;
                    case 3: strWeek="三";
                            break;
                    case 4: strWeek="四";
                            break;
                    case 5: strWeek="五";
                            break;
                    case 6: strWeek="六";
                            break;
                    case 7: strWeek="日";
                            break;
                    default:strWeek="错误";
                            break;
                }
                QTableWidgetItem *tableItem3 = new QTableWidgetItem("星期"+strWeek);
                tableItem3->setTextAlignment(Qt::AlignCenter);
                ui->DaytableWidget->setItem(ti,3,tableItem3);

                QString class1;

                switch(qeruy.value(1).toDate().dayOfWeek())
                {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    {
                        if(isTakeWorks)
                            class1 = "调休";
                        else
                        {
                            if(isholiday)
                                class1="假休";
                            else
                                class1="默认班次";
                        }
                    }
                    break;
                    case 6:
                    {
                        if(isTakeWorks)
                            class1 = "调休";
                        else
                        {
                            if(restType=="4"||restType=="5"||restType=="2")
                            {
                                if(isholiday)
                                    class1="假休";
                                else
                                    class1="休";
                            }
                            else
                                class1="周六上班";
                        }

                    }
                    break;
                    case 7:
                    {
                        if(isTakeWorks)
                            class1 = "调休";
                        else
                        {
                            if(isholiday)
                                class1="假休";
                            else
                                class1="休";
                        }
                    }
                    break;
                    default:    class1="错误";
                        break;
                }
                QTableWidgetItem *tableItem4 = new QTableWidgetItem(class1);
                tableItem4->setTextAlignment(Qt::AlignCenter);
                ui->DaytableWidget->setItem(ti,4,tableItem4);

                strNO1 = QString(qeruy.value(4).toByteArray().data());
                ti++;

                OldDate = qeruy.value(1).toDate();
                OldType = qeruy.value(5).toInt();

            }
            if(num>2)
            {
               num=2;
            }

            int notime = 2-num;
            QString staffday;
            QString noday;
            switch(num)
            {
                case 1:
                {
                    staffday = "0.5";
                    noday = "0.5";
                }
                    break;
                case 2:
                {
                    staffday = "1.0";
                    noday = "0";
                }
                    break;
                default:
                    break;
            }
            QTableWidgetItem *tableItem5;
            QTableWidgetItem *tableItem7;
            QTableWidgetItem *tableItem8;
            QTableWidgetItem *tableItem10;
            QTableWidgetItem *tableItem11;
            QTableWidgetItem *tableItem12;
            if(ui->DaytableWidget->item(ti-1,4)->text()=="休"||ui->DaytableWidget->item(ti-1,4)->text()=="假休")
            {
                int n_SpaceSec = offCardTime.secsTo(onCardTime);
                int n_SpaceHour = n_SpaceSec/60/60;
                int n_SpaceMinute = n_SpaceSec/60%60;
                tableItem5 = new QTableWidgetItem(QString::number(0));
                tableItem7 = new QTableWidgetItem(QString::number(0));
                tableItem8 = new QTableWidgetItem(QString::number(0));
                tableItem10 = new QTableWidgetItem(QString::number(0));
                tableItem11 = new QTableWidgetItem(QString::number(0));
                tableItem12 = new QTableWidgetItem(QString::number(n_WorkHour)+"时"+QString::number(n_WorkMimute)+"分");
            }
            else
            {
                tableItem5 = new QTableWidgetItem(QString::number(1));
                tableItem7 = new QTableWidgetItem(noday);
                tableItem8 = new QTableWidgetItem(QString::number(notime));
                tableItem10 = new QTableWidgetItem(QString::number(n_ONSpaceMinute));
                tableItem11 = new QTableWidgetItem(QString::number(n_earlierTime));
                tableItem12 = new QTableWidgetItem(QString::number(n_overTime/60)+"时"+QString::number(n_overTime%60)+"分");
            }

            tableItem5->setTextAlignment(Qt::AlignCenter);
            ui->DaytableWidget->setItem(ti-1,5,tableItem5);

            QTableWidgetItem *tableItem6 = new QTableWidgetItem(staffday);
            tableItem6->setTextAlignment(Qt::AlignCenter);
            ui->DaytableWidget->setItem(ti-1,6,tableItem6);


            tableItem7->setTextAlignment(Qt::AlignCenter);
            ui->DaytableWidget->setItem(ti-1,7,tableItem7);


            tableItem8->setTextAlignment(Qt::AlignCenter);
            ui->DaytableWidget->setItem(ti-1,8,tableItem8);

            QTableWidgetItem *tableItem9 = new QTableWidgetItem(QString::number(n_WorkHour)+"时"+QString::number(n_WorkMimute)+"分");
            tableItem9->setTextAlignment(Qt::AlignCenter);
            ui->DaytableWidget->setItem(ti-1,9,tableItem9);

            tableItem10->setTextAlignment(Qt::AlignCenter);
            ui->DaytableWidget->setItem(ti-1,10,tableItem10);


            tableItem11->setTextAlignment(Qt::AlignCenter);
            ui->DaytableWidget->setItem(ti-1,11,tableItem11);

            tableItem12->setTextAlignment(Qt::AlignCenter);
            ui->DaytableWidget->setItem(ti-1,12,tableItem12);
            for(int i =5;i<=12;i++)
            {
                QString context = ui->DaytableWidget->item(ti-1,i)->text();
                if(i==6||i == 9||i==12)
                {
                    if(context.left(1)=="0"&&context.left(3).right(1)=="0")
                        ui->DaytableWidget->item(ti-1,i)->setBackgroundColor(QColor(185,235,200));
                }
                else
                {
                    if(context == "0")
                        ui->DaytableWidget->item(ti-1,i)->setBackgroundColor(QColor(185,235,200));
                    else if(i == 10||i == 11)
                        ui->DaytableWidget->item(ti-1,i)->setBackgroundColor(QColor(235,193,184));
                }
            }
        }
    }

}

/*****选择员工，显示当月的日报表数据*****/
void CDAY::_SelectTarget(QString SelectName)
{
    m_nFlag = 1;
    QString strNo = "";
    QString strName = SelectName;
    if(strName == "在职"){
        _UpdateDetail(strNo,2,"");
    }else if(strName == "离职"){

        m_nFlag = 0;
        _UpdateDetail(strNo,0,"");
    }else{
        int j = 0;
        for(int i = 0;i < m_List.size();i++)
        {
            if(strName == m_List.at(i).strName)
            {
                strNo = m_List.at(i).strNo;
                j = 1;
                break;
            }
        }
        if(j){
        _UpdateDetail(strNo,1,"");
        }else if(strName == "未分配"){
            _UpdateDetail(strNo,4,"");
        }
        else
        {
             _UpdateDetail(0,3,strName);
        }
    }
}

void CDAY::on_Day_treeWidget_clicked(const QModelIndex &index)
{
    FindName.clear();
    FindName = ui->Day_treeWidget->currentItem()->text(0);
    if(!FindName.isEmpty())
        _SelectTarget(FindName);
}

/*****报表导出：生成EXCL文件*****/
void CDAY::on_QpbtnLead_clicked()
{
    if(ui->DaytableWidget->rowCount()>0)
    {
        QString filepath = QFileDialog::getSaveFileName(this,tr("save day"),".",tr("Microsoft Office 2003 (*.xls)"));
        if(!filepath.isEmpty())
        {
            QAxObject *excel = new QAxObject(this);
            excel->setControl("Excel.Application");
            excel->dynamicCall("SetVisibel (bool Visibel)","false");
            excel->setProperty("DisplayAlerts",false);

            QAxObject *workbooks = excel->querySubObject("WorkBooks");
            workbooks->dynamicCall("Add");
            QAxObject *workbook = excel->querySubObject("ActiveWorkBook");
            QAxObject *worksheets = workbook->querySubObject("Sheets");
            QAxObject *worksheet = worksheets->querySubObject("Item(int)",1);

            worksheet->setProperty("Name","day");

            QAxObject *cell[9];
            for(int i =1;i<=9;i++)
            {
                cell[i-1] =  worksheet->querySubObject("Cells(int,int)",1,i);
                QString str = "";
                switch(i)
                {
                case 1: str="编号";
                    break;
                case 2: str="姓名";
                    break;
                case 3: str="刷卡日期";
                    break;
                case 4: str="星期";
                    break;
                case 5: str="班次名称";
                    break;
                case 6: str="应出勤天数";
                    break;
                case 7: str="实出勤天数";
                    break;
                case 8: str="缺勤天数";
                    break;
                case 9: str="少打卡次数";
                    break;
                default:
                    break;
                }

                cell[i-1]->setProperty("Value", str);

                if(i==3)
                {
                    cell[i-1]->setProperty("ColumnWidth", 15);
                }
                cell[i-1]->setProperty("HorizontalAlignment", -4108); //左对齐（xlLeft）：-4131  居中（xlCenter）：-4108  右对齐（xlRight）：-4152
                cell[i-1]->setProperty("VerticalAlignment", -4108);  //上对齐（xlTop）-4160 居中（xlCenter）：-4108  下对齐（xlBottom）：-4107
                cell[i-1]->setProperty("WrapText", true);  //内容过多，自动换行
            }
            QAxObject *NewCell[ui->DaytableWidget->rowCount()][9];
            for(int i=1;i<=ui->DaytableWidget->rowCount();i++)
            {
                for(int j=1;j<=9;j++)
                {
                    NewCell[i-1][j-1] = worksheet->querySubObject("Cells(int,int)",i+1,j);
                    NewCell[i-1][j-1]->setProperty("Value",ui->DaytableWidget->item(i-1,j-1)->text());
                    NewCell[i-1][j-1]->setProperty("HorizontalAlignment", -4108); //左对齐（xlLeft）：-4131  居中（xlCenter）：-4108  右对齐（xlRight）：-4152
                    NewCell[i-1][j-1]->setProperty("VerticalAlignment", -4108);  //上对齐（xlTop）-4160 居中（xlCenter）：-4108  下对齐（xlBottom）：-4107
                    NewCell[i-1][j-1]->setProperty("WrapText", true);  //内容过多，自动换行
                }
            }
            workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(filepath));//保存至filepath，注意一定要用QDir::toNativeSeparators将路径中的"/"转换为"\"，不然一定保存不了。
            workbook->dynamicCall("Close()");//关闭工作簿
            excel->dynamicCall("Quit()");//关闭excel
            delete excel;
            excel=NULL;
        }
    }
    else
    {
        QMessageBox::information(this,"提示","没有记录");

    }
}

void CDAY::SlotrestType(QString m_restType)
{
    restType = m_restType;
}

/*****槽函数：月份被点击******/
void CDAY::on_Button_Month_clicked()
{
    isYear = false;
}

/*****槽函数：年份被点击*****/
void CDAY::on_Button_Year_clicked()
{
    isYear = true;
}

/*****槽函数：左日期选择按钮*****/
void CDAY::on_Qpbtn_Left_clicked()
{
    int Year = ui->Button_Year->text().toInt();
    int Month = ui->Button_Month->text().toInt();

    if(isYear)  //默认是变动年份，月份不变
    {
        Year--;
        ui->Button_Year->setText(QString::number(Year));
    }
    else
    {
        Month--;
        if(Month>0)
            ui->Button_Month->setText(QString::number(Month));
        else
        {
          Year--;
          ui->Button_Year->setText(QString::number(Year));
          ui->Button_Month->setText(QString::number(12));
        }
    }
    FindName = "在职";
    _SelectTarget(FindName);
}

/*****槽函数：右日期选择按钮*****/
void CDAY::on_Qpbtn_Right_clicked()
{
    int Year = ui->Button_Year->text().toInt();
    int Month = ui->Button_Month->text().toInt();

    if(isYear)  //默认是变动年份，月份不变
    {
        Year++;
        ui->Button_Year->setText(QString::number(Year));
    }
    else
    {
        Month++;
        if(Month<13)
            ui->Button_Month->setText(QString::number(Month));
        else
        {
          Year++;
          ui->Button_Year->setText(QString::number(Year));
          ui->Button_Month->setText(QString::number(1));
        }
    }
    FindName = "在职";
    _SelectTarget(FindName);
}

/*****槽函数：返回按钮*****/
void CDAY::on_QpbtnBack_clicked()
{
    emit SigReturnMenu();
}

/*****槽函数：搜索按钮*****/
void CDAY::on_QpbtnSearch_clicked()
{
    FindName.clear();
    FindName = ui->SearchData->text();
    if(!FindName.isEmpty())
        _SelectTarget(FindName);
}

void CDAY::on_QpbtnStretching_clicked()
{
    int posX = 160-ui->QpbtnStretching->width();
    int posY = ui->QpbtnStretching->y();
    int nwidth = ui->QpbtnStretching->width();
    int nheight = ui->QpbtnStretching->height();
    int DayMinWidht =0;
    int DayMaxWidht = 160;
    if(ui->QpbtnStretching->pos().x()==posX)
    {
        ui->Day_treeWidget->setGeometry(ui->Day_treeWidget->x(),ui->Day_treeWidget->y(),DayMinWidht,ui->Day_treeWidget->height());
        ui->QpbtnStretching->setGeometry(0,posY,nwidth,nheight);
        ui->QpbtnStretching->setStyleSheet("background-image:url(:/images/StretchingR.png)");
    }
    else if(ui->QpbtnStretching->pos().x()==0)
    {
        ui->Day_treeWidget->setGeometry(ui->Day_treeWidget->x(),ui->Day_treeWidget->y(),DayMaxWidht,ui->Day_treeWidget->height());
        ui->QpbtnStretching->setGeometry(posX,posY,nwidth,nheight);
        ui->QpbtnStretching->setStyleSheet("background-image:url(:/images/StretchingL.png)");
    }

}



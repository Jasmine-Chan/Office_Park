#include "attendance.h"
#include "ui_attendance.h"

CATTENDANCE::CATTENDANCE( QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CATTENDANCE)
{
    ui->setupUi(this);
    currselecteDate.setDate(0,0,0);
    _InitTree3();           //初始化函数：考勤系统树
    _InitTree4();           //初始化函数：考勤系统树
    _InitFullCardDate();    //初始化函数：补打卡
    _InitDoorMachine();     //初始化函数：信号采集
    _InitHolidayDate();     //初始化函数：法定节假日
    _InitFillcardTree();    //初始化函数：补打卡员工树
    _UpdateDoorMachine();   //更新函数：信号采集
    _UpdateHolidayDate();   //更新函数：法定节假日
    _InitCardList();        //更新函数：补卡链表数据
    isholiday = false;
    istakeworks = false;
//    ui->toolBox_2->setStyleSheet("QToolBoxButton { min-height:45px; font-size:12pt;background-color:rgb(105,150,177)})");

    DeleteAction = new QAction(tr("删除所选记录"),this);
    Holiday = new QAction(tr("设置法定节假日"),this);
    DeleteHoliday = new QAction(tr("删除节日"),this);
    FilloutAction = new QAction(tr("补卡"),this);

    CardDataMenu = new QMenu(ui->FullCardDate);
    ui->FullCardDate->setContextMenuPolicy(Qt::CustomContextMenu);

    HolidayMenu = new QMenu(ui->HolidayCalendarWidget);
    HolidayMenu->setStyleSheet("border:1px solid rgb(180,202,216);background-color:white;selection-background-color:rgb(180,202,216);color:black");
    ui->HolidayCalendarWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    DelHolidayMenu = new QMenu(ui->tableWidget);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    FilloutMneu = new QMenu(ui->Fillcard_tree);
    ui->Fillcard_tree->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(DeleteAction,SIGNAL(triggered()),this,SLOT(SlotDelete()));      //设置右键菜单项->删除所选记录
    connect(ui->pbtnAcquisition,SIGNAL(clicked()),this,SLOT(SlotAcquisition()));
    connect(Holiday,SIGNAL(triggered()),this,SLOT(SlotSetHoliday()));       //设置右键菜单项->设置法定节假日
    connect(DeleteHoliday,SIGNAL(triggered()),this,SLOT(SlotDelHoliday())); //设置右键菜单项->删除节日
    connect(FilloutAction,SIGNAL(triggered()),this,SLOT(SlotFillout()));    //设置右键菜单项->补卡

    connect(ui->Fillcard_tree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(CheckSelf(QTreeWidgetItem*,int)));

    ui->racddate_Bar->setAlignment(Qt::AlignRight);
    ui->racddate_Bar->hide();

    AttendanceSet = new CATTENDANCESET;//考勤设置
    layout = new QVBoxLayout;
    layout->addWidget(AttendanceSet);
    layout->setMargin(0);
    ui->QstackeAttendanceSet->setLayout(layout);

    Original = new CORIGINAL;//原始考勤表
    layout = new QVBoxLayout;
    layout->addWidget(Original);
    layout->setMargin(0);
    ui->QstackeOriginal->setLayout(layout);

    Day = new CDAY;//考勤日报表
    layout = new QVBoxLayout;
    layout->addWidget(Day);
    layout->setMargin(0);
    ui->QstackeDay->setLayout(layout);

    Detail = new CDETAIL;//考勤明细表
    layout = new QVBoxLayout;
    layout->addWidget(Detail);
    layout->setMargin(0);
    ui->QstackeDetail->setLayout(layout);

    Monthly = new CMONTHLY;//考勤月报表
    layout = new QVBoxLayout;
    layout->addWidget(Monthly);
    layout->setMargin(0);
    ui->QstackeMonthly->setLayout(layout);

    Askforleave = new askforleave;//请/休假
    layout = new QVBoxLayout;
    layout->addWidget(Askforleave);
    layout->setMargin(0);
    ui->QstackeLeave->setLayout(layout);

    connect(AttendanceSet,SIGNAL(SigMonthlyrestType(QString)),Monthly,SLOT(SlotrestType(QString)));
    connect(Monthly,SIGNAL(SigcardDate(int,int,int)),AttendanceSet,SLOT(SlotMonthlycardDate(int,int,int)));
    connect(AttendanceSet,SIGNAL(SigDayrestType(QString)),Day,SLOT(SlotrestType(QString)));
    connect(Day,SIGNAL(SigcardDate(int,int,int)),AttendanceSet,SLOT(SlotDaycardDate(int,int,int)));

    connect(this,SIGNAL(SigFillcardDate(QDate)),AttendanceSet,SLOT(SlotFillcardDate(QDate)));
    connect(AttendanceSet,SIGNAL(SigFillcardRestType(QString)),this,SLOT(SlotFillcardRestType(QString)));
    connect(this,SIGNAL(SigFocusOut()),Askforleave,SLOT(SlotFocusOut()));
    connect(Original,SIGNAL(SigReturnMenu()),this,SLOT(SlotReturnMenu()));
    connect(Detail,SIGNAL(SigReturnMenu()),this,SLOT(SlotReturnMenu()));
    connect(Day,SIGNAL(SigReturnMenu()),this,SLOT(SlotReturnMenu()));
    connect(Monthly,SIGNAL(SigReturnMenu()),this,SLOT(SlotReturnMenu()));

    QDateTime dateTime = QDateTime::currentDateTime();
    ui->QdateEtime->setDate(dateTime.date());
    ui->QdateStime->setDate(dateTime.date().addDays(-15));
    ui->QdateEtime->setCalendarPopup(true); //设置显示日历选择弹出窗口
    ui->QdateStime->setCalendarPopup(true);
    ui->QdateEtime->setEnabled(false);
    ui->QdateStime->setEnabled(false);
}

CATTENDANCE::~CATTENDANCE()
{
    delete ui;
}

/********检测“补卡员工树”中是否有item被选中********/
void CATTENDANCE::CheckSelf(QTreeWidgetItem *hItem, int)
{
    m_FullCard.clear(); //在更新TreeWidget"补卡员工树"之前，需要先清空

    //hItem->text(0):获取选中的item中的内容
    QString SQL;
    QSqlQuery query;
    QString StrStime = ui->QdateStime->dateTime().toString("yyyy-MM-dd");   //获取QDateEdit的开始时间
    QString StrEtime = ui->QdateEtime->dateTime().toString("yyyy-MM-dd");   //获取QDateEdit的结束时间
    QString staff_name = hItem->text(0);

    QString SQLDepartment = "SELECT * FROM department WHERE Department_Name = '"+ staff_name +"'";
    QSqlQuery QueryDepartment;

    QString SQLLeaveStaff = "SELECT * FROM leave_staff WHERE Leave_Staff_Name = '"+ staff_name +"'";
    QSqlQuery QueryLeaveStaff;
    QueryLeaveStaff.exec(SQLLeaveStaff);
    if(QueryLeaveStaff.next())
    {
        QMessageBox::information(this, "提示", staff_name+"职员已离职");
        return;
    }

    QueryDepartment.exec(SQLDepartment);
    if(QueryDepartment.next())
    {
        if(ui->QchkStime->isChecked() && ui->QchkEtime->isChecked())    //选择了开始时间和结束时间
        {
            if(StrStime > StrEtime) //判断查询的开始日期是否比结束日期大
            {
                QMessageBox::critical(this, "错误","查询的开始日期比结束日期大，请重新选择日期");
                return;
            }
            else
            {
                SQL = "SELECT a.* FROM fillcard a WHERE FillCard_Date between '"+StrStime+"' and '"+StrEtime+"' and FillCard_Staff_No in (select Staff_No from staff where Staff_Department_No = '"+ QueryDepartment.value(1).toString() +"') ORDER BY a.FillCard_Staff_No,a.FillCard_Date";
            }
        }
        else if(ui->QchkStime->isChecked())     //只选择了开始时间
        {
            SQL = "SELECT a.* FROM fillcard a WHERE FillCard_Date >= '"+StrStime+"' and FillCard_Staff_No in (select Staff_No from staff where Staff_Department_No = '"+ QueryDepartment.value(1).toString() +"') ORDER BY a.FillCard_Staff_No,a.FillCard_Date";
        }
        else if(ui->QchkEtime->isChecked())       //只选择了结束时间
        {
            SQL = "SELECT a.* FROM fillcard a WHERE FillCard_Date <= '"+StrEtime+"' and FillCard_Staff_No in (select Staff_No from staff where Staff_Department_No = '"+ QueryDepartment.value(1).toString() +"') ORDER BY a.FillCard_Staff_No,a.FillCard_Date";
        }
        else        //1、既没选开始时间，也没选结束时间     2、程序开始执行时，首先执行这个分支
        {
            SQL = "SELECT * FROM fillcard WHERE FillCard_Staff_No in (select Staff_No from staff where Staff_Department_No = '"+ QueryDepartment.value(1).toString() +"') ORDER BY FillCard_Staff_No,FillCard_Date";
        }
    }
    else
    {
        if(ui->QchkStime->isChecked() && ui->QchkEtime->isChecked())    //选择了开始时间和结束时间
        {
            if(StrStime > StrEtime)     //判断查询的开始日期是否比结束日期大
            {
                QMessageBox::critical(this, "错误","查询的开始日期比结束日期大，请重新选择日期");
                return;
            }
            else
            {
                if(staff_name == "在职")
                {
                    SQL = "SELECT * FROM fillcard WHERE FillCard_Date between '"+StrStime+"' and '"+StrEtime+"'and FillCard_Staff_No in (SELECT Staff_No FROM staff) ORDER BY FillCard_Staff_No, FillCard_Date";
                }
                else if(staff_name == "未分配")
                {
                    SQL = "SELECT * FROM fillcard WHERE FillCard_Date between '"+StrEtime+"' and '"+StrEtime+"'and FillCard_Staff_No in (SELECT Staff_No FROM staff WHERE Staff_Department_No = '') ORDER BY FillCard_Staff_No, FillCard_Date";
                }
                else
                {
                    SQL = "SELECT * FROM fillcard WHERE FillCard_Date between '"+StrEtime+"' and '"+StrEtime+"'and FillCard_Staff_No in (select Staff_No from staff where Staff_Name = '"+ staff_name +"') ORDER BY FillCard_Staff_No,FillCard_Date";
                }
            }
        }
        else if(ui->QchkStime->isChecked())     //只选择了开始时间
        {
            if(staff_name == "在职")
            {
                SQL = "SELECT * FROM fillcard WHERE FillCard_Date >= '"+StrStime+"' and FillCard_Staff_No in (SELECT Staff_No FROM staff) ORDER BY FillCard_Staff_No, FillCard_Date";
            }
            else if(staff_name == "未分配")
            {
                SQL = "SELECT * FROM fillcard WHERE FillCard_Date >= '"+StrStime+"' and FillCard_Staff_No in (SELECT Staff_No FROM staff WHERE Staff_Department_No = '') ORDER BY FillCard_Staff_No, FillCard_Date";
            }
            else
            {
                SQL = "SELECT * FROM fillcard WHERE FillCard_Date >= '"+StrStime+"' and FillCard_Staff_No in (select Staff_No from staff where Staff_Name = '"+ staff_name +"') ORDER BY FillCard_Staff_No,FillCard_Date";
            }
        }
        else if(ui->QchkEtime->isChecked())       //只选择了结束时间
        {
            if(staff_name == "在职")
            {
                SQL = "SELECT * FROM fillcard WHERE FillCard_Date <= '"+StrEtime+"' and FillCard_Staff_No in (SELECT Staff_No FROM staff) ORDER BY FillCard_Staff_No, FillCard_Date";
            }
            else if(staff_name == "未分配")
            {
                SQL = "SELECT * FROM fillcard WHERE FillCard_Date <= '"+StrEtime+"' and FillCard_Staff_No in (SELECT Staff_No FROM staff WHERE Staff_Department_No = '') ORDER BY FillCard_Staff_No, FillCard_Date";
            }
            else
            {
                SQL = "SELECT * FROM fillcard WHERE FillCard_Date <= '"+StrEtime+"' and FillCard_Staff_No in (select Staff_No from staff where Staff_Name = '"+ staff_name +"') ORDER BY FillCard_Staff_No,FillCard_Date";
            }
        }
        else        //1、既没选开始时间，也没选结束时间     2、程序开始执行时，首先执行这个分支
        {

            if(staff_name == "在职")
            {
                SQL = "SELECT * FROM fillcard WHERE FillCard_Staff_No in (SELECT Staff_No FROM staff) ORDER BY FillCard_Staff_No, FillCard_Date";
            }
            else if(staff_name == "未分配")
            {
                SQL = "SELECT * FROM fillcard WHERE FillCard_Staff_No in (SELECT Staff_No FROM staff WHERE Staff_Department_No = '') ORDER BY FillCard_Staff_No, FillCard_Date";
            }
            else
            {
                SQL = "SELECT * FROM fillcard WHERE FillCard_Staff_No in (select Staff_No from staff where Staff_Name = '"+ staff_name +"') ORDER BY FillCard_Staff_No,FillCard_Date";
            }
        }
    }
//    qDebug()<<SQL<<SQLDepartment;

    if(!query.first())
        ui->FullCardDate->clearContents();

    if(query.exec(SQL))
    {
        while(query.next())
        {
            SFullCard cardDate; //补卡信息结构体
            QString StaffSQL = "SELECT a.* FROM staff a WHERE a.Staff_No = '"+query.value(1).toString()+"'";
            QSqlQuery Staffquery;

            if(Staffquery.exec(StaffSQL))
            {
                if(Staffquery.next())   //是否查询到最后一条记录
                {
                    cardDate.strName = Staffquery.value(2).toString();  //查询结果为：staff表Staff_Name字段
                }
            }
            cardDate.strNo = query.value(1).toString(); //查询结果为：fillcard表FillCard_Staff_No字段
            //转换年月日
            cardDate.cardDate.setDate(query.value(2).toDate().year(),query.value(2).toDate().month(),query.value(2).toDate().day());
            cardDate.strType = query.value(3).toString();   //查询结果为：fillcard表FillCard_Type字段
            m_FullCard.append(cardDate);//添加补卡数据
        }
    }
    _UpdateFullCardDate();  //数据更新函数：补打卡
}

/******初始化函数：考勤系统树*******/
void CATTENDANCE::_InitTree3()  //“考勤系统”栏
{
    ui->QType_3->setColumnCount(1);
    ui->QType_3->setShowGrid(false);//显示表格线
    QStringList headers;
    headers<<"序号";
    ui->QType_3->horizontalHeader()->hide();
    ui->QType_3->verticalHeader()->hide();//行头不显示
    ui->QType_3->horizontalHeader()->setClickable(false);//行头不可选
    ui->QType_3->setHorizontalHeaderLabels(headers);
    ui->QType_3->horizontalHeader()->setHighlightSections(false);//表头坍塌
    ui->QType_3->resizeColumnsToContents();
    ui->QType_3->setFont(QFont("Helvetica"));//字体
    ui->QType_3->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->QType_3->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式：一行
    ui->QType_3->setColumnWidth(0,160);

    for(int i=0;i<ui->QType_3->rowCount();i++)
        ui->QType_3->setRowHeight(i,44);
}

/******初始化函数：考勤系统树*******/
void CATTENDANCE::_InitTree4()  //“考勤报表”栏
{
    ui->QType_4->setColumnCount(1);
    ui->QType_4->setShowGrid(false);//显示表格线
    QStringList headers;
    headers<<"序号";
    ui->QType_4->horizontalHeader()->hide();
    ui->QType_4->verticalHeader()->hide();//行头不显示
    ui->QType_4->horizontalHeader()->setClickable(false);//行头不可选
    ui->QType_4->setHorizontalHeaderLabels(headers);
    ui->QType_4->horizontalHeader()->setHighlightSections(false);//表头坍塌
    ui->QType_4->resizeColumnsToContents();
    ui->QType_4->setFont(QFont("Helvetica"));//字体
    ui->QType_4->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->QType_4->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式：一行
    ui->QType_4->setColumnWidth(0,160);

    for(int i=0;i<ui->QType_4->rowCount();i++)
        ui->QType_4->setRowHeight(i,44);
}

/******初始化函数：补打卡*******/
void CATTENDANCE::_InitFullCardDate()
{
    ui->FullCardDate->setColumnCount(4);
    ui->FullCardDate->setShowGrid(true);    //显示网格线
    QStringList headers;
    headers<<"日期"<<"职员"<<"工号"<<"补卡类型";
    ui->FullCardDate->verticalHeader()->hide();
    ui->FullCardDate->horizontalHeader()->setClickable(false);
    ui->FullCardDate->setHorizontalHeaderLabels(headers);
    ui->FullCardDate->horizontalHeader()->setHighlightSections(false);
    ui->FullCardDate->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->FullCardDate->setSelectionBehavior(QAbstractItemView::SelectRows);//选择一行
    ui->FullCardDate->setColumnWidth(0,150);
    ui->FullCardDate->setColumnWidth(1,80);
    ui->FullCardDate->setColumnWidth(2,80);
    ui->FullCardDate->setColumnWidth(3,80);
}

/******数据更新函数：补打卡*******/
void CATTENDANCE::_UpdateFullCardDate()
{
    for(int ti =0;ti<m_FullCard.size();ti++)
    {
        ui->FullCardDate->setRowCount(ti+1);
        QTableWidgetItem *tableWidget1 = new QTableWidgetItem(QString(m_FullCard.at(ti).cardDate.toString("yyyy-MM-dd")));
        tableWidget1->setTextAlignment(Qt::AlignCenter);
        ui->FullCardDate->setItem(ti,0,tableWidget1);   //设置“日期”列

        QTableWidgetItem *tableWidget2 = new QTableWidgetItem(QString(m_FullCard.at(ti).strName));
        tableWidget2->setTextAlignment(Qt::AlignCenter);
        ui->FullCardDate->setItem(ti,1,tableWidget2);   //设置“职员”列

        QTableWidgetItem *tableWidget3 = new QTableWidgetItem(QString(m_FullCard.at(ti).strNo));
        tableWidget3->setTextAlignment(Qt::AlignCenter);
        ui->FullCardDate->setItem(ti,2,tableWidget3);   //设置“工号”列
        QString str;
        if(m_FullCard.at(ti).strType.toInt()==1)
            str="上班卡";
        else if(m_FullCard.at(ti).strType.toInt()==2)
            str="下班卡";

        QTableWidgetItem *tableWidget4 = new QTableWidgetItem(str);
        tableWidget4->setTextAlignment(Qt::AlignCenter);
        ui->FullCardDate->setItem(ti,3,tableWidget4);   //设置“补卡类型”列
    }
}

/******更新函数：补卡链表数据*******/
void CATTENDANCE::_InitCardList()
{
    m_FullCard.clear();
    QString SQL;
    QString StrStime = ui->QdateStime->dateTime().toString("yyyy-MM-dd");   //获取QDateEdit的开始时间
    QString StrEtime = ui->QdateEtime->dateTime().toString("yyyy-MM-dd");   //获取QDateEdit的结束时间
    if(ui->QchkStime->isChecked() && ui->QchkEtime->isChecked())    //选择了开始时间和结束时间
    {
        if(StrStime > StrEtime) //判断查询的开始日期是否比结束日期大
        {
            QMessageBox::critical(this, "错误","查询的开始日期比结束日期大，请重新选择日期");
            return;
        }
        else
        {
            SQL = "SELECT a.* FROM fillcard a WHERE FillCard_Date between '"+StrStime+"' and '"+StrEtime+"' ORDER BY a.FillCard_Staff_No,a.FillCard_Date";
        }
    }
    else if(ui->QchkStime->isChecked())     //只选择了开始时间
    {
        SQL = "SELECT a.* FROM fillcard a WHERE FillCard_Date >= '"+StrStime+"' ORDER BY a.FillCard_Staff_No,a.FillCard_Date";
    }
    else if(ui->QchkEtime->isChecked())       //只选择了结束时间
    {
        SQL = "SELECT a.* FROM fillcard a WHERE FillCard_Date <= '"+StrEtime+"' ORDER BY a.FillCard_Staff_No,a.FillCard_Date";
    }
    else        //1、既没选开始时间，也没选结束时间     2、程序开始执行时，首先执行这个分支
    {
        SQL = "SELECT a.* FROM fillcard a ORDER BY a.FillCard_Staff_No,a.FillCard_Date";
    }

    QSqlQuery query;
    if(query.exec(SQL))
    {
        while(query.next())
        {
            SFullCard cardDate; //补卡信息结构体
            QString StaffSQL = "SELECT a.* FROM staff a WHERE a.Staff_No = '"+query.value(1).toString()+"'";
            QSqlQuery Staffquery;
            if(Staffquery.exec(StaffSQL))
            {
                if(Staffquery.next())   //是否查询到最后一条记录
                {
                    cardDate.strName = Staffquery.value(2).toString();  //查询结果为：staff表Staff_Name字段
                }
            }
            cardDate.strNo = query.value(1).toString(); //查询结果为：fillcard表FillCard_Staff_No字段
            //转换年月日
            cardDate.cardDate.setDate(query.value(2).toDate().year(),query.value(2).toDate().month(),query.value(2).toDate().day());
            cardDate.strType = query.value(3).toString();   //查询结果为：fillcard表FillCard_Type字段
            m_FullCard.append(cardDate);//添加补卡数据
        }
    }
    _UpdateFullCardDate();  //数据更新函数：补打卡
}

/******信号槽函数：添加补卡*******/
void CATTENDANCE::SlotFillout()
{
    QString strName = ui->Fillcard_tree->currentItem()->text(0);    //获取当前item项的文本
    QString strNo;
    QString StaffSQL = "SELECT a.* FROM  staff a WHERE a.Staff_Name = '"+strName+"'";
    QString HolidaySQL = "SELECT * FROM holiday ORDER BY Holiday_StartDay";
    QSqlQuery Staffquery, Holidayquery;
    if(Staffquery.exec(StaffSQL))
    {
        if(Staffquery.next())
        {
            strNo = Staffquery.value(1).toString(); //获取Staff_No
        }
        else
        {
            QMessageBox::information(this,"提示","请选择具体的员工");
            return;
        }
    }

    //补卡类型复选框
    if(!(ui->QchkGo->isChecked() || ui->QchkOff->isChecked()))
    {
        QMessageBox::information(this, "提示", "请选择“上班卡”或者“下班卡”");
        return;
    }

    //确定补卡提示窗
    int ret = QMessageBox::information(this, "提示", "请选择是否确定补卡", QMessageBox::Yes, QMessageBox::No);
    if(ret == QMessageBox::No)
    {
        return;
    }

    currselecteDate = ui->FilloutCalendarWidget->selectedDate();//获取当前选中的日期
    int day = currselecteDate.day();    //Returns the day of the month (1 to 31) of this date.
    QDate date;

    if(Holidayquery.exec(HolidaySQL))
    {
        while(Holidayquery.next())
        {
            switch(Holidayquery.value(1).toInt())   //Holiday_Type
            {
                case 0://“元旦”
                case 2://“清明”
                case 3://“劳动”
                    break;
                case 4://“端午”
                case 5://“中秋”
                {
                    n_List.append(Holidayquery.value(3).toDate().addDays(3));//Holiday_StartDay
                }
                    break;
                case 1://“春节”
                case 6://“国庆节”
                {
                    n_List.append(Holidayquery.value(3).toDate().addDays(-1));
                    n_List.append(Holidayquery.value(3).toDate().addDays(7));
                }
                    break;
                default:
                    break;
            }
        }
    }

    isholiday = false;
    istakeworks = false;
    date.setDate(currselecteDate.year(),currselecteDate.month(),day);
    emit SigFillcardDate(date);     //信号槽函数：单双休 以及奇偶周休对本月补卡天数的影响

    if(Holidayquery.exec(HolidaySQL))
    {
        while(Holidayquery.next())
        {
            for(int i =0 ; i < Holidayquery.value(4).toInt() ; i++)     //Holiday_fate
            {
                if(date.toString("yyyy-MM-dd") == Holidayquery.value(3).toDate().addDays(i).toString("yyyy-MM-dd"))
                {
                    isholiday = true;
                    break;
                }
            }
        }
    }

    for(int i = 0 ; i < n_List.length() ; i++)
    {
        if(n_List.at(i).toString("yyyy-MM-dd") == date.toString("yyyy-MM-dd"))
        {
            istakeworks = true;
            break;
        }
    }

    if(istakeworks||(!isholiday&&(((restType=="1"||restType=="3"||restType=="6")&&date.dayOfWeek()!=7)||((restType=="2"||restType=="4"||restType=="5")&&(date.dayOfWeek()!=6&&date.dayOfWeek()!=7)))))
    {
        //上班卡
        if(ui->QchkGo->isChecked())
        {
            QString ASQL = "SELECT a.* FROM  card_record a WHERE a.Card_Record_Staff_No = '"+strNo+"' AND a.Card_Record_Date = '"+date.toString("yyyy-MM-dd")+"' AND a.Card_Record_Type = '1'";
            QSqlQuery Aquery;
            if(Aquery.exec(ASQL))
            {
                if(!Aquery.next())
                {
                    QString InsertSQL = "INSERT INTO card_record (Card_Record_Staff_No,Card_Record_Date,Card_Record_Time,Card_Record_Type) VALUES ('"+strNo+"','"+date.toString("yyyy-MM-dd")+"','08:30:00','1')";
                    QSqlQuery insertquery;
                    qDebug()<<insertquery.exec(InsertSQL)<<InsertSQL;

                    QString FullCardSQL = "INSERT INTO fillcard (FillCard_Staff_No,FillCard_Date,FillCard_Type) VALUES ('"+strNo+"','"+date.toString("yyyy-MM-dd")+"','1')";
                    QSqlQuery Fullquery;
                    qDebug()<<Fullquery.exec(FullCardSQL)<<FullCardSQL;
                }
            }
        }
        if(ui->QchkOff->isChecked())
        {
            //下班卡
            QString BSQL = "SELECT a.* FROM  card_record a WHERE a.Card_Record_Staff_No = '"+strNo+"' AND a.Card_Record_Date = '"+date.toString("yyyy-MM-dd")+"' AND a.Card_Record_Type = '2'";
            QSqlQuery Bquery;
            if(Bquery.exec(BSQL))
            {
                if(!Bquery.next())
                {
                    QString InsertSQL = "INSERT INTO card_record (Card_Record_Staff_No,Card_Record_Date,Card_Record_Time,Card_Record_Type) VALUES ('"+strNo+"','"+date.toString("yyyy-MM-dd")+"','18:00:00','2')";
                    QSqlQuery insertquery;
                    insertquery.exec(InsertSQL);

                    QString FullCardSQL = "INSERT INTO fillcard (FillCard_Staff_No,FillCard_Date,FillCard_Type) VALUES ('"+strNo+"','"+date.toString("yyyy-MM-dd")+"','2')";
                    QSqlQuery Fullquery;
                    Fullquery.exec(FullCardSQL);
                }
            }
        }
    }
    _InitCardList();    //更新函数：补卡链表数据
}

/******初始化函数：补打卡员工树*******/
void CATTENDANCE::_InitFillcardTree()
{
    m_List.clear();     //保存查询配对的数据
    ui->Fillcard_tree->clear();
    ui->Fillcard_tree->setHeaderHidden(true);
    QString SQL = "SELECT * from department ORDER BY Department_No DESC";
    QSqlQuery query;
    if(query.exec(SQL))
    {
        int i = 1;
        while(query.next())
        {
            SList data;     //“员工ID，编号，姓名信息”结构体
            data.Id = query.value(0).toInt();
            data.strNo = query.value(1).toString();
            data.strName = query.value(2).toString();
            m_List.append(data);
            i++;    //计数作用？？？
        }
    }

    QTreeWidgetItem* B1 = new QTreeWidgetItem(QStringList()<<"在职");
    ui->Fillcard_tree->addTopLevelItem(B1);

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
        B1->addChild(B[i]); //在“在职”目录下添加所有部门的人员姓名
        QString StaffSQL;
        QSqlQuery Staffquery;
        StaffSQL = "SELECT a.Staff_Name FROM `staff` a where  a.Staff_Department_No = '"+m_List.at(i).strNo+"' ORDER BY a.Staff_No ASC";
        if(Staffquery.exec(StaffSQL))
        {
            while(Staffquery.next())
            {
                QTreeWidgetItem *BiB = new QTreeWidgetItem(QStringList()<<Staffquery.value(0).toString());
                B[i]->addChild(BiB);    //在各部门的对应目录下添加相应的人员
            }
        }
    }

    QTreeWidgetItem* B2 = new QTreeWidgetItem(QStringList()<<"离职");
    ui->Fillcard_tree->addTopLevelItem(B2);
    QString LeaveSQL;
    LeaveSQL = "SELECT a.Leave_Staff_Name FROM `leave_staff` a ORDER BY a.Leave_Staff_Id ASC";
    QSqlQuery Leavequery;
    if(Leavequery.exec(LeaveSQL))
    {
        while(Leavequery.next())
        {
            QTreeWidgetItem *B2B = new QTreeWidgetItem(QStringList()<<Leavequery.value(0).toString());
            B2->addChild(B2B);  //添加离职人员姓名
        }
    }
}

/******初始化函数：法定节假日*******/
void CATTENDANCE::_InitHolidayDate()
{
    ui->holidayType->addItem("元旦");
    ui->holidayType->addItem("春节");
    ui->holidayType->addItem("清明节");
    ui->holidayType->addItem("劳动节");
    ui->holidayType->addItem("端午节");
    ui->holidayType->addItem("中秋节");
    ui->holidayType->addItem("国庆节");
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setShowGrid(true);//显示表格线
    ui->tableWidget->verticalHeader()->hide();//行头不显示
    ui->tableWidget->horizontalHeader()->hide();
    ui->tableWidget->horizontalHeader()->setClickable(false);//行头不可选
    ui->tableWidget->horizontalHeader()->setHighlightSections(false);//表头坍塌
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->setFont(QFont("Helvetica"));//字体
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->tableWidget->setColumnWidth(0,200);
    ui->tableWidget->setColumnWidth(1,200);
    ui->tableWidget->setColumnWidth(2,200);
}

/******更新函数：法定节假日*******/
void CATTENDANCE::_UpdateHolidayDate()
{
    QString SQL = "SELECT * FROM holiday ORDER BY Holiday_StartDay";
    QSqlQuery query;
    if(query.exec(SQL))
    {
        HolidayTypeNum = 0;
        ui->tableWidget->setRowCount(HolidayTypeNum);
        while(query.next())
        {
            ui->tableWidget->setRowCount(HolidayTypeNum/3+1);
            QString HolidayType;
            switch(query.value(1).toInt())
            {
                case 0: HolidayType = "元旦";
                    break;
                case 1: HolidayType = "春节";
                    break;
                case 2: HolidayType = "清明节";
                    break;
                case 3: HolidayType = "劳动节";
                    break;
                case 4: HolidayType = "端午节";
                    break;
                case 5: HolidayType = "中秋节";
                    break;
                case 6: HolidayType = "国庆节";
                    break;
                default:
                    break;
            }
            QString showStr = query.value(3).toDate().toString("MM.dd")+"-"+query.value(3).toDate().addDays(query.value(4).toInt()-1).toString("MM.dd")+ "  " +HolidayType;

            QTableWidgetItem *tabledmItem = new QTableWidgetItem(QString(showStr));
            tabledmItem->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(HolidayTypeNum/3,HolidayTypeNum%3,tabledmItem);
            ui->tableWidget->item(HolidayTypeNum/3,HolidayTypeNum%3)->setBackgroundColor(QColor(225,234,239));
            HolidayTypeNum++;
        }
    }
}

/******信号槽函数：删除法定节假日*******/
void CATTENDANCE::SlotDelHoliday()
{
   int row = ui->tableWidget->currentRow();
   int column = ui->tableWidget->currentColumn();
   if(row*3 + column+1 <= HolidayTypeNum && HolidayTypeNum > 0)
   {
        QString seleteStr = ui->tableWidget->currentItem()->text();
        QString strHolidayType = seleteStr.left(seleteStr.length()).right(seleteStr.length()-13);
        int intHolidayType;
        if(strHolidayType == "元旦")
            intHolidayType = 0;
        else if(strHolidayType == "春节")
            intHolidayType = 1;
        else if(strHolidayType == "清明节")
            intHolidayType = 2;
        else if(strHolidayType == "劳动节")
            intHolidayType = 3;
        else if(strHolidayType == "端午节")
            intHolidayType = 4;
        else if(strHolidayType == "中秋节")
            intHolidayType = 5;
        else if(strHolidayType == "国庆节")
            intHolidayType = 6;
        else
            intHolidayType = 7;
        QString strSarteDate = ui->HolidayCalendarWidget->selectedDate().toString("yyyy")+"."+seleteStr.left(5);
        QString SQL = "DELETE FROM holiday WHERE holiday.Holiday_StartDay = '"+strSarteDate+"' AND holiday.Holiday_Type = '"+QString::number(intHolidayType)+"'";
        QSqlQuery query;
        qDebug()<<query.exec(SQL)<<SQL;
        _UpdateHolidayDate();
   }
   else
       QMessageBox::information(this,"提示","目标选取失败");
}

/******信号槽函数：添加节假日*******/
void CATTENDANCE::SlotSetHoliday()
{

    int HolidayDays = 0;
    switch(ui->holidayType->currentIndex())
    {
    case 0: HolidayDays=3;
        break;
    case 1: HolidayDays=7;
        break;
    case 2: HolidayDays=2;
        break;
    case 3: HolidayDays=3;
        break;
    case 4: HolidayDays=3;
        break;
    case 5: HolidayDays=3;
        break;
    case 6: HolidayDays=7;
        break;
    default:
        break;
    }
    QString selectSQL,SQL;
    QSqlQuery selectquery,query;
    selectSQL = "SELECT holiday.* FROM holiday WHERE holiday.Holiday_Type = '"+QString::number(ui->holidayType->currentIndex())+"' ";
    selectquery.exec(selectSQL);
    if(selectquery.numRowsAffected()>0)
        SQL = "UPDATE holiday SET Holiday_Year = '"+ui->HolidayCalendarWidget->selectedDate().toString("yyyy")+"',Holiday_StartDay = '"+ui->HolidayCalendarWidget->selectedDate().toString("yyyy-MM-dd")+"',Holiday_fate = '"+QString::number(HolidayDays)+"' WHERE Holiday_Type = '"+QString::number(ui->holidayType->currentIndex())+"' ";
    else
        SQL = "INSERT INTO holiday (Holiday_Type,Holiday_Year,Holiday_StartDay,Holiday_fate)VALUES ('"+QString::number(ui->holidayType->currentIndex())+"','"+ui->HolidayCalendarWidget->selectedDate().toString("yyyy")+"','"+ui->HolidayCalendarWidget->selectedDate().toString("yyyy-MM-dd")+"','"+QString::number(HolidayDays)+"')";
    qDebug()<<query.exec(SQL)<<SQL;
    _UpdateHolidayDate();
}

/******信号槽函数：补打卡删除*******/
void CATTENDANCE::SlotDelete()
{
    int nRow = ui->FullCardDate->currentRow();
    if(nRow >= 0)
    {
        QString str1 = ui->FullCardDate->item(nRow,2)->text();
        QString str2=ui->FullCardDate->item(nRow,3)->text();    //获取补卡类型
        QString type;
        if(str2=="上班卡")
            type="1";
        if(str2=="下班卡")
            type="2";

        QMessageBox::StandardButton standard = QMessageBox::question(this,tr("提示"),tr("确认删除此记录?"),QMessageBox::Yes|QMessageBox::No);
        if(standard==QMessageBox::Yes)
        {
            QString SQL = "delete  from `fillcard` where FillCard_Staff_No = '"+str1+"' and FillCard_Date = '"+ui->FullCardDate->item(nRow,0)->text()+"' and FillCard_Type = '"+type+"'";

            QString SQL1 = "delete from `card_record` where Card_Record_Staff_No = '"+str1+"' and Card_Record_Date = '"+ui->FullCardDate->item(nRow,0)->text()+"' and Card_Record_Type = '"+type+"'";
            QSqlQuery query,query1;
            qDebug()<<query1.exec(SQL1)<<SQL1<<endl;    //打印SQL语句执行成功与否信息
            if(query.exec(SQL)&&query1.exec(SQL1))
            {
                QMessageBox::information(this,"提示","删除成功！",QMessageBox::Yes);
                if(ui->FullCardDate->rowCount()==1)
                    ui->FullCardDate->setRowCount(0);   //？？？
            }
            else
                QMessageBox::information(this,"提示","删除失败！",QMessageBox::Yes);
         }
        _InitCardList();    //更新函数：补卡链表数据
    }
    else
    {
        QMessageBox::information(this,tr("提示"),tr("请选择要删除的记录"));
    }
}

/******信号槽函数：数据采集*******/
void CATTENDANCE::SlotAcquisition()
{
    int Row = ui->Doormachine->currentRow();
    if(Row>=0)
    {
        QString Addr=ui->Doormachine->item(Row,1)->text();
        QString IpAddr=ui->Doormachine->item(Row,2)->text();
//        Udpserver->_Acqkuisition(Addr,IpAddr);
        emit SigAcquisition(Addr, IpAddr);
    }
    else
        QMessageBox::information(this,tr("提示"),tr("请选中目标！"));
}

/******初始化函数：信号采集*******/
void CATTENDANCE::_InitDoorMachine()    //“考勤报表”->“数据采集”栏
{
    ui->Doormachine->setColumnCount(3);
    ui->Doormachine->setShowGrid(true);
    QStringList headers;
    headers<<"序号"<<"地址"<<"IP地址";
    ui->Doormachine->verticalHeader()->hide();
    ui->Doormachine->horizontalHeader()->setClickable(false);
    ui->Doormachine->setHorizontalHeaderLabels(headers);
    ui->Doormachine->horizontalHeader()->setHighlightSections(false);
    ui->Doormachine->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->Doormachine->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->Doormachine->setColumnWidth(0,40);
    ui->Doormachine->setColumnWidth(1,200);
    ui->Doormachine->setColumnWidth(2,150);
}

/******更新函数：信号采集*******/
void CATTENDANCE::_UpdateDoorMachine()
{
    QString SQL;
    QString Qtype = "访客机";
    SQL = "SELECT * FROM device a WHERE a.Device_Type ='"+Qtype+"' ORDER BY a.Device_Id";
    QSqlQuery query;
    if(query.exec(SQL))
    {   int ti=0;
        while(query.next())
        {
            ui->Doormachine->setRowCount(ti+1);
            QTableWidgetItem *tablewidget1= new QTableWidgetItem(QString::number(ti+1));
            tablewidget1->setTextAlignment(Qt::AlignCenter);
            ui->Doormachine->setItem(ti,0,tablewidget1);

            QTableWidgetItem *tablewidget2= new QTableWidgetItem(QString(query.value(1).toString()));
            tablewidget2->setTextAlignment(Qt::AlignCenter);
            ui->Doormachine->setItem(ti,1,tablewidget2);

            QTableWidgetItem *tablewidget3= new QTableWidgetItem(QString(query.value(3).toString()));
            tablewidget3->setTextAlignment(Qt::AlignCenter);
            ui->Doormachine->setItem(ti,2,tablewidget3);
            ti++;
        }
    }
}

/******信号槽函数：缓存条动态显示*******/
void CATTENDANCE::SlotProgress(int AllRacdNum,int number)
{
    ui->racddate_Bar->show();
    ui->racddate_Bar->setMinimum(0);
    ui->racddate_Bar->setMaximum(AllRacdNum);
    ui->racddate_Bar->setValue(number);
    ui->racddate_Bar->value();

    if(number==AllRacdNum)
    {
        QMessageBox::information(this,tr("提示"),tr("缓冲完成"));
        ui->racddate_Bar->setValue(0);
        ui->racddate_Bar->hide();
    }
}

/*********右键菜单项设置************/
void CATTENDANCE::on_HolidayCalendarWidget_customContextMenuRequested(const QPoint &pos)
{
    HolidayMenu->addAction(Holiday);
    HolidayMenu->exec(QCursor::pos()); //菜单出现的位置为当前鼠标的位置
}

void CATTENDANCE::on_FullCardDate_customContextMenuRequested(const QPoint &pos)
{
    CardDataMenu->addAction(DeleteAction);
    CardDataMenu->exec(QCursor::pos()); //菜单出现的位置为当前鼠标的位置
}

void CATTENDANCE::on_tableWidget_customContextMenuRequested(const QPoint &pos)
{
    DelHolidayMenu->addAction(DeleteHoliday);
    DelHolidayMenu->exec(QCursor::pos()); //菜单出现的位置为当前鼠标的位置
}

void CATTENDANCE::on_Fillcard_tree_customContextMenuRequested(const QPoint &pos)
{
    FilloutMneu->addAction(FilloutAction);
    FilloutMneu->exec(QCursor::pos()); //菜单出现的位置为当前鼠标的位置
}

void CATTENDANCE::on_QType_3_clicked(const QModelIndex &index)
{
    switch(index.row())
    {
        case 0: ui->stackedWidget_3->setCurrentIndex(1);
            break;
        case 1: ui->stackedWidget_3->setCurrentIndex(2);
            break;
        case 2: ui->stackedWidget_3->setCurrentIndex(3);
                _InitFillcardTree();
            break;
        case 3: ui->stackedWidget_3->setCurrentIndex(4);
                Askforleave->_InitLeaveTree();
            break;
        default:
            break;
    }
    ui->tableWidget->setCurrentItem(ui->tableWidget->item(-1,-1));
    ui->holidayType->setCurrentIndex(0);
    ui->HolidayCalendarWidget->setSelectedDate(QDate::currentDate());
    ui->FullCardDate->setCurrentItem(ui->FullCardDate->item(-1,-1));
    ui->FilloutCalendarWidget->setSelectedDate(QDate::currentDate());
    emit SigFocusOut();
}

void CATTENDANCE::on_QType_4_clicked(const QModelIndex &index)
{
    switch(index.row())
    {
        case 0:
        {
            ui->stackedWidget_3->setCurrentIndex(10);
            _UpdateDoorMachine();
            break;
        }
        case 1:
        {
            ui->stackedWidget_3->setCurrentIndex(5);
            Original->_Update();
            break;
        }
        case 2:
        {
            ui->stackedWidget_3->setCurrentIndex(6);
            Detail->_Update();
            break;
        }
        case 3:
        {
            ui->stackedWidget_3->setCurrentIndex(7);
            Day->_Update();
            break;
        }
        case 4:
        {
            ui->stackedWidget_3->setCurrentIndex(8);
            Monthly->_Update();
            break;
        }
        default:
            break;
    }
}

void CATTENDANCE::on_toolBox_2_currentChanged(int index)
{
    ui->stackedWidget_3->setCurrentIndex(0);
}

void CATTENDANCE::on_QpbtnAttendanceSet_clicked()
{
    _SetToolBoxCurrentItem(0);
    ui->stackedWidget_3->setCurrentIndex(1);
    emit SigFocusOut();
}

void CATTENDANCE::on_QpbtnHolidaysSet_clicked()
{
    _SetToolBoxCurrentItem(1);
    ui->stackedWidget_3->setCurrentIndex(2);
    ui->tableWidget->setCurrentItem(ui->tableWidget->item(-1,-1));
    ui->holidayType->setCurrentIndex(0);
    ui->HolidayCalendarWidget->setSelectedDate(QDate::currentDate());
}

void CATTENDANCE::on_QpbtnManually_clicked()
{
    _InitFillcardTree();    //每次进入都重新生成一次界面
    _SetToolBoxCurrentItem(2);
    ui->stackedWidget_3->setCurrentIndex(3);
    ui->FullCardDate->setCurrentItem(ui->FullCardDate->item(-1,-1));
    ui->FilloutCalendarWidget->setSelectedDate(QDate::currentDate());
}

void CATTENDANCE::on_QpbtnLeave_clicked()
{
    _SetToolBoxCurrentItem(3);
    ui->stackedWidget_3->setCurrentIndex(4);
    emit SigFocusOut();
}

void CATTENDANCE::on_QpbtnCollect_clicked()
{
    _SetToolBoxCurrentItem(4);
    ui->stackedWidget_3->setCurrentIndex(10);
}

void CATTENDANCE::on_QpbtnOriginal_clicked()
{
    _SetToolBoxCurrentItem(5);
    ui->stackedWidget_3->setCurrentIndex(5);
}

void CATTENDANCE::on_QpbtnDetail_clicked()
{
    _SetToolBoxCurrentItem(6);
    ui->stackedWidget_3->setCurrentIndex(6);
}

void CATTENDANCE::on_QpbtnDay_clicked()
{
    _SetToolBoxCurrentItem(7);
    ui->stackedWidget_3->setCurrentIndex(7);
}

void CATTENDANCE::on_QpbtnMonthly_clicked()
{
    _SetToolBoxCurrentItem(8);
    ui->stackedWidget_3->setCurrentIndex(8);
}

void CATTENDANCE::on_FilloutCalendarWidget_clicked(const QDate &date)
{
    currselecteDate.setDate(date.year(),date.month(),date.day());
}

void CATTENDANCE::SlotFillcardRestType(QString strType)
{
    restType = strType;
}

void CATTENDANCE::SlotAttendanceClick()
{
    ui->stackedWidget_3->setCurrentIndex(0);
}

void  CATTENDANCE::_SetToolBoxCurrentItem(int nIndex)
{
    int nType = 0;
    switch(nIndex)
    {
        case 0:
        case 1:
        case 2:
        case 3:
            nType = 0;
            break;
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            nType = 1;
            break;
        default:
            break;
    }

    ui->toolBox_2->setCurrentIndex(nType);
    if(nType == 0)
    {
        ui->QType_3->setCurrentItem(ui->QType_3->item(nIndex,0));
    }
    else
    {
        ui->QType_4->setCurrentItem(ui->QType_4->item(nIndex - 4,0));
    }
}

void CATTENDANCE::SlotFocusOut()
{
     ui->QType_3->setCurrentItem(ui->QType_3->item(-1,0));
     ui->QType_4->setCurrentItem(ui->QType_4->item(-1,0));
}

//各报表页面，返回考勤主界面
void CATTENDANCE::SlotReturnMenu()
{
    ui->stackedWidget_3->setCurrentIndex(0);
}

void CATTENDANCE::on_QchkStime_clicked()    //查询时段的开始时间
{
    if(!ui->QchkStime->checkState())
    {
        ui->QdateStime->setEnabled(false);
    }
    else
    {
        ui->QdateStime->setEnabled(true);
    }
}

void CATTENDANCE::on_QchkEtime_clicked()    //查询时段的结束时间
{
    if(!ui->QchkEtime->checkState())
    {
        ui->QdateEtime->setEnabled(false);
    }
    else
    {
        ui->QdateEtime->setEnabled(true);
    }
}

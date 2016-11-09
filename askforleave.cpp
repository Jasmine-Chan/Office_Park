#include "askforleave.h"
#include "ui_askforleave.h"

askforleave::askforleave(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::askforleave)
{
    ui->setupUi(this);
//    QTextCharFormat textCharFormat;
//    textCharFormat.setAnchorName("周");
//    textCharFormat.setAnchor(true);
//    ui->LeaveCalendarWidget->setHeaderTextFormat(textCharFormat);
//    ui->LeaveCalendarWidget->setObjectName("周");

    LeaveAction = new QAction(tr("新建请/休假"),this);
    LeavetreeMenu = new QMenu(ui->Leavetree);
    ui->Leavetree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(LeaveAction,SIGNAL(triggered()),this,SLOT(SlotShowPseudomonas()));

    DeleteAction= new QAction(tr("删除"),this);
    DataMenu = new QMenu(ui->leaveData);
    DataMenu->setStyleSheet("border:1px solid rgb(180,202,216);background-color:white;selection-background-color:rgb(180,202,216);");
    ui->leaveData->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(DeleteAction,SIGNAL(triggered()),this,SLOT(SlotDelete()));

    QDateTime dateTime = QDateTime::currentDateTime();
    ui->QdateETime->setDate(dateTime.date());
    ui->QdateSTime->setDate(dateTime.date().addDays(-15));
    ui->QdateETime->setCalendarPopup(true);      //设置显示日历选择弹出窗口
    ui->QdateSTime->setCalendarPopup(true);
    ui->QdateETime->setEnabled(false);
    ui->QdateSTime->setEnabled(false);

    connect(ui->Leavetree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(CheckSelf(QTreeWidgetItem*,int)));

    _InitLeaveData();   //初始化函数：请/休假数据窗口
    _UpdateLeaveData(); //更新函数：请假数据
    _InitLeaveTree();   //初始化函数：请/休假员工树
}

askforleave::~askforleave()
{
    delete ui;
}

/********检测“请/休假员工树”中是否有item被选中********/
void askforleave::CheckSelf(QTreeWidgetItem *hItem, int)
{
    qDebug()<<"已选中"<<hItem->text(0)<<endl;
    //hItem->text(0):获取选中的item中的内容，返回QString类型
    //因为leave是MySQL中的关键字，所以需要加 ` 符号，`：切换至英文状态，按Esc正下方的键
    QString SQL;
    QSqlQuery check_SQL;
    QString StrStime = ui->QdateSTime->dateTime().toString("yyyy-MM-dd");   //获取QDateEdit的开始时间
    QString StrEtime = ui->QdateETime->dateTime().toString("yyyy-MM-dd");   //获取QDateEdit的结束时间
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
        if(ui->QchkSTime->isChecked() && ui->QchkETime->isChecked())    //选择了开始时间和结束时间
        {
            if(StrStime > StrEtime) //判断查询的开始日期是否比结束日期大
            {
                QMessageBox::critical(this, "错误","查询的开始日期比结束日期大，请重新选择日期");
                return;
            }
            else
            {
                SQL = "SELECT * FROM `leave` WHERE Leave_SDateTime between '"+StrStime+"' and '"+StrEtime+"' and Leave_Staff_No in (SELECT Staff_No FROM staff WHERE Staff_Department_No = '"+ QueryDepartment.value(1).toString() +"') ORDER BY Leave_Staff_No ASC";
            }
        }
        else if(ui->QchkSTime->isChecked())     //只选择了开始时间
        {
            SQL = "SELECT * FROM `leave` WHERE Leave_SDateTime >= '"+StrStime+"' and Leave_Staff_No in (SELECT Staff_No FROM staff WHERE Staff_Department_No = '"+ QueryDepartment.value(1).toString() +"') ORDER BY Leave_Staff_No ASC";
        }
        else if(ui->QchkETime->isChecked())       //只选择了结束时间
        {
            SQL = "SELECT * FROM `leave` WHERE Leave_SDateTime <= '"+StrEtime+"' and Leave_Staff_No in (SELECT Staff_No FROM staff WHERE Staff_Department_No = '"+ QueryDepartment.value(1).toString() +"') ORDER BY Leave_Staff_No ASC";
        }
        else        //1、既没选开始时间，也没选结束时间     2、程序开始执行时，首先执行这个分支
        {
            SQL = "SELECT * FROM `leave` WHERE Leave_Staff_No in (SELECT Staff_No FROM staff WHERE Staff_Department_No = '"+ QueryDepartment.value(1).toString() +"') ORDER BY Leave_Staff_No ASC";
        }
    }
    else
    {
        if(ui->QchkSTime->isChecked() && ui->QchkETime->isChecked())    //选择了开始时间和结束时间
        {
            if(StrStime > StrEtime) //判断查询的开始日期是否比结束日期大
            {
                QMessageBox::critical(this, "错误","查询的开始日期比结束日期大，请重新选择日期");
                return;
            }
            else
            {
                if("在职" == staff_name)
                {
                    SQL = "SELECT * FROM `leave` WHERE Leave_SDateTime between '"+StrStime+"' and '"+StrEtime+"' and Leave_Staff_No in (SELECT Staff_No FROM staff) ORDER BY Leave_Staff_No ASC";
                }
                else if("未分配" == staff_name)
                {
                    SQL = "SELECT * FROM `leave` WHERE Leave_SDateTime between '"+StrStime+"' and '"+StrEtime+"' and Leave_Staff_No in (SELECT Staff_No FROM staff WHERE Staff_Department_No = '') ORDER BY Leave_Staff_No ASC";
                }
                else
                {
                    SQL = "SELECT * FROM `leave` WHERE Leave_SDateTime between '"+StrStime+"' and '"+StrEtime+"' and Leave_Staff_No in (SELECT Staff_No FROM staff WHERE Staff_Name = '"+ staff_name +"') ORDER BY Leave_Staff_No ASC";
                }
            }
        }
        else if(ui->QchkSTime->isChecked())     //只选择了开始时间
        {
            if("在职" == staff_name)
            {
                SQL = "SELECT * FROM `leave` WHERE Leave_SDateTime >= '"+StrStime+"' and Leave_Staff_No in (SELECT Staff_No FROM staff) ORDER BY Leave_Staff_No ASC";
            }
            else if("未分配" == staff_name)
            {
                SQL = "SELECT * FROM `leave` WHERE Leave_SDateTime >= '"+StrStime+"' and Leave_Staff_No in (SELECT Staff_No FROM staff WHERE Staff_Department_No = '') ORDER BY Leave_Staff_No ASC";
            }
            else
            {
                SQL = "SELECT * FROM `leave` WHERE Leave_SDateTime >= '"+StrStime+"' and Leave_Staff_No in (SELECT Staff_No FROM staff WHERE Staff_Name = '"+ staff_name +"') ORDER BY Leave_Staff_No ASC";
            }
        }
        else if(ui->QchkETime->isChecked())       //只选择了结束时间
        {
            if("在职" == staff_name)
            {
                SQL = "SELECT * FROM `leave` WHERE Leave_SDateTime <= '"+StrEtime+"' and Leave_Staff_No in (SELECT Staff_No FROM staff) ORDER BY Leave_Staff_No ASC";
            }
            else if("未分配" == staff_name)
            {
                SQL = "SELECT * FROM `leave` WHERE Leave_SDateTime <= '"+StrEtime+"' and Leave_Staff_No in (SELECT Staff_No FROM staff WHERE Staff_Department_No = '') ORDER BY Leave_Staff_No ASC";
            }
            else
            {
                SQL = "SELECT * FROM `leave` WHERE Leave_SDateTime <= '"+StrEtime+"' and Leave_Staff_No in (SELECT Staff_No FROM staff WHERE Staff_Name = '"+ staff_name +"') ORDER BY Leave_Staff_No ASC";
            }
        }
        else        //1、既没选开始时间，也没选结束时间     2、程序开始执行时，首先执行这个分支
        {
            if("在职" == staff_name)
            {
                SQL = "SELECT * FROM `leave` WHERE Leave_Staff_No in (SELECT Staff_No FROM staff) ORDER BY Leave_Staff_No ASC";
            }
            else if("未分配" == staff_name)
            {
                SQL = "SELECT * FROM `leave` WHERE Leave_Staff_No in (SELECT Staff_No FROM staff WHERE Staff_Department_No = '') ORDER BY Leave_Staff_No ASC";
            }
            else
            {
                SQL = "SELECT * FROM `leave` WHERE Leave_Staff_No in (SELECT Staff_No FROM staff WHERE Staff_Name = '"+ staff_name +"') ORDER BY Leave_Staff_No ASC";
            }
        }
    }
    qDebug()<<SQL<<SQLDepartment;

    //开始执行SQL语句
    if(!check_SQL.first())
    {
        ui->leaveData->clearContents(); //清空TableWidget中所有item数据
    }
    if(check_SQL.exec(SQL))
    {
        int ti = 0;
        while(check_SQL.next())
        {
            ui->leaveData->setRowCount(ti+1);
            QTableWidgetItem *tableWidget1 = new QTableWidgetItem(QString(check_SQL.value(1).toDate().toString("yyyy-MM-dd")));
            tableWidget1->setTextAlignment(Qt::AlignCenter);
            ui->leaveData->setItem(ti,0,tableWidget1);

            QString StaffSQL = "SELECT a.Staff_Name FROM staff a WHERE a.Staff_No = '"+check_SQL.value(2).toString()+"'";
            QSqlQuery Staffquery;
            if(Staffquery.exec(StaffSQL))
            {
                if(Staffquery.next())
                {
                    QTableWidgetItem *tableWidget2 = new QTableWidgetItem(QString(Staffquery.value(0).toString()));
                    tableWidget2->setTextAlignment(Qt::AlignCenter);
                    ui->leaveData->setItem(ti,1,tableWidget2);
                }
            }
            QTableWidgetItem *tableWidget3 = new QTableWidgetItem(QString(check_SQL.value(2).toByteArray().data()));
            tableWidget3->setTextAlignment(Qt::AlignCenter);
            ui->leaveData->setItem(ti,2,tableWidget3);

            QTableWidgetItem *tableWidget4 = new QTableWidgetItem(QString(check_SQL.value(3).toDateTime().toString("yyyy-MM-dd hh:mm")));
            tableWidget4->setTextAlignment(Qt::AlignCenter);
            ui->leaveData->setItem(ti,3,tableWidget4);

            QTableWidgetItem *tableWidget5 = new QTableWidgetItem(QString(check_SQL.value(4).toDateTime().toString("yyyy-MM-dd hh:mm")));
            tableWidget5->setTextAlignment(Qt::AlignCenter);
            ui->leaveData->setItem(ti,4,tableWidget5);
            QString str;
            int AllType = check_SQL.value(5).toInt();
            while(AllType>0)
            {
                int num = AllType%10;
                AllType = AllType/10;
                if(num==1)
                    str=str+"/事假";
                if(num==2)
                    str=str+"/病假";
                if(num==3)
                    str=str+"/年假";
                if(num==4)
                    str=str+"/婚假";
                if(num==5)
                    str=str+"/产假";
                if(num==6)
                    str=str+"/看护假";
            }
            QTableWidgetItem *tableWidget6 = new QTableWidgetItem(str);
            tableWidget6->setTextAlignment(Qt::AlignCenter);
            ui->leaveData->setItem(ti,5,tableWidget6);

            QTableWidgetItem *tableWidget7 = new QTableWidgetItem(QString(check_SQL.value(6).toByteArray().data()));
            tableWidget7->setTextAlignment(Qt::AlignCenter);
            ui->leaveData->setItem(ti,6,tableWidget7);
            ti++;
        }
    }
}

/*****初始化函数：请/休假数据窗口*********/
void askforleave::_InitLeaveData()
{
    ui->leaveData->setColumnCount(8);
    ui->leaveData->setShowGrid(true);
    QStringList headers;
    headers<<"日期"<<"职员"<<"工号"<<"起始时间"<<"截止时间"<<"请假类型"<<"工时"<<"请假事由";
    ui->leaveData->verticalHeader()->hide();
    ui->leaveData->horizontalHeader()->setClickable(false);
    ui->leaveData->setHorizontalHeaderLabels(headers);
    ui->leaveData->horizontalHeader()->setHighlightSections(false);
    ui->leaveData->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->leaveData->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->leaveData->setColumnWidth(0,100);
    ui->leaveData->setColumnWidth(1,80);
    ui->leaveData->setColumnWidth(2,80);
    ui->leaveData->setColumnWidth(3,120);
    ui->leaveData->setColumnWidth(4,120);
    ui->leaveData->setColumnWidth(5,100);
    ui->leaveData->setColumnWidth(6,90);
    ui->leaveData->setColumnWidth(7,100);
}

/******初始化函数：请/休假员工树*********/
void askforleave::_InitLeaveTree()
{
    m_List.clear(); //保存查询配对的数据
    ui->Leavetree->clear();
    ui->Leavetree->setHeaderHidden(true);
    QString SQL = "SELECT * from department ORDER BY Department_No DESC";
    QSqlQuery query;
    if(query.exec(SQL))
    {
        int i = 1;
        while(query.next())
        {
            SList data; //“员工ID，编号，姓名信息”结构体
            data.Id = query.value(0).toInt();
            data.strNo = query.value(1).toString();
            data.strName = query.value(2).toString();
            m_List.append(data);
            i++;    //计数作用
        }
    }

    QTreeWidgetItem* B1 = new QTreeWidgetItem(QStringList()<<"在职");
    ui->Leavetree->addTopLevelItem(B1);

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
    ui->Leavetree->addTopLevelItem(B2);
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

/*****更新函数：请假数据*********/
void askforleave::_UpdateLeaveData()
{
    QString SQL;
    QString StrStime = ui->QdateSTime->dateTime().toString("yyyy-MM-dd");   //获取QDateEdit的开始时间
    QString StrEtime = ui->QdateETime->dateTime().toString("yyyy-MM-dd");   //获取QDateEdit的结束时间

    if(ui->QchkSTime->isChecked() && ui->QchkETime->isChecked())    //选择了开始时间和结束时间
    {
        if(StrStime > StrEtime) //判断查询的开始日期是否比结束日期大
        {
            QMessageBox::critical(this, "错误","查询的开始日期比结束日期大，请重新选择日期");
            return;
        }
        else
        {
            SQL = "SELECT * FROM `leave` WHERE Leave_SDateTime between '"+StrStime+"' and '"+StrEtime+"' ORDER BY Leave_Staff_No ASC";
        }
    }
    else if(ui->QchkSTime->isChecked())     //只选择了开始时间
    {
        SQL = "SELECT * FROM `leave` WHERE Leave_SDateTime >= '"+StrStime+"' ORDER BY Leave_Staff_No ASC";
    }
    else if(ui->QchkETime->isChecked())       //只选择了结束时间
    {
        SQL = "SELECT * FROM `leave` WHERE Leave_SDateTime <= '"+StrEtime+"' ORDER BY Leave_Staff_No ASC";
    }
    else        //1、既没选开始时间，也没选结束时间     2、程序开始执行时，首先执行这个分支
    {
        SQL = "SELECT * FROM `leave` ORDER BY Leave_Staff_No ASC";
    }

    QSqlQuery query;
    if(!query.first())
        ui->leaveData->clearContents(); //清空TableWidget中所有item数据
    if(query.exec(SQL))
    {
        int ti=0;
        while(query.next())
        {
            ui->leaveData->setRowCount(ti+1);
            QTableWidgetItem *tableWidget1 = new QTableWidgetItem(QString(query.value(1).toDate().toString("yyyy-MM-dd")));
            tableWidget1->setTextAlignment(Qt::AlignCenter);
            ui->leaveData->setItem(ti,0,tableWidget1);

            QString StaffSQL = "SELECT a.Staff_Name FROM staff a WHERE a.Staff_No = '"+query.value(2).toString()+"'";
            QSqlQuery Staffquery;
            if(Staffquery.exec(StaffSQL))
            {
                if(Staffquery.next())
                {
                    QTableWidgetItem *tableWidget2 = new QTableWidgetItem(QString(Staffquery.value(0).toString()));
                    tableWidget2->setTextAlignment(Qt::AlignCenter);
                    ui->leaveData->setItem(ti,1,tableWidget2);
                }
            }
            QTableWidgetItem *tableWidget3 = new QTableWidgetItem(QString(query.value(2).toByteArray().data()));
            tableWidget3->setTextAlignment(Qt::AlignCenter);
            ui->leaveData->setItem(ti,2,tableWidget3);

            QTableWidgetItem *tableWidget4 = new QTableWidgetItem(QString(query.value(3).toDateTime().toString("yyyy-MM-dd hh:mm")));
            tableWidget4->setTextAlignment(Qt::AlignCenter);
            ui->leaveData->setItem(ti,3,tableWidget4);

            QTableWidgetItem *tableWidget5 = new QTableWidgetItem(QString(query.value(4).toDateTime().toString("yyyy-MM-dd hh:mm")));
            tableWidget5->setTextAlignment(Qt::AlignCenter);
            ui->leaveData->setItem(ti,4,tableWidget5);
            QString str;
            int AllType = query.value(5).toInt();
            while(AllType>0)
            {
                int num = AllType%10;
                AllType = AllType/10;
                if(num==1)
                    str=str+"/事假";
                if(num==2)
                    str=str+"/病假";
                if(num==3)
                    str=str+"/年假";
                if(num==4)
                    str=str+"/婚假";
                if(num==5)
                    str=str+"/产假";
                if(num==6)
                    str=str+"/看护假";
            }
            QTableWidgetItem *tableWidget6 = new QTableWidgetItem(str);
            tableWidget6->setTextAlignment(Qt::AlignCenter);
            ui->leaveData->setItem(ti,5,tableWidget6);

            QTableWidgetItem *tableWidget7 = new QTableWidgetItem(QString(query.value(6).toByteArray().data()));
            tableWidget7->setTextAlignment(Qt::AlignCenter);
            ui->leaveData->setItem(ti,6,tableWidget7);
//            ti++;

            QTableWidgetItem *tableWidget8 = new QTableWidgetItem(QString(query.value(7).toByteArray().data()));
            tableWidget8->setTextAlignment(Qt::AlignCenter);
            ui->leaveData->setItem(ti,7,tableWidget8);
            ti++;
        }
    }
}

/*****信号槽函数：删除请假条*********/
void askforleave::SlotDelete()
{
    int nRow = ui->leaveData->currentRow();
    if(nRow >= 0){
        QString str1 = ui->leaveData->item(nRow,2)->text();
        QString leavedate = ui->leaveData->item(nRow,0)->text();
        QMessageBox::StandardButton standard = QMessageBox::question(this,tr("提示"),tr("确认删除此记录?"),QMessageBox::Yes|QMessageBox::No);
        if(standard == QMessageBox::Yes)
        {
            QString SQL = "delete  from `leave` where Leave_Staff_No = '"+str1+"' AND Leave_Date = '"+leavedate+"'";
            QSqlQuery query;
            if(query.exec(SQL))
            {
                QMessageBox::information(this,"提示","删除成功！",QMessageBox::Yes);
                if(ui->leaveData->rowCount()==1)
                    ui->leaveData->setRowCount(0);
                _UpdateLeaveData();

            }
            else
                QMessageBox::information(this,"提示","删除失败！",QMessageBox::Yes);
        }

    } else{
        QMessageBox::information(this,tr("提示"),tr("请选择要删除的记录"));
    }     
}

/*****信号槽函数：添加请假条*********/
void askforleave::SlotShowPseudomonas()
{
    SStaff staff;
    QString No, post;
    int department;
    QString Name = ui->Leavetree->currentItem()->text(0);
    QString NameSQL = "SELECT * FROM staff WHERE Staff_Name = '"+ Name +"'";
    QSqlQuery NameQuery;
    if(NameQuery.exec(NameSQL))
    {
        if(NameQuery.next())
        {
            No = NameQuery.value(1).toString();
            department = NameQuery.value(5).toInt();
            post = NameQuery.value(6).toString();
        }
        else
        {
            QMessageBox::information(this, "提示", "请选择员工！");
            return;
        }
    }

    switch(department)
    {
        case 1:
            staff.strDepartment = "技术部";
            break;
        case 2:
            staff.strDepartment = "财务部";
            break;
        case 3:
            staff.strDepartment = "董事会";
            break;
        case 4:
            staff.strDepartment = "商务部";
            break;
        case 5:
            staff.strDepartment = "硬件部";
            break;
        case 6:
            staff.strDepartment = "软件部";
            break;
        case 7:
            staff.strDepartment = "平台部";
            break;
        default:
            staff.strDepartment = "未分组";
    }

    staff.strName = Name;
    staff.strNo = No;
    staff.strPost = post;

    m_Pseudomonas = new Pseudomonas();
    m_Pseudomonas->setAttribute(Qt::WA_DeleteOnClose);
    connect(m_Pseudomonas,SIGNAL(SigAddSuccess()),this,SLOT(SlotAddSuccess()));
    connect(this,SIGNAL(SigLeaveDate(QDate,SStaff)),m_Pseudomonas,SLOT(SlotLeaveDate(QDate,SStaff)));

    emit SigLeaveDate(ui->LeaveCalendarWidget->selectedDate(), staff);

    m_Pseudomonas->show();
}

/*****信号槽函数：添加成功*********/
void askforleave::SlotAddSuccess()
{
    _UpdateLeaveData();
}

void askforleave::on_leaveData_customContextMenuRequested(const QPoint &pos)
{
    DataMenu->addAction(DeleteAction);
    DataMenu->exec(QCursor::pos());
}

void askforleave::on_Leavetree_customContextMenuRequested(const QPoint &pos)
{
    LeavetreeMenu->addAction(LeaveAction);
    LeavetreeMenu->exec(QCursor::pos());
}

/*****控件焦点重置函数函数：鼠标点击离开此页面时*********/
void askforleave::SlotFocusOut()
{
    ui->leaveData->setCurrentItem(ui->leaveData->item(-1,-1));
    ui->LeaveCalendarWidget->setSelectedDate(QDate::currentDate());
}

void askforleave::on_QchkSTime_clicked()    //查询时段的开始时间
{
    if(!ui->QchkSTime->checkState())
    {
        ui->QdateSTime->setEnabled(false);
    }
    else
    {
        ui->QdateSTime->setEnabled(true);
    }
}

void askforleave::on_QchkETime_clicked()    //查询时段的结束时间
{
    if(!ui->QchkETime->checkState())
    {
        ui->QdateETime->setEnabled(false);
    }
    else
    {
        ui->QdateETime->setEnabled(true);
    }
}

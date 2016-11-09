#include "original.h"
#include "ui_original.h"

CORIGINAL::CORIGINAL(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CORIGINAL)
{
    ui->setupUi(this);
    _InitOriginalAttendance();  //原始考勤表初始化函数
    _InitTree();    //原始考勤表职位栏初始化函数
    ui->Button_Year->setText(QString::number(QDate::currentDate().year()));
    ui->Button_Month->setText(QString::number(QDate::currentDate().month()));

    ui->QpbtnBack->setToolTip(tr("返回"));
    ui->QpbtnLead->setToolTip(tr("导出报表"));

    isYear = true;  //默认是变动年份
}

CORIGINAL::~CORIGINAL()
{
    delete ui;
}

void CORIGINAL::_Update()
{
    _InitTree();
    ui->QOriginalAttendance->setRowCount(0);
    FindName.clear();
    ui->SearchData->clear();
}

//原始考勤表职位栏初始化函数
void CORIGINAL::_InitTree()
{
    m_List.clear();
    ui->QDepartment->clear();   //清空员工树
    ui->QDepartment->setHeaderHidden(true);
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
    ui->QDepartment->addTopLevelItem(B1);

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
    ui->QDepartment->addTopLevelItem(B2);
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

bool CORIGINAL::eventFilter(QObject *obj, QEvent *ev)   //事件过滤器？？？
{

}

//原始考勤表初始化函数
void CORIGINAL::_InitOriginalAttendance()
{
    ui->QOriginalAttendance->setColumnCount(7);//设置列数
    ui->QOriginalAttendance->setShowGrid(true);//显示表格线
    QStringList headers;
    headers<<"序号"<<"编号"<<"姓名"<<"日期"<<"星期"<<"时间"<<"打卡类型";
    ui->QOriginalAttendance->verticalHeader()->hide();//行头不显示
    ui->QOriginalAttendance->horizontalHeader()->setClickable(false);//行头不可选
    ui->QOriginalAttendance->setHorizontalHeaderLabels(headers);     //设置表头内容
    ui->QOriginalAttendance->horizontalHeader()->setHighlightSections(false);//表头坍塌
    ui->QOriginalAttendance->resizeColumnsToContents();
    ui->QOriginalAttendance->setFont(QFont("Helvetica"));//字体
    ui->QOriginalAttendance->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->QOriginalAttendance->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式：一行
    ui->QOriginalAttendance->setColumnWidth(0,40);
    ui->QOriginalAttendance->setColumnWidth(1,60);
    ui->QOriginalAttendance->setColumnWidth(2,80);
    ui->QOriginalAttendance->setColumnWidth(3,100);
    ui->QOriginalAttendance->setColumnWidth(4,100);
    ui->QOriginalAttendance->setColumnWidth(5,100);
    ui->QOriginalAttendance->setColumnWidth(6,100);
}

void CORIGINAL::_UpdateOriginalAttendance(QString strNo,int nFlag,QString strName)
{
    QDate StartDate;    //设置界面头的年月日
    StartDate.setYMD(ui->Button_Year->text().toInt(),ui->Button_Month->text().toInt(),1);
    QDate EndDate;
    EndDate.setYMD(ui->Button_Year->text().toInt(),ui->Button_Month->text().toInt(),StartDate.daysInMonth());
    QString strSTime = StartDate.toString("yyyy-MM-dd");
    QString strETime = EndDate.toString("yyyy-MM-dd");
    QString SQL1;
    QString SQL;
    QString CardType;
    if(nFlag == 0)  //离职
    {
        SQL = "SELECT a.Leave_Staff_No,a.Leave_Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,b.Card_Record_Type FROM `leave_staff` a,card_record b WHERE "
                "a.Leave_Staff_No = b.Card_Record_Staff_No and b.Card_Record_Date >= a.Leave_Staff_Enter_Day  and b.Card_Record_Date <= a.Leave_Deal_Day "
                " and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"'  ORDER BY a.Leave_Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";

        SQL1 = "SELECT a.Leave_Staff_No,a.Leave_Staff_Name,c.FillCard_Staff_No,c.FillCard_Date FROM `leave_staff` a,fillcard c WHERE "
                "a.Leave_Staff_No = c.FillCard_Staff_No and c.FillCard_Date >= a.Leave_Staff_Enter_Day  and c.FillCard_Date <= a.Leave_Deal_Day "
                " and c.FillCard_Date >= '"+strSTime+"'  and c.FillCard_Date <= '"+strETime+"'  ORDER BY a.Leave_Staff_No,c.FillCard_Date ASC";
    }
    else if(nFlag == 1)  //部门
    {
        SQL = "SELECT a.Staff_No,a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,b.Card_Record_Type FROM `staff` a,card_record b WHERE "
                "a.Staff_No = b.Card_Record_Staff_No and a.Staff_Department_No = '"+strNo+"' and b.Card_Record_Date >= a.Staff_Enter_Day  "
                " and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";

        SQL1 = "SELECT a.Staff_No,a.Staff_Name,c.FillCard_Staff_No,c.FillCard_Date FROM `staff` a,fillcard c WHERE "
                "a.Staff_No = c.FillCard_Staff_No and a.Staff_Department_No = '"+strNo+"' and c.FillCard_Date >= a.Staff_Enter_Day  "
                " and c.FillCard_Date >= '"+strSTime+"'  and c.FillCard_Date <= '"+strETime+"' ORDER BY a.Staff_No,c.FillCard_Date ASC";
    }
    else if(nFlag == 2) //在职
    {
        SQL = "SELECT a.Staff_No,a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,b.Card_Record_Type FROM `staff` a,card_record b WHERE "
                "b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' "
                "and  a.Staff_No = b.Card_Record_Staff_No ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";

        SQL1 = "SELECT a.Staff_No,a.Staff_Name,c.FillCard_Staff_No,c.FillCard_Date FROM `staff` a,fillcard c WHERE "
                "c.FillCard_Date >= a.Staff_Enter_Day and c.FillCard_Date >= a.Staff_Enter_Day and c.FillCard_Date >= '"+strSTime+"'  and c.FillCard_Date <= '"+strETime+"' "
                "and  a.Staff_No = c.FillCard_Staff_No ORDER BY c.FillCard_Staff_No,c.FillCard_Date ASC";
    }
    else if(nFlag == 3) //姓名
    {
        if(m_nFlag)
        {
            if(strName == "")
            {
                SQL =" SELECT a.Staff_No,a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,b.Card_Record_Type FROM"
                    "`staff` a,card_record b WHERE a.Staff_No = b.Card_Record_Staff_No and a.Staff_Name is NULL and "
                    "b.Card_Record_Date >= a.Staff_Enter_Day and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' "
                    "ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
            }
            else
            {
                SQL =" SELECT a.Staff_No,a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,b.Card_Record_Type FROM"
                        "`staff` a,card_record b WHERE a.Staff_No = b.Card_Record_Staff_No and a.Staff_Name = '"+strName+"' and "
                        "b.Card_Record_Date >= a.Staff_Enter_Day and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' "
                        "ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
            }
        }
        else
        {
            SQL =" SELECT a.Leave_Staff_No,a.Leave_Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,b.Card_Record_Type FROM"
                    "`leave_staff` a,card_record b WHERE a.Leave_Staff_No = b.Card_Record_Staff_No and a.Leave_Staff_Name = '"+strName+"' "
                    "and b.Card_Record_Date >= a.Leave_Staff_Enter_Day  and b.Card_Record_Date <= a.Leave_Deal_Day "
                    " and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' "
                    "ORDER BY a.Leave_Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
        }
    }
    else if(nFlag == 4)    //部门不存在
    {
        SQL = "SELECT a.Staff_No,a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,b.Card_Record_Type FROM (SELECT d.Staff_Name,d.Staff_No,"
                "d.Staff_Enter_Day,d.Staff_Department_No FROM (SELECT f.Staff_No,f.Staff_Name,f.Staff_Enter_Day,f.Staff_Department_No FROM `staff`"
                " f WHERE f.Staff_No NOT IN (SELECT e.Staff_No FROM staff e,department g WHERE e.Staff_Department_No = g.Department_No)) d) a,card_record b WHERE "
                "a.Staff_No = b.Card_Record_Staff_No  and b.Card_Record_Date >= a.Staff_Enter_Day  "
                " and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";

        SQL1 = "SELECT a.Staff_No,a.Staff_Name,c.FillCard_Staff_No,c.FillCard_Date,b.Card_Record_Type FROM (SELECT d.Staff_Name,d.Staff_No,"
                "d.Staff_Enter_Day,d.Staff_Department_No FROM (SELECT f.Staff_No,f.Staff_Name,f.Staff_Enter_Day,f.Staff_Department_No FROM `staff`"
                " f WHERE f.Staff_No NOT IN (SELECT e.Staff_No FROM staff e,department g WHERE e.Staff_Department_No = g.Department_No)) d) a,fillcard c WHERE "
                "a.Staff_No = c.FillCard_Staff_No  and c.FillCard_Date >= a.Staff_Enter_Day  "
                " and c.FillCard_Date >= '"+strSTime+"'  and c.FillCard_Date <= '"+strETime+"' ORDER BY a.Staff_No,c.FillCard_Date ASC";
    }

    QSqlQuery query1;
    QSqlQuery query;
    qDebug()<<"query1"<<query1.exec(SQL1)<<SQL1<<endl;
    if(query.exec(SQL))
    {
        int ti = 0;
        ui->QOriginalAttendance->setRowCount(ti);

        while(query.next())
        {
            ui->QOriginalAttendance->setRowCount(ti + 1);
            QTableWidgetItem *tabledmItem0 = new QTableWidgetItem(QString::number(ti+1));
            tabledmItem0->setTextAlignment(Qt::AlignCenter);
            ui->QOriginalAttendance->setItem(ti, 0, tabledmItem0);//第ti行、0列的内容（序号）

            QTableWidgetItem *tabledmItem1 = new QTableWidgetItem(QString(query.value(0).toByteArray().data()));
            tabledmItem1->setTextAlignment(Qt::AlignCenter);
            ui->QOriginalAttendance->setItem(ti, 1, tabledmItem1);//第ti行、1列的内容（编号）

            QTableWidgetItem *tabledmItem2 = new QTableWidgetItem(QString(query.value(1).toByteArray().data()));
            tabledmItem2->setTextAlignment(Qt::AlignCenter);
            ui->QOriginalAttendance->setItem(ti, 2, tabledmItem2);//第ti行、2列的内容（姓名）

            QTableWidgetItem *tabledmItem3 = new QTableWidgetItem(QString(query.value(2).toDate().toString("yyyy-MM-dd")));
            tabledmItem3->setTextAlignment(Qt::AlignCenter);
            ui->QOriginalAttendance->setItem(ti, 3, tabledmItem3);//第ti行、3列的内容（日期）

            QString strWeek;
            switch(query.value(2).toDate().dayOfWeek())
            {
                case 1:
                    strWeek = "一";
                    break;
                case 2:
                    strWeek = "二";
                    break;
                case 3:
                    strWeek = "三";
                    break;
                case 4:
                    strWeek = "四";
                    break;
                case 5:
                    strWeek = "五";
                    break;
                case 6:
                    strWeek = "六";
                    break;
                case 7:
                    strWeek = "日";
                    break;
                default:
                    strWeek = "错误";
                    break;
            }

            QTableWidgetItem *tabledmItem4 = new QTableWidgetItem("星期"+strWeek);
            tabledmItem4->setTextAlignment(Qt::AlignCenter);
            ui->QOriginalAttendance->setItem(ti, 4, tabledmItem4);//第ti行、4列的内容（星期）

            QTableWidgetItem *tabledmItem5 = new QTableWidgetItem(QString(query.value(3).toTime().toString("hh:mm:ss")));
            tabledmItem5->setTextAlignment(Qt::AlignCenter);
            ui->QOriginalAttendance->setItem(ti, 5, tabledmItem5);//第ti行、5列的内容（时间）

            switch(query.value(5).toInt())
            {
                case 1: CardType="上班卡";
                    break;
                case 2: CardType="下班卡";
                    break;
                default:
                    break;
            }
            QTableWidgetItem *tabledmItem6 = new QTableWidgetItem(QString(CardType));
            tabledmItem6->setTextAlignment(Qt::AlignCenter);       
            ui->QOriginalAttendance->setItem(ti, 6, tabledmItem6);//第ti行、6列的内容
            ti++;
        }
    }
}

void CORIGINAL::_SelectTarget(QString SelectName)
{
    m_nFlag = 1;
    QString strNo = "";
    QString strName = SelectName;
    if(strName == "在职")
    {
        _UpdateOriginalAttendance(strNo,2,"");
    }
    else if(strName == "离职")
    {
        m_nFlag = 0;
        _UpdateOriginalAttendance(strNo,0,"");
    }
    else
    {
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
        if(j)
        {
            _UpdateOriginalAttendance(strNo,1,"");
        }
        else if(strName == "未分配")
        {
            _UpdateOriginalAttendance(strNo,4,"");
        }
        else
        {
            _UpdateOriginalAttendance(0,3,strName);
        }
    }
}

//QDepartment QTreeWidget面板
void CORIGINAL::on_QDepartment_clicked(const QModelIndex &index)
{
    FindName.clear();
    FindName = ui->QDepartment->currentItem()->text(0);
    _SelectTarget(FindName);
}

//QpbtnLead按钮
void CORIGINAL::on_QpbtnLead_clicked()  //符合系统命名规范，系统自动关联槽函数
{
    if(ui->QOriginalAttendance->rowCount()>0)
    {
        QString filepath = QFileDialog::getSaveFileName(this,tr("save original"),".",tr("Microsoft Office 2013 (*.xls)"));
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

            worksheet->setProperty("Name","original");
            QAxObject *cell[6];
            for(int i=1;i<=6;i++)
            {
                cell[i-1]=excel->querySubObject("Cells(int,int)",1,i);
                QString str="";
                switch(i)
                {
                case 1: str="序号";
                    break;
                case 2: str="编号";
                    break;
                case 3: str="姓名";
                    break;
                case 4: str="日期";
                    break;
                case 5: str="星期";
                    break;
                case 6: str="时间";
                    break;
                default:
                    break;
                }
                cell[i-1]->setProperty("Value",str);

                if(i==4)
                {
                    cell[i-1]->setProperty("ColumnWidth",15);
                }
                cell[i-1]->setProperty("HorizontalAlignment",-4108);
                cell[i-1]->setProperty("VerticalAlignment",-4108);
                cell[i-1]->setProperty("WrapText",true);
            }
            QAxObject *NewCell[ui->QOriginalAttendance->rowCount()][6];
            for(int i=0;i<ui->QOriginalAttendance->rowCount();i++)
            {
                for(int j=0;j<6;j++)
                {
                    NewCell[i][j]=worksheet->querySubObject("Cells(int,int)",i+2,j+1);
                    NewCell[i][j]->setProperty("Value",ui->QOriginalAttendance->item(i,j)->text());
                    NewCell[i][j]->setProperty("HorizontalAlignment",-4108);
                    NewCell[i][j]->setProperty("VerticalAlignment",-4108);
                    NewCell[i][j]->setProperty("WrapText",true);
                }
            }
            workbook->dynamicCall("SaveAs (const QString&)",QDir::toNativeSeparators(filepath));
            workbook->dynamicCall("Close()");
            excel->dynamicCall("Quit()");
            delete excel;
            excel=NULL;

        }
    }
    else
    {
        QMessageBox::information(this,"提示","没有记录");
    }
}

/*****槽函数：月份被点击******/
void CORIGINAL::on_Button_Month_clicked()
{
    isYear = false;
}

/*****槽函数：年份被点击*****/
void CORIGINAL::on_Button_Year_clicked()
{
    isYear = true;
}

/*****槽函数：左日期选择按钮*****/
void CORIGINAL::on_Qpbtn_Left_clicked()
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
//    if(!FindName.isEmpty())
    FindName = "在职";
        _SelectTarget(FindName);
}

/*****槽函数：右日期选择按钮*****/
void CORIGINAL::on_Qpbtn_Right_clicked()
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
          int Year = ui->Button_Year->text().toInt();
          Year++;
          ui->Button_Year->setText(QString::number(Year));
          ui->Button_Month->setText(QString::number(1));
        }
    }
//    if(!FindName.isEmpty())
    FindName = "在职";
        _SelectTarget(FindName);
}

/*****槽函数：返回按钮*****/
void CORIGINAL::on_QpbtnBack_clicked()
{
    emit SigReturnMenu();
}

/*****槽函数：搜索按钮*****/
void CORIGINAL::on_QpbtnSearch_clicked()
{
    FindName.clear();
    FindName = ui->SearchData->text();
    if(!FindName.isEmpty())
        _SelectTarget(FindName);
}

void CORIGINAL::on_QpbtnStretching_clicked()
{
    int posX = 160-ui->QpbtnStretching->width();
    int posY = ui->QpbtnStretching->y();
    int nwidth = ui->QpbtnStretching->width();
    int nheight = ui->QpbtnStretching->height();
    int DayMinWidht =0;
    int DayMaxWidht = 160;
    if(ui->QpbtnStretching->pos().x() == posX)
    {
        ui->QpbtnSearch->hide();
        ui->SearchData->hide();
        ui->QDepartment->hide();
        ui->QDepartment->setGeometry(ui->QDepartment->x(),ui->QDepartment->y(),DayMinWidht,ui->QDepartment->height());
        ui->QpbtnStretching->setGeometry(0,posY,nwidth,nheight);
        ui->QpbtnStretching->setStyleSheet("background-image:url(:/images/StretchingR.png)");
    }
    else if(ui->QpbtnStretching->pos().x()==0)
    {
        ui->QpbtnSearch->show();
        ui->SearchData->show();
        ui->QDepartment->show();
        ui->QDepartment->setGeometry(ui->QDepartment->x(),ui->QDepartment->y(),DayMaxWidht,ui->QDepartment->height());
        ui->QpbtnStretching->setGeometry(posX,posY,nwidth,nheight);
        ui->QpbtnStretching->setStyleSheet("background-image:url(:/images/StretchingL.png)");
    }
}



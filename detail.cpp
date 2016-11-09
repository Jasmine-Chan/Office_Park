/*考勤明细表处理类*/

#include "detail.h"
#include "ui_detail.h"
#include <QValidator>
#include <QFileDialog>

CDETAIL::CDETAIL(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDETAIL)
{
    ui->setupUi(this);
    ui->Button_Year->setText(QString::number(QDate::currentDate().year()));
    ui->Button_Month->setText(QString::number(QDate::currentDate().month()));

    isYear = true;  //默认是变动年份

    ui->QpbtnBack->setToolTip(tr("返回"));
    ui->QpbtnLead->setToolTip(tr("导出报表"));

    _InitDetail();
    _InitTree();
}

CDETAIL::~CDETAIL()
{
    delete ui;
}

void CDETAIL::_Update()
{
    _InitTree();
    ui->QDetail->setRowCount(0);
    FindName.clear();
    ui->SearchData->clear();
}

/*****初始化函数：考勤明细表员工树******/
void CDETAIL::_InitTree()
{
    m_List.clear();
    ui->QDepartment->clear();
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

/*****初始化函数：考勤明细表数据******/
void CDETAIL::_InitDetail()
{
    ui->QDetail->setColumnCount(7);
    ui->QDetail->setShowGrid(true);//显示表格线
    QStringList headers;
    headers<<"序号"<<"编号"<<"姓名"<<"日期"<<"星期"<<"打卡1"<<"打卡2";
    ui->QDetail->verticalHeader()->hide();//行头不显示
    ui->QDetail->horizontalHeader()->setClickable(false);//行头不可选
    ui->QDetail->setHorizontalHeaderLabels(headers);
    ui->QDetail->horizontalHeader()->setHighlightSections(false);//表头坍塌
    ui->QDetail->resizeColumnsToContents();
    ui->QDetail->setFont(QFont("Helvetica"));//字体
    ui->QDetail->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->QDetail->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式：一行
    ui->QDetail->setColumnWidth(0,40);
    ui->QDetail->setColumnWidth(1,60);
    ui->QDetail->setColumnWidth(2,80);
    ui->QDetail->setColumnWidth(3,100);
    ui->QDetail->setColumnWidth(4,120);
    ui->QDetail->setColumnWidth(5,100);
    ui->QDetail->setColumnWidth(6,100);
}

/*****更新函数：考勤明细表数据******/
void CDETAIL::_UpdateDetail(QString strNo,int nFlag,QString strName)
{
    QDate StartDate;
    StartDate.setYMD(ui->Button_Year->text().toInt(),ui->Button_Month->text().toInt(),1);
    QDate EndDate;
    EndDate.setYMD(ui->Button_Year->text().toInt(),ui->Button_Month->text().toInt(),StartDate.daysInMonth());
    QString strSTime = StartDate.toString("yyyy-MM-dd");
    QString strETime = EndDate.toString("yyyy-MM-dd");
    QString SQL;
    if(nFlag == 0)
    {//离职
        SQL = "SELECT a.Leave_Staff_No,a.Leave_Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No FROM `leave_staff` a,card_record b WHERE "
                "a.Leave_Staff_No = b.Card_Record_Staff_No and b.Card_Record_Date >= a.Leave_Staff_Enter_Day  and b.Card_Record_Date <= a.Leave_Deal_Day "
                " and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"'  ORDER BY a.Leave_Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
    }
    else if(nFlag == 1)
    {//部门
        SQL = "SELECT a.Staff_No,a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No FROM `staff` a,card_record b WHERE "
                "a.Staff_No = b.Card_Record_Staff_No and a.Staff_Department_No = '"+strNo+"' and b.Card_Record_Date >= a.Staff_Enter_Day  "
                " and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
    }
    else if(nFlag == 2)
    {//在职
        SQL = "SELECT a.Staff_No,a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No FROM `staff` a,card_record b WHERE "
                "b.Card_Record_Date >= a.Staff_Enter_Day and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' "
                "and  a.Staff_No = b.Card_Record_Staff_No ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
    }
    else if(nFlag == 3)
    {//姓名
        if(m_nFlag)
        {
            if(strName == "")
            {
                SQL =" SELECT a.Staff_No,a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No FROM"
                        "`staff` a,card_record b WHERE a.Staff_No = b.Card_Record_Staff_No and a.Staff_Name is NULL and "
                        "b.Card_Record_Date >= a.Staff_Enter_Day and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' "
                        "ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
            }
            else
            {
                SQL =" SELECT a.Staff_No,a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No FROM"
                        "`staff` a,card_record b WHERE a.Staff_No = b.Card_Record_Staff_No and a.Staff_Name = '"+strName+"' and "
                        "b.Card_Record_Date >= a.Staff_Enter_Day and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' "
                        "ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
            }
        }
        else
        {
            SQL =" SELECT a.Leave_Staff_No,a.Leave_Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No FROM"
                    "`leave_staff` a,card_record b WHERE a.Leave_Staff_No = b.Card_Record_Staff_No and a.Leave_Staff_Name = '"+strName+"' "
                    "and b.Card_Record_Date >= a.Leave_Staff_Enter_Day  and b.Card_Record_Date <= a.Leave_Deal_Day "
                    " and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' "
                    "ORDER BY a.Leave_Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
        }
    }
    else if(nFlag == 4)     //部门不存在
    {
        SQL = "SELECT a.Staff_No,a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No FROM (SELECT d.Staff_Name,d.Staff_No,"
                "d.Staff_Enter_Day,d.Staff_Department_No FROM (SELECT f.Staff_No,f.Staff_Name,f.Staff_Enter_Day,f.Staff_Department_No FROM `staff`"
                " f WHERE f.Staff_No NOT IN (SELECT e.Staff_No FROM staff e,department g WHERE e.Staff_Department_No = g.Department_No)) d) a,card_record b WHERE "
                "a.Staff_No = b.Card_Record_Staff_No  and b.Card_Record_Date >= a.Staff_Enter_Day  "
                " and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
    }

    QSqlQuery query;
    qDebug()<<"SQL:"<<SQL;
    if(query.exec(SQL))
    {
        int ti = 0;
        int tj = 0;
        ui->QDetail->setRowCount(0);
        QDate OldDate(0,0,0);
        QString strNO1 = "";
        while(query.next())
        {
            if( strNO1 != QString(query.value(0).toByteArray().data()) || OldDate != query.value(2).toDate())
            {
                if(ti > 0)
                {
                    for(;tj < 12;tj++)
                    {
                        QTableWidgetItem *tabledmItem5 = new QTableWidgetItem("");
                        tabledmItem5->setTextAlignment(Qt::AlignCenter);
                        ui->QDetail->setItem((ti-1), 5+tj, tabledmItem5);
                    }
                }
                ti++;
                OldDate = query.value(2).toDate();
                ui->QDetail->setRowCount(ti);
                strNO1 = QString(query.value(0).toByteArray().data());
                tj = 0;
                QTableWidgetItem *tabledmItem0 = new QTableWidgetItem(QString::number(ti));
                tabledmItem0->setTextAlignment(Qt::AlignCenter);
                ui->QDetail->setItem((ti-1), 0, tabledmItem0);
                QTableWidgetItem *tabledmItem1 = new QTableWidgetItem(QString(query.value(0).toByteArray().data()));
                tabledmItem1->setTextAlignment(Qt::AlignCenter);
                ui->QDetail->setItem((ti-1), 1, tabledmItem1);

                QTableWidgetItem *tabledmItem2 = new QTableWidgetItem(QString(query.value(1).toByteArray().data()));
                tabledmItem2->setTextAlignment(Qt::AlignCenter);
                ui->QDetail->setItem((ti-1), 2, tabledmItem2);

                QTableWidgetItem *tabledmItem3 = new QTableWidgetItem(QString(query.value(2).toDate().toString("yyyy-MM-dd")));
                tabledmItem3->setTextAlignment(Qt::AlignCenter);
                ui->QDetail->setItem((ti-1), 3, tabledmItem3);
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
                ui->QDetail->setItem((ti-1), 4, tabledmItem4);
            }
            if(tj < 2)
            {
                QTableWidgetItem *tabledmItem5 = new QTableWidgetItem(QString(query.value(3).toTime().toString("hh:mm:ss")));
                tabledmItem5->setTextAlignment(Qt::AlignCenter);
                ui->QDetail->setItem((ti-1), 5+tj, tabledmItem5);
            }
            tj++;
        }
        if(ti > 0)
        {
            for(;tj < 2;tj++)
            {
                QTableWidgetItem *tabledmItem5 = new QTableWidgetItem("");
                tabledmItem5->setTextAlignment(Qt::AlignCenter);
                ui->QDetail->setItem((ti-1), 5+tj, tabledmItem5);
                ui->QDetail->item(ti-1,5+tj)->setBackgroundColor(QColor(185,235,200));
            }
        }
    }
}

/*****选择员工，显示当月的考勤明细表数据*****/
void CDETAIL::_SelectTarget(QString SelectName)
{
    m_nFlag = 1;
    QString strNo = "";
    QString strName = SelectName;
    if(strName == "在职")
    {
        _UpdateDetail(strNo,2,"");
    }
    else if(strName == "离职")
    {
        m_nFlag = 0;
        _UpdateDetail(strNo,0,"");
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
            _UpdateDetail(strNo,1,"");
        }
        else if(strName == "未分配")
        {
            _UpdateDetail(strNo,4,"");
        }
        else
        {
            _UpdateDetail(0,3,strName);
        }
    }
}

void CDETAIL::on_QDepartment_clicked(const QModelIndex &index)
{
    FindName.clear();
    FindName = ui->QDepartment->currentItem()->text(0);
    _SelectTarget(FindName);
}

//将内容保存到office表格文件中
void CDETAIL::on_QpbtnLead_clicked()
{
    if(ui->QDetail->rowCount() > 0)
    {
        QString filepath=QFileDialog::getSaveFileName(this,tr("Save orbit"),".",tr("Microsoft Office 2013 (*.xls)"));//获取保存路径
        if(!filepath.isEmpty()){
            QAxObject *excel = new QAxObject(this);
            excel->setControl("Excel.Application");//连接Excel控件
            excel->dynamicCall("SetVisible (bool Visible)","false");//不显示窗体
            excel->setProperty("DisplayAlerts", false);//不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示

            QAxObject *workbooks = excel->querySubObject("WorkBooks");//获取工作簿集合
            workbooks->dynamicCall("Add");//新建一个工作簿
            QAxObject *workbook = excel->querySubObject("ActiveWorkBook");//获取当前工作簿
            QAxObject *worksheets = workbook->querySubObject("Sheets");//获取工作表集合
            QAxObject *worksheet = worksheets->querySubObject("Item(int)",1);//获取工作表集合的工作表1，即sheet1

            worksheet->setProperty("Name", "anenda");  //设置工作表名称
            QAxObject *cell[17];
            for(int i = 1;i <= 17;i++)
            {
                cell[i-1] = worksheet->querySubObject("Cells(int,int)", 1, i);
                QString str = "";
                switch(i)
                {
                    case 1:
                        str = "序号";
                        break;
                    case 2:
                        str = "员工编号";
                        break;
                    case 3:
                        str = "姓名";
                        break;
                    case 4:
                        str = "日期";
                        break;
                    case 5:
                        str = "星期";
                        break;
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                    case 10:
                    case 11:
                    case 12:
                    case 13:
                    case 14:
                    case 15:
                    case 16:
                    case 17:
                        str = "打卡" + QString::number(i - 5);
                        break;
                    default:
                        break;
                }
                cell[i-1]->setProperty("Value", str);
                if(i == 4)
                {
                    cell[i-1]->setProperty("ColumnWidth", 15);
                }
                cell[i-1]->setProperty("HorizontalAlignment", -4108); //左对齐（xlLeft）：-4131  居中（xlCenter）：-4108  右对齐（xlRight）：-4152
                cell[i-1]->setProperty("VerticalAlignment", -4108);  //上对齐（xlTop）-4160 居中（xlCenter）：-4108  下对齐（xlBottom）：-4107
                cell[i-1]->setProperty("WrapText", true);  //内容过多，自动换行
            }

            QAxObject *NewCell[ui->QDetail->rowCount()][17];
            for(int j = 1;j <= ui->QDetail->rowCount();j++)
            {
                for(int r = 1;r <= 17;r++)
                {
                    NewCell[j-1][r-1] = worksheet->querySubObject("Cells(int,int)", j + 1, r);
                    NewCell[j-1][r-1]->setProperty("Value", ui->QDetail->item(j-1,r-1)->text());
                    NewCell[j-1][r-1]->setProperty("HorizontalAlignment", -4108); //左对齐（xlLeft）：-4131  居中（xlCenter）：-4108  右对齐（xlRight）：-4152
                    NewCell[j-1][r-1]->setProperty("VerticalAlignment", -4108);  //上对齐（xlTop）-4160 居中（xlCenter）：-4108  下对齐（xlBottom）：-4107
                    NewCell[j-1][r-1]->setProperty("WrapText", true);  //内容过多，自动换行
                }
            }

            workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(filepath));//保存至filepath，注意一定要用QDir::toNativeSeparators将路径中的"/"转换为"\"，不然一定保存不了。
            workbook->dynamicCall("Close()");//关闭工作簿
            excel->dynamicCall("Quit()");//关闭excel
            delete excel;
            excel = NULL;
        }
    }
    else
    {
        QMessageBox::information(this,"提示","没有记录");
    }
}

/*****槽函数：月份被点击******/
void CDETAIL::on_Button_Month_clicked()
{
    isYear = false;
}

/*****槽函数：年份被点击*****/
void CDETAIL::on_Button_Year_clicked()
{
    isYear = true;
}

/*****槽函数：左日期选择按钮*****/
void CDETAIL::on_Qpbtn_Left_clicked()
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
void CDETAIL::on_Qpbtn_Right_clicked()
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
void CDETAIL::on_QpbtnBack_clicked()
{
    emit SigReturnMenu();
}

/*****槽函数：搜索按钮*****/
void CDETAIL::on_QpbtnSearch_clicked()
{
    FindName.clear();
    FindName = ui->SearchData->text();
    if(!FindName.isEmpty())
        _SelectTarget(FindName);
}

void CDETAIL::on_QpbtnStretching_clicked()
{
    int posX = 160-ui->QpbtnStretching->width();
    int posY = ui->QpbtnStretching->y();
    int nwidth = ui->QpbtnStretching->width();
    int nheight = ui->QpbtnStretching->height();
    int DayMinWidht =0;
    int DayMaxWidht = 160;
    if(ui->QpbtnStretching->pos().x()==posX)
    {
        ui->QDepartment->setGeometry(ui->QDepartment->x(),ui->QDepartment->y(),DayMinWidht,ui->QDepartment->height());
        ui->QpbtnStretching->setGeometry(0,posY,nwidth,nheight);
        ui->QpbtnStretching->setStyleSheet("background-image:url(:/images/StretchingR.png)");
    }
    else if(ui->QpbtnStretching->pos().x()==0)
    {
        ui->QDepartment->setGeometry(ui->QDepartment->x(),ui->QDepartment->y(),DayMaxWidht,ui->QDepartment->height());
        ui->QpbtnStretching->setGeometry(posX,posY,nwidth,nheight);
        ui->QpbtnStretching->setStyleSheet("background-image:url(:/images/StretchingL.png)");
    }
}

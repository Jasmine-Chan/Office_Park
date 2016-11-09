/*离职人员信息处理类*/
#include "dimission.h"
#include "ui_dimission.h"

CDIMISSION::CDIMISSION(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDIMISSION)
{
    ui->setupUi(this);

    DetailOrEdit = 0;
    ti =0;
    m_nChangeNoList = 0;
    dimissionIsEmpty = true;
    ui->Dimission_stacked->setCurrentIndex(0);
    ui->QpbtnSearch->setShortcut( QKeySequence::InsertParagraphSeparator );
    ui->Head_Photo_D->setDisabled(true);
    ui->Name_D->setDisabled(true);
    ui->Number_D->setDisabled(true);
    ui->Keyword_Edit->setPlaceholderText("关键字(名字或工号)");

    DimissionMenu = new QMenu(ui->QDimission);
    DeleteAction = new QAction(tr("删除"),this);
    DetailAction = new QAction(tr("详情"),this);
    EditAction = new QAction("编辑",this);

    ui->QDimission->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->QDimissionList->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->Dimission_stacked->setCurrentIndex(0);

    ui->QpbtnRetuen->setToolTip(tr("退出"));
    ui->QpbtnLogout->setToolTip(tr("删除"));
    ui->QpbtnEdit->setToolTip(tr("编辑"));

    connect(DeleteAction, SIGNAL(triggered()), this, SLOT(SlotDelete()));
    connect(DetailAction, SIGNAL(triggered()), this, SLOT(SlotDetail()));
    connect(EditAction,SIGNAL(triggered()),this,SLOT(SlotEdit()));

    _InitDimission();
    _InitDimissionList();
    _SelectStatus(0);
    _Change();
}

CDIMISSION::~CDIMISSION()
{
    delete ui;
}

void CDIMISSION::_Update()
{
    ui->Dimission_stacked->setCurrentIndex(0);
    ui->Keyword_Edit->clear();
    ui->QDimission->clear();
}

void CDIMISSION::_Change()
{
    if(m_nChangeNoList)
    {
        ui->QDimission->show();
        ui->QDimissionList->hide();
        m_nChangeNoList = 0;
    }
    else
    {
        ui->QDimission->hide();
        ui->QDimissionList->show();
        m_nChangeNoList = 1;
    }
    _UpdateDimission(m_strSql);
}

void CDIMISSION::_InitDimissionList()
{
    ui->QDimissionList->setColumnCount(11);
    ui->QDimissionList->setShowGrid(true);//显示表格线
    QStringList headers;
    headers<<"序号"<<"姓名"<<"性别"<<"工号"<<"职位"<<"入职时间"<<"部门"<<"身份证号"<<"离职时间"<<"处理人"<<"离职原因";
    ui->QDimissionList->verticalHeader()->hide();//行头不显示
    ui->QDimissionList->horizontalHeader()->setClickable(false);//行头不可选
    ui->QDimissionList->setHorizontalHeaderLabels(headers);
    ui->QDimissionList->horizontalHeader()->setHighlightSections(false);
    ui->QDimissionList->resizeColumnsToContents();
    ui->QDimissionList->setFont(QFont("Helvetica"));
    ui->QDimissionList->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->QDimissionList->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式：一行
    ui->QDimissionList->setColumnWidth(0,50);
    ui->QDimissionList->setColumnWidth(1,50);
    ui->QDimissionList->setColumnWidth(2,50);
    ui->QDimissionList->setColumnWidth(3,50);
    ui->QDimissionList->setColumnWidth(4,50);
    ui->QDimissionList->setColumnWidth(5,100);
    ui->QDimissionList->setColumnWidth(6,50);
    ui->QDimissionList->setColumnWidth(7,100);
    ui->QDimissionList->setColumnWidth(8,100);
    ui->QDimissionList->setColumnWidth(9,100);
    ui->QDimissionList->setColumnWidth(10,100);
}

/*****初始化函数：初始化界面*****/
void CDIMISSION::_InitDimission()
{
    ui->QDimission->setColumnCount(5);
    ui->QDimission->verticalHeader()->hide();//行头不显示
    ui->QDimission->horizontalHeader()->hide();
    ui->QDimission->resizeColumnsToContents();
    ui->QDimission->setShowGrid(false);
    ui->QDimission->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->QDimission->setColumnWidth(0,120);
    ui->QDimission->setColumnWidth(1,120);
    ui->QDimission->setColumnWidth(2,120);
    ui->QDimission->setColumnWidth(3,120);
    ui->QDimission->setColumnWidth(4,120);
}

/*****更新函数：离职员工数据*****/
void CDIMISSION::_UpdateDimission(QString SQLq)
{
    dimissionIsEmpty = true;
    QString SQL;
    if(SQLq.length())
        SQL = SQLq;
    else
        SQL = "SELECT * FROM leave_staff a ORDER BY a.Leave_Deal_Day DESC";

    m_strSql = SQL;
    QSqlQuery query;
    if(m_nChangeNoList == 0)
    {
        if(query.exec(SQL))
        {
            ti=0;
            ui->QDimission->setRowCount(ti);
            while(query.next())
            {
                ui->QDimission->setRowHeight(ti/5,90);
                ui->QDimission->setRowCount(ti/5+1);
                dimissionIsEmpty = false;
                QTableWidgetItem *tabledmItem = new QTableWidgetItem(QString(query.value(2).toString()));
                tabledmItem->setTextAlignment(Qt::AlignVCenter);
                ui->QDimission->setItem(ti/5,ti%5,tabledmItem);

                ui->QDimission->item(ti/5,ti%5)->setIcon(QIcon(query.value(12).toString()));
                ti++;
            }
            ui->QDimission->setIconSize(QSize(120,90));
            for(int i = 4;i>((ti-1)%5);i--)
            {
                ui->QDimission->setRowHeight(ti/5,90);
                QTableWidgetItem *tabledmItemK = new QTableWidgetItem(QString(""));
                tabledmItemK->setTextAlignment(Qt::AlignCenter);
                ui->QDimission->setItem(ti/5,i,tabledmItemK);
            }
        }
    }
    else
    {
        if(query.exec(SQL))
        {
            int ti = 0;
            ui->QDimissionList->setRowCount(ti);
            while(query.next()){
                dimissionIsEmpty = false;
                ui->QDimissionList->setRowCount(ti+1);
                QTableWidgetItem *tablemItem0 = new QTableWidgetItem(QString::number(ti+1));
                tablemItem0->setTextAlignment(Qt::AlignCenter);
                ui->QDimissionList->setItem(ti,0,tablemItem0);

                QTableWidgetItem *tablemItem1 = new QTableWidgetItem(query.value(2).toString());
                tablemItem1->setTextAlignment(Qt::AlignCenter);
                ui->QDimissionList->setItem(ti,1,tablemItem1);
                QString str;
                if(query.value(3).toInt() == 0)
                {
                    str = "男";
                }
                else
                {
                    str = "女";
                }
                QTableWidgetItem *tablemItem2 = new QTableWidgetItem(str);
                tablemItem2->setTextAlignment(Qt::AlignCenter);
                ui->QDimissionList->setItem(ti,2,tablemItem2);

                QTableWidgetItem *tablemItem3 = new QTableWidgetItem(query.value(1).toString());
                tablemItem3->setTextAlignment(Qt::AlignCenter);
                ui->QDimissionList->setItem(ti,3,tablemItem3);

                QTableWidgetItem *tablemItem4 = new QTableWidgetItem(query.value(6).toString());
                tablemItem4->setTextAlignment(Qt::AlignCenter);
                ui->QDimissionList->setItem(ti,4,tablemItem4);

                QTableWidgetItem *tablemItem5 = new QTableWidgetItem(query.value(7).toDateTime().toString("yyyy-MM-dd"));
                tablemItem5->setTextAlignment(Qt::AlignCenter);
                ui->QDimissionList->setItem(ti,5,tablemItem5);

                QTableWidgetItem *tablemItem6 = new QTableWidgetItem(query.value(5).toString());
                tablemItem6->setTextAlignment(Qt::AlignCenter);
                ui->QDimissionList->setItem(ti,6,tablemItem6);

                QTableWidgetItem *tablemItem7 = new QTableWidgetItem(query.value(4).toString());
                tablemItem7->setTextAlignment(Qt::AlignCenter);
                ui->QDimissionList->setItem(ti,7,tablemItem7);

                QTableWidgetItem *tablemItem8 = new QTableWidgetItem(query.value(9).toDateTime().toString("yyyy-MM-dd"));
                tablemItem8->setTextAlignment(Qt::AlignCenter);
                ui->QDimissionList->setItem(ti,8,tablemItem8);

                QTableWidgetItem *tablemItem9 = new QTableWidgetItem(query.value(10).toString());
                tablemItem9->setTextAlignment(Qt::AlignCenter);
                ui->QDimissionList->setItem(ti,9,tablemItem9);

                QTableWidgetItem *tablemItem10 = new QTableWidgetItem(query.value(11).toString());
                tablemItem10->setTextAlignment(Qt::AlignCenter);
                ui->QDimissionList->setItem(ti,10,tablemItem10);
                ti++;
            }
        }
    }
}

/*****信号槽函数：删除离职员工*****/
void CDIMISSION::SlotDelete()
{
    if(m_nChangeNoList == 0)
    {
        if(ui->QDimission->rowCount()>0&&ui->QDimission->columnCount()>0&&!ui->QDimission->currentItem()->text().isEmpty())
        {
            QString SQL = "delete FROM `leave_staff` where leave_staff.Leave_Staff_Name = '"+ui->QDimission->currentItem()->text()+"'";
            QSqlQuery query;
            if(query.exec(SQL))
            {
                QMessageBox::information(this,"提示","删除成功");
            }
            else
            {
                QMessageBox::information(this,"提示","请选择删除对象");
            }
            _UpdateDimission(m_strSql);
        }
        else
            QMessageBox::information(this,tr("提示"),tr("目标选取失败"));
    }
    else
    {
        int nRow = ui->QDimissionList->currentRow();
        if(nRow >= 0 )
        {
            QString SQL = "delete FROM `leave_staff` where Leave_Staff_Enter_Day = '"+ui->QDimissionList->item(nRow,5)->text()+"' and leave_staff.Leave_Staff_Name = '"+ui->QDimissionList->item(nRow,1)->text()+"'";
            QSqlQuery query;
            if(query.exec(SQL))
            {
                QMessageBox::information(this,"提示","删除成功");
            }
            else
            {
                QMessageBox::information(this,"提示","请选择删除对象");
            }
            _UpdateDimission(m_strSql);
        }
        else
        {
            QMessageBox::information(this,tr("提示"),tr("列表中没有数据，无法执行删除操作"));
        }
    }
}

/*****信号槽函数：查看离职员工信息*****/
void CDIMISSION::SlotDetail()
{
    if(m_nChangeNoList == 0)
    {
        if(ui->QDimission->rowCount()>0&&ui->QDimission->columnCount()>0&&!ui->QDimission->currentItem()->text().isEmpty())
        {
            DetailOrEdit = 0;
            _SelectStatus(0);
            _InitDepartmentForQcombobox();
            _InitConcreteInformation();
        }
        else
            QMessageBox::information(this,tr("提示"),tr("目标选取失败"));
    }
    else
    {
        if(ui->QDimissionList->currentRow() >= 0)
        {
            DetailOrEdit = 0;
            _SelectStatus(0);
            _InitDepartmentForQcombobox();
            _InitConcreteInformation();
        }
        else
        {
            QMessageBox::information(this,tr("提示"),tr("列表中没有数据，无法查看"));
        }
    }
}

/*****信号槽函数：编辑离职员工信息*****/
void CDIMISSION::SlotEdit()
{
    if(m_nChangeNoList == 0)
    {
        if(ui->QDimission->rowCount()>0 && ui->QDimission->columnCount()>0 && !ui->QDimission->currentItem()->text().isEmpty())
        {
            DetailOrEdit =1;
            _SelectStatus(1);
            _InitDepartmentForQcombobox();
            _InitConcreteInformation();
        }
        else
            QMessageBox::information(this,tr("提示"),tr("目标选取失败"));
    }
    else
    {
        if(ui->QDimissionList->currentRow() >= 0)
        {
            DetailOrEdit =1;
            _SelectStatus(1);
            _InitDepartmentForQcombobox();
            _InitConcreteInformation();
        }
        else
        {
            QMessageBox::information(this,tr("提示"),tr("列表中没有数据，无法选取对象进行编辑"));
        }
    }
}

void CDIMISSION::on_QDimission_customContextMenuRequested(const QPoint &pos)
{
    DimissionMenu->addAction(DetailAction);
    DimissionMenu->addAction(DeleteAction);
    DimissionMenu->addAction(EditAction);
    DimissionMenu->exec(QCursor::pos());
}

void CDIMISSION::on_Head_Photo_D_clicked()
{
    QString Path = QFileDialog::getOpenFileName();

    QByteArray faly = ui->QDimission->item(Row,1)->text().toLatin1();
    QByteArray byte1 = Path.toUtf8();
    ::WritePrivateProfileStringA("Head",faly.data(), byte1.data(), "E:\\officepark\\DimissionHead.ini");
    ui->Head_Photo_D->setStyleSheet("border-image:url("+Path+")");
}

void CDIMISSION::on_QpbtnClose_clicked()
{
    _SelectStatus(DetailOrEdit);
    _InitConcreteInformation();
}

/*****初始化函数：离职员工详细信息*****/
void CDIMISSION::_InitConcreteInformation()
{
    QString SQL;
    if(m_nChangeNoList == 0)
    {
        SQL = "SELECT * FROM leave_staff a WHERE a.Leave_Staff_Name = '"+ui->QDimission->currentItem()->text()+"'";
    }
    else
    {
        SQL = "SELECT * FROM leave_staff a WHERE a.Leave_Staff_Name = '"+ui->QDimissionList->item(ui->QDimissionList->currentRow(),1)->text()+"'";
    }
    QSqlQuery query;
    if(query.exec(SQL))
    {
        if(query.next())
        {
            ui->Name_D->setText(query.value(2).toString());
            ui->Number_D->setText(query.value(1).toString());
            ui->Sex_D->setCurrentIndex(query.value(3).toInt());
            ui->ID_Number_D->setText(query.value(4).toString());
            QString DepartmentSQL = "SELECT * FROM department ORDER BY Department_No";
            QSqlQuery departmentquery;
            int Index = 0;
            departmentquery.exec(DepartmentSQL);
            ui->Department_D->setCurrentIndex(0);
            while(departmentquery.next())
            {
                Index++;
                if(departmentquery.value(1).toInt()==query.value(5).toInt())
                {
                    ui->Department_D->setCurrentIndex(Index);
                    break;
                }
            }
            ui->Job_D->setText(query.value(6).toString());
            ui->Entry_Time_D->setDate(query.value(7).toDate());
            ui->Dimission_Time_D->setDate(query.value(8).toDate());
            ui->Leave_Deal_Day_D->setDate(query.value(9).toDate());
            ui->MakeMan_D->setText(query.value(10).toString());
            ui->Reason_D->setText(query.value(11).toString());
            ui->Head_Photo_D->setStyleSheet("border-image:url("+query.value(12).toString()+")");
            ui->Dimission_stacked->setCurrentIndex(1);
        }
    }
}

/*****初始化函数：添加详细信息中的部门选项控件*****/
void CDIMISSION::_InitDepartmentForQcombobox()
{
    ui->Department_D->clear();
    ui->Department_D->addItem("未分组");
    QString SQL = "SELECT * FROM department ORDER BY Department_No";
    QSqlQuery query;
    if(query.exec(SQL))
    {
        while(query.next())
            ui->Department_D->addItem(query.value(2).toString());
    }
}

void CDIMISSION::_SelectStatus(int Status)
{
    if(!Status)
    {
        ui->Sex_D->setDisabled(true);
        ui->ID_Number_D->setDisabled(true);
        ui->Department_D->setDisabled(true);
        ui->Job_D->setDisabled(true);
        ui->Entry_Time_D->setDisabled(true);
        ui->Dimission_Time_D->setDisabled(true);
        ui->Leave_Deal_Day_D->setDisabled(true);
        ui->MakeMan_D->setDisabled(true);
        ui->Reason_D->setDisabled(true);
    }
    else
    {
        ui->Sex_D->setDisabled(false);
        ui->ID_Number_D->setDisabled(false);
        ui->Department_D->setDisabled(false);
        ui->Job_D->setDisabled(false);
        ui->Entry_Time_D->setDisabled(false);
        ui->Dimission_Time_D->setDisabled(false);
        ui->Leave_Deal_Day_D->setDisabled(false);
        ui->MakeMan_D->setDisabled(false);
        ui->Reason_D->setDisabled(false);
        _SetFocusOrder();
    }
}

void CDIMISSION::on_QpbtnSave_clicked()
{
    QString Department = "";
    QString SelectSQL = "SELECT * FROM department WHERE department.Department_Name = '"+ui->Department_D->currentText()+"'";
    QSqlQuery selectquery;
    selectquery.exec(SelectSQL);
    if(selectquery.next())
    {
        Department = selectquery.value(1).toString();   //部门ID
        QString SQL = "UPDATE leave_staff SET Leave_Staff_No = '"+ui->Number_D->text()+"',Leave_Staff_Name = '"+ui->Name_D->text()+"',Leave_Staff_Sex = '"+QString::number(ui->Sex_D->currentIndex())+"',"
                "Leave_Staff_Id_Card = '"+ui->ID_Number_D->text()+"',Leave_Staff_Department_Id = '"+Department+"',Leave_Staff_Post = '"+ui->Job_D->text()+"',Leave_Staff_Enter_Day = '"+ui->Entry_Time_D->text()+"',"
                "Leave_Day = '"+ui->Dimission_Time_D->text()+"',Leave_Deal_Day = '"+ui->Leave_Deal_Day_D->text()+"',Leave_Deal = '"+ui->MakeMan_D->text()+"',Leave_Reasons = '"+ui->Reason_D->text()+"' WHERE Leave_Staff_Name = "
                "'"+ui->Name_D->text()+"' AND Leave_Staff_No = '"+ui->Number_D->text()+"'";
        QSqlQuery query;

        if(query.exec(SQL))
        {
            QMessageBox::information(this,"提示","数据保存成功");
        }
        else
        {
            QMessageBox::information(this,"提示","保存失败");
        }
    }
    else
    {
         QMessageBox::information(this,"提示","保存失败,部门不存在");
    }
    _UpdateDimission("");       //修改完之后，刷新界面
}

void CDIMISSION::on_QpbtnSearch_clicked()
{
    ui->QDimission->setCurrentItem(ui->QDimission->item(-1,-1));
    QString Number_ID = "";

    switch(ui->Keyword_Edit->text().length())
    {
        case 1: Number_ID = "0000"+ui->Keyword_Edit->text();
            break;
        case 2:Number_ID = "000"+ui->Keyword_Edit->text();
            break;
        case 3:Number_ID = "00"+ui->Keyword_Edit->text();
            break;
        case 4:Number_ID = "0"+ui->Keyword_Edit->text();
            break;
        case 5:Number_ID = ui->Keyword_Edit->text();
            break;
        default:
            break;
    }

    if(!ui->Keyword_Edit->text().isEmpty())
    {
        QString NameSQL = "SELECT * FROM leave_staff WHERE Leave_Staff_Name like '%"+ui->Keyword_Edit->text()+"%' ORDER BY Leave_Deal_Day DESC";
        QSqlQuery namequery;
        qDebug()<<namequery.exec(NameSQL)<<endl;
        if(namequery.next())
        {
            _UpdateDimission(NameSQL);
        }
        else
        {
            NameSQL.clear();
            namequery.clear();
            NameSQL = "SELECT * FROM leave_staff WHERE Leave_Staff_No like '%"+Number_ID+"%' ORDER BY Leave_Deal_Day DESC";
            qDebug()<<namequery.exec(NameSQL)<<NameSQL<<endl;
            if(namequery.next())
                _UpdateDimission(NameSQL);
            else
            {
                ui->QDimission->setRowCount(0);
                QMessageBox::information(this,"提示","未找到匹配项");
                ui->QDimission->clear();
            }
        }
    }
    else
    {
        _UpdateDimission("");
        if(dimissionIsEmpty)
            QMessageBox::information(this,"提示","离职员工为空");
    }

}

void CDIMISSION::on_QpbtnRetuen_clicked()
{
     ui->Dimission_stacked->setCurrentIndex(0);
}

void CDIMISSION::on_QpbtnLogout_clicked()
{
    SlotDelete();
    ui->Dimission_stacked->setCurrentIndex(0);
}

void CDIMISSION::on_QpbtnEdit_clicked()
{
    _SelectStatus(1);
}

void CDIMISSION::SlotFocusOut()
{
    ui->QDimission->setCurrentItem(ui->QDimission->item(-1,-1));
}

void CDIMISSION::_SetFocusOrder()
{
    ui->Sex_D->setFocus();
    ui->Sex_D->setStyleSheet("QComboBox:focus{border:1px solid green;}");
    ui->Department_D->setStyleSheet("QComboBox:focus{border:1px solid green;}");
    ui->Job_D->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->ID_Number_D->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->Entry_Time_D->setStyleSheet("QDateEdit:focus{border:1px solid green;}");
    ui->Dimission_Time_D->setStyleSheet("QDateEdit:focus{border:1px solid green;}");
    ui->Leave_Deal_Day_D->setStyleSheet("QDateEdit:focus{border:1px solid green;}");
    ui->MakeMan_D->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->Reason_D->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    QWidget::setTabOrder(ui->Sex_D,ui->Department_D);
    QWidget::setTabOrder(ui->Department_D,ui->Job_D);
    QWidget::setTabOrder(ui->Job_D,ui->ID_Number_D);
    QWidget::setTabOrder(ui->ID_Number_D,ui->Entry_Time_D);
    QWidget::setTabOrder(ui->Entry_Time_D,ui->Dimission_Time_D);
    QWidget::setTabOrder(ui->Dimission_Time_D,ui->Leave_Deal_Day_D);
    QWidget::setTabOrder(ui->Leave_Deal_Day_D,ui->MakeMan_D);
    QWidget::setTabOrder(ui->MakeMan_D,ui->Reason_D);
}

void CDIMISSION::on_QpbtnChange_clicked()
{
    _Change();
}

void CDIMISSION::on_QDimissionList_customContextMenuRequested(const QPoint &pos)
{
    DimissionMenu->addAction(DetailAction);
    DimissionMenu->addAction(DeleteAction);
    DimissionMenu->addAction(EditAction);
    DimissionMenu->exec(QCursor::pos());
}

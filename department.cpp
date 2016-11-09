#include "department.h"
#include "ui_department.h"

CDEPARTMENT::CDEPARTMENT(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDEPARTMENT)
{
    ui->setupUi(this);
    ti=0;   //初始化，公司数量为0
    DepartmentMenu = new QMenu(ui->departmentName);
    UpdateAction = new QAction(tr("修改"),this);
    DeleteAction = new QAction(tr("删除"),this);
    ui->departmentName->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(UpdateAction,SIGNAL(triggered()),this,SLOT(SlotUpdateDepament()));
    connect(DeleteAction,SIGNAL(triggered()),this,SLOT(SlotDeleteDepament()));
    m_nFlag = 0;
    AddRow = -1;
    AddColumn = -1;
    UpdateRow = -1;
    UpdateColumn = -1;
    _InitDepartment();
    _UpdateDepartment();
    ui->departmentName->setMouseTracking(true);
}

CDEPARTMENT::~CDEPARTMENT()
{
    delete ui;
}

void CDEPARTMENT::_Update()
{
    ui->Name_lineEdit->clear();
    ui->No_lineEdit->clear();
    ui->leader_lineEdit->clear();
    ui->parent_lineEdit->clear();
    _UpdateDepartment();
    m_nFlag = 0;
}

/*****初始化函数：部门信息数据*****/
void CDEPARTMENT::_InitDepartment()
{
    ui->departmentName->setColumnCount(3);//列数为3
    ui->departmentName->setShowGrid(true);//显示表格线
    ui->departmentName->setFont(QFont("Helvetica"));//设置字体
    ui->departmentName->verticalHeader()->hide();//隐藏行头
    ui->departmentName->horizontalHeader()->hide();//隐藏列头
    ui->departmentName->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置不可编辑
    ui->departmentName->setColumnWidth(0,226);//设置每列的宽度
    ui->departmentName->setColumnWidth(1,226);
    ui->departmentName->setColumnWidth(2,226);
}

/********界面更新*********/
void CDEPARTMENT::_UpdateDepartment()
{
    _SelectStatus(0);   //设置文本输入框的输入状态
    _List();            //初始化函数：部门链表
    QString SQL = "SELECT * from department ORDER BY Department_No";
    QSqlQuery query;
    if(query.exec(SQL))
    {
        ti = 0;//部门数量
        ui->departmentName->setRowCount(ti);//根据部门数量设置行数
        while(query.next())
        {
            ui->departmentName->setRowHeight(ti/3,110);//行编号从0开始
            ui->departmentName->setRowCount(ti/3+1);
            //query.value(2) 部门名字
            QTableWidgetItem *tabledmItem = new QTableWidgetItem(QString(query.value(2).toString()));
            tabledmItem->setTextAlignment(Qt::AlignCenter);
            ui->departmentName->setItem(ti/3,ti%3,tabledmItem);
            ti++;
        }

        ui->departmentName->setRowHeight(ti/3,110);
        ui->departmentName->setRowCount(ti/3+1);

        QTableWidgetItem *tabledmItemN = new QTableWidgetItem(QString("+"));
        tabledmItemN->setTextAlignment(Qt::AlignCenter);
        ui->departmentName->setItem(ti/3,ti%3,tabledmItemN);
        for(int i = 2; i > (ti%3); i--)     //一行不足时，剩余的用空白显示
        {
            ui->departmentName->setRowHeight(ti/3,110);
            QTableWidgetItem *tabledmItemK = new QTableWidgetItem(QString(""));
            tabledmItemK->setTextAlignment(Qt::AlignCenter);
            ui->departmentName->setItem(ti/3,i,tabledmItemK);
        }
    }
}

/*****初始化函数：部门链表*****/
void CDEPARTMENT::_List()
{
    m_List.clear();
    QString SQL = "SELECT * from department ORDER BY Department_No DESC";
    QSqlQuery query;

    SList data1;
    data1.Id = 0;
    data1.strNo = "";
    data1.strName = "";
    m_List.append(data1);//为什么要在这里先添加一条数据

    if(query.exec(SQL))
    {
        int i = 1;
        while(query.next())
        {
            SList data;
            data.Id = i;
            data.strNo = query.value(1).toString();
            data.strName = query.value(2).toString();
            m_List.append(data);
            i++;
        }
    }
}

/*****槽函数：保存按钮********/
void CDEPARTMENT::on_QpbtnSave_clicked()
{
    if(ui->Name_lineEdit->text().isEmpty())
    {
        QMessageBox::information(this,"提示","部门名称不能为空");
        return;
    }
    else if(ui->No_lineEdit->text().isEmpty())
    {
        QMessageBox::information(this,"提示","部门编号不能为空");
        return;
    }
    else if(ui->leader_lineEdit->text().isEmpty())
    {
        QMessageBox::information(this,"提示","部门领导不能为空");
        return;
    }
    else
    {
        QString strNo = ui->No_lineEdit->text();
        switch(strNo.length())
        {
            case 1:
                strNo = "0000" + strNo;
                break;
            case 2:
                strNo = "000" + strNo;
                break;
            case 3:
                strNo = "00" + strNo;
                break;
            case 4:
                strNo = "0" + strNo;
                break;
            case 5:
                strNo = strNo;
                break;
            default:
                strNo = "";
                break;
        }
        QString seleteSQL = "SELECT * FROM department WHERE Department_No = '"+strNo+"'";
        QSqlQuery seletequery;
        seletequery.exec(seleteSQL);
        if(seletequery.next()&&m_nFlag==1)
        {
            QMessageBox::information(this,"提示","此部门编号已占用,请重新输入编号");
            return;
        }
        else
        {
            QString strName = ui->Name_lineEdit->text();
            QString strPrincipal = ui->leader_lineEdit->text();
            QString strSuperior = ui->parent_lineEdit->text();
            QString strSuperior_Id;
            QString SQL = "SELECT a.* FROM department a WHERE a.Department_Name = '"+strSuperior+"'";
            QSqlQuery query;
            if(query.exec(SQL))
            {
                if(query.next())
                    strSuperior_Id = query.value(1).toString();
                else
                    strSuperior_Id = "";
            }
            if(m_nFlag==1)  //m_nFlag: 1->插入    2->修改
            {
                QString insertSQL = "insert into  department (Department_No,Department_Name,Department_Principal,Department_Superior_Id) values "
                        "('"+strNo+"','"+strName+"','"+strPrincipal+"','"+strSuperior_Id+"')";
                QSqlQuery insertquery;
                insertquery.exec(insertSQL);
            }
            else if(m_nFlag=2)
            {
                QString updateSQL = "Update department Set Department_No = '"+strNo+"',Department_Name = '"+strName+"',"
                        "Department_Principal = '"+ui->leader_lineEdit->text()+"',Department_Superior_Id = '"+strSuperior_Id+"' where "
                        "Department_No = '"+strNo+"'";
                QSqlQuery updatequery;
                updatequery.exec(updateSQL);
            }
            _UpdateDepartment();
            ui->departmentName->setEditTriggers(QAbstractItemView::NoEditTriggers);
            AddRow = -1;
            AddColumn = -1;
            UpdateRow = -1;
            UpdateColumn = -1;
            m_nFlag = 0;
        }
    }
}

/*****槽函数：取消按钮********/
void CDEPARTMENT::on_QpbtnClose_clicked()
{
    if(m_nFlag==1)
    {
        ti--;
    }
    m_nFlag = 0;
    AddRow = -1;
    AddColumn = -1;
    UpdateRow = -1;
    UpdateColumn = -1;
    _UpdateDepartment();
}

/*****槽函数：选择部门********/
void CDEPARTMENT::on_departmentName_clicked(const QModelIndex &index)
{
    QString Name = ui->departmentName->item(index.row(),index.column())->text();//获取被点中的item中的文本
    if(Name == "+" && m_nFlag == 2)
    {
        ui->departmentName->setCurrentItem(ui->departmentName->item(UpdateRow,UpdateColumn));
        QMessageBox::information(this,"提示","正处于信息修改状态,请保存后再添加...");
        return;
    }
    ui->Name_lineEdit->clear();
    ui->No_lineEdit->clear();
    ui->leader_lineEdit->clear();
    ui->parent_lineEdit->clear();
    _SelectStatus(0);

    if(Name == "+"||(index.row() == UpdateRow && index.column() == UpdateColumn)||(index.row() == AddRow && index.column() == AddColumn && ui->departmentName->item(index.row(),index.column())->text().isEmpty()))
    {
        _SelectStatus(2);//2->使编号文本框为可输入
        ui->Name_lineEdit->setFocus();
    }
    if(Name == "+")//&&ti!=0&&!ui->departmentName->item((ti-1)/3,(ti-1)%3)->text().isEmpty())
    {
//        if(!isLeftButton)
//            return;
        m_nFlag=1;
        if(ti != 0 && !ui->departmentName->item((ti-1)/3,(ti-1)%3)->text().isEmpty())
        {
            ui->departmentName->setEditTriggers(QAbstractItemView::NoEditTriggers);

            m_nFlag=1;
            AddRow = ti/3;
            AddColumn = ti%3;
            ti++;
            ui->departmentName->setRowCount(ti/3+1);
            ui->departmentName->setRowHeight((ti-1)/3,110);
            QTableWidgetItem *tabledmItemM = new QTableWidgetItem(QString(""));
            tabledmItemM->setTextAlignment(Qt::AlignCenter);
            ui->departmentName->setItem((ti-1)/3,(ti-1)%3,tabledmItemM);

            ui->departmentName->setRowHeight(ti/3,110);
            QTableWidgetItem *tabledmItemN = new QTableWidgetItem(QString("+"));
            tabledmItemN->setTextAlignment(Qt::AlignCenter);
            ui->departmentName->setItem(ti/3,ti%3,tabledmItemN);

            for(int i = 2;i>(ti%3);i--)
            {
                QTableWidgetItem *tabledmItemK = new QTableWidgetItem(QString(""));
                tabledmItemK->setTextAlignment(Qt::AlignCenter);
                ui->departmentName->setItem(ti/3,i,tabledmItemK);
            }
        }
    }
    else if(Name.length())
    {
        QString SQL = "SELECT a.* FROM department a WHERE a.Department_Name = '"+Name+"'";
        QSqlQuery query;
        if(query.exec(SQL))
        {
            if(query.next())
            {
                ui->Name_lineEdit->setText(query.value(2).toString());
                ui->No_lineEdit->setText(query.value(1).toString());
                ui->leader_lineEdit->setText(query.value(3).toString());
                QString selSQL = "SELECT a.* FROM department a WHERE a.Department_No = '"+query.value(4).toString()+"'";
                QSqlQuery selquery;
                if(selquery.exec(selSQL))
                {
                    if(selquery.next())
                        ui->parent_lineEdit->setText(selquery.value(2).toString());
                    else
                        ui->parent_lineEdit->setText("");
                }
            }
        }
    }
    else
    {
        ui->No_lineEdit->setEnabled(true);
        return ;
    }
}

/*******函数：设置文本输入框的输入状态********/
void CDEPARTMENT::_SelectStatus(int status)
{
    ui->No_lineEdit->setDisabled(true);//设置文本框为不可输入
    if(status)
    {
        _SetFocusOrder();   //设置LineEdit输入焦点
        ui->Name_lineEdit->setDisabled(false);//设置文本框为可输入
        ui->leader_lineEdit->setDisabled(false);
        ui->parent_lineEdit->setDisabled(false);
        if(status==2)
            ui->No_lineEdit->setDisabled(false);
    }
    else
    {
        ui->Name_lineEdit->setDisabled(true);//设置文本框为不可输入
        ui->leader_lineEdit->setDisabled(true);
        ui->parent_lineEdit->setDisabled(true);
    }
}

/*****信号槽函数：更改部门信息*****/
void CDEPARTMENT::SlotUpdateDepament()
{   if(m_nFlag==1)
    {
        ui->departmentName->setCurrentItem(ui->departmentName->item(AddRow,AddColumn));
        QMessageBox::information(this,"提示","正处于添加状态,请完善部门信息后再修改...");
        return;
    }
    _SelectStatus(1);
    m_nFlag=2;
    ui->Name_lineEdit->setFocus();
    UpdateRow=ui->departmentName->currentIndex().row();
    UpdateColumn=ui->departmentName->currentIndex().column();
    ui->departmentName->setEditTriggers(QAbstractItemView::NoEditTriggers);//修改某部门信息时，其他右键操作不响应
}

/*****信号槽函数：删除部门*****/
void CDEPARTMENT::SlotDeleteDepament()
{
    QString strName = ui->departmentName->currentItem()->text();
    QString deleteSQL = "delete  from department where Department_Name = '"+strName+"'";
    QSqlQuery deletequery;
    qDebug()<<deletequery.exec(deleteSQL)<<deleteSQL<<endl;
    ti--;
    ui->Name_lineEdit->clear();
    ui->No_lineEdit->clear();
    ui->leader_lineEdit->clear();
    ui->parent_lineEdit->clear();
    _UpdateDepartment();
}

/********函数：右键操作*********/
void CDEPARTMENT::on_departmentName_customContextMenuRequested(const QPoint &pos)
{
    DepartmentMenu->addAction(UpdateAction);
    DepartmentMenu->addAction(DeleteAction);
    DepartmentMenu->exec(QCursor::pos());
}

/******函数：设置LineEdit输入焦点******/
void CDEPARTMENT::_SetFocusOrder()
{
    ui->Name_lineEdit->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->No_lineEdit->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->leader_lineEdit->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->parent_lineEdit->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    QWidget::setTabOrder(ui->Name_lineEdit,ui->No_lineEdit);
    QWidget::setTabOrder(ui->No_lineEdit,ui->leader_lineEdit);
    QWidget::setTabOrder(ui->leader_lineEdit,ui->parent_lineEdit);
}

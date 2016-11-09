#include "system.h"
#include "ui_system.h"

CSYSTEM::CSYSTEM(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CSYSTEM)
{
    ui->setupUi(this);
    CUser = new CUSER();
    UserMenu = new QMenu(ui->QUser);
    _InitUser();
    ui->QUser->setContextMenuPolicy(Qt::CustomContextMenu);
    UserUpdateAction = new QAction(tr("修改"),this);
    UserDeleteAction = new QAction(tr("删除"),this);
    UserAddAction = new QAction(tr("添加"),this);
    connect(CUser,SIGNAL(SigUpdateUser()),this,SLOT(SlotUpdateUser()));
    connect(UserUpdateAction, SIGNAL(triggered()), this, SLOT(SlotUserUpdate()));
    connect(UserDeleteAction, SIGNAL(triggered()), this, SLOT(SlotUserDelete()));
    connect(UserAddAction, SIGNAL(triggered()), this, SLOT(SlotUserAdd()));
}

CSYSTEM::~CSYSTEM()
{
    delete ui;
}

void CSYSTEM::SlotUpdateUser()
{
    _UpdateUser();
}

void CSYSTEM::_UpdateToUser(SUser sUser)
{
    m_sUser = sUser;
    _UpdateUser();
}

//操作员管理修改动作的响应
void CSYSTEM::SlotUserUpdate()
{
    if(m_sUser.nGrade == 3)
    {
        QMessageBox::information(this, "提示", "您当前权限为操作员，无法修改用户信息，请联系管理员！");
        return;
    }
    int ti = ui->QUser->currentRow();
    if(ti >= 0)
    {
        QString strName = ui->QUser->item(ti, 2)->text();   //姓名
        QString strUser = ui->QUser->item(ti, 1)->text();   //用户名
        int nFlag = 2;
        CUser->_Update(strName,strUser,nFlag,m_sUser.nGrade);
        CUser->show();
    }
    else
    {
        QMessageBox::information(this,tr("提示"),tr("请选中目标"));
    }
}

/************************************
  1.系统用户管理删除函数
  2.超级管理员拥有删除所有人
  3.普通管理员只能删除操作员
  4.操作员不拥有删除权限
***************************************/
void CSYSTEM::SlotUserDelete()
{
    int ti = ui->QUser->currentRow();

    QString strUser = ui->QUser->item(ti, 1)->text();
    if(strUser == m_sUser.strUser)
    {
        QMessageBox::information(this,tr("提示"),tr("删除失败，您无法删除自身账户，请联系管理员！"));
        return;
    }
    else if(m_sUser.nGrade == 3)     //操作员
    {
        QMessageBox::information(this,tr("提示"),tr("您当前权限为操作员，没有删除用户权限，请联系管理员！"));
        return;
    }

    if(ti >= 0)
    {
        QString strName = ui->QUser->item(ti, 2)->text();
        QString strGrade = ui->QUser->item(ti,3)->text();
        if(m_sUser.nGrade == 2)     //管理员
        {
            if(strUser == m_sUser.strUser)
            {
                QMessageBox::information(this,tr("提示"),tr("删除失败，您无法删除自身账户，请联系管理员！"));
                return;
            }
            else if(strGrade == "管理员")
            {
                QMessageBox::information(this,tr("提示"),tr("您当前权限为普通管理员，不能删除高于或等于您权限的用户，请联系管理员！"));
                return;
            }
        }
        else if(m_sUser.nGrade == 1)    //超级管理员
        {
            if(strUser == m_sUser.strUser)
            {
                QMessageBox::information(this,tr("提示"),tr("删除失败，您无法删除自身账户！"));
                return;
            }
        }
        CMYSQL CMysql;
        switch( QMessageBox::question( this, tr("提示"),"确定要删除用户:"+strUser,tr("Yes"), tr("No"),0, 1 ) )
        {
            case 0:
                if(CMysql._DeleteUser(strName,strUser))
                {
                    QMessageBox::information(this,tr("提示"),tr("删除成功"));
                    _UpdateUser();
                }
                break;
            default :
                break;
        }
    }
    else
    {
        QMessageBox::information(this,tr("提示"),tr("请选中目标"));
    }
}

/*****更新函数：权限账户管理界面******/
void CSYSTEM::_UpdateUser()
{
    QString SQL = "SELECT *FROM user  ORDER BY User_Grade ASC";
    QSqlQuery query;

    if(query.exec(SQL))
    {
        int ti = 0;
        ui->QUser->setRowCount(ti);
        while(query.next())
        {
            int nGrade = query.value(4).toInt();    //获取权限
            if(nGrade >= m_sUser.nGrade)    //只显示权限序号比当前用户大的用户信息，即：当前用户只能查看权限比自身的用户
            {
                ui->QUser->setRowCount(ti + 1);

                QTableWidgetItem *tabledmItem0 = new QTableWidgetItem(QString::number(ti+1));
                tabledmItem0->setTextAlignment(Qt::AlignCenter);
                ui->QUser->setItem(ti, 0, tabledmItem0);

                QTableWidgetItem *tabledmItem1 = new QTableWidgetItem(QString(query.value(1).toByteArray().data()));
                tabledmItem1->setTextAlignment(Qt::AlignCenter);
                ui->QUser->setItem(ti, 1, tabledmItem1);    //用户名

                QTableWidgetItem *tabledmItem2 = new QTableWidgetItem(QString(query.value(3).toByteArray().data()));
                tabledmItem2->setTextAlignment(Qt::AlignCenter);
                ui->QUser->setItem(ti, 2, tabledmItem2);    //姓名

                QString strGrade;
                switch(nGrade)
                {
                    case 1:
                        strGrade = "超级管理员";
                        break;
                    case 2:
                        strGrade = "管理员";
                        break;
                    case 3:
                        strGrade = "操作员";
                        break;
                    default :
                        break;
                }

                QTableWidgetItem *tabledmItem3 = new QTableWidgetItem(strGrade);
                tabledmItem3->setTextAlignment(Qt::AlignCenter);
                ui->QUser->setItem(ti, 3, tabledmItem3);
                ti++;
            }
        }
    }
}

/********初始化函数：用户信息窗口********/
void CSYSTEM::_InitUser()
{
    ui->QUser->setColumnCount(4);
    ui->QUser->setShowGrid(true);//显示表格线
    QStringList headers;
    headers<<"序号"<<"用户名"<<"姓名"<<"级别";
    ui->QUser->verticalHeader()->hide();//行头不显示
    ui->QUser->setHorizontalHeaderLabels(headers);
    ui->QUser->horizontalHeader()->setClickable(false);//行头不可选
    ui->QUser->horizontalHeader()->setHighlightSections(false);
    ui->QUser->resizeColumnsToContents();
    ui->QUser->setFont(QFont("Helvetica"));
    ui->QUser->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->QUser->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式：一行
    ui->QUser->setColumnWidth(0,150);
    ui->QUser->setColumnWidth(1,200);
    ui->QUser->setColumnWidth(2,200);
    ui->QUser->setColumnWidth(3,208);
}

//操作员管理添加动作的响应
void CSYSTEM::SlotUserAdd()
{
    if(m_sUser.nGrade == 3)
    {
        QMessageBox::information(this, "提示", "您当前权限为操作员,没有添加用户权限，请联系管理员！");
        return;
    }
    int nFlag = 1;  //表示添加用户
    CUser->_Update("","",nFlag,m_sUser.nGrade);
    CUser->show();
}

void CSYSTEM::on_QUser_customContextMenuRequested(const QPoint &pos)
{
    UserMenu->addAction(UserAddAction);
    UserMenu->addAction(UserUpdateAction);
    UserMenu->addAction(UserDeleteAction);
    UserMenu->exec(QCursor::pos());
}


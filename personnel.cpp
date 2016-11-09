 //人事界面初始化
#include "personnel.h"
#include "ui_personnel.h"

CPERSONNEL::CPERSONNEL(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CPERSONNEL)
{
    ui->setupUi(this);
    pbtnAllSheet = 0;
    InitType();
    ui->stackedWidget_2->setCurrentIndex(0);
    Company = new CCOMPANY;//公司
    layout = new QVBoxLayout;
    layout->addWidget(Company);
    layout->setMargin(0);
    ui->QstackeCompany->setLayout(layout);
    connect(Company,SIGNAL(SigClose()),this,SLOT(SlotClose()));

    Department = new CDEPARTMENT;//部门
    layout = new QVBoxLayout;//
    layout->addWidget(Department);
    layout->setMargin(0);
    ui->QstackeDepartment->setLayout(layout);
    connect(Department,SIGNAL(SigClose()),this,SLOT(SlotClose()));

    Dimission = new CDIMISSION;//离职
    layout = new QVBoxLayout;
    layout->addWidget(Dimission);
    layout->setMargin(0);
    ui->QstackeDimission->setLayout(layout);

    Staff = new CSTAFF;//员工
    layout = new QVBoxLayout;
    layout->addWidget(Staff);
    layout->setMargin(0);
    ui->QstackeStaff->setLayout(layout);

    connect(Staff,SIGNAL(SigClose()),this,SLOT(SlotClose()));
    connect(Staff,SIGNAL(SigStaff_Staff_Leave(int)),this,SLOT(SlotStaff_Staff_Leave(int)));
    connect(Staff,SIGNAL(SigDeleteAll()),this,SLOT(SlotDeleteAll()));

    connect(this,SIGNAL(SigFocusOut()),Staff,SLOT(SlotFocusOut()));
    connect(this, SIGNAL(SigRecvDeleteAll()), Staff, SLOT(SlotRecvDeleteAll()));
    connect(this,SIGNAL(SigFocusOut()),Dimission,SLOT(SlotFocusOut()));
}

CPERSONNEL::~CPERSONNEL()
{
    delete ui;
}

void CPERSONNEL::_UpdateToStaff()
{
    Staff->_UpdateToStaff();
}

void CPERSONNEL::_SetInit()
{
//ui->stackedWidget_2->setCurrentIndex(0);
}

void CPERSONNEL::SlotStaff_Staff_Leave(int Id)
{
    emit SigPersonnel_Staff_Leave(Id);
}

void CPERSONNEL::SlotClose()
{
   ui->stackedWidget_2->setCurrentIndex(0);
}

//void CPERSONNEL::on_QpbtnParameter_clicked()
//{
//  ui->stackedWidget_2->setCurrentIndex(5);
//}

void CPERSONNEL::InitType()
{
    ui->QType_2->setColumnCount(1);
    ui->QType_2->setShowGrid(false);//显示表格线
    QStringList headers;
    headers<<"序号";
    ui->QType_2->horizontalHeader()->hide();
    ui->QType_2->verticalHeader()->hide();//行头不显示
    ui->QType_2->horizontalHeader()->setClickable(false);//行头不可选
    ui->QType_2->setHorizontalHeaderLabels(headers);
    ui->QType_2->horizontalHeader()->setHighlightSections(false);//表头坍塌
    ui->QType_2->resizeColumnsToContents();
    ui->QType_2->setFont(QFont("Helvetica"));//字体
    ui->QType_2->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->QType_2->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式：一行
    ui->QType_2->setColumnWidth(0,160);

    for(int i=0;i<ui->QType_2->rowCount();i++)
        ui->QType_2->setRowHeight(i,44);
}

void CPERSONNEL::on_QType_2_clicked(const QModelIndex &index)
{
    switch(index.row())
    {
        case 0:
        {
            ui->stackedWidget_2->setCurrentIndex(1);
            Company->_Init();
            pbtnAllSheet=1;
            break;
        }
        case 1:
        {
            ui->stackedWidget_2->setCurrentIndex(2);
            Department->_Update();
            pbtnAllSheet=2;
            break;
        }
        case 2:
        {
            ui->stackedWidget_2->setCurrentIndex(3);
            Staff->_Update();
            pbtnAllSheet=3;
            break;
        }
        case 3:
        {
            ui->stackedWidget_2->setCurrentIndex(4);
            Dimission->_Update();
            Dimission->_UpdateDimission("");
            pbtnAllSheet=4;
            break;
        }
        case 4:
        {
            ui->stackedWidget_2->setCurrentIndex(5);
            pbtnAllSheet=5;
            break;
        }
        case 5:
        {
            ui->stackedWidget_2->setCurrentIndex(6);
            pbtnAllSheet=6;
            break;
        }
        default:
            break;
    }
    emit SigFocusOut();
}

void CPERSONNEL::SlotPersonnelClick()
{
    ui->stackedWidget_2->setCurrentIndex(0);
}

void CPERSONNEL::on_QpbtnCompany_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(1);
    Company->_Init();
    pbtnAllSheet=1;
    emit SigFocusOut();
}

void CPERSONNEL::on_QpbtnDepartment_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(2);
    Department->_Update();
    pbtnAllSheet=2;
    emit SigFocusOut();
}

void CPERSONNEL::on_QpbtnStaff_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(3);
    Staff->_Update();
    pbtnAllSheet=3;
    emit SigFocusOut();
}

void CPERSONNEL::on_QpbtnDimission_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(4);
    Dimission->_Update();
    Dimission->_UpdateDimission("");//查询数据库，刷新离职员工界面
    pbtnAllSheet=4;
    emit SigFocusOut();
}

void CPERSONNEL::SlotFocusOut()
{
     ui->QType_2->setCurrentItem(ui->QType_2->item(-1,0));
}

//发送删除所有员工
void CPERSONNEL::SlotDeleteAll()
{
    emit SigDeleteAllStaff();
}

//接收删除所有员工
void CPERSONNEL::SlotRecvDeleteAllStaff()
{
    emit SigRecvDeleteAll();
}

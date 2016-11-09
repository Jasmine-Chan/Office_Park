#include "staff.h"
#include "ui_staff.h"

CSTAFF::CSTAFF(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CSTAFF)
{
    ui->setupUi(this);
    InsertOrUpdate = 0; //插入或更新员工信息
    tj = 0;
    isFalseImages = false;
    isAddStaff =false;
    m_nChangeNoList = 0;
    ui->textEdit->setDisabled(true);//不可编辑

    _InitStaff();       //初始化函数：员工图片窗口
    _InitStaffList();   //初始化函数：员工列表窗口
    _InitDepament();    //初始化函数：部门窗口
    _UpdateStaffList(); //更新函数：员工列表数据
    _UpdateDepament();  //更新函数：部门数据

    CMysql = new CMYSQL();
    CLeaveStaff = new CLEAVESTAFF();

    menu2 = new QMenu(ui->QStaffList);
    menu = new QMenu(ui->QStaff);
    LogOutAction = new QAction("删除",this);
    LogOutAllAction = new QAction("删除全部",this);
    UpdataAction = new QAction("编辑",this);
    Detailed = new QAction("详情",this);
    AddStaffAction = new QAction("添加",this);

    ui->Depament_stacked->setCurrentIndex(0);//设置当前界面

    connect(LogOutAction, SIGNAL(triggered()), this, SLOT(SlotLogOut()));
    connect(UpdataAction, SIGNAL(triggered()), this, SLOT(SlotUpdata()));
    connect(Detailed, SIGNAL(triggered()), this, SLOT(SlotDetails()));
    connect(LogOutAllAction, SIGNAL(triggered()), this, SLOT(SlotLogOutAll()));
    connect(AddStaffAction, SIGNAL(triggered()), this, SLOT(SlotAddStaff()));

    ui->QStaff->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->QStaffList->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->QStaff,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(onUserClickListener(QModelIndex)));
//    connect(ui->QStaff->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(SlotHeaderClicked(int)));
//    connect(ui->QStaff->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(SlotHeaderClicked(int)));
    connect(CLeaveStaff,SIGNAL(SigLeaveStaff(QString)),this,SLOT(SlotLeaveStaff(QString)));
//    connect(CHandle,SIGNAL(SigInit()),this,SLOT(SlotInit()));

    ui->Name_E->setToolTip("姓名不能为空");
    ui->QpbtnEdit->setToolTip("编辑");
    ui->QpbtnLogout->setToolTip("删除");
    ui->QpbtnRetuen->setToolTip("返回");

    _Change();
}

CSTAFF::~CSTAFF()
{
    delete ui;
}

void CSTAFF::onUserClickListener(const QModelIndex &index)
{
//    ui->Depament_stacked->setCurrentIndex(1);
}

void CSTAFF::_Update()
{
    ui->Depament_stacked->setCurrentIndex(0);
    _UpdateDepament();
    ui->QStaff->clear();
}

void CSTAFF::_UpdateToStaff()
{
    on_Depament_clicked(ui->Depament->currentIndex());
}

/********初始化函数：部门窗口********/
void CSTAFF::_InitDepament()
{
    ui->Depament->setColumnCount(3);
    ui->Depament->setShowGrid(true);
    ui->Depament->setFont(QFont("Helvetica"));
    ui->Depament->verticalHeader()->hide();
    ui->Depament->horizontalHeader()->hide();
    ui->Depament->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->Depament->setColumnWidth(0,226);
    ui->Depament->setColumnWidth(1,226);
    ui->Depament->setColumnWidth(2,226);
}

/********初始化函数：员工图片窗口********/
void CSTAFF::_InitStaff()
{
    ui->QStaff->setColumnCount(5);      //图片模式下设置为5列
    ui->QStaff->setShowGrid(false);
    ui->QStaff->setFont(QFont("Helvetica"));
    ui->QStaff->verticalHeader()->hide();
    ui->QStaff->horizontalHeader()->hide();
    ui->QStaff->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->QStaff->setColumnWidth(0,120);
    ui->QStaff->setColumnWidth(1,120);
    ui->QStaff->setColumnWidth(2,120);
    ui->QStaff->setColumnWidth(3,120);
    ui->QStaff->setColumnWidth(4,120);
}

/****员工图片数据界面和列表数据界面切换****/
void CSTAFF::_Change()
{
    if(m_nChangeNoList)     //0：列表界面    1：图片界面
    {
        ui->QStaff->show();
        ui->QStaffList->hide();
        m_nChangeNoList = 0;
        ui->QpbtnChange->setText("切\n换\n到\n列\n表\n模\n式");
    }
    else
    {
        ui->QStaff->hide();
        ui->QStaffList->show();
        m_nChangeNoList = 1;
        ui->QpbtnChange->setText("切\n换\n到\n图\n片\n模\n式");
    }
    _UpdateStaff();
}

/********初始化函数：员工列表窗口********/
void CSTAFF::_InitStaffList()
{
    ui->QStaffList->setColumnCount(12);//列表模式下，默认显示12列，对应数据库12个字段
    ui->QStaffList->setShowGrid(true);//显示表格线
    QStringList headers;
    headers<<"序号"<<"姓名"<<"性别"<<"工号"<<"职位"<<"入职时间"<<"学历"<<"身份证号"<<"电话"<<"毕业时间"<<"民族"<<"籍贯";
    ui->QStaffList->verticalHeader()->hide();//行头不显示
    ui->QStaffList->horizontalHeader()->setClickable(false);//列头不可选
    ui->QStaffList->setHorizontalHeaderLabels(headers);
    ui->QStaffList->horizontalHeader()->setHighlightSections(false);//选中不高亮
    ui->QStaffList->resizeColumnsToContents();//自适应内容宽度
    ui->QStaffList->setFont(QFont("Helvetica"));
    ui->QStaffList->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->QStaffList->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式：一行
    ui->QStaffList->setColumnWidth(0,50);
    ui->QStaffList->setColumnWidth(1,50);
    ui->QStaffList->setColumnWidth(2,35);
    ui->QStaffList->setColumnWidth(3,50);
    ui->QStaffList->setColumnWidth(4,100);
    ui->QStaffList->setColumnWidth(5,100);
    ui->QStaffList->setColumnWidth(6,50);
    ui->QStaffList->setColumnWidth(7,100);
    ui->QStaffList->setColumnWidth(8,100);
    ui->QStaffList->setColumnWidth(9,100);
    ui->QStaffList->setColumnWidth(10,30);
    ui->QStaffList->setColumnWidth(11,80);
}

/********更新函数：部门数据********/
void CSTAFF::_UpdateDepament()
{
    QString SQL = "SELECT * from department ORDER BY Department_No";
    QSqlQuery query;
    int ti;     //记录部门数量
    if(query.exec(SQL))
    {
        ti = 0;
        while(query.next())
        {
            ui->Depament->setRowHeight(ti/3,110);
            ui->Depament->setRowCount(ti/3+1);

            QTableWidgetItem *tabledmItem = new QTableWidgetItem(QString(query.value(2).toString()));
            tabledmItem->setTextAlignment(Qt::AlignCenter);
            ui->Depament->setItem(ti/3,ti%3,tabledmItem);
            ti++;
        }

        ui->Depament->setRowHeight(ti/3,110);
        ui->Depament->setRowCount(ti/3+1);
        QTableWidgetItem *tabledmItemALL = new QTableWidgetItem(QString("全公司"));
        tabledmItemALL->setTextAlignment(Qt::AlignCenter);
        ui->Depament->setItem(ti/3,ti%3,tabledmItemALL);

        ui->Depament->setRowHeight((ti+1)/3,110);
        ui->Depament->setRowCount((ti+1)/3+1);
        QTableWidgetItem *tabledmItemN = new QTableWidgetItem(QString("未分组"));
        tabledmItemN->setTextAlignment(Qt::AlignCenter);
        ui->Depament->setItem((ti+1)/3,(ti+1)%3,tabledmItemN);
        for(int i = 2;i>((ti+1)%3);i--)
        {
            ui->Depament->setRowHeight((ti+1)/3,110);
            QTableWidgetItem *tabledmItemK = new QTableWidgetItem(QString(""));
            tabledmItemK->setTextAlignment(Qt::AlignCenter);
            ui->Depament->setItem((ti+1)/3,i,tabledmItemK);
        }
    }
}

/********更新函数：员工列表数据*******/
void CSTAFF::_UpdateStaffList()
{
    QString SQL;
    if(m_strDepament_Id == "全公司")
    {
        SQL = "SELECT a.* FROM staff a ORDER BY a.Staff_No";
    }
    else if(m_strDepament_Id == "未分组")
    {
        SQL = "SELECT staff.* FROM staff where Staff_Id NOT IN (SELECT a.Staff_Id from staff a,department b WHERE  a.Staff_Department_No = b.Department_No) ORDER BY Staff_Department_No DESC";
    }
    else
    {
        SQL = "SELECT a.* FROM staff a WHERE a.Staff_Department_No = "+m_strDepament_Id+" ORDER BY a.Staff_No";
    }
    QSqlQuery query;
    if(query.exec(SQL))
    {
        int ti = 0;     //部门员工人数
        ui->QStaffList->setRowCount(ti);
        while(query.next())
        {
            ui->QStaffList->setRowCount(ti+1);
            QTableWidgetItem *tablemItem0 = new QTableWidgetItem(QString::number(ti+1));
            tablemItem0->setTextAlignment(Qt::AlignCenter);
            ui->QStaffList->setItem(ti,0,tablemItem0);//序号

            QTableWidgetItem *tablemItem1 = new QTableWidgetItem(query.value(2).toString());//姓名
            tablemItem1->setTextAlignment(Qt::AlignCenter);
            ui->QStaffList->setItem(ti,1,tablemItem1);
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
            ui->QStaffList->setItem(ti,2,tablemItem2);  //性别

            QTableWidgetItem *tablemItem3 = new QTableWidgetItem(query.value(1).toString());    //员工编号
            tablemItem3->setTextAlignment(Qt::AlignCenter);
            ui->QStaffList->setItem(ti,3,tablemItem3);

            QTableWidgetItem *tablemItem4 = new QTableWidgetItem(query.value(6).toString());    //职位
            tablemItem4->setTextAlignment(Qt::AlignCenter);
            ui->QStaffList->setItem(ti,4,tablemItem4);

            QTableWidgetItem *tablemItem5 = new QTableWidgetItem(query.value(7).toDateTime().toString("yyyy-MM-dd"));
            tablemItem5->setTextAlignment(Qt::AlignCenter);
            ui->QStaffList->setItem(ti,5,tablemItem5);  //入职日期

            switch(query.value(11).toInt())
            {
                case 0:
                    str = "小学";
                    break;
                case 1:
                    str = "初中";
                    break;
                case 2:
                    str = "高中";
                    break;
                case 3:
                    str = "大学";
                    break;
                case 4:
                    str = "硕士";
                    break;
                case 5:
                    str = "博士";
                    break;
                case 6:
                    str = "教授";
                    break;
                default:
                    str = "其他";
                    break;
            }
            QTableWidgetItem *tablemItem6 = new QTableWidgetItem(str);
            tablemItem6->setTextAlignment(Qt::AlignCenter);
            ui->QStaffList->setItem(ti,6,tablemItem6);//学历

            QTableWidgetItem *tablemItem7 = new QTableWidgetItem(query.value(4).toString());//身份证号
            tablemItem7->setTextAlignment(Qt::AlignCenter);
            ui->QStaffList->setItem(ti,7,tablemItem7);

            QTableWidgetItem *tablemItem8 = new QTableWidgetItem(query.value(15).toString());//手机号码
            tablemItem8->setTextAlignment(Qt::AlignCenter);
            ui->QStaffList->setItem(ti,8,tablemItem8);

            QTableWidgetItem *tablemItem9 = new QTableWidgetItem(query.value(14).toDateTime().toString("yyyy-MM-dd"));
            tablemItem9->setTextAlignment(Qt::AlignCenter);
            ui->QStaffList->setItem(ti,9,tablemItem9);//毕业时间

            switch(query.value(10).toInt())
            {
                case 0:
                    str = "汉族";
                    break;
                case 1:
                    str = "满族";
                    break;
                case 2:
                    str = "回族";
                    break;
                default:
                    str = "其他";
                    break;
            }
            QTableWidgetItem *tablemItem10 = new QTableWidgetItem(str);//民族
            tablemItem10->setTextAlignment(Qt::AlignCenter);
            ui->QStaffList->setItem(ti,10,tablemItem10);

            QTableWidgetItem *tablemItem11 = new QTableWidgetItem(query.value(9).toString());
            tablemItem11->setTextAlignment(Qt::AlignCenter);
            ui->QStaffList->setItem(ti,11,tablemItem11);//居住地址
            ti++;
        }
    }
}

/********更新函数：员工数据********/
void CSTAFF::_UpdateStaffInformation(QString Depament_Id)
{
    QString SQL;
    if(Depament_Id == "全公司")
        SQL = "SELECT a.* FROM staff a ORDER BY a.Staff_No";
    else if(Depament_Id == "未分组")
        SQL = "SELECT staff.* FROM staff where Staff_Id NOT IN (SELECT a.Staff_Id from staff a,department b WHERE  a.Staff_Department_No = b.Department_No) ORDER BY Staff_Department_No DESC";
    else
        SQL = "SELECT a.* FROM staff a WHERE a.Staff_Department_No = "+Depament_Id+" ORDER BY a.Staff_No";

    QSqlQuery query;
    qDebug()<<SQL<<query.exec(SQL)<<endl;
    if(query.exec(SQL))
    {
        tj = 0;
        ui->QStaff->setRowCount(tj);
        while(query.next())
        {
            ui->QStaff->setRowHeight(tj/5,90);
            ui->QStaff->setRowCount(tj/5+1);
            QIcon Icon = QIcon(query.value(22).toString());     //头像图片路径
//            QTableWidgetItem *tabledmItem = new QTableWidgetItem(QString(query.value(2).toString()));   //name
            QTableWidgetItem *tabledmItem = new QTableWidgetItem(QString(query.value(1).toString()));   //ID
            tabledmItem->setTextAlignment(Qt::AlignVCenter);
            ui->QStaff->setItem(tj/5,tj%5,tabledmItem);
            ui->QStaff->item(tj/5,tj%5)->setIcon(Icon);
            tj++;
        }

        ui->QStaff->setRowCount(tj/5+1);

        QTableWidgetItem *tabledmItemN = new QTableWidgetItem(QString(""));
        tabledmItemN->setTextAlignment(Qt::AlignCenter);
        QIcon Icon = QIcon(":/images/Add.png");
        ui->QStaff->setItem(tj/5,tj%5,tabledmItemN);
        ui->QStaff->item(tj/5,tj%5)->setIcon(Icon);

        ui->QStaff->setIconSize(QSize(90,100));
        ui->QStaff->setRowHeight(tj/5,90);

//        for(int i = 4;i>(tj%5);i--)   //作用：占位符？
//        {
//            ui->QStaff->setRowHeight(tj/5,90);
//            QTableWidgetItem *tabledmItemK = new QTableWidgetItem(QString("123"));
//            tabledmItemK->setTextAlignment(Qt::AlignCenter);
//            ui->QStaff->setItem(tj/5,i,tabledmItemK);
//        }
    }
}

void CSTAFF::_UpdateStaff()
{
    if(m_nChangeNoList == 0)    //0:列表  1:图片
    {
        _UpdateStaffInformation(m_strDepament_Id);
    }
    else
    {
        _UpdateStaffList();
    }
}

/********点击部门********/
void CSTAFF::clickDepartmentType(QModelIndex index)
{
    QString Depament = ui->Depament->item(index.row(),index.column())->text();
    QString Depament_Id;
    if(Depament == "未分组" || Depament == "全公司")
    {
//        _UpdateStaffInformation(Depament);
        m_strDepament_Id = Depament;
    }
    else
    {
        QString SQL = "SELECT a.* FROM department a WHERE a.Department_Name = '"+Depament+"'";
        QSqlQuery query;
        query.exec(SQL);
        query.next();
        Depament_Id = query.value(1).toString();
        m_strDepament_Id = Depament_Id;
//        _UpdateStaffInformation(Depament_Id);
    }
    _UpdateStaff();
}

void CSTAFF::on_Depament_clicked(const QModelIndex &index)
{
    clickDepartmentType(index);
}

/********信号槽函数：进入注销模式********/
void CSTAFF::SlotLogOut()
{
    if(m_nChangeNoList == 0)    //图片模式
    {
        if(!ui->QStaff->currentItem()->text().isEmpty())
        {
            QString SQL = "SELECT * from staff a WHERE a.Staff_No = '"+ui->QStaff->currentItem()->text()+"'";
            QSqlQuery query;
            if(query.exec(SQL))
            {
                if(query.next())
                {
                    if(ui->QStaff->currentItem()->text().length()>0)
                    {
                        SStaff sStaff;
                        sStaff.strNo = query.value(1).toString();
                        sStaff.strName = query.value(2).toString();
                        if(query.value(3).toString() == "0")
                            sStaff.nSex = 0;
                        else
                            sStaff.nSex = 1;
                        QString seleteSQL = "SELECT a.Department_Name FROM department a WHERE a.Department_No = '"+query.value(5).toString()+"'";
                        QSqlQuery seletequery;
                        seletequery.exec(seleteSQL);
                        QString strDepartment = "";
                        if(seletequery.next())
                            strDepartment = seletequery.value(0).toString();
                        sStaff.strDepartment = strDepartment;
                        sStaff.strPost = query.value(6).toString();
                        sStaff.strEnter = query.value(7).toString();
                        sStaff.strIdCard = query.value(4).toString();
                        sStaff.strPhone = query.value(15).toString();
                        sStaff.nPolitics = query.value(8).toInt();
                        sStaff.strNative = query.value(9).toString();
                        sStaff.nNation = query.value(10).toInt();
                        sStaff.nEducation = query.value(11).toInt();
                        sStaff.strBirthday = query.value(12).toString();
                        sStaff.nMaritalStatus = query.value(13).toInt();
                        sStaff.strGraduationDate = query.value(14).toString();
                        sStaff.strEmail = query.value(16).toString();
                        sStaff.strAdd = query.value(17).toString();
                        sStaff.strGraduationSchool = query.value(18).toString();
                        sStaff.strMajor = query.value(19).toString();
                        sStaff.strRemark = query.value(20).toString();
                        sStaff.strImagePath = query.value(22).toString();

                        CLeaveStaff->_Update(sStaff);
                        CLeaveStaff->show();
                        return;
                    }
                    else
                    {
                        QMessageBox::information(this,"提示","请选择要处理的对象");
                        return;
                    }
                }
            }
        }
        QMessageBox::information(this,"提示","目标选取失败");
    }
    else
    {
        if(ui->QStaffList->currentRow() >= 0)
        {
            QString SQL = "SELECT * from staff a WHERE a.Staff_Name = '"+ui->QStaffList->item(ui->QStaffList->currentRow(),1)->text()+"'";
            QSqlQuery query;
            if(query.exec(SQL))
            {
                if(query.next())
                {
                    if(ui->QStaffList->item(ui->QStaffList->currentRow(),1)->text().length()>0)
                    {
                        SStaff sStaff;
                        sStaff.strNo = query.value(1).toString();
                        sStaff.strName = query.value(2).toString();
                        if(query.value(3).toString() == "0")    //性别
                            sStaff.nSex = 0;
                        else
                            sStaff.nSex = 1;
                        QString seleteSQL = "SELECT a.Department_Name FROM department a WHERE a.Department_No = '"+query.value(5).toString()+"'";
                        QSqlQuery seletequery;
                        seletequery.exec(seleteSQL);

                        QString strDepartment = "";
                        if(seletequery.next())
                            strDepartment = seletequery.value(0).toString();
                        sStaff.strDepartment = strDepartment;
                        sStaff.strPost = query.value(6).toString();
                        sStaff.strEnter = query.value(7).toString();
                        sStaff.strIdCard = query.value(4).toString();
                        sStaff.strPhone = query.value(15).toString();
                        sStaff.nPolitics = query.value(8).toInt();
                        sStaff.strNative = query.value(9).toString();
                        sStaff.nNation = query.value(10).toInt();
                        sStaff.nEducation = query.value(11).toInt();
                        sStaff.strBirthday = query.value(12).toString();
                        sStaff.nMaritalStatus = query.value(13).toInt();
                        sStaff.strGraduationDate = query.value(14).toString();
                        sStaff.strEmail = query.value(16).toString();
                        sStaff.strAdd = query.value(17).toString();
                        sStaff.strGraduationSchool = query.value(18).toString();
                        sStaff.strMajor = query.value(19).toString();
                        sStaff.strRemark = query.value(20).toString();
                        sStaff.strImagePath = query.value(22).toString();

                        CLeaveStaff->_Update(sStaff);
                        CLeaveStaff->show();
                        return;
                    }
                }
            }
        }
        else
        {
            QMessageBox::information(this, tr("提示"), tr("请选择一行数据"));
        }
    }
}

//员工离职成功接收函数
void CSTAFF::SlotLeaveStaff(QString strNo)
{
    clickDepartmentType(ui->Depament->currentIndex());
    ui->Depament_stacked->setCurrentIndex(0);
    emit SigStaff_Staff_Leave(strNo.toInt());//CUdpServer->_StaffLeave(Id);
}

//上传头像
void CSTAFF::on_pushButton_clicked()
{
        QString UsablestrNo;
        switch(ui->Number_E->text().length())
        {
            case 1: UsablestrNo = "0000"+ui->Number_E->text();
                break;
            case 2: UsablestrNo = "000"+ui->Number_E->text();
                break;
            case 3: UsablestrNo = "00"+ui->Number_E->text();
                break;
            case 4: UsablestrNo = "0"+ui->Number_E->text();
                break;
            case 5: UsablestrNo = ui->Number_E->text();
                break;
            default:
                break;
        }

        QString ImagePath = QFileDialog::getOpenFileName();
//        QIcon Icon = QIcon(ImagePath);
        QPixmap Pixmap(ImagePath);

        if(ImagePath!="")
        {
            if(Pixmap.width()<100||Pixmap.height()<120)
            {
                QMessageBox::information(this,"提示","上传失败,请上传像素大于100x120的图片");
                return;
            }
            ui->pushButton->setStyleSheet("border-image:url("+ImagePath+")");

            QString SelectSQL = "SELECT * FROM staff WHERE Staff_No = '"+UsablestrNo+"'";
            QSqlQuery selectquery;
            qDebug()<<selectquery.exec(SelectSQL)<<SelectSQL<<endl;
            if(!(selectquery.next()&&isAddStaff))
            {
                Path = ImagePath;
            }
        }
}

//图片模式，右键
void CSTAFF::on_QStaff_customContextMenuRequested(const QPoint &pos)
{
    menu2->addAction(Detailed);
    menu2->addAction(LogOutAction);
    menu2->addAction(LogOutAllAction);
    menu2->addAction(UpdataAction);
    menu2->exec(QCursor::pos());
}

/********信号槽函数：进入编辑模式********/
void CSTAFF::SlotUpdata()
{
    if(m_nChangeNoList == 0)//图片界面
    {
        if(!ui->QStaff->currentItem()->text().isEmpty())
        {
            InsertOrUpdate = 0;
            clearLineEdit();
            _InitDepartmentForQcombobox();  //初始化“部门”选项的下拉式菜单
            _UpdateConcreteInformation();   //将选中的行的信息填入编辑页面中
            whetherEnabled(1);
            ui->Number_E->setEnabled(false);
            ui->JobTime_E->setEnabled(false);
            ui->Depament_stacked->setCurrentIndex(1);//跳转到编辑页面，0是初始页面
        }
        else
            QMessageBox::information(this,"提示","选取目标失败");
    }
    else    //列表界面
    {
        if(ui->QStaffList->currentRow() >= 0)
        {
            InsertOrUpdate = 0; //添加员工
            clearLineEdit();
            _InitDepartmentForQcombobox();  //初始化“部门”选项的下拉式菜单
            _UpdateConcreteInformation();
            whetherEnabled(1);
            ui->Number_E->setEnabled(false);
            ui->JobTime_E->setEnabled(false);
            ui->Depament_stacked->setCurrentIndex(1);//跳转到编辑页面，0是初始页面
        }
        else
        {
            QMessageBox::information(this, tr("提示"), tr("请选取一行数据"));
        }
    }
}

/********信号槽函数：进入详情模式********/
void CSTAFF::SlotDetails()
{
    if(m_nChangeNoList == 0)    //图片模式
    {
        if(!ui->QStaff->currentItem()->text().isEmpty())
        {
            InsertOrUpdate = 0;
            clearLineEdit();
            _InitDepartmentForQcombobox();
            _UpdateConcreteInformation();
            whetherEnabled(0);
            ui->Number_E->setEnabled(false);
            ui->JobTime_E->setEnabled(false);
            ui->Depament_stacked->setCurrentIndex(1);//跳转到详情页面，0是初始页面
        }
        else
            QMessageBox::information(this,"提示","选取目标失败");
    }
    else
    {
        if(ui->QStaffList->currentRow() >= 0)
        {
            InsertOrUpdate = 0;
            clearLineEdit();
            _InitDepartmentForQcombobox();
            _UpdateConcreteInformation();
            whetherEnabled(0);
            ui->Number_E->setEnabled(false);
            ui->JobTime_E->setEnabled(false);
            ui->Depament_stacked->setCurrentIndex(1);//跳转到详情页面，0是初始页面
        }
        else
        {
           QMessageBox::information(this,"提示","请选取一行数据");
        }
    }
}

/******清空输入文本框*******/
void CSTAFF::clearLineEdit()
{
    ui->Name_E->clear();
    ui->Number_E->clear();
    ui->Sex_E->setCurrentIndex(0);
    ui->Department_E->setCurrentIndex(0);
    ui->Position_E->clear();
    ui->JobTime_E->setDate(QDate::currentDate());
    ui->CardNo_E->clear();
    ui->Telephone_E->clear();
    ui->Political_E->setCurrentIndex(0);
    ui->NativePlace_E->clear();
    ui->Nation_E->setCurrentIndex(0);
    ui->Educational_E->setCurrentIndex(0);
    ui->BirthDay_E->setDate(QDate(1993,1,1));
    ui->Marry_E->setCurrentIndex(0);
    ui->Graduation_E->setDate(QDate(QDate::currentDate().year(),7,1));
    ui->Mailbox_E->clear();
    ui->Addr_E->clear();
    ui->School_E->clear();
    ui->Specilty_E->clear();
    ui->Remarks_E->clear();
    ui->Status_E->setCurrentIndex(0);
}

/******函数：各个输入文本框是否可输入**********/
void CSTAFF::whetherEnabled(int sign)
{
    if(sign)
    {
        ui->Name_E->setEnabled(true);
        ui->Sex_E->setEnabled(true);
        ui->Department_E->setEnabled(true);
        ui->Position_E->setEnabled(true);
        ui->CardNo_E->setEnabled(true);
        ui->Telephone_E->setEnabled(true);
        ui->Political_E->setEnabled(true);
        ui->NativePlace_E->setEnabled(true);
        ui->Nation_E->setEnabled(true);
        ui->Educational_E->setEnabled(true);
        ui->BirthDay_E->setEnabled(true);
        ui->Marry_E->setEnabled(true);
        ui->Graduation_E->setEnabled(true);
        ui->Mailbox_E->setEnabled(true);
        ui->Addr_E->setEnabled(true);
        ui->School_E->setEnabled(true);
        ui->Specilty_E->setEnabled(true);
        ui->Remarks_E->setEnabled(true);
        ui->Status_E->setEnabled(true);
        ui->pushButton->setEnabled(true);
        _SetFocusOrder();
    }
    else
    {
        ui->Name_E->setEnabled(false);
        ui->Number_E->setEnabled(false);
        ui->Sex_E->setEnabled(false);
        ui->Department_E->setEnabled(false);
        ui->JobTime_E->setEnabled(false);
        ui->Position_E->setEnabled(false);
        ui->CardNo_E->setEnabled(false);
        ui->Telephone_E->setEnabled(false);
        ui->Political_E->setEnabled(false);
        ui->NativePlace_E->setEnabled(false);
        ui->Nation_E->setEnabled(false);
        ui->Educational_E->setEnabled(false);
        ui->BirthDay_E->setEnabled(false);
        ui->Marry_E->setEnabled(false);
        ui->Graduation_E->setEnabled(false);
        ui->Mailbox_E->setEnabled(false);
        ui->Addr_E->setEnabled(false);
        ui->School_E->setEnabled(false);
        ui->Specilty_E->setEnabled(false);
        ui->Remarks_E->setEnabled(false);
        ui->Status_E->setEnabled(false);
        ui->pushButton->setEnabled(false);
    }
}

/********更新函数：员工详细信息********/
void CSTAFF::_UpdateConcreteInformation()
{
    QString SQL;
    if(m_strDepament_Id == 0)   //找不到对应的部门
    {
        SQL = "SELECT * from staff a WHERE a.Staff_No = '"+ui->QStaff->currentItem()->text()+"'";
    }
    else
    {
        if(m_nChangeNoList == 1)    //列表模式
        {
            SQL = "SELECT * from staff a WHERE a.Staff_No = '"+ui->QStaffList->item(ui->QStaffList->currentRow(),3)->text()+"'";
        }
        else
        {
            SQL = "SELECT * from staff a WHERE a.Staff_No = '"+ui->QStaff->currentItem()->text()+"'";
        }
    }
    QSqlQuery query;
    if(query.exec(SQL))
    {
        if(query.next())
        {
            QString DepartmentSQL = "SELECT * FROM department ORDER BY Department_No";
            QSqlQuery departmentquery;
            ui->Department_E->setCurrentIndex(0);//未分组
            int Index = 0;
            if(departmentquery.exec(DepartmentSQL))
            {
                while(departmentquery.next())
                {
                    Index++;
                    if(query.value(5).toInt() == departmentquery.value(1).toInt())
                        ui->Department_E->setCurrentIndex(Index);
                }
            }
            ui->Name_E->setText(query.value(2).toString());
            ui->Number_E->setText(query.value(1).toString());   //工号
            ui->Sex_E->setCurrentIndex(query.value(3).toInt());
            ui->Position_E->setText(query.value(6).toString());
            ui->JobTime_E->setDate(query.value(7).toDate());
            ui->CardNo_E->setText(query.value(4).toString());   //身份证
            ui->Telephone_E->setText(query.value(15).toString());
            ui->Political_E->setCurrentIndex(query.value(8).toInt());
            ui->NativePlace_E->setText(query.value(9).toString());
            ui->Nation_E->setCurrentIndex(query.value(10).toInt());
            ui->Educational_E->setCurrentIndex(query.value(11).toInt());
            ui->BirthDay_E->setDate(query.value(12).toDate());
            ui->Marry_E->setCurrentIndex(query.value(13).toInt());
            ui->Graduation_E->setDate(query.value(14).toDate());
            ui->Mailbox_E->setText(query.value(16).toString());
            ui->Addr_E->setText(query.value(17).toString());
            ui->School_E->setText(query.value(18).toString());
            ui->Specilty_E->setText(query.value(19).toString());
            ui->Remarks_E->setText(query.value(20).toString());
            ui->Status_E->setCurrentIndex(query.value(21).toInt());
            ui->pushButton->setStyleSheet("border-image:url("+query.value(22).toString()+")");
            Path = query.value(22).toString();
        }
    }
}

/********点击添加员工********/
void CSTAFF::on_QStaff_clicked(const QModelIndex &index)
{
    if((index.row() == tj/5)&&(index.column() == tj%5))
    {
        InsertOrUpdate = 1;
        isAddStaff = true;
        Path = "E:/officepark/images/human.png";
        _InitDepartmentForQcombobox();
        ui->pushButton->setStyleSheet("border-image:url('"+Path+"')");
        ui->Depament_stacked->setCurrentIndex(1);
        clearLineEdit();
        whetherEnabled(1);
        ui->Number_E->setEnabled(true);
        ui->JobTime_E->setEnabled(true);
        QString DepartmentSQL = "SELECT * FROM department ORDER BY Department_No";
        QSqlQuery departmentquery;
        ui->Department_E->setCurrentIndex(0);
        int Index = 0;
        if(departmentquery.exec(DepartmentSQL))
        {
            while(departmentquery.next())
            {
                Index++;
                if(ui->Depament->currentItem()->text() == departmentquery.value(2).toString())
                    ui->Department_E->setCurrentIndex(Index);
            }
        }
//        if(ui->Depament->NoEditTriggers)
//            ui->Department_E->setText("未分组");
//        else
//        {
//            ui->Department_E->setText(ui->Depament->currentItem()->text());
//        }
    }
}

/********添加的员工数据放入员工链表中********/
bool CSTAFF::_InitStructSStaff()
{
    QString strNo;
    SStaff sStaff;
    sStaff.strAdd = ui->Addr_E->text();
    sStaff.strBirthday = ui->BirthDay_E->text();
    QString SQL = "SELECT a.Department_No FROM department a WHERE a.Department_Name = '"+ui->Department_E->currentText()+"'";
    QSqlQuery query;
    if(query.exec(SQL))
    {
        if(query.next())
            sStaff.strDepartment = query.value(0).toString();
        else
            sStaff.strDepartment = "";
    }
    sStaff.strEmail = ui->Mailbox_E->text();
    sStaff.strEnter = ui->JobTime_E->text();
    sStaff.strGraduationDate = ui->Graduation_E->text();
    sStaff.strGraduationSchool = ui->School_E->text();
    sStaff.strIdCard = ui->CardNo_E->text();
    sStaff.strRemark = ui->Remarks_E->toPlainText();

    sStaff.strMajor = ui->Specilty_E->text();
    sStaff.strNative = ui->NativePlace_E->text();

    switch(ui->Number_E->text().length())
    {
        case 1: strNo = "0000"+ui->Number_E->text();
            break;
        case 2: strNo = "000"+ui->Number_E->text();
            break;
        case 3: strNo = "00"+ui->Number_E->text();
            break;
        case 4: strNo = "0"+ui->Number_E->text();
            break;
        case 5: strNo = ui->Number_E->text();
            break;
        default:
            break;
    }
    QString NameSQL = "SELECT * FROM staff WHERE staff.Staff_Name = '"+ui->Name_E->text()+"'";
    QSqlQuery Namequery;
    Namequery.exec(NameSQL);
    if(Namequery.next())
    {
        if(Namequery.value(1).toString()!=ui->Number_E->text())
        {
            QMessageBox::information(this,"提示","你与同事同名，请在名字后加标号区分");
            return 0;
        }
        else
            sStaff.strName = ui->Name_E->text();
    }
    else
        sStaff.strName = ui->Name_E->text();

    if(isAddStaff)
    {
        QString SelectSQL;
        SelectSQL = "SELECT a.* FROM staff a,leave_staff b WHERE a.Staff_No = '"+strNo+"' OR b.Leave_Staff_No = '"+strNo+"'";
        QSqlQuery selectquery;
        qDebug()<<selectquery.exec(SelectSQL)<<SelectSQL<<endl;
        if(selectquery.next()&&InsertOrUpdate)
        {

            QMessageBox::information(this,"提示","此工号已占用,请更改");
            return 0;
        }
        else
        {
            sStaff.strNo = strNo;
        }

    }
    else
    {
        sStaff.strNo = strNo;
    //     sStaff.strName = ui->Name_E->text();
    }

    sStaff.strPhone = ui->Telephone_E->text();
    sStaff.strPost = ui->Position_E->text();
    sStaff.strImagePath = Path;
    sStaff.nNation = ui->Nation_E->currentIndex();

    sStaff.nSex = ui->Sex_E->currentIndex();

    sStaff.nEducation = ui->Educational_E->currentIndex();

    sStaff.nPolitics = ui->Political_E->currentIndex();

    sStaff.nMaritalStatus=ui->Marry_E->currentIndex();

    if(InsertOrUpdate == 1)
    {
        if(CMysql->_InsertStaff(sStaff))
        {
            QMessageBox::information(this,"提示","员工添加成功");
            return 1;
        }
        else
        {
            QMessageBox::information(this,"提示","添加失败");
            return 0;
        }
    }
    else if(InsertOrUpdate == 0)
    {
        if(CMysql->_UpdateStaff(sStaff,ui->Number_E->text(),ui->JobTime_E->text()))
        {
            QMessageBox::information(this,"提示","信息更新成功");
            return 1;
        }
        else
        {
            QMessageBox::information(this,"提示","更新失败");
            return 0;
        }
    }
    isAddStaff = false;
}

//"保存"按钮
void CSTAFF::on_QpbtnSave_clicked()
{
    CMYSQL CMysql;
    if(ui->Name_E->text().isEmpty())
    {
        ui->Name_E->setFocus();
        QMessageBox::information(this,"提示","姓名不能为空");
    }
    else if(ui->Number_E->text().isEmpty())
    {
        QMessageBox::information(this,"提示","工号不能为空");
    }
//    else if(CMysql._IsHaveNo(ui->Number_E->text())){
//        QMessageBox::information(this,"提示","工号已存在");
//    }
    else if(ui->BirthDay_E->text().isEmpty())
    {
        QMessageBox::information(this,"提示","生日不能为空");
    }
    else if(ui->JobTime_E->text().isEmpty())
    {
        QMessageBox::information(this,"提示","入职时间不能为空");
    }
    else if(ui->Graduation_E->text().isEmpty())
    {
        QMessageBox::information(this,"提示","毕业时间不能为空");
    }
    else
    {
        if(_InitStructSStaff())
        {
            whetherEnabled(0);
            ui->Depament_stacked->setCurrentIndex(0);
        }
        clickDepartmentType(ui->Depament->currentIndex());
    }
}

void CSTAFF::on_QpbtnLogout_clicked()
{
    SlotLogOut();
}

void CSTAFF::on_QpbtnRetuen_clicked()
{
    ui->Depament_stacked->setCurrentIndex(0);
}

void CSTAFF::on_QpbtnEdit_clicked()
{
    whetherEnabled(1);
    InsertOrUpdate = 0;
}

void CSTAFF::on_QpbtnClose_clicked()
{
    if(InsertOrUpdate)
        clearLineEdit();
    else
        _UpdateConcreteInformation();
    whetherEnabled(InsertOrUpdate);
}

void CSTAFF::SlotFocusOut()
{
    ui->QStaff->setCurrentItem(ui->QStaff->item(10000,0));
    ui->Depament->setCurrentItem(ui->Depament->item(10000,0));
}

//初始化“部门”选项的下拉式菜单
void CSTAFF::_InitDepartmentForQcombobox()
{
    ui->Department_E->clear();
    ui->Department_E->addItem("未分组");
    QString SQL = "SELECT * FROM department ORDER BY Department_No";
    QSqlQuery query;
    if(query.exec(SQL))
    {
        while(query.next())
            ui->Department_E->addItem(query.value(2).toString());
    }
}

void CSTAFF::_SetFocusOrder()
{
    ui->Name_E->setFocus();
    ui->Name_E->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->Number_E->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->Department_E->setStyleSheet("QComboBox:focus{border:1px solid green;}");
    ui->Position_E->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->JobTime_E->setStyleSheet("QDateTime:focus{border:1px solid green;}");
    ui->Status_E->setStyleSheet("QComboBox:focus{border:1px solid green;}");
    ui->Telephone_E->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->Mailbox_E->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->Addr_E->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->CardNo_E->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->Marry_E->setStyleSheet("QComboBox:focus{border:1px solid green;}");
    ui->Graduation_E->setStyleSheet("QDateTime:focus{border:1px solid green;}");
    ui->Sex_E->setStyleSheet("QComboBox:focus{border:1px solid green;}");
    ui->BirthDay_E->setStyleSheet("QDatetime:focus{border:1px solid green;}");
    ui->Nation_E->setStyleSheet("QComboBox:focus{border:1px solid green;}");
    ui->Political_E->setStyleSheet("QComboBox:focus{border:1px solid green;}");
    ui->NativePlace_E->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->School_E->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->Specilty_E->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->Educational_E->setStyleSheet("QComboBox:focus{border:1px solid green;}");
    ui->Remarks_E->setStyleSheet("QTextEdit:focus{border:1px solid green;}");
    QWidget::setTabOrder(ui->Name_E, ui->Number_E);
    QWidget::setTabOrder(ui->Number_E, ui->Department_E);
    QWidget::setTabOrder(ui->Department_E, ui->Position_E);
    QWidget::setTabOrder(ui->Position_E, ui->JobTime_E);
    QWidget::setTabOrder(ui->JobTime_E, ui->Status_E);
    QWidget::setTabOrder(ui->Status_E, ui->Telephone_E);
    QWidget::setTabOrder(ui->Telephone_E, ui->Mailbox_E);
    QWidget::setTabOrder(ui->Mailbox_E, ui->Addr_E);
    QWidget::setTabOrder(ui->Addr_E, ui->CardNo_E);
    QWidget::setTabOrder(ui->CardNo_E, ui->Marry_E);
    QWidget::setTabOrder(ui->Marry_E, ui->Graduation_E);
    QWidget::setTabOrder(ui->Graduation_E, ui->Sex_E);
    QWidget::setTabOrder(ui->Sex_E, ui->BirthDay_E);
    QWidget::setTabOrder(ui->BirthDay_E, ui->Nation_E);
    QWidget::setTabOrder(ui->Nation_E, ui->Political_E);
    QWidget::setTabOrder(ui->Political_E, ui->NativePlace_E);
    QWidget::setTabOrder(ui->NativePlace_E, ui->School_E);
    QWidget::setTabOrder(ui->School_E, ui->Specilty_E);
    QWidget::setTabOrder(ui->Specilty_E, ui->Educational_E);
    QWidget::setTabOrder(ui->Educational_E, ui->Remarks_E);
}

//切换界面
void CSTAFF::on_QpbtnChange_clicked()
{
    _Change();
}

//列表模式，右键
void CSTAFF::on_QStaffList_customContextMenuRequested(const QPoint &pos)
{
    menu->addAction(Detailed);
    menu->addAction(LogOutAction);
    menu->addAction(LogOutAllAction);
    menu->addAction(UpdataAction);
    menu->addAction(AddStaffAction);
    menu->exec(QCursor::pos());
}

/********信号槽函数：发送注销所有命令********/
void CSTAFF::SlotLogOutAll()
{
    if(!QMessageBox::question( this, tr("提示"),tr("执行此操作，将删除公司所有员工记录，请确定是否执行此操作！"),tr("Yes"), tr("No"),0, 1))
    {
        emit SigDeleteAll();
    }
}

/********信号槽函数：进入注销全部模式********/
void CSTAFF::SlotRecvDeleteAll()
{
    QString deleteAllUser = "DELETE FROM staff";
    QSqlQuery DeleteAll;
    QString deleteAllSQL = "DELETE FROM leave_staff";
    QSqlQuery DeleteAllQuery;
    if(DeleteAll.exec(deleteAllUser) && DeleteAllQuery.exec(deleteAllSQL))
    {
        QMessageBox::information(this, tr("提示"), tr("所有员工信息删除成功！"));
        clickDepartmentType(ui->Depament->currentIndex());
        ui->Depament_stacked->setCurrentIndex(0);
        return;
    }
    else
    {
        QMessageBox::information(this, tr("提示"), tr("删除失败！"));
        return;
    }
}

//列表模式下，添加员工
void CSTAFF::SlotAddStaff()
{
    InsertOrUpdate = 1;
    isAddStaff = true;
    Path = "E:/officepark/images/human.png";
    _InitDepartmentForQcombobox();
    ui->pushButton->setStyleSheet("border-image:url('"+Path+"')");
    ui->Depament_stacked->setCurrentIndex(1);
    clearLineEdit();
    whetherEnabled(1);
    ui->Number_E->setEnabled(true);
    ui->JobTime_E->setEnabled(true);
    QString DepartmentSQL = "SELECT * FROM department ORDER BY Department_No";
    QSqlQuery departmentquery;
    ui->Department_E->setCurrentIndex(0);
    int Index = 0;
    if(departmentquery.exec(DepartmentSQL))
    {
        while(departmentquery.next())
        {
            Index++;
            if(ui->Depament->currentItem()->text() == departmentquery.value(2).toString())
                ui->Department_E->setCurrentIndex(Index);
        }
    }
}

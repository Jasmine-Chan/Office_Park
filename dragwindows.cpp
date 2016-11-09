#include "dragwindows.h"
#include "ui_dragwindows.h"
#include <QtGui>
dragwindows::dragwindows(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dragwindows)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);  //隐藏标题栏
    IsIconicRecv = false;   //用于判断是不是画图区
    strLocation.clear();
    PointLocction.setX(0);
    PointLocction.setY(0);

    MapMenu = new QMenu(this);
    deleteAction = new QAction(tr("删除控件"),ui->tableWidget); //右键删除菜单
    addAction    = new QAction(tr("添加控件"),ui->tableWidget);
    uploadAction = new QAction(tr("上传背景图片"),ui->tableWidget);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);//自定义菜单

    connect(deleteAction, SIGNAL(triggered()), this, SLOT(SlotDeleteWidget()));    //删除
    connect(addAction, SIGNAL(triggered()), this, SLOT(SlotAddWidget()));          //添加
    connect(uploadAction, SIGNAL(triggered()), this, SLOT(SlotUpLoadBackground()));//上传
    this->setAcceptDrops(true); //此属性保存是否已启用此控件的拖放事件

    QDir dir;
    strPath = dir.currentPath() + "/SmartHome.ini"; //获取配置文件路径
}

dragwindows::~dragwindows()
{
    delete ui;
}

//鼠标单击事件
void dragwindows::mousePressEvent(QMouseEvent *event)
{
    if(!(Qt::LeftButton == event->button())) //鼠标右键点击
    {
        DeletePoint = event->pos();
        on_tableWidget_customContextMenuRequested(DeletePoint);//右键菜单
        return;
    }

    strLocation.clear();    //清空鼠标在控件区图标上的坐标
    PointLocction.setX(0);
    PointLocction.setY(0);

    QLabel *child = static_cast<QLabel*>(childAt(event->pos()));//获取的是控件区或者画图区的Label控件
    if (!child||child->pos().isNull())
    {
        return;
    }
    child->setScaledContents(true);//自动缩放
    child->resize(64, 64);
    //为了方便在读写配置文件的时候，根据字段名获取相应的值
    WidgetId = ::GetPrivateProfileIntA("WidgetPosition", (QString::number(child->pos().x())+" and "+QString::number(child->pos().y())).toAscii().data(), 030, strPath.toAscii().data());
    strLocation = QString::number(WidgetId);    //没有这步，则报错
    emit SigLocation(strLocation);  //触发调用SlotLocation(QString location)函数
    PointLocction.setX(child->pos().x());
    PointLocction.setY(child->pos().y());

    QPixmap pixmap = child->pixmap()->scaled(64, 64);   //复制，并对原始图片进行缩放
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);//将pixmap和QPoint数据写入dataStream的itemData中
    dataStream << pixmap << QPoint(event->pos() - child->pos());//表示鼠标点击坐标和控件左上角之间的距离

    //定义一个可拖放的剪切板
    //使用QMimeData保存这些信息，据说是为了interoperability(两台不同电脑共同工作交通的能力)
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);//字符串用于指定这个MIME的format，方便其他widget或者程序识别拖拽信息。

    //定义一个可拖放通道
    QDrag *drag = new QDrag(this);  //创建拖拽操作
    drag->setMimeData(mimeData);    //将数据传给drag
    drag->setPixmap(pixmap);        //拖拽过程中显示的图片
    drag->setHotSpot(event->pos() - child->pos());  //设置热点

    //点击后的原图形、变色填充
    QPixmap tempPixmap = pixmap;
    QPainter painter;
    painter.begin(&tempPixmap);
    painter.fillRect(pixmap.rect(), QColor(127, 127, 127, 127));
    painter.end();  //结束绘图
    child->setPixmap(tempPixmap);

    if(IsIconicRecv)   //控件区的不能移动
    {
        QString IDSQL = "SELECT * FROM widget WHERE Widget_X = '"+QString::number(child->pos().x())+"' AND Widget_Y = '"+QString::number(child->pos().y())+"'";
        QSqlQuery IDQuery;
        if(IDQuery.exec(IDSQL))
        {
            while(IDQuery.next())
            {
                widget_ID = IDQuery.value(0).toInt();   //获取控件在数据库中的ID
            }
        }
    }

    //点击拷贝后的副本图形
    //检测是不是移动操作，默认是拷贝操作（Qt::CopyAction）
    if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction)
    {
        //移动
        child->close();
    }
    else
    {
        //拷贝
        child->show();  //显示拷贝后的副本
        child->setPixmap(pixmap);//恢复颜色
    }
}

//当用户把一个对象拖动到这个窗体上时，就会调用dragEnterEvent()
void dragwindows::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata"))
    {
        //在这个应用程序中，拖动操作的源是一个小部件，这个函数返回该源；否则返回0
        //该操作的源是使用QDrag对象实例化拖动的第一个参数
        if (event->source() == this) //拖动的部分是否在本窗口
        {
            if(!IsIconicRecv)
                return;
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
        else    //检测是否将控件区的控件拖动到画图区已有的控件之上
        {
            if(IsIconicRecv)    //判断是不是画图区
            {
                event->acceptProposedAction();
            }
        }
    }
    else
    {
        event->ignore();
    }
}

//拖动过程中可以实时监测
void dragwindows::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata"))
    {
        if (event->source() == this) //拖动的部分是否在本窗口
        {
            //检测是否将画图区的控件移动到画图区其他已存在的控件之上
            if(!IsIconicRecv)   //控件区的不能移动
                return;

            QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
            QDataStream dataStream(&itemData, QIODevice::ReadOnly);

            QPixmap pixmap;
            QPoint offset;
            dataStream >> pixmap >> offset;//dataStream数据流中读取数据到pixmap和offset中

            UpdateRect = QRect(event->pos() - offset, QSize(64, 64));   //根据鼠标和控件的偏移值，实时改变被拖动矩形的坐标

            QString MoveSQL = "SELECT * FROM widget ORDER BY Widget_Id";
            QSqlQuery MoveQuery;
            if(MoveQuery.exec(MoveSQL))
            {
                while(MoveQuery.next())
                {
                    if(MoveQuery.value(0).toInt() != widget_ID) //解决被拖动控件和自身原先区域排斥问题
                    {
                        OrignalRect = QRect(MoveQuery.value(1).toInt(), MoveQuery.value(2).toInt(), 64, 64);
                        if(UpdateRect.intersects(OrignalRect)) //相交
                        {
                            event->ignore();
                            return;
                        }
                    }
                    else
                    {
                        event->setDropAction(Qt::MoveAction);
                        event->accept();
                    }
                }
                event->setDropAction(Qt::MoveAction);
                event->accept();
            }
        }
        else
        {
            if(IsIconicRecv)    //判断是不是画图区
            {
                //MIME:Multipurpose Internet Mail Extensions type 多用途因特网邮件扩展类型
                QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
                QDataStream dataStream(&itemData, QIODevice::ReadOnly);

                QPixmap pixmap;
                QPoint offset;
                dataStream >> pixmap >> offset;//dataStream数据流中读取数据到pixmap和offset中

                QPoint ImagePath = event->pos()-offset; //ImagePath是被拖动控件左上角的坐标
                UpRect = QRect(ImagePath, QSize(64, 64));   //获取拖动控件的大小

                //判断控件重叠操作：遍历数据库，判断该位置上是否已存在控件
                QString JudgeSQL = "SELECT * FROM widget ORDER BY Widget_Id";
                QSqlQuery JudgeQuery;
                if(JudgeQuery.exec(JudgeSQL))
                {
                    while(JudgeQuery.next())
                    {
                        DownRect = QRect(JudgeQuery.value(1).toInt(), JudgeQuery.value(2).toInt(), 64, 64);
                        if(UpRect.intersects(DownRect))    //相交
                        {
                            event->ignore();
                            return;
                        }
                    }
                    event->acceptProposedAction();
                }
            }
        }
    }
    else
    {
        event->ignore();
    }
}

//当用户在窗口部件上放下一个对象时，就会调用dropEvent()
void dragwindows::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata"))//设置格式
    {
        QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint offset;
        dataStream >> pixmap >> offset; //offset控件左上角坐标和鼠标当前坐标之间的距离

        QLabel *newIcon = new QLabel(this);
        newIcon->setPixmap(pixmap);
        newIcon->move(event->pos()-offset);
        newIcon->show();
        newIcon->setScaledContents(true);
        newIcon->resize(64, 64);    //设置在放下的时候，按比例缩小
        newIcon->setAttribute(Qt::WA_DeleteOnClose);

        if (event->source()==this)
        {
            event->setDropAction(Qt::MoveAction);
            event->accept();

            if(IsIconicRecv)    //只有画图区的控件才需要写入数据库
            {
                QString UpdateSQL = "UPDATE widget SET Widget_X='"+QString::number((event->pos()-offset).x())+"',Widget_Y='"+QString::number((event->pos()-offset).y())+"' WHERE Widget_X = '"+QString::number(PointLocction.x())+"' AND Widget_Y = '"+QString::number(PointLocction.y())+"'";
                QSqlQuery query;
                qDebug()<<UpdateSQL<<query.exec(UpdateSQL)<<endl;
            }
        }
        else
        {
            event->acceptProposedAction();
            QPoint ImagePath = event->pos()-offset; //ImagePath:控件在画图区的左上角的坐标
            QByteArray faly = strLocation.toLatin1();

            char buf[100]={0};
            ::GetPrivateProfileStringA("Path", faly.data(), "00", buf, 100, strPath.toAscii().data());
            QString Path = QString(buf);
            memset(buf,0,100);

            QString SQL= "INSERT INTO widget (widget.Widget_X,widget.Widget_Y,widget.Widget_Path) VALUES ('"+QString::number(ImagePath.x())+"','"+QString::number(ImagePath.y())+"','"+Path+"')";
            QSqlQuery query;
            qDebug()<<query.exec(SQL)<<SQL<<endl;
        }
    }
    else
    {
        event->ignore();
    }
}

void dragwindows::SlotLocation(QString location)
{
    strLocation = location;
}

/******初始化函数：显示画图区的控件Label**********/
void dragwindows::_InitMap()
{
    if(!IsIconicRecv)   //如果是控件区，则不执行
        return;

    _InitBackground();  //画图区背景初始化

    QString SQL = "SELECT * FROM widget ORDER BY widget.Widget_Id";
    QSqlQuery query;

    if(query.exec(SQL))
    {
        while(query.next())
        {
            QLabel *boatIcon = new QLabel(this);    //加载控件
            boatIcon->setPixmap(QPixmap(query.value(3).toString()));//Widget_Path
            boatIcon->move(query.value(1).toInt(),query.value(2).toInt());//Widget_X Widget_Y
            boatIcon->show();
            boatIcon->setScaledContents(true);
            boatIcon->resize(QSize(64, 64));
            boatIcon->setAttribute(Qt::WA_DeleteOnClose);
        }
    }
}

/*****初始化函数：初始化画图区背景*****/
void dragwindows::_InitBackground()
{
    char buf[60] = {0};
    ::GetPrivateProfileStringA("Background", "background", "00", buf, 60, strPath.toAscii().data());
    BackgroundStr = buf;
    this->setStyleSheet(" QTableWidget{border-image:url("+BackgroundStr+");}"); //自适应大小
}

void dragwindows::_Init(bool IsRecv)
{
    IsIconicRecv = IsRecv;  //画图区 or 控件区
    _InitMap();
}

/*******信号槽函数：删除画图区 or 控件区的控件**********/
void dragwindows::SlotDeleteWidget()
{
    //获取鼠标的当前坐标，根据坐标获取相应控件
    QLabel *child = static_cast<QLabel*>(childAt(DeletePoint));
    if(IsIconicRecv)    //画图区删除操作
    {
        QString SELSQL = "SELECT * FROM widget ORDER BY widget.Widget_Id";
        QSqlQuery SELquery;
        if (child && child->x() != 0 && child->y() != 0)
        {
            if(SELquery.exec(SELSQL))
            {
                while(SELquery.next())
                {
                    QString SQL = "DELETE FROM widget WHERE widget.Widget_X = '"+QString::number(child->x())+"' AND widget.Widget_Y = '"+QString::number(child->y())+"'";
                    QSqlQuery query;
                    query.exec(SQL);  //数据库已经刷新
                    delete child;   //避免野指针
                    update();   //QWidget::update,更新界面
                    break;
                }
            }
        }
        else
        {
            QMessageBox::critical(this, tr("删除错误"), tr("请选择要删除的控件！"));
            return;
        }
    }
    else    //控件区删除操作
    {
        if (child && child->x() != 0 && child->y() != 0)
        {
            WidgetId = ::GetPrivateProfileIntA("WidgetPosition", (QString::number(child->pos().x())+" and "+QString::number(child->pos().y())).toAscii().data(), 030, strPath.toAscii().data());
            //参数3为：NULL表示删除该条数据
            WritePrivateProfileStringA("Path", QString::number(WidgetId).toAscii().data(), NULL, strPath.toAscii().data());
            WritePrivateProfileStringA("WidgetPosition", (QString::number(child->pos().x())+" and "+QString::number(child->pos().y())).toAscii().data(), NULL, strPath.toAscii().data());

            int count = GetPrivateProfileIntA("WidgetCount", "count", 003, strPath.toAscii().data());
            count--;
            WritePrivateProfileStringA("WidgetCount", "count", QString::number(count).toAscii().data(), strPath.toAscii().data());
            delete child;
            update();
        }
        else
        {
            QMessageBox::critical(this, tr("删除错误"), tr("请选择要删除的控件！"));
            return;
        }
    }
}

/*******信号槽函数：添加控件区的控件**********/
void dragwindows::SlotAddWidget()
{
    int count = GetPrivateProfileIntA("WidgetCount", "count", 0000, strPath.toAscii().data());
    if(count == 10) //限制最大10个控件
    {
        QMessageBox::critical(this, tr("添加失败"), tr("添加控件数量已达上限（10），无法添加"));
        return;
    }
    QString tempValue = NULL;
    char buf[80] = {0}; //保存控件路径
    for(int j = 1; j <= count+1; j++)
    {
        if(tempValue == "123456")
        {
            LabelPath = QFileDialog::getOpenFileName(this, tr("open file"), QDir::currentPath(), tr("Images(*.png *.xpm *.jpg)"));
            if(LabelPath != NULL)
            {
                if(LabelPath.length() > 80)
                {
                    QMessageBox::warning(this, "错误", "所选控件路径过长，请重新选择！");
                    return;
                }
                WritePrivateProfileStringA("Path", QString::number(j-1).toUtf8().data(), LabelPath.toUtf8().data(), strPath.toAscii().data());
                WritePrivateProfileStringA("WidgetCount", "count", QString::number(count+1).toAscii().data(), strPath.toAscii().data());
                flushWidget(j-1, LabelPath);  //添加之后，刷新界面
                return;
            }
        }
        memset(buf, 0, 80);
        //遍历：查看缺失的是哪一个编号的控件
        GetPrivateProfileStringA("Path", QString::number(j).toUtf8().data(), "123456", buf, 80, strPath.toAscii().data());
        tempValue = buf;
    }

    LabelPath = QFileDialog::getOpenFileName(this, tr("open file"), QDir::currentPath(), tr("Images(*.png *.xpm *.jpg)"));
    if(LabelPath != NULL)
    {
        WritePrivateProfileStringA("Path", QString::number(count+1).toUtf8().data(), LabelPath.toUtf8().data(), strPath.toAscii().data());
        WritePrivateProfileStringA("WidgetCount", "count", QString::number(count+1).toAscii().data(), strPath.toAscii().data());
        flushWidget(count+1, LabelPath);  //添加之后，刷新界面
    }
}

/*******信号槽函数：上传画图区的背景图片**********/
void dragwindows::SlotUpLoadBackground()
{
    QString BackgroundPath = QFileDialog::getOpenFileName(this, tr("open file"), QDir::currentPath());
    //如果没有该字段，则新建该字段
    if(BackgroundPath != NULL)
    {
        if(BackgroundPath.length() > 60)
        {
            QMessageBox::warning(this, "错误", "所选背景图片路径过长，请重新选择！");
            return;
        }
        ::WritePrivateProfileStringA("Background", "background", BackgroundPath.toUtf8().data(), strPath.toAscii().data());
        _InitBackground();  //上传完成之后，刷新
    }
}

//右键“删除控件”
void dragwindows::on_tableWidget_customContextMenuRequested(const QPoint &pos)//当前鼠标的坐标
{
    if(IsIconicRecv)
    {
        MapMenu->addAction(deleteAction);   //右键---删除
        MapMenu->addAction(uploadAction);   //右键---上传
        MapMenu->exec(QCursor::pos());
    }
    else
    {
        MapMenu->addAction(deleteAction);   //右键---删除
        MapMenu->addAction(addAction);      //右键---添加
        MapMenu->exec(QCursor::pos());
    }
}

/*******控件区刷新函数***********/
void dragwindows::flushWidget(int i, QString Path)
{
    if(i%2 == 0)    //右边
    {
        temp = 40+23*i/2+64*(i/2-1);
        QPoint Point1(24*2+64,temp);
        QLabel *boatIcon1 = new QLabel(this);//可以执行这个函数，表示this是控件区对象
        boatIcon1->setPixmap(QPixmap(Path));
        boatIcon1->move(Point1);
        boatIcon1->show();
        boatIcon1->setScaledContents(true);
        boatIcon1->resize(64, 64);
        boatIcon1->setAttribute(Qt::WA_DeleteOnClose);
        ::WritePrivateProfileStringA("WidgetPosition", (QString::number(Point1.x())+" and "+QString::number(Point1.y())).toAscii().data(), QString::number(i).toAscii().data(), strPath.toAscii().data());
    }
    else    //左边
    {
        temp = 40+23*(i+1)/2+64*((i+1)/2-1);
        QPoint Point2(24,temp);
        QLabel *boatIcon2 = new QLabel(this);
        boatIcon2->setPixmap(QPixmap(Path));
        boatIcon2->move(Point2);
        boatIcon2->show();
        boatIcon2->setScaledContents(true);
        boatIcon2->resize(64, 64);
        boatIcon2->setAttribute(Qt::WA_DeleteOnClose);
        ::WritePrivateProfileStringA("WidgetPosition", (QString::number(Point2.x())+" and "+QString::number(Point2.y())).toAscii().data(), QString::number(i).toAscii().data(), strPath.toAscii().data());
    }
}

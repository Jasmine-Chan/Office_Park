#include "basicwindow.h"
#include "ui_basicwindow.h"

BasicWindow::BasicWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BasicWindow)
{
    ui->setupUi(this);
    this->setMouseTracking(true);

    //设置在不按鼠标的情况下也触发鼠标移动事件，注意QMainWindow的情况:centralWidget()->setMouseTracking(true);
    isLeftPressed=false;//左键是否按下标志
    curPos=0;//标记鼠标左击时的位置
    this->setMinimumSize(400,300);//设置最小尺寸
    QCursor cursor;    //鼠标光标类
    cursor.setShape(Qt::ArrowCursor);//设置鼠标为箭头形状
 //   ui->pushButton->setCursor(cursor);//当放在按钮上时，为箭头
   // cursor.setShape(Qt::OpenHandCursor);//当放在主窗口上时，为手形
    QWidget::setCursor(cursor);
 //   qDebug()<<"h="<<this->height();
    setWindowFlags(Qt::FramelessWindowHint);//设置主窗口无边框
 //   qDebug()<<this->minimumHeight();
//    qDebug()<<parent->size();
    _Interface();
    m_nIsMax = 0;  //窗口模式标志位
}

void BasicWindow::_Init(int x,int y)
{
     this->resize(x,y);
    _Interface();
}

BasicWindow::~BasicWindow()
{
    delete ui;
}

//计算当前鼠标在哪个区域
int BasicWindow::countFlag(QPoint p, int row)
{

    if(p.y()<MARGIN)
        return 10+row;
    else if(p.y()>this->height()-MARGIN)
        return 30+row;
    else
        return 20+row;
}

//根据鼠标位置确定其执行事件
void BasicWindow::setCursorType(int flag)
{
    Qt::CursorShape cursor;
       switch(flag)
       {
       case 11:
       case 33:
           cursor=Qt::SizeFDiagCursor;break;
       case 13:
       case 31:
           cursor=Qt::SizeBDiagCursor;break;
       case 21:
       case 23:
           cursor=Qt::SizeHorCursor;break;
       case 12:
       case 32:
           cursor=Qt::SizeVerCursor;break;
       case 22:
           cursor=Qt::ArrowCursor;break;
       default:
           // QApplication::restoreOverrideCursor();//恢复鼠标指针性状
            cursor=Qt::ArrowCursor;
            break;

       }
       setCursor(cursor);
}

//判断鼠标位置在水平方向的位置
int BasicWindow::countRow(QPoint p)
{

     return (p.x()<MARGIN)?1:(p.x()>(this->width()-MARGIN)?3:2);
}

//按鼠标左键改变鼠标的形状
void BasicWindow::mousePressEvent(QMouseEvent *event)
{
    emit movesignal();
    if(event->button()==Qt::LeftButton)
    {
        this->isLeftPressed=true;
        QCursor cursor;
        cursor.setShape(Qt::ClosedHandCursor);
        QApplication::setOverrideCursor(cursor);//改变鼠标指针形状
        QPoint temp=event->globalPos();         //返回其指针位置
        pLast=temp;
        curPos=countFlag(event->pos(),countRow(event->pos()));
        event->ignore();
    }
}

//释放鼠标左键
void BasicWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(isLeftPressed)
            isLeftPressed=false;
        QApplication::restoreOverrideCursor();//恢复鼠标指针性状
        event->ignore();//关闭鼠标接收参数
}

//设置窗口模型
void BasicWindow::_Interface()
{
    QBitmap bmp(this->size());
       bmp.fill();
       QPainter p(&bmp);
       p.setPen(Qt::NoPen);
       p.setBrush(Qt::black);
       p.drawRoundedRect(bmp.rect(),10,10);
       setMask(bmp);

//    QBitmap bmp(this->size());
//    bmp.fill();
//    QPainter p(&bmp);

//    p.setRenderHint(QPainter::Antialiasing);
//    int arcR = 25;
//    QRect rect = this->rect();
//    QPainterPath path;
//    //逆时针
//    path.moveTo(arcR, 0);
//    path.arcTo(0, 0, arcR * 2, arcR * 2, 90.0f, 90.0f);

//    path.lineTo(0, rect.height()-arcR);
//    path.arcTo(0, rect.height()-arcR*2, arcR * 2, arcR * 2, 180.0f, 90.0f);

//    path.lineTo(rect.width()-arcR, rect.height());
//    path.arcTo(rect.width()-arcR*2, rect.height()-arcR*2, arcR * 2, arcR * 2, 270.0f, 90.0f);

//    path.lineTo(rect.width(), arcR);
//    path.arcTo(rect.width() - arcR * 2, 0, arcR * 2, arcR * 2, 0.0f, 90.0f);
//    path.lineTo(arcR, 0);

//    p.drawPath(path);
//    p.fillPath(path, QBrush(Qt::red)); //arm和windows平台没有这行代码将显示一个透明的空空的框
//    setMask(bmp);//
}
void BasicWindow::Double()
{
    if(windowState()!=Qt::WindowFullScreen){
        setWindowState(Qt::WindowFullScreen);
        m_nIsMax = 1;
    }else {
        setWindowState(Qt::WindowNoState);//恢复正常模式
        m_nIsMax = 0;
    }

    _Interface();
}

void BasicWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    qDebug()<<"111";
    if(event->button()==Qt::LeftButton)
       {
           if(windowState()!=Qt::WindowFullScreen)
               setWindowState(Qt::WindowFullScreen);
           else setWindowState(Qt::WindowNoState);//恢复正常模式
       }
       _Interface();
       event->ignore();
}

//窗口移动和伸缩函数
void BasicWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(m_nIsMax == 0){
    int poss=countFlag(event->pos(),countRow(event->pos()));
   //  qDebug()<<"pos="<<poss;
       setCursorType(poss);
       if(isLeftPressed)
       {
           QPoint ptemp=event->globalPos();
           ptemp=ptemp-pLast;
           if(curPos==22)//移动窗口
           {
               ptemp=ptemp+pos();
               move(ptemp);
           }
           else
           {
               QRect wid=geometry();

               switch(curPos)//改变窗口的大小
               {

               case 11:wid.setTopLeft(wid.topLeft()+ptemp);break;
               case 13:wid.setTopRight(wid.topRight()+ptemp);break;
               case 31:wid.setBottomLeft(wid.bottomLeft()+ptemp);break;
               case 33:wid.setBottomRight(wid.bottomRight()+ptemp);break;
               case 12:wid.setTop(wid.top()+ptemp.y());break;
               case 21:wid.setLeft(wid.left()+ptemp.x());break;
               case 23:wid.setRight(wid.right()+ptemp.x());break;
               case 32:wid.setBottom(wid.bottom()+ptemp.y());break;
               }
               _Interface();
               setGeometry(wid);
           }


           pLast=event->globalPos();//更新位置
//           qDebug()<< event->pos().x()<<endl;
//           qDebug()<< event->pos().y()<<endl;
//           qDebug()<< event->globalPos().y()<<endl;
       }
    }
       event->ignore();
}

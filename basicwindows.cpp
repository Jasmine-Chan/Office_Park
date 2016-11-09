#include "basicwindows.h"
#include "ui_basicwindows.h"

basicWindows::basicWindows(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::basicWindows)
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
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMinMaxButtonsHint);;//设置主窗口无边框
    _Interface();
}

void basicWindows::_Init(int x,int y)
{
    this->resize(x,y);
    _Interface();
}

basicWindows::~basicWindows()
{
    delete ui;
}

//计算当前鼠标在哪个区域
int basicWindows::countFlag(QPoint p, int row)
{

    if(p.y()<MARGIN)
        return 10+row;
    else if(p.y()>this->height()-MARGIN)
        return 30+row;
    else
        return 20+row;
}

//根据鼠标位置确定其执行事件
void basicWindows::setCursorType(int flag)
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
int basicWindows::countRow(QPoint p)
{

     return (p.x()<MARGIN)?1:(p.x()>(this->width()-MARGIN)?3:2);
}

//按鼠标左键改变鼠标的形状
void basicWindows::mousePressEvent(QMouseEvent *event)
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
void basicWindows::mouseReleaseEvent(QMouseEvent *event)
{
        if(isLeftPressed)
            isLeftPressed=false;
        QApplication::restoreOverrideCursor();//恢复鼠标指针性状
        event->ignore();//关闭鼠标接收参数
}

//设置窗口模型
void basicWindows::_Interface()
{
//    QBitmap bmp(this->size());
//       bmp.fill();
//       QPainter p(&bmp);
//       p.setPen(Qt::NoPen);
//       p.setBrush(Qt::black);
//       p.drawRoundedRect(bmp.rect(),10,10);
//       setMask(bmp);

    QBitmap bmp(this->size());
    bmp.fill();
    QPainter p(&bmp);

    p.setRenderHint(QPainter::Antialiasing);
    int arcR = 6;
    QRect rect = this->rect();
    QPainterPath path;
    //逆时针
    path.moveTo(arcR, 0);
    path.arcTo(0, 0, arcR * 2, arcR * 2, 90.0f, 90.0f);

    arcR = 3;
    path.lineTo(0, rect.height()-arcR);
    path.arcTo(0, rect.height()-arcR*2, arcR * 2, arcR * 2, 180.0f, 90.0f);

    path.lineTo(rect.width()-arcR, rect.height());
    path.arcTo(rect.width()-arcR*2, rect.height()-arcR*2, arcR * 2, arcR * 2, 270.0f, 90.0f);

    arcR = 6;
    path.lineTo(rect.width(), arcR);
    path.arcTo(rect.width() - arcR * 2, 0, arcR * 2, arcR * 2, 0.0f, 90.0f);
    path.lineTo(arcR, 0);

    p.drawPath(path);
    p.fillPath(path, QBrush(Qt::red)); //arm和windows平台没有这行代码将显示一个透明的空空的框
    setMask(bmp);//
}

void basicWindows::Double()
{
//    if(windowState() != Qt::WindowFullScreen)
//    {
//        setWindowState(Qt::WindowFullScreen);
//        m_nIsMax = 1;
//    }
//    else
//    {
//        setWindowState(Qt::WindowNoState);//恢复正常模式
    showNormal();
//        m_nIsMax = 0;
//    }

    _Interface();
}

//void basicWindows::mouseDoubleClickEvent(QMouseEvent *event)
//{
//    if(event->button()==Qt::LeftButton)
//       {
//           if(windowState()!=Qt::WindowFullScreen)
//               setWindowState(Qt::WindowFullScreen);
//           else setWindowState(Qt::WindowNoState);//恢复正常模式
//       }
//       _Interface();
//       event->ignore();
//}

//窗口移动和伸缩函数
void basicWindows::mouseMoveEvent(QMouseEvent *event)
{
    int poss = countFlag(event->pos(),countRow(event->pos()));
    setCursorType(poss);
    if(isLeftPressed)
    {
        QPoint ptemp = event->globalPos();
        ptemp = ptemp-pLast;
    if(curPos == 22)//移动窗口
    {
        ptemp = ptemp+pos();
        move(ptemp);
    }
//           else
//           {
//               QRect wid=geometry();

//               switch(curPos)//改变窗口的大小
//               {

//               case 11:
//                   {wid.setTopLeft(wid.topLeft()+ptemp);
//                    qDebug()<<"右下移动"<<endl;
//                   }
//                    break;
//               case 13:
//                   {wid.setTopRight(wid.topRight()+ptemp);
//                    qDebug()<<"右下移动"<<endl;
//                   }
//                    break;
//               case 31:
//                   {wid.setBottomLeft(wid.bottomLeft()+ptemp);
//                    qDebug()<<"右下移动"<<endl;
//                   }
//                    break;
//               case 33:/*wid.setBottomRight(wid.bottomRight()+ptemp);break;*/
//                        {   wid.setBottomRight(wid.bottomRight()+ptemp);
////                            wid.setRight(wid.right()+ptemp.x());
////                            wid.setBottom(wid.bottom()+ptemp.y());
//                            qDebug()<<"右下移动"<<endl;
//                            break;
//                        }
//               case 12:
//                   {wid.setTop(wid.top()+ptemp.y());
//                    qDebug()<<"上移动"<<endl;
//                   }
//                    break;
//               case 21:
//                   {wid.setLeft(wid.left()+ptemp.x());
//                    qDebug()<<"左移动"<<endl;
//                   }
//                    break;
//               case 23:
//               {wid.setRight(wid.right()+ptemp.x());
//                   qDebug()<<"右移动"<<endl;
//                }
//                   break;

//               case 32:
//                   {wid.setBottom(wid.bottom()+ptemp.y());
//                    qDebug()<<"下移动"<<endl;
//                   }
//                    break;
//               }
//               _Interface();
//               setGeometry(wid);
//           }
       pLast = event->globalPos();//更新位置
   }
   event->ignore();
}

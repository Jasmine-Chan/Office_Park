#include "form3.h"
#include "ui_form3.h"

Form3::Form3(BasicWindow *parent) :
    BasicWindow(parent),
    ui(new Ui::Form3)
{
    ui->setupUi(this);
//    _Init(this->size().width(),this->size().height());
//   QPushButton *btn3=new QPushButton(this);
//    btn3->setStyleSheet("color:red;background-color:rgb(200,155,100)");
//    btn3->setText("Button3");
  //  ui->QpushButton->setStyleSheet("/*color:red;background-color:rgb(200,155,100);*/background-image:url(:/images/OfficeParks6.png);");
}

Form3::~Form3()
{
    delete ui;
}

//双击窗口最大化
void Form3::mouseDoubleClickEvent(QMouseEvent *event)
{

    if(event->button()==Qt::LeftButton)
       {
        Double();
       }
       event->ignore();
}


//最大化函数
void Form3::on_QpbtnMax_clicked()
{
    Double();
}

//最小化函数
void Form3::on_QpbtnMin_clicked()
{
    setWindowState(Qt::WindowMinimized);
}

//窗口关闭函数
void Form3::on_QpbtnClose_clicked()
{
    close();
}

#ifndef DEAL_H
#define DEAL_H
/*******************************
  报警记录处理部分
  ********************************/
#include <QWidget>
#include <struct.h>

namespace Ui {
    class deal;
}

class deal : public QWidget
{
    Q_OBJECT

public:
    explicit deal(QWidget *parent = 0);
    ~deal();
    void _Update(SAlarmDeal AlarmDeal);

private slots:
    void on_pbtnDeal_clicked();
    void on_pbtnClose_clicked();

signals:
    void SigDealDate(QString whetherMesIfo,QString dealHuman,QString Messages);

private:
    Ui::deal *ui;

};

#endif // DEAL_H

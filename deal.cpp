#include "deal.h"
#include "ui_deal.h"

deal::deal(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::deal)
{
    ui->setupUi(this);
    ui->IP_Add->clear();
    ui->Add->clear();
    ui->Number->clear();
    ui->DeviceType->clear();
    ui->Stime->clear();
    ui->Etime->clear();
    ui->Misinformation->setChecked(true);   //默认选中“误报”
}

deal::~deal()
{
    delete ui;
}

void deal::on_pbtnDeal_clicked()
{
    QString whetherMesIfo, dealHuman, Messages;
    if(ui->Misinformation->isChecked())
    {
        whetherMesIfo = "误报";
    }
    else if(ui->information->isChecked())
    {
        whetherMesIfo = "非误报";
    }
    dealHuman = ui->DealHuman->text();
    Messages = ui->DealMessage->toPlainText();

    emit SigDealDate(whetherMesIfo,dealHuman,Messages);


}

void deal::on_pbtnClose_clicked()
{
    close();
}

void deal::_Update(SAlarmDeal AlarmDeal)
{
    ui->IP_Add->clear();
    ui->Add->clear();
    ui->Number->clear();
    ui->DeviceType->clear();
    ui->Stime->clear();
    ui->Etime->clear();

    ui->IP_Add->setText(AlarmDeal.QAlarmIPAdd);
    ui->Add->setText(AlarmDeal.QAlarmAdd);
    ui->Number->setText(AlarmDeal.QAlarmNo);
    ui->DeviceType->setText(AlarmDeal.QAlarmDeviceType);
    ui->Stime->setText(AlarmDeal.QAlarmStime);
    ui->Etime->setText(AlarmDeal.QAlarmEtime);
}

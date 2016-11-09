#include "company.h"
#include "ui_company.h"
#include "QDebug"
CCOMPANY::CCOMPANY(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CCOMPANY)
{
    ui->setupUi(this);
    m_nFlag = -1;
    ui->Company_lineEdit->setDisabled(true);
    ui->Company_lineEdit->setPlaceholderText("(请点击上传JPG、PNG格式图片，大小限制在100K内)");
    ui->QRCode_lineEdit->setDisabled(true);
    ui->QRCode_lineEdit->setPlaceholderText("扫码关注");
    _Init();
}

CCOMPANY::~CCOMPANY()
{
    delete ui;
}

void CCOMPANY::_Init()
{
    _InitLine(0);
    QString SQL = "select *from Company";
    QSqlQuery query;
    if(query.exec(SQL)){
        if(query.first()){
            m_nFlag = query.value(0).toInt();
            ui->QlineCompanyCName->setText(query.value(1).toString());
            ui->QlineCompanyEName->setText(query.value(2).toString());
            ui->QlineCompanyAddr->setText(query.value(3).toString());
            ui->QlineCompanyLegalPerson->setText(query.value(4).toString());
            ui->QlineCompanyPhone->setText(query.value(5).toString());
            ui->QlineCompanyFax->setText(query.value(6).toString());
            ui->camp_pushButton->setStyleSheet("border-image:url("+query.value(7).toString()+")");
            ui->QpbtnAddCompany->setText("修改");
        }else{
            m_nFlag = -1;
            ui->QpbtnAddCompany->setText("添加");
        }
    }
}
void CCOMPANY::_InitLine(int n)
{
    if(n == 0)
    {
        ui->QlineCompanyAddr->setEnabled(false);
        ui->QlineCompanyCName->setEnabled(false);
        ui->QlineCompanyEName->setEnabled(false);
        ui->QlineCompanyFax->setEnabled(false);
        ui->QlineCompanyLegalPerson->setEnabled(false);
        ui->QlineCompanyPhone->setEnabled(false);
        ui->QpbtnSave->setEnabled(false);
        ui->camp_pushButton->setEnabled(false);
    }
    else
    {
        ui->QlineCompanyAddr->setEnabled(true);
        ui->QlineCompanyCName->setEnabled(true);
        ui->QlineCompanyEName->setEnabled(true);
        ui->QlineCompanyFax->setEnabled(true);
        ui->QlineCompanyLegalPerson->setEnabled(true);
        ui->QlineCompanyPhone->setEnabled(true);
        ui->QpbtnSave->setEnabled(true);
        ui->camp_pushButton->setEnabled(true);
        _SetFocusOrder();
    }
}

void CCOMPANY::on_QpbtnAddCompany_clicked()
{
    _InitLine(1);
}

void CCOMPANY::on_QpbtnSave_clicked()
{
    if(m_nFlag >= 0 ){
        QString SQL = "update company Set Company_Cname = '"+ui->QlineCompanyCName->text()+"',Company_Ename = '"+ui->QlineCompanyEName->text()+"',"
                "Company_Addr = '"+ui->QlineCompanyAddr->text()+"',Company_Legal_Person = '"+ui->QlineCompanyLegalPerson->text()+"',"
                "Company_Phone = '"+ui->QlineCompanyPhone->text()+"',Company_Fax = '"+ui->QlineCompanyFax->text()+"',Company_Logo_Path = '"+LogoPath+"' where Company_Id = '"+QString::number(m_nFlag)+"'";
        QSqlQuery query;
        query.exec(SQL);
    }else{
        QString SQL = "insert into company (Company_Cname,Company_Ename,Company_Addr,Company_Legal_Person,Company_Phone,Company_Fax,Company_Logo_Path)"
                " values ('"+ui->QlineCompanyCName->text()+"','"+ui->QlineCompanyEName->text()+"','"+ui->QlineCompanyAddr->text()+"',"
                "'"+ui->QlineCompanyLegalPerson->text()+"','"+ui->QlineCompanyPhone->text()+"','"+ui->QlineCompanyFax->text()+"','"+LogoPath+"')";
        QSqlQuery query;
        bool b = query.exec(SQL);
    }
    _InitLine(0);
    ui->QpbtnSave->setEnabled(false);
}

void CCOMPANY::on_QpbtnClose_clicked()
{
    emit SigClose();
}

void CCOMPANY::on_camp_pushButton_clicked()
{
    LogoPath = QFileDialog::getOpenFileName();
    if(LogoPath!="")
    {
        ui->camp_pushButton->setStyleSheet("border-image:url("+LogoPath+")");
    }
}

void CCOMPANY::_SetFocusOrder()
{
    ui->QlineCompanyCName->setFocus();
    ui->QlineCompanyCName->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->QlineCompanyEName->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->QlineCompanyAddr->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->QlineCompanyLegalPerson->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->QlineCompanyPhone->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    ui->QlineCompanyFax->setStyleSheet("QLineEdit:focus{border:1px solid green;}");
    QWidget::setTabOrder(ui->QlineCompanyCName,ui->QlineCompanyEName);
    QWidget::setTabOrder(ui->QlineCompanyEName,ui->QlineCompanyAddr);
    QWidget::setTabOrder(ui->QlineCompanyAddr,ui->QlineCompanyLegalPerson);
    QWidget::setTabOrder(ui->QlineCompanyLegalPerson,ui->QlineCompanyPhone);
    QWidget::setTabOrder(ui->QlineCompanyPhone,ui->QlineCompanyFax);
}



#include "handle.h"
#include "ui_handle.h"
#include <QMessageBox>
CHANDLE::CHANDLE(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CHANDLE)
{
    ui->setupUi(this);
    m_nFlag = 0;
    ui->QcomSex->addItem("男");
    ui->QcomSex->addItem("女");

    ui->QcomNation->addItem("汉族");
    ui->QcomNation->addItem("满族");
    ui->QcomNation->addItem("回族");
    ui->QcomNation->addItem("其它");

    ui->QcomMarital->addItem("未婚");
    ui->QcomMarital->addItem("已婚");

    ui->QcomEducation->addItem("小学");
    ui->QcomEducation->addItem("初中");
    ui->QcomEducation->addItem("高中");
    ui->QcomEducation->addItem("大专");
    ui->QcomEducation->addItem("本科");
    ui->QcomEducation->addItem("硕士");
    ui->QcomEducation->addItem("博士");
    ui->QcomEducation->addItem("教授");

    ui->QcomPolitics->addItem("群众");
    ui->QcomPolitics->addItem("团员");
    ui->QcomPolitics->addItem("党员");

    ui->QdateEnter->setCalendarPopup(true);
    ui->QdateBirthday->setCalendarPopup(true);
    ui->QdateGraduation->setCalendarPopup(true);
}

CHANDLE::~CHANDLE()
{
    delete ui;
}

void CHANDLE::_InitDepartment()
{
    ui->QcomDepartment->clear();
    ui->QcomDepartment->addItem(" ");
    QString SQL = "SELECT * from department ORDER BY Department_Id DESC";
    QSqlQuery query;
    if(query.exec(SQL))
    {
        int i = 1;
        while(query.next()){
            SList data;
            data.Id = i;
            data.strNo = query.value(1).toString();
            data.strName = query.value(2).toString();
            ui->QcomDepartment->addItem(data.strName);
            m_List.append(data);
            i++;
        }
    }
}

void CHANDLE::_UpdateStaff(SStaff *sStaff)
{
    sStaff->strAdd = ui->QlineAddr->text();
    sStaff->strBirthday = ui->QdateBirthday->text();
    sStaff->strDepartment = "";

    sStaff->strEmail = ui->QlineEmail->text();
    sStaff->strEnter = ui->QdateEnter->text();
    sStaff->strGraduationDate = ui->QdateGraduation->text();
    sStaff->strGraduationSchool = ui->QlineGraduationSchool->text();
    sStaff->strIdCard = ui->QlineIdCard->text();
    sStaff->strRemark = ui->QtextRemark->toPlainText();
    sStaff->strName = ui->QlineName->text();
    sStaff->strMajor = ui->QlineMajor->text();
    sStaff->strNative = ui->QlineNative->text();
    sStaff->strNo = ui->QlineNo->text();
    sStaff->strPhone = ui->QlinePhone->text();
    sStaff->strPost = ui->QlinePost->text();

    sStaff->nNation = ui->QcomNation->currentIndex();
    sStaff->nSex = ui->QcomSex->currentIndex();
    sStaff->nEducation = ui->QcomEducation->currentIndex();
    sStaff->nPolitics = ui->QcomPolitics->currentIndex();
    sStaff->nMaritalStatus = ui->QcomMarital->currentIndex();
    for(int i = 0;i < m_List.size();i++)
    {
        if(m_List.at(i).strName == ui->QcomDepartment->currentText())
        {
            sStaff->strDepartment = m_List.at(i).strNo;
            break;
        }
    }
}

void CHANDLE::_Update()
{
    ui->QpbtnOK->setText("添加");
    m_nFlag = 1;
    _InitDepartment();

    ui->QcomDepartment->setCurrentIndex(0);
    ui->QcomEducation->setCurrentIndex(0);
    ui->QcomMarital->setCurrentIndex(0);
    ui->QcomNation->setCurrentIndex(0);
    ui->QcomPolitics->setCurrentIndex(0);
    ui->QcomSex->setCurrentIndex(0);

    ui->QlineAddr->clear();
    ui->QlineEmail->clear();
    ui->QlineGraduationSchool->clear();
    ui->QlineIdCard->clear();
    ui->QlineName->clear();
    ui->QlineNative->clear();
    ui->QlineNo->clear();
    ui->QlinePhone->clear();
    ui->QlinePost->clear();

    QDate date = QDate::currentDate();
    ui->QdateBirthday->setDate(date);
    ui->QdateEnter->setDate(date);
    ui->QdateGraduation->setDate(date);
}

void CHANDLE::_Update(SStaff sStaff)
{
    ui->QpbtnOK->setText("确定");
    m_nFlag = 2;
    _InitDepartment();

    ui->QcomDepartment->setCurrentIndex(0);
    ui->QcomEducation->setCurrentIndex(0);
    ui->QcomMarital->setCurrentIndex(0);
    ui->QcomNation->setCurrentIndex(0);
    ui->QcomPolitics->setCurrentIndex(0);
    ui->QcomSex->setCurrentIndex(0);

    ui->QlineAddr->clear();
    ui->QlineEmail->clear();
    ui->QlineGraduationSchool->clear();
    ui->QlineIdCard->clear();
    ui->QlineName->clear();
    ui->QlineNative->clear();
    ui->QlineNo->clear();
    ui->QlinePhone->clear();
    ui->QlinePost->clear();

    ui->QdateBirthday->clear();
    ui->QdateEnter->clear();
    ui->QdateGraduation->clear();

    ui->QcomEducation->setCurrentIndex(sStaff.nEducation);
    ui->QcomMarital->setCurrentIndex(sStaff.nMaritalStatus);
    ui->QcomNation->setCurrentIndex(sStaff.nNation);
    ui->QcomPolitics->setCurrentIndex(sStaff.nPolitics);
    ui->QcomSex->setCurrentIndex(sStaff.nSex);

    for(int i = 0;i < m_List.size();i++)
    {
        if(m_List.at(i).strName == sStaff.strDepartment)
        {
            int nId = m_List.at(i).Id;
            ui->QcomDepartment->setCurrentIndex(nId);
            break;
        }
    }
    qDebug()<<"1111"<<sStaff.strAdd;
    ui->QlineAddr->setText(sStaff.strAdd);
    ui->QlineEmail->setText(sStaff.strEmail);
    ui->QdateBirthday->setDate(QDate::fromString(sStaff.strBirthday,"yyyy-MM-dd"));
    ui->QdateEnter->setDate(QDate::fromString(sStaff.strEnter,"yyyy-MM-dd"));
    m_strEnterTime = sStaff.strEnter;
    ui->QdateGraduation->setDate(QDate::fromString(sStaff.strGraduationDate,"yyyy-MM-dd"));
    ui->QlineGraduationSchool->setText(sStaff.strGraduationSchool);
    ui->QlineIdCard->setText(sStaff.strIdCard);
    ui->QlineMajor->setText(sStaff.strMajor);
    ui->QlineName->setText(sStaff.strName);
    ui->QlineNative->setText(sStaff.strNative);
    ui->QlineNo->setText(sStaff.strNo);
    m_strNO = sStaff.strNo;
    ui->QlinePhone->setText(sStaff.strPhone);
    ui->QlinePost->setText(sStaff.strPost);
    ui->QtextRemark->setText(sStaff.strRemark);

}

void CHANDLE::on_QpbtnOK_clicked()
{
    if(m_nFlag == 1){
        if(ui->QlineName->text() == "")
        {
            QMessageBox::information(this,"提示","姓名不能为空");
            return ;
        }
        if(ui->QlinePhone->text() == "")
        {
            QMessageBox::information(this,"提示","电话不能为空");
            return ;
        }
        if(ui->QlinePost->text() == "")
        {
            QMessageBox::information(this,"提示","职位不能为空");
            return ;
        }
        if(ui->QlineNo->text() == "")
        {
            QMessageBox::information(this,"提示","员工编号不能为空");
            return ;
        }else{
           if(CMysql->_SelectNo(ui->QlineNo->text()))
           {
               QMessageBox::information(this,"提示","员工编号已存在");
               return ;
           }
        }
        SStaff sStaff;
        _UpdateStaff(&sStaff);
        if(CMysql->_InsertStaff(sStaff)){
            QMessageBox::information(this,"提示","添加成功");
            emit SigInit();
            close();
            return ;
        }else{
            QMessageBox::information(this,"提示","添加失败");
            return ;
        }
    }else if(m_nFlag == 2){
        if(ui->QlineName->text() == "")
        {
            QMessageBox::information(this,"提示","姓名不能为空");
            return ;
        }
        if(ui->QlinePhone->text() == "")
        {
            QMessageBox::information(this,"提示","电话不能为空");
            return ;
        }
        if(ui->QlinePost->text() == "")
        {
            QMessageBox::information(this,"提示","职位不能为空");
            return ;
        }
        if(ui->QlineNo->text() == "")
        {
            QMessageBox::information(this,"提示","员工编号不能为空");
            return ;
        }else{
           if(ui->QlineNo->text() != m_strNO){
           if(CMysql->_SelectNo(ui->QlineNo->text()))
           {
               QMessageBox::information(this,"提示","员工编号已存在");
               return ;
           }
           }
        }
        SStaff sStaff;
        _UpdateStaff(&sStaff);
        if(CMysql->_UpdateStaff(sStaff,m_strNO,m_strEnterTime)){
            QMessageBox::information(this,"提示","修改成功");
            emit SigInit();
            close();
            return ;
        }else{
            QMessageBox::information(this,"提示","修改失败");
            return ;
        }
    }
}

void CHANDLE::on_QpbtnClose_clicked()
{
    close();
}

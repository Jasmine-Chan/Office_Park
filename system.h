#ifndef SYSTEM_H
#define SYSTEM_H

#include "struct.h"
#include <QWidget>
#include <QMenu>
#include "mysql.h"
#include "user.h"

namespace Ui {
    class CSYSTEM;
}

class CSYSTEM : public QWidget
{
    Q_OBJECT

public:
    explicit CSYSTEM(QWidget *parent = 0);
    ~CSYSTEM();
    void _UpdateToUser(SUser sUser);

private slots:
    void on_QUser_customContextMenuRequested(const QPoint &pos);
    void SlotUserUpdate();
    void SlotUserDelete();
    void SlotUserAdd();
    void SlotUpdateUser();//增删改成功后信号

private:
    Ui::CSYSTEM *ui;
    SUser m_sUser;
    CUSER *CUser;
    QMenu *UserMenu;
    QAction *UserUpdateAction;
    QAction *UserDeleteAction;
    QAction *UserAddAction;

private:
    void _InitUser();
    void _UpdateUser();

};

#endif // SYSTEM_H

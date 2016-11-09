#ifndef ICONICO_H
#define ICONICO_H
/*******************************
  画图界面，《现已不用》
  ********************************/
#include <QWidget>
#include <windows.h>
#include <QLabel>
#include <QFrame>
#include <vector>
#include <QSqlQuery>
#include <QMenu>
#include <QAction>

using namespace std;
namespace Ui {
    class iconico;
}

class iconico : public QWidget
{
    Q_OBJECT

public:
    explicit iconico(QWidget *parent = 0);
    void _InitMap();
    ~iconico();

private:
    Ui::iconico *ui;
    void mousePressEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    QString strLocation;
    QPoint PointLocction;
    int MapNumber;
    bool isWidget;

private:
    QMenu *MapMenu;
    QAction *deleteAction;

signals:
    void SigLocation(QString location);

private slots:
    void SlotLocation(QString location);
    void on_Map_customContextMenuRequested(const QPoint &pos);
    void SlotDeleteWidget();
};

#endif // ICONICO_H

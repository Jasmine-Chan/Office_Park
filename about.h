#ifndef ABOUT_H
#define ABOUT_H

#include <QWidget>

namespace Ui {
    class about;
}

class about : public QWidget
{
    Q_OBJECT

public:
    explicit about(QWidget *parent = 0);
    ~about();

private:
    Ui::about *ui;

public slots:

};

#endif // ABOUT_H

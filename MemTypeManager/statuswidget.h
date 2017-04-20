#ifndef STATUSWIDGET_H
#define STATUSWIDGET_H

#include <QWidget>

namespace Ui {
class statuswidget;
}

class statuswidget : public QWidget
{
    Q_OBJECT

public:
    explicit statuswidget(QWidget *parent = 0);
    ~statuswidget();

private:
    Ui::statuswidget *ui;
};

#endif // STATUSWIDGET_H

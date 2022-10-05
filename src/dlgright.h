#ifndef DLGRIGHT_H
#define DLGRIGHT_H

#include <QDialog>

namespace Ui {
class dlgRight;
}

class dlgRight : public QDialog
{
    Q_OBJECT

public:
    explicit dlgRight(QWidget *parent = nullptr);
    ~dlgRight();

private:
    Ui::dlgRight *ui;
};

#endif // DLGRIGHT_H

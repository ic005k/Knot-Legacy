#include "dlgright.h"
#include "ui_dlgright.h"

dlgRight::dlgRight(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgRight)
{
    ui->setupUi(this);
}

dlgRight::~dlgRight()
{
    delete ui;
}

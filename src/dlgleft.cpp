#include "dlgleft.h"
#include "ui_dlgleft.h"

dlgLeft::dlgLeft(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgLeft)
{
    ui->setupUi(this);
}

dlgLeft::~dlgLeft()
{
    delete ui;
}

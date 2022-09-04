#include "dlgweb.h"
#include "ui_dlgweb.h"

dlgWeb::dlgWeb(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgWeb)
{
    ui->setupUi(this);
}

dlgWeb::~dlgWeb()
{
    delete ui;
}

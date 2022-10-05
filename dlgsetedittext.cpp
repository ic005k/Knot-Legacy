#include "dlgsetedittext.h"
#include "ui_dlgsetedittext.h"

dlgSetEditText::dlgSetEditText(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgSetEditText)
{
    ui->setupUi(this);
}

dlgSetEditText::~dlgSetEditText()
{
    delete ui;
}

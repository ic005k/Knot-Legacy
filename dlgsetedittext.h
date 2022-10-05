#ifndef DLGSETEDITTEXT_H
#define DLGSETEDITTEXT_H

#include <QDialog>

namespace Ui {
class dlgSetEditText;
}

class dlgSetEditText : public QDialog
{
    Q_OBJECT

public:
    explicit dlgSetEditText(QWidget *parent = nullptr);
    ~dlgSetEditText();

private:
    Ui::dlgSetEditText *ui;
};

#endif // DLGSETEDITTEXT_H

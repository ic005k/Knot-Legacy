#include "msgdialog.h"

#include "mainwindow.h"
#include "ui_msgdialog.h"

extern MainWindow* mw_one;

msgDialog::msgDialog(QWidget* parent) : QDialog(parent), ui(new Ui::msgDialog) {
  ui->setupUi(this);

  this->layout()->setMargin(0);
  this->installEventFilter(this);
  initDlg();

  // this->show();
}

msgDialog::~msgDialog() { delete ui; }

void msgDialog::initDlg() {
  ui->lblText->setWordWrap(true);
  ui->lblText->adjustSize();

  this->setFixedHeight(240);
  this->setFixedWidth(mw_one->width());
  this->setGeometry(mw_one->geometry().x(),
                    (mw_one->height() - this->height()) / 2, this->width(),
                    this->height());
  this->setModal(true);
  ui->lblTime->setText(QDateTime::currentDateTime().toString());
}

bool msgDialog::eventFilter(QObject* obj, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnOK_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(obj, evn);
}

void msgDialog::on_btnOK_clicked() { close(); }

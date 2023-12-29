#include "ShowMessage.h"

#include "MainWindow.h"
#include "ui_ShowMessage.h"

extern MainWindow* mw_one;

ShowMessage::ShowMessage(QWidget* parent)
    : QDialog(parent), ui(new Ui::ShowMessage) {
  ui->setupUi(this);
  setModal(true);
  QFont font = this->font();
  font.setBold(true);
  ui->lblTitle->setFont(font);
  ui->lblTitle->adjustSize();
  ui->lblTitle->setWordWrap(true);

  ui->lblMsgTxt->adjustSize();
  ui->lblMsgTxt->setWordWrap(true);

  mw_one->set_btnStyle(this);
}

ShowMessage::~ShowMessage() { delete ui; }

bool ShowMessage::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      close();
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void ShowMessage::init() {
  setFixedHeight(mw_one->height());
  setFixedWidth(mw_one->width());
  setGeometry(mw_one->geometry().x(), mw_one->geometry().y(), width(),
              height());
  show();
}

void ShowMessage::showMsg(QString title, QString msgtxt, int btnCount) {
  // btnCount 1:Ok 2:Cancel + Ok

  ui->lblTitle->setText(title);
  ui->lblMsgTxt->setText(msgtxt);
  if (btnCount == 1) {
    ui->btnCancel->hide();
    ui->btnOk->show();
  }
  if (btnCount == 2) {
    ui->btnCancel->show();
    ui->btnOk->show();
  }

  init();
}

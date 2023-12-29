#include "ShowMessage.h"

#include "MainWindow.h"
#include "ui_ShowMessage.h"

extern MainWindow* mw_one;

ShowMessage::ShowMessage(QWidget* parent)
    : QDialog(parent), ui(new Ui::ShowMessage) {
  ui->setupUi(this);

  this->layout()->setMargin(0);
  setWindowFlag(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
  ui->widget->setStyleSheet("background-color:rgba(0, 0, 0,25%);");
  ui->frame->setStyleSheet(
      "QFrame{background-color: rgb(255, 255, 255);border-radius:10px; "
      "border:0px solid gray;}");

  setModal(true);
  QFont font = this->font();
  font.setBold(true);
  ui->lblTitle->setFont(font);
  ui->lblTitle->adjustSize();
  ui->lblTitle->setWordWrap(true);

  ui->lblMsgTxt->adjustSize();
  ui->lblMsgTxt->setWordWrap(true);

  ui->hframe->setFrameShape(QFrame::HLine);
  ui->hframe->setStyleSheet("QFrame{background:red;min-height:2px}");

  mw_one->set_btnStyle(this);
}

ShowMessage::~ShowMessage() { delete ui; }

bool ShowMessage::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnCancel_clicked();
    }

    return true;
  }

  return QWidget::eventFilter(watch, evn);
}

void ShowMessage::init() {
  isValue = false;
  setFixedHeight(mw_one->height());
  setFixedWidth(mw_one->width());
  setGeometry(mw_one->geometry().x(), mw_one->geometry().y(), width(),
              height());
  show();
}

bool ShowMessage::showMsg(QString title, QString msgtxt, int btnCount) {
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

  while (!isHidden()) QCoreApplication::processEvents();

  return isValue;
}

void ShowMessage::on_btnCancel_clicked() {
  isValue = false;
  close();
}

void ShowMessage::on_btnOk_clicked() {
  isValue = true;
  close();
}

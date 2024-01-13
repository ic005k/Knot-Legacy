#include "ShowMessage.h"

#include "MainWindow.h"
#include "ui_ShowMessage.h"

extern MainWindow* mw_one;
extern QString copyText;

ShowMessage::ShowMessage(QWidget* parent)
    : QDialog(parent), ui(new Ui::ShowMessage) {
  ui->setupUi(this);
  this->installEventFilter(this);

  this->layout()->setMargin(0);
  setWindowFlag(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
  ui->widget->setStyleSheet("background-color:rgba(0, 0, 0,30%);");

  if (mw_one->isDark)
    ui->frame->setStyleSheet(
        "QFrame{background-color: #455364;color: #FFFFFF;border-radius:10px; "
        "border:0px solid gray;}");
  else
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
  if (mw_one->isDark)
    ui->hframe->setStyleSheet(
        "QFrame{background:rgb(0,205,205);min-height:2px}");
  else
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

  if (evn->type() == QEvent::MouseButtonPress) {
    if (ui->btnCancel->isHidden() && ui->btnCopy->isHidden() &&
        ui->btnOk->isHidden()) {
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

  int x, y, w, h;

#ifdef Q_OS_ANDROID
  w = this->width() - 20;

#else
  w = 350;
  if (w >= mw_one->width()) {
    w = mw_one->width() - 20;
  }
#endif

  ui->frame->setFixedWidth(w);

  h = ui->frame->height();
  x = this->x() + (this->width() - w) / 2;
  y = this->y() + (this->height() - h) / 2;
  ui->frame->setGeometry(x, y, w, h);

  show();
}

bool ShowMessage::showMsg(QString title, QString msgtxt, int btnCount) {
  // btnCount 1:Ok 2:Cancel + Ok

  if (btnCount == 2 || btnCount == 3) {
    msgtxt = msgtxt + "\n\n";
  }

  if (btnCount == 1) btnCount = 0;

  if (btnCount == 0) {
    ui->btnCancel->hide();
    ui->btnCopy->hide();
    ui->btnOk->hide();
  }
  if (btnCount == 1) {
    ui->btnCancel->hide();
    ui->btnOk->show();
    ui->btnCopy->hide();
  }
  if (btnCount == 2) {
    ui->btnCancel->show();
    ui->btnOk->show();
    ui->btnCopy->hide();
  }

  if (btnCount == 3) {
    ui->btnCancel->show();
    ui->btnOk->show();
    ui->btnCopy->show();
  }

  ui->lblTitle->setText(title);
  ui->lblMsgTxt->setText(msgtxt);

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

void ShowMessage::on_btnCopy_clicked() {
  QClipboard* clipboard = QApplication::clipboard();
  clipboard->setText(copyText);
  isValue = false;
  close();
}

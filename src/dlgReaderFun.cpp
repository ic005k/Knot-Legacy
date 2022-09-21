#include "dlgReaderFun.h"

#include "mainwindow.h"
#include "ui_dlgReaderFun.h"
#include "ui_mainwindow.h"

extern int fontSize;
extern MainWindow* mw_one;

dlgReaderFun::dlgReaderFun(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgReaderFun) {
  ui->setupUi(this);
  ui->btnFont->setStyleSheet("border:none");
  ui->btnFontLess->setStyleSheet("border:none");
  ui->btnFontPlus->setStyleSheet("border:none");
}

dlgReaderFun::~dlgReaderFun() { delete ui; }

void dlgReaderFun::init() {
  setGeometry(mw_one->geometry().x(), mw_one->ui->quickWidget->y(), width(),
              mw_one->ui->quickWidget->height());
  setWindowFlags(Qt::WindowStaysOnTopHint);
  show();
}

bool dlgReaderFun::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      close();
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgReaderFun::on_hSlider_sliderReleased() {
  mw_one->mydlgReader->on_hSlider_sliderReleased(ui->hSlider->value());
}

void dlgReaderFun::on_btnFontPlus_clicked() {
  mw_one->textFontSize++;
  mw_one->mydlgReader->setFontSize(mw_one->textFontSize);
}

void dlgReaderFun::on_btnFontLess_clicked() {
  if (mw_one->textFontSize <= 8) return;
  mw_one->textFontSize--;
  mw_one->mydlgReader->setFontSize(mw_one->textFontSize);
}

void dlgReaderFun::on_btnFont_clicked() {
  close();
  mw_one->mydlgReader->on_btnFont_clicked();
}

void dlgReaderFun::on_hSlider_sliderMoved(int position) {
  mw_one->on_hSlider_sliderMoved(position);
}

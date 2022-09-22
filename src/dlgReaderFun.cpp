#include "dlgReaderFun.h"

#include "mainwindow.h"
#include "ui_dlgReaderFun.h"
#include "ui_mainwindow.h"

extern int fontSize;
extern MainWindow* mw_one;
extern QString iniDir;

dlgReaderFun::dlgReaderFun(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgReaderFun) {
  QPalette pal = palette();
  pal.setColor(QPalette::Background, QColor(128, 42, 42, 100));
  setPalette(pal);

  ui->setupUi(this);

  ui->btnFont->setStyleSheet("border:none");
  ui->btnFontLess->setStyleSheet("border:none");
  ui->btnFontPlus->setStyleSheet("border:none");
  QFont f(this->font());
  f.setPointSize(10);
  ui->btnStyle1->setFont(f);
  ui->btnStyle2->setFont(f);
  ui->btnStyle3->setFont(f);
}

dlgReaderFun::~dlgReaderFun() { delete ui; }

void dlgReaderFun::init() {
  setGeometry(mw_one->geometry().x(), mw_one->ui->quickWidget->y(), width(),
              mw_one->ui->quickWidget->height());
  // setWindowFlags(Qt::WindowStaysOnTopHint);

  setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool | Qt::FramelessWindowHint);

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

void dlgReaderFun::on_btnStyle1_clicked() {
  QSettings Reg(iniDir + "reader.ini", QSettings::IniFormat);
  Reg.setIniCodec("utf-8");
  Reg.setValue("/Reader/Style", "1");
  mw_one->mydlgReader->readerStyle = "1";
  mw_one->mydlgReader->setReaderStyle();
}

void dlgReaderFun::on_btnStyle2_clicked() {
  QSettings Reg(iniDir + "reader.ini", QSettings::IniFormat);
  Reg.setIniCodec("utf-8");
  Reg.setValue("/Reader/Style", "2");
  mw_one->mydlgReader->readerStyle = "2";
  mw_one->mydlgReader->setReaderStyle();
}

void dlgReaderFun::on_btnStyle3_clicked() {
  QSettings Reg(iniDir + "reader.ini", QSettings::IniFormat);
  Reg.setIniCodec("utf-8");
  Reg.setValue("/Reader/Style", "3");
  mw_one->mydlgReader->readerStyle = "3";
  mw_one->mydlgReader->setReaderStyle();
}

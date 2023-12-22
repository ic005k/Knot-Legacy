#include "ReaderFun.h"

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_ReaderFun.h"

extern int fontSize;
extern MainWindow* mw_one;
extern QString iniDir, privateDir;

dlgReaderFun::dlgReaderFun(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgReaderFun) {
  QPalette pal = palette();

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  pal.setColor(QPalette::Background, QColor(128, 42, 42, 100));
#else
  pal.setColor(QPalette::Window, QColor(128, 42, 42, 100));
#endif

  setPalette(pal);

  ui->setupUi(this);
  ui->btnFont->hide();

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
  setGeometry(mw_one->geometry().x(), mw_one->geometry().y(), width(),
              mw_one->ui->qwReader->height());
  // setWindowFlags(Qt::WindowStaysOnTopHint);

  setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool | Qt::FramelessWindowHint);

  show();
}

bool dlgReaderFun::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      close();
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgReaderFun::on_hSlider_sliderReleased() {
  mw_one->m_Reader->on_hSlider_sliderReleased(ui->hSlider->value());
}

void dlgReaderFun::on_btnFontPlus_clicked() {
  mw_one->textFontSize++;
  mw_one->m_Reader->setFontSize(mw_one->textFontSize);
}

void dlgReaderFun::on_btnFontLess_clicked() {
  if (mw_one->textFontSize <= 8) return;
  mw_one->textFontSize--;
  mw_one->m_Reader->setFontSize(mw_one->textFontSize);
}

void dlgReaderFun::on_btnFont_clicked() {
  close();
  mw_one->m_Reader->selectFont();
}

void dlgReaderFun::on_hSlider_sliderMoved(int position) {
  mw_one->on_hSlider_sliderMoved(position);
}

void dlgReaderFun::on_btnStyle1_clicked() {
  QSettings Reg(privateDir + "reader.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  Reg.setValue("/Reader/Style", "1");
  mw_one->m_Reader->readerStyle = "1";
  mw_one->m_Reader->setReaderStyle();
}

void dlgReaderFun::on_btnStyle2_clicked() {
  QSettings Reg(privateDir + "reader.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  Reg.setValue("/Reader/Style", "2");
  mw_one->m_Reader->readerStyle = "2";
  mw_one->m_Reader->setReaderStyle();
}

void dlgReaderFun::on_btnStyle3_clicked() {
  QSettings Reg(privateDir + "reader.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  Reg.setValue("/Reader/Style", "3");
  mw_one->m_Reader->readerStyle = "3";
  mw_one->m_Reader->setReaderStyle();
}

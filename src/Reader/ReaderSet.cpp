#include "ReaderSet.h"

#include "src/MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_ReaderSet.h"

extern int fontSize, readerFontSize;
extern MainWindow* mw_one;
extern Method* m_Method;
extern QString iniDir, privateDir;
extern QSettings* iniPreferences;
extern QRegularExpression regxNumber;

ReaderSet::ReaderSet(QWidget* parent) : QDialog(parent), ui(new Ui::ReaderSet) {
  QPalette pal = palette();
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  pal.setColor(QPalette::Background, QColor(10, 10, 10, 200));
#else
  pal.setColor(QPalette::Window, QColor(10, 10, 10, 200));
#endif
  setPalette(pal);

  ui->setupUi(this);
  setModal(true);

  // setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool |
  // Qt::FramelessWindowHint);

  this->installEventFilter(this);
  ui->hSlider->installEventFilter(this);

  ui->btnFontLess->setStyleSheet("border:none");
  ui->btnFontPlus->setStyleSheet("border:none");

  QFont f = m_Method->getNewFont(fontSize);
  ui->btnStyle1->setFont(f);
  ui->btnStyle2->setFont(f);
  ui->btnStyle3->setFont(f);
  ui->btnFont->setFont(f);
  ui->lblProg->setFont(f);

  int nHeight = m_Method->getFontHeight();
  ui->btnFont->setFixedHeight(nHeight * 3.5);

  f.setPointSize(12);
  ui->lblInfo->setFont(f);
  ui->lblInfo->adjustSize();
  ui->lblInfo->setWordWrap(true);

  QValidator* validator =
      new QRegularExpressionValidator(regxNumber, ui->editPage);
  ui->editPage->setValidator(validator);
}

ReaderSet::~ReaderSet() { delete ui; }

void ReaderSet::init() {
  int x, y, w, h;
  w = mw_one->width();
  h = this->height();
  x = mw_one->geometry().x();
  y = mw_one->geometry().y() + mw_one->ui->qwReader->geometry().y() +
      (mw_one->ui->qwReader->height() - h) / 2;
  setFixedWidth(w);
  setGeometry(x, y, w, h);

  ui->hSlider->setStyleSheet(mw_one->ui->hsH->styleSheet());

  QStringList list = mw_one->ui->btnPages->text().split("\n");
  if (list.count() == 2) {
    ui->hSlider->setValue(list.at(0).toInt());
  }

  show();
}

bool ReaderSet::eventFilter(QObject* watch, QEvent* evn) {
  QMouseEvent* event = static_cast<QMouseEvent*>(evn);
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  if (watch == ui->hSlider) {
    if (event->type() == QEvent::MouseButtonRelease) {
      on_hSlider_sliderReleased();
      on_hSlider_sliderMoved(ui->hSlider->value());
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void ReaderSet::on_hSlider_sliderReleased() {
  mw_one->m_Reader->on_hSlider_sliderReleased(ui->hSlider->value());
}

void ReaderSet::on_btnFontPlus_clicked() {
  readerFontSize++;
  mw_one->m_Reader->setFontSize(readerFontSize);
}

void ReaderSet::on_btnFontLess_clicked() {
  if (readerFontSize <= 8) return;
  readerFontSize--;
  mw_one->m_Reader->setFontSize(readerFontSize);
}

void ReaderSet::on_hSlider_sliderMoved(int position) {
  mw_one->on_hSlider_sliderMoved(position);
}

void ReaderSet::updateProgress() {
  if (this->isHidden()) return;

  QStringList list = mw_one->ui->btnPages->text().split("\n");
  if (list.count() == 2) {
    QString cur = list.at(0);
    QString total = list.at(1);
    ui->lblProg->setText(tr("Reading Progress") + " : " + cur + " -> " + total);
  }
}

void ReaderSet::on_btnStyle1_clicked() {
  QSettings Reg(privateDir + "reader.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  Reg.setValue("/Reader/Style", "1");
  mw_one->m_Reader->readerStyle = "1";
  mw_one->m_Reader->setReaderStyle();
}

void ReaderSet::on_btnStyle2_clicked() {
  QSettings Reg(privateDir + "reader.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  Reg.setValue("/Reader/Style", "2");
  mw_one->m_Reader->readerStyle = "2";
  mw_one->m_Reader->setReaderStyle();
}

void ReaderSet::on_btnStyle3_clicked() {
  QSettings Reg(privateDir + "reader.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  Reg.setValue("/Reader/Style", "3");
  mw_one->m_Reader->readerStyle = "3";
  mw_one->m_Reader->setReaderStyle();
}

void ReaderSet::on_btnFont_clicked() {
  QString fileName;
  fileName = QFileDialog::getOpenFileName(this, tr("Font"), "",
                                          tr("Font Files (*.*)"));
  if (fileName == "") return;

#ifdef Q_OS_ANDROID
  fileName = m_Method->getRealPathFile(fileName);
#endif

  QString readerFont = mw_one->m_Preferences->setFontDemo(
      fileName, ui->btnFont, this->font().pointSize());
  iniPreferences->setValue("/Options/ReaderFont", fileName);

  mw_one->m_Reader->savePageVPos();
  mw_one->ui->qwReader->rootContext()->setContextProperty("FontName",
                                                          readerFont);
  mw_one->m_Reader->setPageVPos();
}

void ReaderSet::on_hSlider_valueChanged(int value) { Q_UNUSED(value); }

void ReaderSet::on_btnGoPage_clicked() {
  if (ui->editPage->text().trimmed() == "") return;

  int nPage = ui->editPage->text().toInt();
  if (nPage <= 0) nPage = 1;
  if (nPage > ui->hSlider->maximum()) nPage = ui->hSlider->maximum();
  ui->hSlider->setValue(nPage);
  on_hSlider_sliderMoved(nPage);
  on_hSlider_sliderReleased();
}

void ReaderSet::on_btnBack_clicked() { close(); }

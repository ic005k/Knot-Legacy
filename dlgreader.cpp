#include "dlgreader.h"

#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_dlgreader.h"

extern MainWindow* mw_one;
extern QString iniFile, iniDir;
extern bool isImport;
extern int fontSize;

dlgReader::dlgReader(QWidget* parent) : QDialog(parent), ui(new Ui::dlgReader) {
  ui->setupUi(this);
  this->installEventFilter(this);
  QScroller::grabGesture(ui->textBrowser, QScroller::LeftMouseButtonGesture);
  ui->textBrowser->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  // ui->textBrowser->setTextInteractionFlags(Qt::NoTextInteraction);
  mw_one->setSCrollPro(ui->textBrowser);
}

dlgReader::~dlgReader() { delete ui; }

bool dlgReader::eventFilter(QObject* obj, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(obj, evn);
}

void dlgReader::keyReleaseEvent(QKeyEvent* event) { Q_UNUSED(event); }

void dlgReader::on_btnBack_clicked() {
  saveReader();
  close();
}

void dlgReader::on_btnOpen_clicked() {
  fileName =
      QFileDialog::getOpenFileName(this, tr("Knot"), "", tr("Txt Files (*.*)"));
  openFile(fileName);
}

void dlgReader::openFile(QString fileName) {
  if (QFile(fileName).exists()) {
    QString txt = mw_one->loadText(fileName);
    ui->textBrowser->setPlainText(txt);
    QFileInfo fi(fileName);
    ui->lblTitle->setText(fi.baseName());
    ui->lblTitle->hide();
  }
}

void dlgReader::saveReader() {
  QSettings Reg(iniDir + "reader.ini", QSettings::IniFormat);
  Reg.setValue("/Reader/SliderPos",
               ui->textBrowser->verticalScrollBar()->sliderPosition());
  Reg.setValue("/Reader/FileName", fileName);
  Reg.setValue("/Reader/FontSize", ui->textBrowser->font().pointSize());
}

void dlgReader::initReader() {
  QSettings Reg(iniDir + "reader.ini", QSettings::IniFormat);
  QFont font;
  font.setPointSize(Reg.value("/Reader/FontSize", fontSize).toInt());
  ui->textBrowser->setFont(font);
  ui->textBrowser->setFont(font);
  fileName = Reg.value("/Reader/FileName").toString();
  openFile(fileName);
  vpos = Reg.value("/Reader/SliderPos").toLongLong();
}

void dlgReader::on_btnFontPlus_clicked() {
  QFont font;
  int size = ui->textBrowser->font().pointSize();
  size = size + 1;
  font.setPointSize(size);
  ui->textBrowser->setFont(font);
  saveReader();
}

void dlgReader::on_btnFontLess_clicked() {
  QFont font;
  int size = ui->textBrowser->font().pointSize();
  size = size - 1;
  font.setPointSize(size);
  ui->textBrowser->setFont(font);
  saveReader();
}

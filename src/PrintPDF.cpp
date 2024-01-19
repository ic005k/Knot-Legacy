#include "PrintPDF.h"

#include "MainWindow.h"
#include "ui_PrintPDF.h"

extern MainWindow* mw_one;
extern Method* m_Method;

PrintPDF::PrintPDF(QWidget* parent) : QDialog(parent), ui(new Ui::PrintPDF) {
  ui->setupUi(this);
  setWindowFlag(Qt::FramelessWindowHint);
  QString style = "QDialog{border-radius:0px;border:0px solid darkred;}";
  this->setStyleSheet(style);

  mw_one->set_btnStyle(this);
}

PrintPDF::~PrintPDF() { delete ui; }

void PrintPDF::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event)
  m_Method->closeGrayWindows();
}

bool PrintPDF::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnCancel_clicked();
    }

    return true;
  }

  return QWidget::eventFilter(watch, evn);
}

void PrintPDF::on_btnCancel_clicked() {
  strValue = "";
  close();
}

QString PrintPDF::getItem(QString title, QString lblText, QStringList valueList,
                          int valueIndex) {
  this->setWindowTitle(title);
  ui->lblText->setText(lblText);
  ui->cbox->clear();
  ui->cbox->addItems(valueList);
  ui->cbox->setCurrentIndex(valueIndex);

  int x, y, w, h;
  w = mw_one->width() - 40;
  h = this->height();
  x = mw_one->geometry().x() + (mw_one->geometry().width() - w) / 2;
  y = mw_one->geometry().y() + (mw_one->geometry().height() - h) / 2;
  setGeometry(x, y, w, h);

  m_Method->showGrayWindows();
  show();
  while (!isHidden()) QCoreApplication::processEvents();

  return strValue;
}

void PrintPDF::on_btnOk_clicked() {
  strValue = ui->cbox->currentText();
  close();
}

#include "dlgSetText.h"

#include "src/mainwindow.h"
#include "ui_dlgSetText.h"

extern MainWindow* mw_one;

dlgSetText::dlgSetText(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgSetText) {
  QPalette pal = palette();
  pal.setColor(QPalette::Background, QColor(28, 42, 255, 120));
  setPalette(pal);

  ui->setupUi(this);
}

dlgSetText::~dlgSetText() { delete ui; }

void dlgSetText::init(int x, int y, int w, int h) {
  setGeometry(x, y, w, h);
  setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool | Qt::FramelessWindowHint);
  show();
}

bool dlgSetText::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      close();
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgSetText::on_btnBack_clicked() { mw_one->on_btnCancelSel_clicked(); }

void dlgSetText::on_btnCopy_clicked() { mw_one->on_btnCopy_clicked(); }

void dlgSetText::on_btnSearch_clicked() { mw_one->on_btnSearch_clicked(); }

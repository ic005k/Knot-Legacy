#include "dlgloadpic.h"

#include "src/mainwindow.h"
#include "ui_dlgloadpic.h"
extern MainWindow* mw_one;

dlgLoadPic::dlgLoadPic(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgLoadPic) {
  ui->setupUi(this);
  this->installEventFilter(this);

  ui->btnZoom->hide();
  ui->btnReduce->hide();
  this->layout()->setMargin(1);
}

dlgLoadPic::~dlgLoadPic() { delete ui; }

bool dlgLoadPic::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgLoadPic::on_btnBack_clicked() { close(); }

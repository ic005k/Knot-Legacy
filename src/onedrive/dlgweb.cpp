#include "dlgweb.h"

#include "src/mainwindow.h"
#include "ui_dlgweb.h"

extern MainWindow* mw_one;

dlgWeb::dlgWeb(QWidget* parent) : QDialog(parent), ui(new Ui::dlgWeb) {
  ui->setupUi(this);
  this->installEventFilter(this);
}

dlgWeb::~dlgWeb() { delete ui; }

bool dlgWeb::eventFilter(QObject* obj, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(obj, evn);
}

void dlgWeb::on_btnBack_clicked() {
  this->close();
  mw_one->refreshMainUI();
}

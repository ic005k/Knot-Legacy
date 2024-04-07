#include "ReceiveShare.h"

#include "src/MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_ReceiveShare.h"

extern MainWindow* mw_one;
extern Method* m_Method;

ReceiveShare::ReceiveShare(QWidget* parent)
    : QDialog(parent), ui(new Ui::ReceiveShare) {
  ui->setupUi(this);
  installEventFilter(this);
  setModal(true);
  ui->lblTip->setWordWrap(true);
  ui->lblTip->adjustSize();
  ui->lblTip->setText(tr("Tip: You can paste this data wherever you need it."));
  mw_one->set_ToolButtonStyle(this);
}

ReceiveShare::~ReceiveShare() { delete ui; }

void ReceiveShare::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event)
  QClipboard* m_Clip = QApplication::clipboard();
  m_Clip->setText(m_Method->strReceiveShareData);

  m_Method->closeGrayWindows();
}

bool ReceiveShare::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      Close();

      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void ReceiveShare::init() {
  int x, y, w, h;
  x = mw_one->geometry().x();
  y = mw_one->geometry().y();
  w = mw_one->width();
  h = mw_one->height() / 3;
  setGeometry(x, y, w, h);
  m_Method->showGrayWindows();
  show();
}

void ReceiveShare::Close() {
  m_Method->closeGrayWindows();
  close();
}

void ReceiveShare::on_btnAddToTodo_clicked() {
  if (mw_one->ui->frameTodo->isHidden()) {
    mw_one->on_btnTodo_clicked();
    mw_one->ui->editTodo->setText(m_Method->strReceiveShareData);
    mw_one->on_btnAddTodo_clicked();
    Close();
  }
}

void ReceiveShare::on_btnAddToNote_clicked() {
  if (mw_one->ui->frameNotes->isHidden()) {
    mw_one->on_btnNotes_clicked();
    mw_one->on_btnEdit_clicked();
    mw_one->m_Notes->m_EditSource->insertPlainText(
        m_Method->strReceiveShareData);
    Close();
  }
}

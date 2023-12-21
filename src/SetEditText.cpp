#include "SetEditText.h"

#include "src/MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_SetEditText.h"

extern MainWindow *mw_one;

dlgSetEditText::dlgSetEditText(QWidget *parent)
    : QDialog(parent), ui(new Ui::dlgSetEditText) {
  ui->setupUi(this);

  mw_one->set_btnStyle(this);

  ui->lineEdit->setReadOnly(true);

  this->installEventFilter(this);
  ui->btnLeft0->installEventFilter(this);
  ui->btnLeft1->installEventFilter(this);
  ui->btnRight0->installEventFilter(this);
  ui->btnRight1->installEventFilter(this);
  ui->btnClose->installEventFilter(this);
  ui->btnCopy->installEventFilter(this);
  ui->btnCut->installEventFilter(this);
  ui->btnPaste->installEventFilter(this);
  ui->btnSetAll->installEventFilter(this);
  ui->btnDel->installEventFilter(this);
  ui->btnBing->installEventFilter(this);
  ui->lineEdit->installEventFilter(this);

  int a = 500;
  int b = 50;
  ui->btnLeft0->setAutoRepeat(true);       // 启用长按
  ui->btnLeft0->setAutoRepeatDelay(a);     // 触发长按的时间
  ui->btnLeft0->setAutoRepeatInterval(b);  // 长按时click信号间隔

  ui->btnLeft1->setAutoRepeat(true);
  ui->btnLeft1->setAutoRepeatDelay(a);
  ui->btnLeft1->setAutoRepeatInterval(b);

  ui->btnRight0->setAutoRepeat(true);
  ui->btnRight0->setAutoRepeatDelay(a);
  ui->btnRight0->setAutoRepeatInterval(b);

  ui->btnRight1->setAutoRepeat(true);
  ui->btnRight1->setAutoRepeatDelay(a);
  ui->btnRight1->setAutoRepeatInterval(b);
}

dlgSetEditText::~dlgSetEditText() { delete ui; }

void dlgSetEditText::on_btnClose_clicked() {
  ui->lineEdit->clear();
  close();
}

void dlgSetEditText::init(int y) {
  setGeometry(mw_one->geometry().x() + (mw_one->width() - this->width()) / 2, y,
              this->width(), this->height());
  setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);

  show();
}

bool dlgSetEditText::eventFilter(QObject *watch, QEvent *evn) {
  QMouseEvent *event = static_cast<QMouseEvent *>(evn);
  if (event->type() == QEvent::MouseButtonPress) {
    isMousePress = true;
    isMouseRelease = false;
  }

  if (event->type() == QEvent::MouseButtonRelease) {
    isMousePress = false;
    isMouseMove = false;
    isMouseRelease = true;
  }

  if (event->type() == QEvent::MouseMove) {
    isMouseMove = true;

    if (isMousePress) {
      if (watch == this || watch == ui->lineEdit) {
        int y = event->globalY();
        if (y <= 0) y = 0;
        if (y >= mw_one->height() - height()) y = mw_one->height() - height();
        this->setGeometry(geometry().x(), y, width(), height());
      }
    }
  }

  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      close();
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgSetEditText::on_btnCopy_clicked() {
  mw_one->m_Notes->byTextEdit->copy();

  ui->lineEdit->clear();

  close();
}

void dlgSetEditText::on_btnCut_clicked() {
  mw_one->m_Notes->byTextEdit->cut();

  close();
}

void dlgSetEditText::on_btnPaste_clicked() {
  mw_one->m_Notes->byTextEdit->paste();

  close();
}

void dlgSetEditText::on_btnSetAll_clicked() {
  mw_one->m_Notes->byTextEdit->selectAll();
}

void dlgSetEditText::on_btnLeft1_clicked() {
  mw_one->m_Notes->start--;
  if (mw_one->m_Notes->start < 0) mw_one->m_Notes->start = 0;

  mw_one->m_Notes->selectText(mw_one->m_Notes->start, mw_one->m_Notes->end);
  ui->lineEdit->setCursorPosition(0);
}

void dlgSetEditText::on_btnLeft0_clicked() {
  mw_one->m_Notes->start++;
  if (mw_one->m_Notes->start >= mw_one->m_Notes->end)
    mw_one->m_Notes->start = mw_one->m_Notes->end - 1;

  mw_one->m_Notes->selectText(mw_one->m_Notes->start, mw_one->m_Notes->end);
  ui->lineEdit->setCursorPosition(0);
}

void dlgSetEditText::on_btnRight1_clicked() {
  mw_one->m_Notes->end++;

  mw_one->m_Notes->selectText(mw_one->m_Notes->start, mw_one->m_Notes->end);

  if (ui->lineEdit->text().trimmed() == "") {
    mw_one->m_Notes->end--;
    mw_one->m_Notes->selectText(mw_one->m_Notes->start, mw_one->m_Notes->end);
  }
}

void dlgSetEditText::on_btnRight0_clicked() {
  mw_one->m_Notes->end--;
  if (mw_one->m_Notes->end <= mw_one->m_Notes->start)
    mw_one->m_Notes->end = mw_one->m_Notes->start + 1;

  mw_one->m_Notes->selectText(mw_one->m_Notes->start, mw_one->m_Notes->end);
}

void dlgSetEditText::on_btnBing_clicked() {
  QString str = ui->lineEdit->text().trimmed();
  if (str.length() > 0) {
    QString strurl = "https://bing.com/search?q=" + str;
    QUrl url(strurl);
    QDesktopServices::openUrl(url);
    on_btnClose_clicked();
  }
}

void dlgSetEditText::on_btnDel_clicked() {
  if (ui->lineEdit->text().length() > 0)
    mw_one->m_Notes->byTextEdit->textCursor().removeSelectedText();
  on_btnClose_clicked();
}

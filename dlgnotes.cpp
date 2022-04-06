#include "dlgnotes.h"

#include "mainwindow.h"
#include "ui_dlgnotes.h"
#include "ui_mainwindow.h"
extern MainWindow* mw_one;
extern bool loading;
extern QString noteText;
extern int curPos;
extern int sliderPos;

dlgNotes::dlgNotes(QWidget* parent) : QDialog(parent), ui(new Ui::dlgNotes) {
  ui->setupUi(this);
  this->installEventFilter(this);
  //鼠标不可选中文本
  ui->textBrowser->setTextInteractionFlags(Qt::NoTextInteraction);
  QScroller::grabGesture(ui->textEdit, QScroller::LeftMouseButtonGesture);
  QScroller::grabGesture(ui->textBrowser, QScroller::LeftMouseButtonGesture);
}

dlgNotes::~dlgNotes() { delete ui; }

void dlgNotes::on_btnBack_clicked() {
  noteText = ui->textEdit->toPlainText();
  curPos = ui->textEdit->textCursor().position();
  sliderPos = ui->textEdit->verticalScrollBar()->sliderPosition();
  QString text = QString::number(curPos) + "|" + QString::number(sliderPos) +
                 "|" + noteText;
  mw_one->ui->tabWidget->setTabToolTip(mw_one->ui->tabWidget->currentIndex(),
                                       text);
  if (!ui->textEdit->isHidden()) mw_one->startSave("notes");
  ui->textEdit->clear();
  close();
}

void dlgNotes::on_textEdit_textChanged() {}

bool dlgNotes::eventFilter(QObject* obj, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(obj, evn);
}

void dlgNotes::keyReleaseEvent(QKeyEvent* event) { Q_UNUSED(event); }

void dlgNotes::resizeEvent(QResizeEvent* event) {
  Q_UNUSED(event);
  qDebug() << "resize" << ui->textEdit->height();
}

void dlgNotes::init_Notes() {
  int index = mw_one->ui->tabWidget->currentIndex();
  QString str = mw_one->ui->tabWidget->tabToolTip(index);
  QStringList list = str.split("|");
  if (list.count() > 2) {
    ui->textEdit->setPlainText(list.at(2));
    QTextCursor tmpCursor = ui->textEdit->textCursor();
    QString curPos = list.at(0);
    tmpCursor.setPosition(curPos.toInt());
    ui->textEdit->setTextCursor(tmpCursor);
    sliderPos = list.at(1).toInt();
    ui->textEdit->verticalScrollBar()->setSliderPosition(sliderPos);

  } else
    ui->textEdit->setPlainText(str);
}

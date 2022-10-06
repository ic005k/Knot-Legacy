#include "dlgsetedittext.h"

#include "src/mainwindow.h"
#include "ui_dlgsetedittext.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;

dlgSetEditText::dlgSetEditText(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgSetEditText) {
  ui->setupUi(this);
  ui->lineEdit->setReadOnly(true);
  ui->lineEdit->setText("");
}

dlgSetEditText::~dlgSetEditText() { delete ui; }

void dlgSetEditText::on_btnClose_clicked() {
  ui->lineEdit->clear();
  mw_one->mydlgMainNotes->ui->editSource->textCursor().clearSelection();
  close();
}

void dlgSetEditText::init(int y) {
  setGeometry(mw_one->geometry().x() + (mw_one->width() - this->width()) / 2, y,
              this->width(), this->height());
  setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
  show();
}

bool dlgSetEditText::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      close();
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgSetEditText::on_btnCopy_clicked() {
  mw_one->mydlgMainNotes->ui->editSource->copy();

  ui->lineEdit->clear();
  mw_one->mydlgMainNotes->ui->editSource->textCursor().clearSelection();
  close();
}

void dlgSetEditText::on_btnCut_clicked() {
  mw_one->mydlgMainNotes->ui->editSource->cut();
  close();
}

void dlgSetEditText::on_btnPaste_clicked() {
  mw_one->mydlgMainNotes->ui->editSource->paste();
  close();
}

void dlgSetEditText::on_btnSetAll_clicked() {
  mw_one->mydlgMainNotes->ui->editSource->selectAll();
}

void dlgSetEditText::on_btnLeft1_clicked() {
  mw_one->mydlgMainNotes->start--;
  mw_one->mydlgMainNotes->selectText(mw_one->mydlgMainNotes->start,
                                     mw_one->mydlgMainNotes->end);
  ui->lineEdit->setText(
      mw_one->mydlgMainNotes->ui->editSource->textCursor().selectedText());
}

void dlgSetEditText::on_btnLeft0_clicked() {
  mw_one->mydlgMainNotes->start++;
  mw_one->mydlgMainNotes->selectText(mw_one->mydlgMainNotes->start,
                                     mw_one->mydlgMainNotes->end);
  ui->lineEdit->setText(
      mw_one->mydlgMainNotes->ui->editSource->textCursor().selectedText());
}

void dlgSetEditText::on_btnRight1_clicked() {
  mw_one->mydlgMainNotes->end++;
  mw_one->mydlgMainNotes->selectText(mw_one->mydlgMainNotes->start,
                                     mw_one->mydlgMainNotes->end);
  ui->lineEdit->setText(
      mw_one->mydlgMainNotes->ui->editSource->textCursor().selectedText());
}

void dlgSetEditText::on_btnRight0_clicked() {
  mw_one->mydlgMainNotes->end--;
  mw_one->mydlgMainNotes->selectText(mw_one->mydlgMainNotes->start,
                                     mw_one->mydlgMainNotes->end);
  ui->lineEdit->setText(
      mw_one->mydlgMainNotes->ui->editSource->textCursor().selectedText());
}

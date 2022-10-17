#include "src/RemarksAbout.h"

#include "mainwindow.h"
#include "ui_RemarksAbout.h"
#include "ui_mainwindow.h"
extern MainWindow* mw_one;
extern bool loading, zh_cn;
extern QString noteText;
extern int curPos;

dlgRemarks::dlgRemarks(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgRemarks) {
  ui->setupUi(this);

  setModal(true);
  this->installEventFilter(this);
  ui->textEdit->installEventFilter(this);
  ui->textEdit->viewport()->installEventFilter(this);

  QScroller::grabGesture(ui->textEdit, QScroller::LeftMouseButtonGesture);
  mw_one->setSCrollPro(ui->textEdit);

  ui->lblAbout->adjustSize();
  ui->lblAbout->setWordWrap(true);
  ui->lblLogo->adjustSize();
  ui->lblLogo->setText("");
  ui->lblLogo->setFixedHeight(185);
  ui->lblLogo->setFixedWidth(185);
  ui->lblLogo->setStyleSheet(
      "QLabel{"
      "border-image:url(:/res/apk.png) 4 4 4 4 stretch stretch;"
      "}");
}

dlgRemarks::~dlgRemarks() { delete ui; }

void dlgRemarks::on_btnBack_clicked() {
  mw_one->mydlgMainNotes->m_SetEditText->close();

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
  mw_one->closeGrayWindows();
}

void dlgRemarks::on_textEdit_textChanged() {}

bool dlgRemarks::eventFilter(QObject* obj, QEvent* evn) {
  if (obj == ui->textEdit->viewport()) {
    mw_one->mydlgMainNotes->getEditPanel(ui->textEdit, evn);
  }

  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(obj, evn);
}

void dlgRemarks::keyReleaseEvent(QKeyEvent* event) { Q_UNUSED(event); }

void dlgRemarks::resizeEvent(QResizeEvent* event) {
  Q_UNUSED(event);
  qDebug() << "resize" << ui->textEdit->height();
}

void dlgRemarks::init_Notes() {
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

void dlgRemarks::on_btnDL_clicked() {
  QString str;
  str =
      "https://github.com/ic005k/Knot/releases/download/2022/"
      "android-build-release-signed.apk";
  QUrl url(str);
  QDesktopServices::openUrl(url);
}

void dlgRemarks::on_btnHomePage_clicked() {
  QString str;
  str = "https://github.com/ic005k/Knot/issues";
  QUrl url(str);
  QDesktopServices::openUrl(url);
}

void dlgRemarks::on_btnMirrorDL_clicked() {
  QString str =
      "https://ghproxy.com/https://github.com/ic005k/Knot/releases/download/"
      "2022/android-build-release-signed.apk";
  QUrl url(str);
  QDesktopServices::openUrl(url);
}

void dlgRemarks::on_btnPaste_clicked() { ui->textEdit->paste(); }

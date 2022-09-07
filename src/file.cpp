#include "file.h"

#include <QClipboard>
#include <QDebug>
#include <QFile>

#include "mainwindow.h"
#include "src/onedrive/qtonedrive.h"
#include "src/onedrive/qtonedriveauthorizationdialog.h"
#include "ui_dlgOneDrive.h"
#include "ui_mainwindow.h"

extern MainWindow *mw_one;
extern QtOneDriveAuthorizationDialog *dialog_;

File::File() { connect(this, SIGNAL(sourceChanged()), this, SLOT(readFile())); }

void File::setSource(const QString &source) {
  m_source = source;
  emit sourceChanged();

  mw_one->strText = m_source;
}

QString File::source() const { return m_source; }

void File::setText(const QString &text) {
  QFile file(m_source);
  if (!file.open(QIODevice::WriteOnly)) {
    m_text = "";
    qDebug() << "Error:" << m_source << "open failed!";
  } else {
    qint64 byte = file.write(text.toUtf8());
    if (byte != text.toUtf8().size()) {
      m_text = text.toUtf8().left(byte);
      qDebug() << "Error:" << m_source << "open failed!";
    } else {
      m_text = text;
    }

    file.close();
  }

  emit textChanged();
}

void File::readFile() {
  QFile file(m_source);
  if (file.exists()) {
    if (!file.open(QIODevice::ReadOnly)) {
      m_text = "";
      qDebug() << "Error:" << m_source << "open failed!";
    } else
      m_text = file.readAll();
  }

  emit textChanged();
}

void File::setStr(QString str) { m_text = str; }

QString File::text() const { return m_text; }

qreal File::textPos() {
  if (!mw_one->ui->frameReader->isHidden())
    mw_one->mydlgReader->textPos = m_textPos;

  if (!mw_one->ui->frameMemo->isHidden())
    mw_one->mydlgMainNotes->sliderPos = m_textPos;

  qDebug() << "m_textPos" << m_textPos;

  return m_textPos;
}

qreal File::curX() {
  mw_one->curx = m_curX;
  qDebug() << "m_curX" << m_curX;
  return m_curX;
}

void File::setTextPos(qreal &textPos) { m_textPos = textPos; }

void File::setCurX(qreal &curX) { m_curX = curX; }

QString File::webEnd() { return m_strUri; }

void File::setWebEnd(QString &strUri) {
  m_strUri = strUri;
  mw_one->strRefreshUrl = m_strUri;

  if (m_strUri.contains("?code=")) dialog_->sendMsg(m_strUri);

  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(m_strUri);

  qDebug() << "web end uri = " << m_strUri;
}

QString File::prog() const { return m_prog; }

void File::setProg(const QString &prog) {
  m_prog = prog;
  mw_one->ui->progressBar->setValue(m_prog.toInt());
}

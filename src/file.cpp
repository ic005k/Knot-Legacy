#include "file.h"

#include <QDebug>
#include <QFile>

#include "mainwindow.h"

extern MainWindow *mw_one;
File::File() { connect(this, SIGNAL(sourceChanged()), this, SLOT(readFile())); }

void File::setSource(const QString &source) {
  m_source = source;
  emit sourceChanged();
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
  mw_one->mydlgReader->textPos = m_textPos;
  qDebug() << "m_textPos" << m_textPos;
  return m_textPos;
}

qreal File::curX() {
  mw_one->curx = m_curX;
  qDebug() << "m_curX" << m_curX;
  return m_curX;
}

void File::setTextPos(qreal &textPos) { m_textPos = textPos; }

void File::setcurX(qreal &curX) { m_curX = curX; }

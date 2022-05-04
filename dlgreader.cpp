#include "dlgreader.h"

#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_dlgreader.h"

extern MainWindow* mw_one;
extern QString iniFile, iniDir;
extern bool isImport;
extern int fontSize;

dlgReader::dlgReader(QWidget* parent) : QDialog(parent), ui(new Ui::dlgReader) {
  ui->setupUi(this);
  this->installEventFilter(this);
  ui->textBrowser->installEventFilter(this);
  this->setContentsMargins(0, 0, 0, 0);
  this->layout()->setContentsMargins(0, 0, 0, 0);
  this->layout()->setMargin(0);
  myedit = new QPlainTextEdit;
  QScroller::grabGesture(ui->textBrowser, QScroller::LeftMouseButtonGesture);
  ui->textBrowser->verticalScrollBar()->setStyleSheet("width:0px;");
  // ui->textBrowser->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  ui->textBrowser->setTextInteractionFlags(Qt::NoTextInteraction);
  mw_one->setSCrollPro(ui->textBrowser);
  ui->textBrowser->verticalScrollBar()->setVisible(false);
  ui->textBrowser->setStyleSheet(
      "background-image: url(:/src/b.png);border-width:0;border-style:outset;");
  this->setStyleSheet(
      "background-image: url(:/src/b.png);border-width:0;border-style:outset;");

  connect(ui->textBrowser->verticalScrollBar(), &QScrollBar::valueChanged, this,
          &dlgReader::getPages);

  ui->lblTitle->hide();
  ui->btnBack->hide();
  ui->frame->hide();
  ui->progressBar->hide();
  ui->progressBar->setMaximumHeight(4);
  ui->progressBar->setStyleSheet(
      "QProgressBar{border:0px solid #FFFFFF;"
      "height:30;"
      "background:rgba(25,255,25,0);"
      "text-align:right;"
      "color:rgb(255,255,255);"
      "border-radius:0px;}"

      "QProgressBar:chunk{"
      "border-radius:0px;"
      "background-color:rgba(25,25,255,250);"
      "}");
  ui->hSlider->setStyleSheet(
      "QSlider::groove:horizontal{ \
                                    height: 12px; \
                                    left: 5px; \
                                    right: 5px; \
                                    border-image: url(:/src/qslider_bg.png);\
                                    } \
                                    QSlider::handle:horizontal{ \
                                    border-radius: 1px; \
                                    width:  25px; \
                                    height: 35px; \
                                    margin-top: -5px; \
                                    margin-left: -0px; \
                                    margin-bottom: -5px; \
                                    margin-right: -0px; \
                                    border-image:url(:/src/qslider_btn.png);} \
                                    QSlider::sub-page:horizontal{border-image: url(:/src/qslider.png);}");

  ui->btnFontLess->setStyleSheet("border:none");
  ui->btnFontPlus->setStyleSheet("border:none");
  ui->btnOpen->setStyleSheet("border:none");
  ui->btnPage->setStyleSheet("border:none");
  ui->btnBack->setStyleSheet("border:none");
  ui->btnPageNext->setStyleSheet("border:none");
  ui->btnPageUp->setStyleSheet("border:none");
  ui->btnLines->setStyleSheet("border:none");
  QFont f;
  f.setPointSize(11);
  f.setBold(true);
  ui->btnPage->setFont(f);
  ui->btnLines->setFont(f);
}

dlgReader::~dlgReader() { delete ui; }

bool dlgReader::eventFilter(QObject* obj, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  QMouseEvent* event = static_cast<QMouseEvent*>(evn);  //将之转换为鼠标事件
  if (obj == ui->textBrowser) {
    static int press_x;
    static int press_y;
    static int relea_x;
    static int relea_y;

    if (event->type() == QEvent::MouseButtonPress) {
      press_x = event->globalX();
      press_y = event->globalY();
      x = 0;
      y = 0;
      w = ui->textBrowser->width();
      h = ui->textBrowser->height();
      qDebug() << "Press:" << press_x << press_y;
    }

    if (event->type() == QEvent::MouseButtonRelease) {
      relea_x = event->globalX();
      relea_y = event->globalY();
      qDebug() << "Release:" << relea_x << relea_y;
    }

    int abc = 500;
    //判断滑动方向（右滑）
    if ((relea_x - press_x) > 30 &&
        event->type() == QEvent::MouseButtonRelease &&
        qAbs(relea_y - press_y) < 50) {
      if (iPage - baseLines <= 0) return QWidget::eventFilter(obj, evn);
      ui->lblTitle->setPixmap(ui->textBrowser->grab());
      //捕获当前界面并绘制到label上
      QPropertyAnimation* animation1 =
          new QPropertyAnimation(ui->lblTitle, "geometry");
      // new QPropertyAnimation(ui->textBrowser, "geometry");
      animation1->setDuration(abc);
      animation1->setStartValue(QRect(x, y, w, h));
      animation1->setEndValue(QRect(w * 1, y, w, h));

      on_btnPageUp_clicked();

      QPropertyAnimation* animation2 =
          new QPropertyAnimation(ui->textBrowser, "geometry");
      animation2->setDuration(abc);
      animation2->setStartValue(QRect(-w * 2, y, w, h));
      animation2->setEndValue(QRect(x, y, w, h));

      QParallelAnimationGroup* group = new QParallelAnimationGroup;  //动画容器
      group->addAnimation(animation1);
      group->addAnimation(animation2);
      group->start();
    }

    //判断滑动方向（左滑）
    if ((press_x - relea_x) > 30 &&
        event->type() == QEvent::MouseButtonRelease &&
        qAbs(relea_y - press_y) < 50) {
      if (iPage + baseLines > totallines) return QWidget::eventFilter(obj, evn);
      ui->lblTitle->setPixmap(ui->textBrowser->grab());

      QPropertyAnimation* animation1 =
          new QPropertyAnimation(ui->lblTitle, "geometry");
      // new QPropertyAnimation(ui->textBrowser, "geometry");
      animation1->setDuration(abc);
      animation1->setStartValue(QRect(x, y, w, h));
      animation1->setEndValue(QRect(-w, y, w, h));

      on_btnPageNext_clicked();

      QPropertyAnimation* animation2 =
          new QPropertyAnimation(ui->textBrowser, "geometry");
      animation2->setDuration(abc);
      animation2->setStartValue(QRect(w * 2, y, w, h));
      animation2->setEndValue(QRect(x, y, w, h));

      QParallelAnimationGroup* group = new QParallelAnimationGroup;
      group->addAnimation(animation1);
      group->addAnimation(animation2);
      group->start();
    }
  }

  return QWidget::eventFilter(obj, evn);
}

void dlgReader::keyReleaseEvent(QKeyEvent* event) { Q_UNUSED(event); }

void dlgReader::on_btnBack_clicked() {
  saveReader();
  close();
}

void dlgReader::on_btnOpen_clicked() {
  fileName =
      QFileDialog::getOpenFileName(this, tr("Knot"), "", tr("Txt Files (*.*)"));
  openFile(fileName);
}

void dlgReader::openFile(QString fileName) {
  isOpen = false;
  if (QFile(fileName).exists()) {
    ui->textBrowser->clear();
    iPage = 0;
    sPos = 0;
    ui->hSlider->setValue(0);
    ui->frame->hide();

    readTextList.clear();
    readTextList = readText(fileName);
    totallines = readTextList.count();

    isLines = true;
    getLines();
    isLines = false;

    getPages();
    isOpen = true;
  }
}

QString dlgReader::getTextEditLineText(QPlainTextEdit* txtEdit, int i) {
  QTextBlock block = txtEdit->document()->findBlockByNumber(i);
  txtEdit->setTextCursor(QTextCursor(block));
  QString lineText = txtEdit->document()->findBlockByNumber(i).text().trimmed();
  return lineText;
}

void dlgReader::saveReader() {
  QSettings Reg(iniDir + "reader.ini", QSettings::IniFormat);
  vpos = ui->textBrowser->verticalScrollBar()->sliderPosition();
  Reg.setValue("/Reader/SliderPos", vpos);
  Reg.setValue("/Reader/FileName", fileName);
  Reg.setValue("/Reader/FontSize", ui->textBrowser->font().pointSize());
  Reg.setValue("/Reader/iPage", iPage);
}

void dlgReader::initReader() {
  QSettings Reg(iniDir + "reader.ini", QSettings::IniFormat);
  QFont font;
  font.setPointSize(Reg.value("/Reader/FontSize", fontSize).toInt());
  font.setLetterSpacing(QFont::AbsoluteSpacing, 2);
  ui->textBrowser->setFont(font);
  fileName = Reg.value("/Reader/FileName").toString();
  openFile(fileName);
  vpos = Reg.value("/Reader/SliderPos").toULongLong();
}

void dlgReader::on_btnFontPlus_clicked() {
  QFont font;
  int size = ui->textBrowser->font().pointSize();
  size = size + 1;
  font.setPointSize(size);
  font.setLetterSpacing(QFont::AbsoluteSpacing, 2);
  ui->textBrowser->setFont(font);
  saveReader();
}

void dlgReader::on_btnFontLess_clicked() {
  QFont font;
  int size = ui->textBrowser->font().pointSize();
  size = size - 1;
  font.setPointSize(size);
  font.setLetterSpacing(QFont::AbsoluteSpacing, 2);
  ui->textBrowser->setFont(font);
  saveReader();
}

void dlgReader::drawB() {
  ui->textBrowser->setStyleSheet(
      "background:transparent;border-width:0;border-style:outset;");
  ui->textBrowser->setAutoFillBackground(true);
  QPainter* p = new QPainter(this);
  QImage img(":/src/b.png");
  QRect re(
      QPoint(ui->textBrowser->geometry().x(), ui->textBrowser->geometry().y()),
      ui->textBrowser->size());
  p->drawImage(re, img);
}

void dlgReader::on_textBrowser_textChanged() {
  // QTextBlockFormat bf = ui->textBrowser->textCursor().blockFormat();
  // bf.setLineHeight(20, QTextBlockFormat::LineDistanceHeight);
  // ui->textBrowser->textCursor().setBlockFormat(bf);
  //设置文字行间距
}

void dlgReader::getPages() {
  if (isPages) {
    ui->hSlider->setTickInterval(1);
    int baseh = ui->textBrowser->height();
    qulonglong th = ui->textBrowser->verticalScrollBar()->maximum();
    int page = th / baseh;
    qulonglong cpos = ui->textBrowser->verticalScrollBar()->sliderPosition();
    int cp = cpos / baseh;
    ui->btnPage->setText(QString::number(cp) + " / " + QString::number(page));
    ui->hSlider->setMaximum(page);
    ui->hSlider->setMinimum(0);
    // qDebug() << "th:" << th << "cpos:" << cpos;
  }
}

void dlgReader::getLines() {
  if (isLines) {
    ui->hSlider->setTickInterval(1);
    ui->hSlider->setMinimum(0);
    ui->hSlider->setMaximum(totallines / baseLines - 1);
    ui->hSlider->setValue(sPos);
    ui->btnLines->setText(tr("Pages") + "\n" +
                          QString::number(ui->hSlider->value() + 1) + " / " +
                          QString::number(totallines / baseLines));
    iPage = ui->hSlider->value() * baseLines;
    qDebug() << "iPage" << iPage << ui->hSlider->value();

    int count = iPage + baseLines;
    QString txt1;
    for (int i = iPage; i < count; i++) {
      iPage++;
      // txt1 = txt1 + getTextEditLineText(myedit, i) + "\n" + strSpace;
      txt1 = txt1 + readTextList.at(i) + "\n" + strSpace;
    }

    QString qsShow =
        "<p style='line-height:28px; width:100% ; white-space: pre-wrap; '>" +
        txt1 + "</p>";
    ui->textBrowser->setHtml(qsShow);
    ui->textBrowser->verticalScrollBar()->setSliderPosition(0);
  }
}

void dlgReader::on_btnPage_clicked() {
  isPages = true;
  isLines = false;
  if (ui->frame->isHidden()) {
    int h0 = ui->textBrowser->height();
    ui->hSlider->setValue(ui->textBrowser->verticalScrollBar()->value() / h0);
    ui->hSlider->setMaximum(ui->textBrowser->verticalScrollBar()->maximum() /
                            h0);
    ui->frame->show();
  } else
    ui->frame->hide();
}

void dlgReader::on_hSlider_sliderMoved(int position) {
  if (isPages) {
    ui->textBrowser->verticalScrollBar()->setSliderPosition(
        position * ui->textBrowser->height());
  }
  if (isLines) {
    int max = totallines / baseLines;
    if (position >= max) position = max;
    sPos = position;
    getLines();
  }
}

void dlgReader::on_btnPageUp_clicked() {
  int count = iPage - baseLines;
  if (count <= 0) return;
  QString txt1;
  ui->textBrowser->clear();

  for (int i = count - baseLines; i < count; i++) {
    iPage--;
    // txt1 = txt1 + getTextEditLineText(myedit, i) + "\n" + strSpace;
    txt1 = txt1 + readTextList.at(i) + "\n" + strSpace;
  }

  QString qsShow =
      "<p style='line-height:28px; width:100% ; white-space: pre-wrap; '>" +
      txt1 + "</p>";
  ui->textBrowser->setHtml(qsShow);
  ui->textBrowser->verticalScrollBar()->setSliderPosition(0);

  ui->hSlider->setMaximum(totallines / baseLines);
  ui->btnLines->setText(tr("Pages") + "\n" +
                        QString::number(iPage / baseLines) + " / " +
                        QString::number(totallines / baseLines));

  getPages();
}

void dlgReader::on_btnPageNext_clicked() {
  int count = iPage + baseLines;
  if (count > totallines) return;
  QString txt1;
  ui->textBrowser->clear();

  for (int i = iPage; i < count; i++) {
    iPage++;
    // txt1 = txt1 + getTextEditLineText(myedit, i) + "\n" + strSpace;
    txt1 = txt1 + readTextList.at(i) + "\n" + strSpace;
  }

  QString qsShow =
      "<p style='line-height:28px; width:100% ; white-space: pre-wrap; '>" +
      txt1 + "</p>";
  ui->textBrowser->setHtml(qsShow);

  ui->textBrowser->verticalScrollBar()->setSliderPosition(0);

  ui->hSlider->setMaximum(totallines / baseLines);
  ui->btnLines->setText(tr("Pages") + "\n" +
                        QString::number(iPage / baseLines) + " / " +
                        QString::number(totallines / baseLines));

  getPages();
}

void dlgReader::on_btnLines_clicked() {
  ui->hSlider->setMaximum(totallines / baseLines - 1);
  ui->hSlider->setTickInterval(1);
  ui->hSlider->setValue(iPage / baseLines);
  if (ui->frame->isHidden()) {
    isLines = true;
    isPages = false;
    ui->frame->show();
  } else {
    isLines = false;
    isPages = true;
    ui->frame->hide();
  }
}

QStringList dlgReader::readText(QString textFile) {
  QStringList list;
  if (QFile(textFile).exists()) {
    QFile file(textFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
      qDebug() << tr("Cannot read file %1:\n%2.")
                      .arg(QDir::toNativeSeparators(textFile),
                           file.errorString());

    } else {
      QTextStream in(&file);
      in.setCodec("UTF-8");
      QString text = in.readAll();
      list = text.split("\n");
    }
    file.close();
  }

  return list;
}

void dlgReader::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event);
  saveReader();
}

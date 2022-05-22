#include "dlgreader.h"

#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_dlgreader.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;
extern QString iniFile, iniDir;
extern bool isImport, zh_cn;
extern int fontSize;

dlgReader::dlgReader(QWidget* parent) : QDialog(parent), ui(new Ui::dlgReader) {
  ui->setupUi(this);
  ui->textBrowser->hide();

  qmlRegisterType<File>("MyModel", 1, 0, "File");

  this->installEventFilter(this);
  ui->textBrowser->installEventFilter(this);
  this->setContentsMargins(0, 0, 0, 0);
  this->layout()->setContentsMargins(0, 0, 0, 0);
  this->layout()->setMargin(0);

  // SmoothScrollBar* vScrollBar = new SmoothScrollBar();
  // ui->textBrowser->setVerticalScrollBar(vScrollBar);
  QScroller::grabGesture(ui->textBrowser, QScroller::LeftMouseButtonGesture);
  mw_one->setSCrollPro(ui->textBrowser);
  ui->textBrowser->verticalScrollBar()->setStyleSheet("width:0px;");
  ui->textBrowser->setTextInteractionFlags(Qt::NoTextInteraction);

  ui->textBrowser->setStyleSheet(
      "background-image: url(:/src/b.png);border-width:0;border-style:outset;");
  this->setStyleSheet(
      "background-image: url(:/src/b.png);border-width:0;border-style:outset;");

  connect(ui->textBrowser->verticalScrollBar(), &QScrollBar::valueChanged, this,
          &dlgReader::getPages);

  mw_one->ui->lblTitle->hide();
  mw_one->ui->frameFun->hide();
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

  mw_one->ui->hSlider->setStyleSheet(ui->hSlider->styleSheet());
  mw_one->ui->btnFontLess->setStyleSheet("border:none");
  mw_one->ui->btnFontPlus->setStyleSheet("border:none");
  mw_one->ui->btnOpen->setStyleSheet("border:none");
  mw_one->ui->btnPage->setStyleSheet("border:none");
  mw_one->ui->btnBack->setStyleSheet("border:none");
  mw_one->ui->btnPageNext->setStyleSheet("border:none");
  mw_one->ui->btnPageUp->setStyleSheet("border:none");
  mw_one->ui->btnLines->setStyleSheet("border:none");
  QFont f;
  f.setPointSize(11);
  f.setBold(true);
  mw_one->ui->btnLines->setFont(f);
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
      // qDebug() << "Press:" << press_x << press_y;
    }

    if (event->type() == QEvent::MouseButtonRelease) {
      relea_x = event->globalX();
      relea_y = event->globalY();
      // qDebug() << "Release:" << relea_x << relea_y;
    }

    int abc = 300;
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
      animation2->setStartValue(QRect(-w * 1, y, w, h));
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
      animation2->setStartValue(QRect(w * 1, y, w, h));
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
  close();
  saveReader();
}

void dlgReader::on_btnOpen_clicked() {
  QString openfile =
      QFileDialog::getOpenFileName(this, tr("Knot"), "", tr("Txt Files (*.*)"));

  if (openfile.mid(openfile.length() - 4, 4) == "epub") {
    isEpub = true;
    QString dirpath = iniDir + "temp/";
    QProcess* pro = new QProcess;
    pro->execute("unzip", QStringList() << "-o" << openfile << "-d" << dirpath);
    pro->waitForFinished();

    QDir dir(dirpath);
    QStringList nameFilters;
    nameFilters << "*.html";
    QStringList filesTemp =
        dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
    htmlFiles.clear();
    for (int j = 0; j < filesTemp.count(); j++) {
      if (filesTemp.at(j).mid(0, 1) != ".") htmlFiles.append(filesTemp.at(j));
    }
    qDebug() << htmlFiles;
    QString str = mw_one->loadText(dirpath + htmlFiles.at(0));
    qDebug() << str;
    setQML(str);

  } else {
    isEpub = false;
    openFile(openfile);
  }
}

void dlgReader::openFile(QString file) {
  isOpen = false;
  if (QFile(file).exists()) {
    if (!mw_one->ui->frameQML->isHidden()) saveReader();
    fileName = file;
    mw_one->ui->lblTitle->hide();

    iPage = 0;
    sPos = 0;
    ui->hSlider->setValue(0);
    ui->frameFun->hide();

    readTextList.clear();
    readTextList = readText(fileName);
    totallines = readTextList.count();

    isOpen = true;
    goPostion();
    setVPos();
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
  Reg.setIniCodec("utf-8");

  Reg.setValue("/Reader/FileName", fileName);
  Reg.setValue("/Reader/vpos" + fileName, textPos);
  int page = iPage;
  Reg.setValue("/Reader/iPage" + fileName, page - baseLines);
  Reg.setValue("/Reader/FontSize", mw_one->textFontSize);

  qDebug() << "textPos" << textPos;
}

void dlgReader::initReader() {
  QSettings Reg(iniDir + "reader.ini", QSettings::IniFormat);
  Reg.setIniCodec("utf-8");

  QFont font;
  int fsize = Reg.value("/Reader/FontSize", 18).toInt();
  mw_one->textFontSize = fsize;
  mw_one->ui->quickWidget->rootContext()->setContextProperty("FontSize", fsize);
  font.setPointSize(fsize);
  font.setLetterSpacing(QFont::AbsoluteSpacing, 2);  //字间距

  fontname = Reg.value("/Reader/FontName", mw_one->font().family()).toString();
  mw_one->ui->quickWidget->rootContext()->setContextProperty("FontName",
                                                             fontname);

  fileName = Reg.value("/Reader/FileName").toString();
  if (fileName == "" && zh_cn) fileName = ":/src/test.txt";

  openFile(fileName);
  mw_one->ui->quickWidget->setSource(QUrl(QStringLiteral("qrc:/text.qml")));
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

void dlgReader::on_textBrowser_textChanged() {}

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
    mw_one->ui->hSlider->setTickInterval(1);
    mw_one->ui->hSlider->setMinimum(0);
    mw_one->ui->hSlider->setMaximum(totallines / baseLines - 1);
    mw_one->ui->hSlider->setValue(sPos);
    mw_one->ui->btnLines->setText(
        tr("Pages") + "\n" + QString::number(mw_one->ui->hSlider->value() + 1) +
        " / " + QString::number(totallines / baseLines));
    iPage = mw_one->ui->hSlider->value() * baseLines;
    qDebug() << "iPage" << iPage << mw_one->ui->hSlider->value();

    int count = iPage + baseLines;
    QString txt1;
    for (int i = iPage; i < count; i++) {
      iPage++;
      QString str = readTextList.at(i);
      if (str.trimmed() != "")
        txt1 = txt1 + readTextList.at(i).trimmed() + "\n" + strSpace;
    }

    QString qsShow =
        "<p style='line-height:32px; width:100% ; white-space: pre-wrap; '>" +
        txt1 + "</p>";

    setQML(qsShow);
  }
}

void dlgReader::setQML(QString txt1) {
  // strPage = "<body style=\"line-height:137.5% ;\">" + txt1;

  // mw_one->ui->quickWidget->setSource(QUrl(QStringLiteral("qrc:/text.qml")));
  mw_one->ui->quickWidget->rootContext()->setContextProperty("strText", txt1);
}

void dlgReader::on_btnPage_clicked() {
  QStringList listFonts;
  QFontDatabase fontDatebase;
  foreach (QString family, fontDatebase.families()) {
    listFonts.append(family);
  }

  QListWidget* list = new QListWidget(mw_one);
  mw_one->listSelFont = list;
  list->setStyleSheet(mw_one->listStyle);
  list->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  list->setVerticalScrollMode(QListWidget::ScrollPerPixel);
  QScroller::grabGesture(list, QScroller::LeftMouseButtonGesture);
  mw_one->setSCrollPro(list);
  QFont font0;
  font0.setPointSize(fontSize);
  list->setFont(font0);

  for (int i = 0; i < listFonts.count(); i++) {
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(QSize(130, 30));  // item->sizeHint().width()
    item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    item->setText(listFonts.at(i));
    list->addItem(item);
  }

  connect(list, &QListWidget::itemClicked, [=]() {
    fontname = list->currentItem()->text();
    mw_one->ui->quickWidget->rootContext()->setContextProperty("FontName",
                                                               fontname);
    QSettings Reg(iniDir + "reader.ini", QSettings::IniFormat);
    Reg.setIniCodec("utf-8");
    Reg.setValue("/Reader/FontName", fontname);
    list->close();
  });

  list->setGeometry(0, 0, mw_one->width(), mw_one->height());
  if (list->count() > 0) {
    list->setCurrentRow(0);
    for (int i = 0; i < list->count(); i++) {
      if (list->item(i)->text() == fontname) {
        list->setCurrentRow(i);
        break;
      }
    }
  }
  list->show();
  list->setFocus();

  return;
  bool ok;
  QFontDialog fd;
  QFont font = get_Font();
  font = fd.getFont(&ok, font);
  if (ok) {
    QSettings Reg(iniDir + "reader.ini", QSettings::IniFormat);
    Reg.setIniCodec("utf-8");
    Reg.setValue("/Reader/FontName", font.family());
    mw_one->ui->quickWidget->rootContext()->setContextProperty("FontName",
                                                               font.family());
  }
}

QFont dlgReader::get_Font() {
  QFont font;

  QSettings Reg(iniDir + "reader.ini", QSettings::IniFormat);
  Reg.setIniCodec("utf-8");
  font.setFamily(Reg.value("/Reader/FontName", "Menlo").toString());

  font.setPixelSize(Reg.value("/Reader/FontSize", 12).toInt());
  font.setBold(Reg.value("/Reader/FontBold").toBool());
  font.setItalic(Reg.value("/Reader/FontItalic").toBool());
  font.setUnderline(Reg.value("/Reader/FontUnderline").toBool());

  return font;
}

void dlgReader::on_hSlider_sliderMoved(int position) {
  mw_one->ui->lblTitle->hide();
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
  mw_one->ui->lblTitle->hide();

  int count = iPage - baseLines;
  if (count <= 0) return;
  textPos = 0;
  QString txt1;
  ui->textBrowser->clear();

  for (int i = count - baseLines; i < count; i++) {
    iPage--;
    QString str = readTextList.at(i);
    if (str.trimmed() != "") txt1 = txt1 + readTextList.at(i) + "\n" + strSpace;
  }

  QString qsShow =
      "<p style='line-height:32px; width:100% ; white-space: pre-wrap; '>" +
      txt1 + "</p>";

  mw_one->ui->hSlider->setMaximum(totallines / baseLines);
  mw_one->ui->btnLines->setText(tr("Pages") + "\n" +
                                QString::number(iPage / baseLines) + " / " +
                                QString::number(totallines / baseLines));
  mw_one->ui->progReader->setMaximum(totallines / baseLines);
  mw_one->ui->progReader->setValue(iPage / baseLines);

  mw_one->ui->quickWidget->rootContext()->setContextProperty("textPos", 0);
  setQML(qsShow);
}

void dlgReader::on_btnPageNext_clicked() {
  mw_one->ui->lblTitle->hide();
  QString qsShow;
  if (!isEpub) {
    int count = iPage + baseLines;
    if (count > totallines) return;
    textPos = 0;
    QString txt1;

    for (int i = iPage; i < count; i++) {
      iPage++;
      QString str = readTextList.at(i);
      if (str.trimmed() != "")
        txt1 = txt1 + readTextList.at(i) + "\n" + strSpace;
    }

    qsShow =
        "<p style='line-height:32px; width:100% ; white-space: pre-wrap; '>" +
        txt1 + "</p>";

    mw_one->ui->hSlider->setMaximum(totallines / baseLines);
    mw_one->ui->btnLines->setText(tr("Pages") + "\n" +
                                  QString::number(iPage / baseLines) + " / " +
                                  QString::number(totallines / baseLines));
    mw_one->ui->progReader->setMaximum(totallines / baseLines);
    mw_one->ui->progReader->setValue(iPage / baseLines);

    mw_one->ui->quickWidget->rootContext()->setContextProperty("textPos", 0);
  } else {
    htmlIndex++;
    if (htmlIndex == htmlFiles.count()) htmlIndex = htmlFiles.count() - 1;
    qsShow = mw_one->loadText(iniDir + "temp/" + htmlFiles.at(htmlIndex));
  }
  setQML(qsShow);
}

void dlgReader::on_btnLines_clicked() {
  mw_one->ui->lblTitle->hide();
  mw_one->ui->hSlider->setMaximum(totallines / baseLines - 1);
  mw_one->ui->hSlider->setTickInterval(1);
  mw_one->ui->hSlider->setValue(iPage / baseLines);
  if (mw_one->ui->frameFun->isHidden()) {
    isLines = true;
    isPages = false;
    mw_one->ui->frameFun->show();

  } else {
    isLines = false;
    isPages = true;
    mw_one->ui->frameFun->hide();
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

void dlgReader::paintEvent(QPaintEvent* event) { Q_UNUSED(event); }

void dlgReader::goPostion() {
  if (isOpen) {
    QSettings Reg(iniDir + "reader.ini", QSettings::IniFormat);
    Reg.setIniCodec("utf-8");

    iPage = Reg.value("/Reader/iPage" + fileName, 0).toULongLong();

    if (iPage >= 0) {
      on_btnPageNext_clicked();

    } else
      iPage = 0;
  }
}

void dlgReader::setVPos() {
  QSettings Reg(iniDir + "reader.ini", QSettings::IniFormat);
  Reg.setIniCodec("utf-8");
  qreal vpos = Reg.value("/Reader/vpos" + fileName, 0).toReal();
  qDebug() << "vpos=" << vpos;
  if (vpos > 10)
    mw_one->ui->quickWidget->rootContext()->setContextProperty("textPos", vpos);
}

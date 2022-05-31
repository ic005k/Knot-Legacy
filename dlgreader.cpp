#include "dlgreader.h"

#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_dlgreader.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;
extern QString iniFile, iniDir;
extern bool isImport, zh_cn, isAndroid, isIOS;
extern int fontSize;

dlgReader::dlgReader(QWidget* parent) : QDialog(parent), ui(new Ui::dlgReader) {
  ui->setupUi(this);
  ui->textBrowser->hide();

  myDocHandler = new DocumentHandler(this);
  qmlRegisterType<File>("MyModel1", 1, 0, "File");
  qmlRegisterType<DocumentHandler>("MyModel2", 1, 0, "DocumentHandler");

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

  openFile(openfile);
}

void dlgReader::openFile(QString openfile) {
  isOpen = false;
  if (QFile(openfile).exists()) {
    mw_one->ui->lblTitle->hide();
    mw_one->ui->frameFun->hide();
    mw_one->ui->lblBookName->setText("");
    mw_one->ui->lblBookName->setWordWrap(true);
    mw_one->ui->quickWidget->setSource(QUrl(QStringLiteral("qrc:/text.qml")));

    if (!mw_one->ui->frameQML->isHidden()) saveReader();

    if (QFile(openfile).exists()) {
      readTextList.clear();
      readTextList = readText(openfile);

    } else
      return;

    if (readTextList.count() <= 0) return;
    QString strHead = readTextList.at(0);

    // if (openfile.mid(openfile.length() - 4, 4) == "epub") {
    if (strHead.trimmed().mid(0, 2) == "PK") {
      QString dirpath, dirpathbak;
      // dirpath = "/storage/emulated/0/epubtemp/";
      dirpath = iniDir + "temp/";
      dirpathbak = iniDir + "tempbak/";

      QDir dir;
      dir.rename(dirpath, dirpathbak);

      // QFile::copy(":/src/unzip", iniDir + "unzip");

      QString temp = iniDir + "temp.zip";
      QFile::remove(temp);
      if (!QFile::copy(openfile, temp)) {
        QMessageBox box;
        box.setText(openfile + "\n!=\n" + temp);
        box.exec();
      } else {
        QMessageBox box;
        QProcess* pro = new QProcess;

        pro->start(iniDir + "unzip", QStringList()
                                         << "-o" << temp << "-d" << dirpath);
        QString str = pro->readAll();
        pro->waitForFinished(5000);

        box.setText(temp + "\n" + mw_one->getFileSize(QFile(temp).size(), 2) +
                    "\n" +
                    mw_one->getFileSize(QFile(iniDir + "unzip").size(), 2) +
                    "\n" + str);
        // box.exec();
      }

#ifdef Q_OS_MAC
      QProcess* pro = new QProcess;
      pro->execute("unzip", QStringList() << "-o" << temp << "-d" << dirpath);
      pro->waitForFinished();
#endif

#ifdef Q_OS_ANDROID
      QAndroidJniObject javaZipFile = QAndroidJniObject::fromString(temp);
      QAndroidJniObject javaZipDir = QAndroidJniObject::fromString(dirpath);
      QAndroidJniObject m_activity = QAndroidJniObject::fromString("Unzip");
      m_activity.callStaticMethod<void>(
          "com.x/MyActivity", "Unzip",
          "(Ljava/lang/String;Ljava/lang/String;)V",
          javaZipFile.object<jstring>(), javaZipDir.object<jstring>());

#endif

      qDebug() << openfile << dirpath;

      QString strFullPath;
      QString str0 = dirpath + "META-INF/container.xml";
      if (!QFile(str0).exists()) {
        deleteDirfile(dirpath);
        QDir dir;
        dir.rename(dirpathbak, dirpath);
        qDebug() << "====== isEpub == false ======";
        return;
      } else {
        isEpub = true;
        mw_one->ui->lblBookName->show();
      }

      QStringList conList = readText(str0);
      for (int i = 0; i < conList.count(); i++) {
        QString str = conList.at(i);
        if (str.contains("full-path")) {
          QStringList list1 = str.split(" ");
          if (list1.count() > 0) {
            for (int j = 0; j < list1.count(); j++) {
              QString str1 = list1.at(j);
              if (str1.contains("full-path")) {
                QStringList list2 = str1.split("\"");
                if (list2.count() > 0) {
                  strFullPath = list2.at(1);
                }
              }
            }
          }
        }
      }

      QString strOpfFile = dirpath + strFullPath;
      QFileInfo fi(strOpfFile);
      QString strOpfPath = fi.path() + "/";
      QStringList opfList = readText(strOpfFile);
      htmlFiles.clear();
      QString strTitle;
      if (opfList.count() < 5) {
        QString str;
        for (int j = 0; j < opfList.count(); j++) str = str + opfList.at(j);
        str = str.replace(">", ">|");
        qDebug() << ">|  " << str;
        QStringList list = str.split("|");
        opfList.clear();
        for (int j = 0; j < list.count(); j++) {
          opfList.append(list.at(j));
        }
      }
      if (opfList.count() > 1) {
        for (int i = 0; i < opfList.count(); i++) {
          QString str0 = opfList.at(i);
          str0 = str0.trimmed();
          if (str0.contains("href=") && str0.mid(0, 5) == "<item") {
            QString str1 = str0;
            QString str2, str3;
            str1.replace("href=", "|");
            str1 = str1.trimmed();
            for (int m = 0; m < str1.length(); m++) {
              if (str1.mid(m, 1) == "|") {
                str2 = str1.mid(m + 1, str1.length() - m);
                break;
              }
            }
            QStringList list = str2.split("\"");
            if (list.count() > 0) str3 = list.at(1);

            if (str3.contains("htm")) htmlFiles.append(strOpfPath + str3);
          }

          // title
          if (str0.contains("<dc:title>")) {
            QString str = str0;
            str = str.replace("<dc:title>", "");
            str = str.replace("</dc:title>", "");
            strTitle = str.trimmed();
          }
        }
      }

      qDebug() << strFullPath << htmlFiles;
      if (htmlFiles.count() == 0) {
        deleteDirfile(dirpath);
        QDir dir;
        dir.rename(dirpathbak, dirpath);
        qDebug() << "====== htmlFiles Count== 0 ======";
        return;
      } else {
        deleteDirfile(dirpathbak);
        mw_one->ui->lblBookName->setText(strTitle);
      }

    } else {
      isEpub = false;
      mw_one->ui->lblBookName->hide();
      iPage = 0;
      sPos = 0;
      ui->hSlider->setValue(0);
      totallines = readTextList.count();
    }

    fileName = openfile;
#ifdef Q_OS_MAC
    QFileInfo fi(openfile);
    mw_one->ui->lblBookName->setText(fi.baseName());
    mw_one->ui->lblBookName->show();
#endif

#ifdef Q_OS_ANDROID

#endif

    ui->frameFun->hide();
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
  Reg.setValue("/Reader/FontName", fontname);
  Reg.setValue("/Reader/FontSize", mw_one->textFontSize);
  Reg.setValue("/Reader/vpos" + fileName, textPos);
  if (!isEpub) {
    Reg.setValue("/Reader/iPage" + fileName, iPage - baseLines);
  } else
    Reg.setValue("/Reader/htmlIndex" + fileName, htmlIndex);

  qDebug() << "textPos" << textPos << "htmlIndex=" << htmlIndex;
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

void dlgReader::getLines() {
  if (isLines) {
    QString qsShow;

    mw_one->ui->hSlider->setTickInterval(1);
    mw_one->ui->hSlider->setMinimum(0);
    mw_one->ui->hSlider->setValue(sPos);

    if (!isEpub) {
      mw_one->ui->hSlider->setMaximum(totallines / baseLines - 1);
      mw_one->ui->btnLines->setText(
          tr("Pages") + "\n" +
          QString::number(mw_one->ui->hSlider->value() + 1) + " / " +
          QString::number(totallines / baseLines));
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

      qsShow =
          "<p style='line-height:32px; width:100% ; white-space: pre-wrap; '>" +
          txt1 + "</p>";
      setQML(qsShow);

    } else {
      mw_one->ui->hSlider->setMaximum(htmlFiles.count());
      htmlIndex = sPos - 1;
      if (htmlIndex < 0) htmlIndex = 0;
      // qsShow = mw_one->loadText(htmlFiles.at(htmlIndex));
      setQMLHtml();
    }
  }
}

void dlgReader::setQML(QString txt1) {
  // strPage = "<body style=\"line-height:137.5% ;\">" + txt1;

  // mw_one->ui->quickWidget->setSource(QUrl(QStringLiteral("qrc:/text.qml")));
  mw_one->ui->quickWidget->rootContext()->setContextProperty("strText", txt1);
}

void dlgReader::on_btnFont_clicked() {
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
    saveReader();
    mw_one->ui->quickWidget->rootContext()->setContextProperty("FontName",
                                                               fontname);

    list->close();
    setVPos();
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

  if (isLines) {
    int max;
    if (!isEpub)
      max = totallines / baseLines;
    else
      max = htmlFiles.count();
    if (position >= max) position = max;
    sPos = position;
    getLines();
  }
}

void dlgReader::on_btnPageUp_clicked() {
  mw_one->ui->lblTitle->hide();
  QString qsShow;

  if (!isEpub) {
    int count = iPage - baseLines;
    if (count <= 0) return;
    textPos = 0;
    QString txt1;
    ui->textBrowser->clear();

    for (int i = count - baseLines; i < count; i++) {
      iPage--;
      QString str = readTextList.at(i);
      if (str.trimmed() != "")
        txt1 = txt1 + readTextList.at(i) + "\n" + strSpace;
    }

    qsShow =
        "<p style='line-height:32px; width:100% ; white-space: pre-wrap; '>" +
        txt1 + "</p>";

    setQML(qsShow);

    mw_one->ui->hSlider->setMaximum(totallines / baseLines);
    mw_one->ui->btnLines->setText(tr("Pages") + "\n" +
                                  QString::number(iPage / baseLines) + " / " +
                                  QString::number(totallines / baseLines));
    mw_one->ui->progReader->setMaximum(totallines / baseLines);
    mw_one->ui->progReader->setValue(iPage / baseLines);

  } else {
    htmlIndex--;
    if (htmlIndex < 0) htmlIndex = 0;
    // qsShow = mw_one->loadText(htmlFiles.at(htmlIndex));
    setQMLHtml();

    mw_one->ui->hSlider->setMaximum(htmlFiles.count());
    mw_one->ui->btnLines->setText(tr("Pages") + "\n" +
                                  QString::number(htmlIndex + 1) + " / " +
                                  QString::number(htmlFiles.count()));
    mw_one->ui->progReader->setMaximum(htmlFiles.count());
    mw_one->ui->progReader->setValue(htmlIndex + 1);
  }
  // mw_one->ui->quickWidget->rootContext()->setContextProperty("textPos", 0);
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
    setQML(qsShow);

    mw_one->ui->hSlider->setMaximum(totallines / baseLines);
    mw_one->ui->btnLines->setText(tr("Pages") + "\n" +
                                  QString::number(iPage / baseLines) + " / " +
                                  QString::number(totallines / baseLines));
    mw_one->ui->progReader->setMaximum(totallines / baseLines);
    mw_one->ui->progReader->setValue(iPage / baseLines);

  } else {
    htmlIndex++;
    if (htmlIndex == htmlFiles.count()) htmlIndex = htmlFiles.count() - 1;
    // qsShow = mw_one->loadText(htmlFiles.at(htmlIndex));

    setQMLHtml();

    mw_one->ui->hSlider->setMaximum(htmlFiles.count());
    mw_one->ui->btnLines->setText(tr("Pages") + "\n" +
                                  QString::number(htmlIndex + 1) + " / " +
                                  QString::number(htmlFiles.count()));
    mw_one->ui->progReader->setMaximum(htmlFiles.count());
    mw_one->ui->progReader->setValue(htmlIndex + 1);

    // mw_one->ui->quickWidget->rootContext()->setContextProperty(
    //     "baseUrl", htmlFiles.at(htmlIndex));
  }
  // mw_one->ui->quickWidget->rootContext()->setContextProperty("textPos", 0);
}

void dlgReader::setQMLHtml() {
  QVariant msg = htmlFiles.at(htmlIndex);
  QQuickItem* root = mw_one->ui->quickWidget->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "loadHtml", Q_ARG(QVariant, msg));
}

void dlgReader::on_btnLines_clicked() {
  mw_one->ui->lblTitle->hide();
  mw_one->ui->hSlider->setTickInterval(1);
  if (!isEpub) {
    mw_one->ui->hSlider->setMinimum(0);
    mw_one->ui->hSlider->setMaximum(totallines / baseLines - 1);
    mw_one->ui->hSlider->setValue(iPage / baseLines);
  } else {
    mw_one->ui->hSlider->setMinimum(1);
    mw_one->ui->hSlider->setMaximum(htmlFiles.count());
    mw_one->ui->hSlider->setValue(htmlIndex + 1);
  }

  if (mw_one->ui->frameFun->isHidden()) {
    isLines = true;

    mw_one->ui->frameFun->show();

  } else {
    isLines = false;

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
    htmlIndex = Reg.value("/Reader/htmlIndex" + fileName, 0).toInt() - 1;
    if (htmlIndex <= 0) htmlIndex = -1;
    qDebug() << "htmlIndex=" << htmlIndex;

    if (iPage >= 0 || htmlIndex >= -1) {
      on_btnPageNext_clicked();

    } else {
      iPage = 0;
      htmlIndex = 0;
    }
  }
}

void dlgReader::setVPos() {
  QSettings Reg(iniDir + "reader.ini", QSettings::IniFormat);
  Reg.setIniCodec("utf-8");
  textPos = Reg.value("/Reader/vpos" + fileName, 0).toReal();

  if (textPos > 10) {
    // mw_one->ui->quickWidget->rootContext()->setContextProperty("textPos",
    // textPos);

    QQuickItem* root = mw_one->ui->quickWidget->rootObject();
    QMetaObject::invokeMethod((QObject*)root, "setVPos",
                              Q_ARG(QVariant, textPos));
  }
}

int dlgReader::deleteDirfile(QString dirName) {
  QDir directory(dirName);
  if (!directory.exists()) {
    return true;
  }

  QString srcPath = QDir::toNativeSeparators(dirName);
  if (!srcPath.endsWith(QDir::separator())) srcPath += QDir::separator();

  QStringList fileNames = directory.entryList(
      QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
  bool error = false;
  for (QStringList::size_type i = 0; i != fileNames.size(); ++i) {
    QString filePath = srcPath + fileNames.at(i);
    QFileInfo fileInfo(filePath);
    if (fileInfo.isFile() || fileInfo.isSymLink()) {
      QFile::setPermissions(filePath, QFile::WriteOwner);
      if (!QFile::remove(filePath)) {
        error = true;
      }
    } else if (fileInfo.isDir()) {
      if (!deleteDirfile(filePath)) {
        error = true;
      }
    }
  }

  if (!directory.rmdir(QDir::toNativeSeparators(directory.path()))) {
    error = true;
  }
  return !error;
}

void dlgReader::setFontSize(int textFontSize) {
  saveReader();
  mw_one->ui->quickWidget->rootContext()->setContextProperty("FontSize",
                                                             textFontSize);
  setVPos();
}

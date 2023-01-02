#include "src/RemarksAbout.h"

#include "mainwindow.h"
#include "ui_RemarksAbout.h"
#include "ui_mainwindow.h"
extern MainWindow *mw_one;
extern bool loading, zh_cn;
extern QString noteText, appName, ver;
extern int curPos;

dlgRemarks::dlgRemarks(QWidget *parent)
    : QDialog(parent), ui(new Ui::dlgRemarks) {
  ui->setupUi(this);

  mw_one->set_btnStyle(this);

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

  manager = new QNetworkAccessManager(this);
  connect(manager, SIGNAL(finished(QNetworkReply *)), this,
          SLOT(replyFinished(QNetworkReply *)));
  blAutoCheckUpdate = true;
  CheckUpdate();
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

bool dlgRemarks::eventFilter(QObject *obj, QEvent *evn) {
#ifdef Q_OS_ANDROID
  if (obj == ui->textEdit->viewport()) {
    mw_one->mydlgMainNotes->getEditPanel(ui->textEdit, evn);
  }
#endif

  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(obj, evn);
}

void dlgRemarks::keyReleaseEvent(QKeyEvent *event) { Q_UNUSED(event); }

void dlgRemarks::resizeEvent(QResizeEvent *event) {
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

void dlgRemarks::on_btnHomePage_clicked() {
  QString str;
  str = "https://github.com/ic005k/Knot/issues";
  QUrl url(str);
  QDesktopServices::openUrl(url);
}

void dlgRemarks::on_btnPaste_clicked() { ui->textEdit->paste(); }

void dlgRemarks::CheckUpdate() {
  QNetworkRequest quest;
  quest.setUrl(QUrl("https://api.github.com/repos/ic005k/" + appName +
                    "/releases/latest"));
  quest.setHeader(QNetworkRequest::UserAgentHeader, "RT-Thread ART");
  manager->get(quest);
}

void dlgRemarks::replyFinished(QNetworkReply *reply) {
  QString str = reply->readAll();
  parse_UpdateJSON(str);
  reply->deleteLater();
}

QString dlgRemarks::getUrl(QVariantList list) {
  QString androidUrl, macUrl, winUrl, linuxUrl;
  for (int i = 0; i < list.count(); i++) {
    QVariantMap map = list[i].toMap();
    QString fName = map["name"].toString();

    if (fName.contains("android"))
      androidUrl = map["browser_download_url"].toString();
    if (fName.contains("Mac")) macUrl = map["browser_download_url"].toString();
    if (fName.contains("Win")) winUrl = map["browser_download_url"].toString();
    if (fName.contains("Linux"))
      linuxUrl = map["browser_download_url"].toString();
  }

#ifdef Q_OS_ANDROID
  return androidUrl;
#endif

#ifdef Q_OS_MAC
  return macUrl;
#endif

#ifdef Q_OS_WIN
  return winUrl;
#endif

  return linuxUrl;
}

int dlgRemarks::parse_UpdateJSON(QString str) {
  QJsonParseError err_rpt;
  QJsonDocument root_Doc = QJsonDocument::fromJson(str.toUtf8(), &err_rpt);

  if (err_rpt.error != QJsonParseError::NoError) {
    if (!blAutoCheckUpdate)
      mw_one->showMsgBox(appName, tr("Network error!"), "", 1);
    blAutoCheckUpdate = false;
    return -1;
  }
  if (root_Doc.isObject()) {
    QJsonObject root_Obj = root_Doc.object();

    QVariantList list = root_Obj.value("assets").toArray().toVariantList();
    QString Url = getUrl(list);
    s_link = "https://ghproxy.com/" + Url;
    qDebug() << "s_link" << s_link << Url;

    QJsonObject PulseValue = root_Obj.value("assets").toObject();
    QString Verison = root_Obj.value("tag_name").toString();
    QString UpdateTime = root_Obj.value("published_at").toString();
    QString ReleaseNote = root_Obj.value("body").toString();

    if (Verison > ver && Url != "") {
      QString warningStr = tr("New version detected!") + "\n" +
                           tr("Version: ") + "V" + Verison + "\n" +
                           tr("Published at: ") + UpdateTime + "\n" +
                           tr("Release Notes: ") + "\n" + ReleaseNote;

      // int ret = QMessageBox::warning(this, "", warningStr, tr("Download"),
      //                                tr("Cancel"));
      // for Android ret = 3 Mac ret = 0 or 1(Cancel)
      int ret = QMessageBox::warning(this, "", warningStr, tr("Download"));

      if (ret >= 0) {
#ifdef Q_OS_ANDROID
        show_download();
#else
        // const QUrl url("https://github.com/ic005k/" + appName +
        //                "/releases/latest");
        QDesktopServices::openUrl(QUrl(s_link));
#endif
      }
    } else {
      if (!blAutoCheckUpdate)
        mw_one->showMsgBox(tr("Upgrade Check"),
                           tr("You are currently using the latest version!"),
                           "", 1);
    }
  }
  blAutoCheckUpdate = false;
  return 0;
}

void dlgRemarks::show_download() {
  int aver = getAndroidVer();
  // Android7.0及以上
  if (aver >= 24) {
    m_AutoUpdate = new AutoUpdateDialog(this);
    int y = (mw_one->height() - m_AutoUpdate->height()) / 2;
    m_AutoUpdate->setGeometry(mw_one->geometry().x(), y, mw_one->width(),
                              m_AutoUpdate->height());

    mw_one->showGrayWindows();
    m_AutoUpdate->show();
    m_AutoUpdate->startDownload(s_link);
    qDebug() << "start dl..... " << s_link;
    this->close();
  } else {
    // Android6.0及以下通过浏览器下载
    QUrl url(s_link);
    QDesktopServices::openUrl(url);
  }
}

void dlgRemarks::on_btnCheckUpdate_clicked() { CheckUpdate(); }

void dlgRemarks::on_btnDownloadUP_clicked() {
  if (s_link == "") return;

#ifdef Q_OS_ANDROID
  show_download();
#else
  // const QUrl url("https://github.com/ic005k/" + appName +
  // "/releases/latest");
  QDesktopServices::openUrl(QUrl(s_link));
#endif
}

int dlgRemarks::getAndroidVer() {
  int a = 0;
#ifdef Q_OS_UNIX
  a = 24;
#endif

#ifdef Q_OS_WIN
  a = 24;
#endif

#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject jo = QAndroidJniObject::fromString("ver");
  a = jo.callStaticMethod<int>("com.x/MyActivity", "getAndroidVer", "()I");
#else
  QJniObject jo = QJniObject::fromString("ver");
  a = jo.callStaticMethod<int>("com.x/MyActivity", "getAndroidVer", "()I");
#endif

#endif
  return a;
}

#include "LoadPic.h"

#include "MainWindow.h"
#include "ui_MainWindow.h"
extern MainWindow* mw_one;
extern Method* m_Method;
extern QString picfile;

LoadPic::LoadPic(QWidget* parent) : QDialog(parent) {
  // mw_one->ui->f_ImgFun->setObjectName("myframe");
  // mw_one->ui->f_ImgFun->setStyleSheet(
  //     "QFrame#myframe{border-image:url(:/res/b.png)}");

  QFont font = this->font();
  font.setPointSize(13);
  mw_one->ui->lblImgInfo->setFont(font);
  mw_one->ui->lblImgInfo->adjustSize();
  mw_one->ui->lblImgInfo->setWordWrap(true);
  mw_one->ui->lblImgInfo->setText("");

  this->installEventFilter(this);
}

LoadPic::~LoadPic() {}

void LoadPic::initMain(QString imgFile) {
  mw_one->ui->lblImgInfo->setText(
      imgFile + "  " + mw_one->getFileSize(QFile(imgFile).size(), 2));

  if (mw_one->isReaderVisible) mw_one->ui->frameReader->hide();
  if (mw_one->isMemoVisible) mw_one->ui->frameNotes->hide();

  mw_one->ui->qw_Img->rootContext()->setContextProperty("myW", mw_one->width());
  mw_one->ui->qw_Img->rootContext()->setContextProperty("myH",
                                                        mw_one->height());

  mw_one->ui->f_ImgView->show();

  mw_one->ui->qw_Img->rootContext()->setContextProperty(
      "imgW", mw_one->ui->qw_Img->width() * 10);
  mw_one->ui->qw_Img->rootContext()->setContextProperty(
      "imgH", mw_one->ui->qw_Img->height() * 10);
  mw_one->ui->qw_Img->rootContext()->setContextProperty("imgFile", imgFile);
  mw_one->ui->qw_Img->setSource(
      QUrl(QStringLiteral("qrc:/src/qmlsrc/imgview.qml")));
}

bool LoadPic::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

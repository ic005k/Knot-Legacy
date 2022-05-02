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

  connect(ui->textBrowser->verticalScrollBar(), &QScrollBar::valueChanged, this,
          &dlgReader::getPages);

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
  if (QFile(fileName).exists()) {
    QString txt = mw_one->loadText(fileName);
    // ui->textBrowser->setPlainText(txt);
    QFileInfo fi(fileName);
    ui->lblTitle->setText(fi.baseName());
    ui->lblTitle->hide();

    // QFont font = ui->textBrowser->font();
    // font.setLetterSpacing(QFont::AbsoluteSpacing, 2);
    // ui->textBrowser->setFont(font);

    /*QTextDocument* doc = ui->textBrowser->document();
    QTextCursor textcursor = ui->textBrowser->textCursor();
    for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next()) {
      QTextBlockFormat tbf = it.blockFormat();
      tbf.setLineHeight(5, QTextBlockFormat::LineDistanceHeight);
      textcursor.setPosition(it.position());
      textcursor.setBlockFormat(tbf);
      ui->textBrowser->setTextCursor(textcursor);
    }*/

    // ui->textBrowser->setHtml(qsShow);
    ui->textBrowser->clear();
    myedit->setPlainText(txt);
    QString txt1;
    for (int i = 0; i < 15; i++) {
      iPage++;
      txt1 = txt1 + getTextEditLineText(myedit, i) + "\n";
    }

    QString qsShow =
        "<p style='line-height:28px; width:100% ; white-space: pre-wrap; '>" +
        txt1 + "</p>";
    ui->textBrowser->setHtml(qsShow);

    ui->textBrowser->verticalScrollBar()->setSliderPosition(0);
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
  this->setFixedHeight(mw_one->height());
  this->setFixedWidth(mw_one->width());
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
  int baseh = ui->textBrowser->height();
  qulonglong th = ui->textBrowser->verticalScrollBar()->maximum();
  int page = th / baseh;
  qulonglong cpos = ui->textBrowser->verticalScrollBar()->sliderPosition();
  int cp = cpos / baseh;
  ui->btnPage->setText(QString::number(cp) + " / " + QString::number(page));
  ui->hSlider->setMaximum(page);
  ui->hSlider->setMinimum(1);
  // qDebug() << "th:" << th << "cpos:" << cpos;
}

void dlgReader::on_btnPage_clicked() {
  if (ui->frame->isHidden()) {
    ui->hSlider->setValue(ui->textBrowser->verticalScrollBar()->value() /
                          ui->textBrowser->height());
    ui->frame->show();
  } else
    ui->frame->hide();
}

void dlgReader::on_hSlider_sliderMoved(int position) {
  ui->textBrowser->verticalScrollBar()->setSliderPosition(
      position * ui->textBrowser->height());
}

void dlgReader::on_btnPageUp_clicked() {
  QString txt1;
  ui->textBrowser->clear();
  int count = iPage - 15;
  for (int i = count - 15; i < count; i++) {
    iPage--;
    txt1 = txt1 + getTextEditLineText(myedit, i) + "\n";
  }

  QString qsShow =
      "<p style='line-height:28px; width:100% ; white-space: pre-wrap; '>" +
      txt1 + "</p>";
  ui->textBrowser->setHtml(qsShow);

  ui->textBrowser->verticalScrollBar()->setSliderPosition(0);
}

void dlgReader::on_btnPageNext_clicked() {
  QString txt1;
  ui->textBrowser->clear();
  int count = iPage + 15;
  for (int i = iPage; i < count; i++) {
    iPage++;
    txt1 = txt1 + getTextEditLineText(myedit, i) + "\n";
  }

  QString qsShow =
      "<p style='line-height:28px; width:100% ; white-space: pre-wrap; '>" +
      txt1 + "</p>";
  ui->textBrowser->setHtml(qsShow);

  ui->textBrowser->verticalScrollBar()->setSliderPosition(0);
}

#ifndef METHOD_H
#define METHOD_H

#include <QDialog>
#include <QInputDialog>
#include <QQuickWidget>
#include <QTableWidget>

namespace Ui {
class Method;
}

class Method : public QDialog {
  Q_OBJECT

 public:
  explicit Method(QWidget *parent = nullptr);
  ~Method();
  Ui::Method *ui;

  QString qssSlider;

  QString btnStyle =
      "QToolButton {background-color: rgb(236, 236, 236);color: black; "
      "border-radius:10px; "
      "border:1px solid gray; } QToolButton:pressed { background-color: "
      "rgb(220,220,230);}";

  QString btnStyleDark =
      "QToolButton {background-color: rgb(51, 51, 51);color: white; "
      "border-radius:10px; "
      "border:1px solid gray; } QToolButton:pressed { background-color: "
      "rgb(22,22,23);}";

  QString pushbtnStyle =
      "QPushButton {background-color: rgb(236, 236, 236);border-radius:10px; "
      "border:1px solid gray; } QPushButton:pressed { background-color: "
      "rgb(220,220,230);}";

  QString listWidgetStyle =
      "QListWidget::indicator{width:25;height:25;right: 5px;}"
      "QListView {outline: none;}"
      "#listWidget::item {background-color: #ffffff;color: #000000;border: "
      "transparent;border-bottom: 1px solid #dbdbdb;padding: 8px;height: 85;}"
      "#listWidget::item:hover {background-color: #f5f5f5;}"
      "#listWidget::item:selected {border-left: 5px solid #777777;}";

  QString setPushButtonQss(
      QToolButton *btn,                              // 按钮对象
      int radius = 5,                                // 圆角半径
      int padding = 8,                               // 间距
      const QString &normalColor = "#34495E",        // 正常颜色
      const QString &normalTextColor = "#FFFFFF",    // 文字颜色
      const QString &hoverColor = "#4E6D8C",         // 悬停颜色
      const QString &hoverTextColor = "#F0F0F0",     // 悬停文字颜色
      const QString &pressedColor = "#2D3E50",       // 按下颜色
      const QString &pressedTextColor = "#B8C6D1");  // 按下文字颜色

  QString vsbarStyle =
      "QScrollBar:vertical{"  // 垂直滑块整体
      "width:30px;"
      "background:#FFFFFF;"   // 背景色
      "padding-top:25px;"     // 上预留位置（放置向上箭头）
      "padding-bottom:25px;"  // 下预留位置（放置向下箭头）
      "padding-left:3px;"     // 左预留位置（美观）
      "padding-right:3px;"    // 右预留位置（美观）
      "border-left:1px solid #d7d7d7;}"     // 左分割线
      "QScrollBar::handle:vertical{"        // 滑块样式
      "background:#dbdbdb;"                 // 滑块颜色
      "border-radius:6px;"                  // 边角圆润
      "min-height:60px;}"                   // 滑块最小高度
      "QScrollBar::handle:vertical:hover{"  // 鼠标触及滑块样式
      "background:#d0d0d0;}"                // 滑块颜色
      "QScrollBar::add-line:vertical{"      // 向下箭头样式
      "background:url(:/src/down.png) bottom no-repeat;}"
      "QScrollBar::sub-line:vertical{"  // 向上箭头样式
      "background:url(:/src/up.png) top no-repeat;}";

  QString vsbarStyleSmall =
      "QScrollBar:vertical{"  // 垂直滑块整体
      "width:6px;"
      "background:rgb(255,255,255);"  // 背景色
      "padding-top:0px;"              // 上预留位置（放置向上箭头）
      "padding-bottom:0px;"           // 下预留位置（放置向下箭头）
      "padding-left:1px;"             // 左预留位置（美观）
      "padding-right:1px;"            // 右预留位置（美观）
      "border-left:0px solid #d7d7d7;}"     // 左分割线
      "QScrollBar::handle:vertical{"        // 滑块样式
      "background:rgb(202,197,191);"        // 滑块颜色
      "border-radius:6px;"                  // 边角圆润
      "min-height:60px;}"                   // 滑块最小高度
      "QScrollBar::handle:vertical:hover{"  // 鼠标触及滑块样式
      "background:#d0d0d0;}"                // 滑块颜色
      "QScrollBar::add-line:vertical{"      // 向下箭头样式
      "background:url() center no-repeat;}"
      "QScrollBar::sub-line:vertical{"  // 向上箭头样式
      "background:url() center no-repeat;}";

  QString listStyleMain =
      "QListWidget{outline:0px;}"
      "QListWidget::item:selected{background:rgb(255,0,0); border:0px "
      "blue;margin:0px,0px,0px,0px;border-radius:5;"
      "color:white}";

  QString qssMenu =
      "QMenu {"
      "border: 1px solid rgb(172, 172, 172);"
      "border-radius: 3px; }"
      "QMenu::item {"
      "border-bottom: 1px solid rgb(172, 172, 172);"
      "padding:10px 10px;"
      "margin:0px 0px; }"
      "QMenu::item:selected {"
      "background-color: rgb(62, 186, 231); }";

  QDialog *getProgBar();
  void startSearch();
  void initSearchResults();

  void init();

  void setCurrentIndex(int index);
  void clearAll();
  int getCount();
  void delItem(int index);

  void addItem(QString text_tab, QString text0, QString text1, QString text2,
               QString text3, int itemH);

  void setCurrentIndexFromQW(QQuickWidget *qw, int index);
  void clearAllBakList(QQuickWidget *qw);
  int getCountFromQW(QQuickWidget *qw);
  void delItemBakList(QQuickWidget *qw, int index);
  void addItemToQW(QQuickWidget *qw, QString text0, QString text1,
                   QString text2, QString text3, int itemH);

  QString getText3(QQuickWidget *qw, int index);
  int getCurrentIndexFromQW(QQuickWidget *qw);
  QString getText0(QQuickWidget *qw, int index);

  void saveCurNoteIndex();
  QList<int> getCurNoteIndex();
  void modifyItemText2(QQuickWidget *qw, int index, QString strText);

  void modifyItemText0(QQuickWidget *qw, int index, QString strText);
  void insertItem(QQuickWidget *qw, QString text0, QString text1, QString text2,
                  QString text3, int curIndex);

  void gotoEnd(QQuickWidget *qw);

  void setSCrollPro(QObject *obj);
  void init_all_notes();

  bool eventFilterReader(QObject *watch, QEvent *evn);

  void showDelMsgBox(QString title, QString info);

  void saveRecycleTabName(QString keyStr, QString tabName);
  QString getRecycleTabName(QString keyStr);

  QFont getNewFont(int maxSize);

  int getFontHeight();

  void closeKeyboard();

  void modifyItemText3(QQuickWidget *qw, int index, QString strText);
  QInputDialog *inputDialog(QString windowsTitle, QString lblEdit,
                            QString defaultValue);

  void setDark(QString strDark);

  void showGrayWindows();
  void closeGrayWindows();

  QString getRealPathFile(QString strFile);

 protected:
  bool eventFilter(QObject *watchDlgSearch, QEvent *evn) override;

 public slots:

  void clickNoteBook();
  void clickNoteList();

  void showNoteBookMenu(int x, int y);
  void showNotsListMenu(int x, int y);
  void clickMainDate();

  void clickMainDateData();
  void clickMainEventData();
  void reeditMainEventData();
  void setTypeRenameText();
  void okType();
 private slots:

 private:
  void setCellText(int row, int column, QString str, QTableWidget *table);
  void generateData(int count);
  int nProgressBarType = 2;

  int x, y, w, h;

  QWidget *m_widget;
  void setMainTabCurrentIndex();
};

#endif  // METHOD_H

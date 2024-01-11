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
};

#endif  // METHOD_H

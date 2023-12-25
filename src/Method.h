#ifndef METHOD_H
#define METHOD_H

#include <QDialog>
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

  void startSearch();
  void initSearchResults();

  void init();

  void setCurrentIndex(int index);
  void clearAll();
  int getCount();
  void delItem(int index);

  void addItem(QString text0, QString text1, QString text2, QString text3,
               int itemH);

  void setCurrentIndexBakList(QQuickWidget *qw, int index);
  void clearAllBakList(QQuickWidget *qw);
  int getCountBakList(QQuickWidget *qw);
  void delItemBakList(QQuickWidget *qw, int index);
  void addItemBakList(QQuickWidget *qw, QString text0, QString text1,
                      QString text2, QString text3, int itemH);

  QString getText3(QQuickWidget *qw, int index);
  int getCurrentIndexBakList(QQuickWidget *qw);
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
};

#endif  // METHOD_H

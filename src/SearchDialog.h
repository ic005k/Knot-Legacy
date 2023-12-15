#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QQuickWidget>
#include <QTableWidget>

namespace Ui {
class SearchDialog;
}

class SearchDialog : public QDialog {
  Q_OBJECT

 public:
  explicit SearchDialog(QWidget *parent = nullptr);
  ~SearchDialog();
  Ui::SearchDialog *ui;

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
  protected:
  bool eventFilter(QObject *watchDlgSearch, QEvent *evn) override;

 public slots:
  void on_btnBack_clicked();
  void on_btnSearch_clicked();

  void clickNoteBook();
  void clickNoteList();

 private slots:

  void on_btnClearText_clicked();

 private:
  void setCellText(int row, int column, QString str, QTableWidget *table);
  void generateData(int count);
};

#endif  // SEARCHDIALOG_H

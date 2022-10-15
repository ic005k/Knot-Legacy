#ifndef DLGNOTESLIST_H
#define DLGNOTESLIST_H

#include <QDialog>
#include <QKeyEvent>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTreeWidgetItem>

namespace Ui {
class dlgNotesList;
}

class dlgNotesList : public QDialog {
  Q_OBJECT

 public:
  explicit dlgNotesList(QWidget *parent = nullptr);
  ~dlgNotesList();
  Ui::dlgNotesList *ui;

  QTreeWidget *twrb;
  QTreeWidget *tw;
  QString currentMDFile;
  void delFile(QString file);

  void saveNotesList();
  void initNotesList();
  void init();

  void setWinPos();

 protected:
  bool eventFilter(QObject *watch, QEvent *evn) override;

  void closeEvent(QCloseEvent *event) override;
 private slots:
  void on_btnClose_clicked();

  void on_btnNewNoteBook_clicked();

  void on_btnNewNote_clicked();

  void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

  void on_btnRename_clicked();

  void on_btnDel_clicked();

  void on_btnImport_clicked();

  void on_btnExport_clicked();

  void on_btnRecycle_clicked();

  void on_btnBack_clicked();

  void on_btnRestore_clicked();

  void on_btnDel_2_clicked();

  private:
  void addToRecycle(QTreeWidgetItem *item);
};

#endif  // DLGNOTESLIST_H

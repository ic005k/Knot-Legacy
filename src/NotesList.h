#ifndef NOTESLIST_H
#define NOTESLIST_H

#include <QDialog>
#include <QDirIterator>
#include <QKeyEvent>
#include <QListWidget>
#include <QListWidgetItem>
#include <QRandomGenerator>
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

  bool isSave = false;
  QTreeWidget *twrb;
  QTreeWidget *tw;
  QString currentMDFile;
  void delFile(QString file);

  void saveNotesList();
  void initNotesList();
  void init();

  void setWinPos();

  void addItem(QTreeWidget *tw, QTreeWidgetItem *item);

  void saveRecycle();
  void initRecycle();
  void clearFiles();
  void getAllFiles(const QString &foldPath, QStringList &folds,
                   const QStringList &formats);

 protected:
  bool eventFilter(QObject *watch, QEvent *evn) override;

  void closeEvent(QCloseEvent *event) override;
 public slots:
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
  QStringList files;
  void clearMD_Pic(QTreeWidget *tw);
  void removePicFromMD(QString mdfile);
  void removeFromFiles(QString str);
};

#endif  // NOTESLIST_H

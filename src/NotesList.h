#ifndef NOTESLIST_H
#define NOTESLIST_H

#include <QApplication>
#include <QDialog>
#include <QDirIterator>
#include <QInputMethod>
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

  int findCount;
  QList<QTreeWidgetItem *> findResultList;
  bool isSave = false;
  QTreeWidget *twrb;
  QTreeWidget *tw;
  QString currentMDFile;
  void delFile(QString file);

  void saveNotesList();
  void initNotesList();

  void setWinPos();

  void addItem(QTreeWidget *tw, QTreeWidgetItem *item);

  void saveRecycle();
  void initRecycle();
  void clearFiles();
  void getAllFiles(const QString &foldPath, QStringList &folds,
                   const QStringList &formats);

  void setNoteName(QString name);
  void moveBy(int ud);

  QString getCurrentMDFile();
  void init_NotesListMenu(QMenu *mainMenu);
  void init_NoteBookMenu(QMenu *mainMenu);

  int getNoteBookCurrentIndex();
  int getNotesListCurrentIndex();
  void setNoteBookCurrentIndex(int index);
  void setNotesListCurrentIndex(int index);
  int getNoteBookCount();
  int getNotesListCount();
  void loadAllNoteBook();
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

  void on_KVChanged();
 private slots:
  void on_btnFind_clicked();

  void on_btnPrev_clicked();

  void on_btnNext_clicked();

  void on_editFind_textChanged(const QString &arg1);

  void on_editFind_returnPressed();

  void on_btnUp_clicked();

  void on_btnDown_clicked();

  void on_actionAdd_NoteBook_triggered();
  void on_actionDel_NoteBook_triggered();
  void on_actionRename_NoteBook_triggered();
  void on_actionMoveUp_NoteBook_triggered();
  void on_actionMoveDown_NoteBook_triggered();

  void on_actionAdd_Note_triggered();
  void on_actionDel_Note_triggered();
  void on_actionRename_Note_triggered();
  void on_actionMoveUp_Note_triggered();
  void on_actionMoveDown_Note_triggered();
  void on_actionImport_Note_triggered();
  void on_actionExport_Note_triggered();

 private:
  QInputMethod *pAndroidKeyboard = QApplication::inputMethod();
  QStringList files;
  void clearMD_Pic(QTreeWidget *tw);
  void removePicFromMD(QString mdfile);
  void removeFromFiles(QString str);
};

#endif  // NOTESLIST_H

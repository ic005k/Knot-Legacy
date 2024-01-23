#ifndef NEWNOTEBOOK_H
#define NEWNOTEBOOK_H

#include <QDialog>
#include <QKeyEvent>

namespace Ui {
class NewNoteBook;
}

class NewNoteBook : public QDialog {
  Q_OBJECT

 public:
  explicit NewNoteBook(QWidget *parent = nullptr);
  ~NewNoteBook();
  Ui::NewNoteBook *ui;

  QStringList getValue();
  protected:
  bool eventFilter(QObject *watch, QEvent *evn) override;

  void closeEvent(QCloseEvent *event) override;
 private slots:
  void on_btnCancel_clicked();

  void on_btnOk_clicked();

 private:
  QString strValue;
};

#endif  // NEWNOTEBOOK_H

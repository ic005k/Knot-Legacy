#ifndef DLGNOTES_H
#define DLGNOTES_H

#include <QDialog>

namespace Ui {
class dlgNotes;
}

class dlgNotes : public QDialog {
  Q_OBJECT

 public:
  explicit dlgNotes(QWidget *parent = nullptr);
  ~dlgNotes();
  Ui::dlgNotes *ui;

 public slots:
  bool eventFilter(QObject *obj, QEvent *event) override;

 protected:
   void keyReleaseEvent(QKeyEvent *event);
 private slots:
  void on_btnBack_clicked();

  void on_textEdit_textChanged();

 private:
};

#endif  // DLGNOTES_H

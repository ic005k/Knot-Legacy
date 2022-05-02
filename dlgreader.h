#ifndef DLGREADER_H
#define DLGREADER_H

#include <QDialog>

namespace Ui {
class dlgReader;
}

class dlgReader : public QDialog {
  Q_OBJECT

 public:
  explicit dlgReader(QWidget *parent = nullptr);
  ~dlgReader();
  Ui::dlgReader *ui;

  void saveReader();
  void initReader();
  QString fileName;
  void openFile(QString fileName);
  qlonglong vpos;
  bool one = false;

 protected:
  bool eventFilter(QObject *obj, QEvent *evn) override;
  void keyReleaseEvent(QKeyEvent *event) override;
 private slots:
  void on_btnBack_clicked();

  void on_btnOpen_clicked();

  void on_btnFontPlus_clicked();

  void on_btnFontLess_clicked();

 private:
};

#endif  // DLGREADER_H

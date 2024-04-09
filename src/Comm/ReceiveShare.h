#ifndef RECEIVESHARE_H
#define RECEIVESHARE_H

#include <QClipboard>
#include <QDialog>
#include <QKeyEvent>

#include "src/Comm/Method.h"

namespace Ui {
class ReceiveShare;
}

class ReceiveShare : public QDialog {
  Q_OBJECT

 public:
  explicit ReceiveShare(QWidget *parent = nullptr);
  ~ReceiveShare();
  Ui::ReceiveShare *ui;

  void init();

  void closeAllActiveWindows();
  QObjectList getAllFrame(QObjectList lstUIControls);

  void shareImages(const QString &title, const QStringList &imagesPathList);
  void shareString(const QString &title, const QString &content);

  void shareImage(const QString &title, const QString &path,
                  const QString &fileType);

 protected:
  bool eventFilter(QObject *watch, QEvent *evn) override;

  void closeEvent(QCloseEvent *event) override;

 private slots:
  void on_btnAddToTodo_clicked();

  void on_btnAddToNote_clicked();

  void on_btnTest_clicked();

  void on_btnInsertToNote_clicked();

 private:
  void Close();
};

#endif  // RECEIVESHARE_H

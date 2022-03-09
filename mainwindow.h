#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QMainWindow>
#include <QSettings>
#include <QTextEdit>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  QTimer *tmer;
  QString strDate;
  void saveData();
  void readData();
  QString loadText(QString textFile);
  void TextEditToFile(QTextEdit *txtEdit, QString fileName);
public slots:
  void init_Stats();
protected:
  void closeEvent(QCloseEvent *event);
private slots:

  void timerUpdate();

  void on_btnPlus_clicked();

  void on_btnLess_clicked();

 private:
  Ui::MainWindow *ui;
};
#endif  // MAINWINDOW_H

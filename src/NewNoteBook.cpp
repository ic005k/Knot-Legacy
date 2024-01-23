#include "NewNoteBook.h"

#include "MainWindow.h"
#include "ui_NewNoteBook.h"

extern MainWindow* mw_one;
extern Method* m_Method;

NewNoteBook::NewNoteBook(QWidget* parent)
    : QDialog(parent), ui(new Ui::NewNoteBook) {
  ui->setupUi(this);
  setWindowFlag(Qt::FramelessWindowHint);

  QStringList list;
  list.append("Main Root");
  int count = mw_one->m_NotesList->ui->treeWidget->topLevelItemCount();
  for (int i = 0; i < count; i++) {
    list.append(mw_one->m_NotesList->ui->treeWidget->topLevelItem(i)
                    ->text(0)
                    .trimmed());
  }
  ui->cboxRoot->addItems(list);
}

NewNoteBook::~NewNoteBook() { delete ui; }

bool NewNoteBook::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnCancel_clicked();
    }

    return true;
  }

  return QWidget::eventFilter(watch, evn);
}

void NewNoteBook::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event)
  m_Method->closeGrayWindows();
}

void NewNoteBook::on_btnCancel_clicked() {
  strValue = "";
  close();
}

void NewNoteBook::on_btnOk_clicked() {
  strValue = ui->editName->text().trimmed();
  close();
}

QStringList NewNoteBook::getValue() {
  QStringList list;
  list.append(ui->cboxRoot->currentText().trimmed());
  list.append(ui->editName->text().trimmed());

  int x, y, w, h;
  w = mw_one->width() - 20;
  h = this->height();
  x = mw_one->geometry().x() + (mw_one->width() - w) / 2;
  y = 150;
  setGeometry(x, y, w, h);

  m_Method->showGrayWindows();
  show();
  while (!isHidden()) QCoreApplication::processEvents();

  return list;
}

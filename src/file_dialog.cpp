#include "file_dialog.h"
#include "utils.h"

/*
// sample usage
  FileDialog *_f_dlg = new FileDialog(false);
//  _f_dlg->setOption(QFileDialog::ReadOnly, true);
  _f_dlg->setWindowTitle("Choose items to upload");
  _f_dlg->resize(850,525);
  qDebug() << _f_dlg->exec();
  qDebug() << _f_dlg->selectedFiles();
// qDebug() << _f_dlg->exec();

 QList <QUrl> list = _f_dlg->selectedUrls();

for (int j = 0; j < list.count(); ++j) {
  qDebug() << list.at(j).path();
}
*/

FileDialog::FileDialog(bool isDownload) : QFileDialog() {

  setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);

  mIsDownload = isDownload;

  m_btnOpen = NULL;
  m_listView = NULL;
  m_treeView = NULL;
  m_selectedFiles.clear();

  this->setFileMode(QFileDialog::Directory);
  this->setOption(QFileDialog::DontUseNativeDialog, true);

  QList<QPushButton *> btns = this->findChildren<QPushButton *>();
  for (int i = 0; i < btns.size(); ++i) {
    QString text = btns[i]->text();
    if (text.toLower().contains("open") || text.toLower().contains("choose")) {
      m_btnOpen = btns[i];
      break;
    }
  }

  if (!m_btnOpen)
    return;

  // watch selection changes
  findChild<QWidget *>("listView")->installEventFilter(this);
  findChild<QWidget *>("treeView")->installEventFilter(this);

  if (!mIsDownload) {
    m_btnOpen->installEventFilter(this);
    m_btnOpen->disconnect(SIGNAL(clicked()));
    connect(m_btnOpen, SIGNAL(clicked()), this, SLOT(chooseClicked()));
  }

  // findChild<QLabel *>("FileName")->setText("asdadaas");
  findChild<QLineEdit *>("fileNameEdit")->setReadOnly(true);

  if (!mIsDownload) {
    // fileNameEdit lable
    this->setLabelText(QFileDialog::FileName,
                       QFileDialog::tr("Items to upload:"));
  }

  // enable multi select for both files and dirs
  m_listView = findChild<QListView *>("listView");
  if (m_listView) {
    if (!mIsDownload) {
      m_listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }

    m_listView->setAcceptDrops(false);
  }

  m_treeView = findChild<QTreeView *>();
  if (m_treeView) {

    m_treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_treeView->resizeColumnToContents(1);
    m_treeView->resizeColumnToContents(2);
    m_treeView->resizeColumnToContents(3);

    // don't allow drag and drop inside m_treeView
    m_treeView->setAcceptDrops(false);

    if (!mIsDownload) {
      m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }
  }

  // customise buttons
  auto settings = GetSettings();
  QString iconsColour = settings->value("Settings/iconsColour").toString();

  QString img_add = "";

  if (iconsColour == "white") {
    img_add = "_inv";
  }

  findChild<QToolButton *>("toParentButton")
      ->setIcon(
          QIcon(":media/images/qbutton_icons/vuparrow" + img_add + ".png"));

  findChild<QToolButton *>("forwardButton")
      ->setIcon(
          QIcon(":media/images/qbutton_icons/vrightarrow" + img_add + ".png"));

  findChild<QToolButton *>("backButton")
      ->setIcon(
          QIcon(":media/images/qbutton_icons/vleftarrow" + img_add + ".png"));

  findChild<QToolButton *>("newFolderButton")
      ->setIcon(QIcon(":media/images/qbutton_icons/mkdir" + img_add + ".png"));

  findChild<QToolButton *>("detailModeButton")
      ->setIcon(QIcon(":media/images/qbutton_icons/detailed_view" + img_add +
                      ".png"));

  findChild<QToolButton *>("listModeButton")
      ->setIcon(
          QIcon(":media/images/qbutton_icons/list_view" + img_add + ".png"));

  /*
  // custom sidebar URLs

    QList<QUrl> urls;

    urls = this->sidebarUrls();

    urls.clear();

    urls << QUrl("file:");
    urls << QUrl::fromLocalFile(
        QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first());
    urls << QUrl::fromLocalFile(
        QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)
            .first());
    urls << QUrl::fromLocalFile(
        QStandardPaths::standardLocations(QStandardPaths::DownloadLocation)
            .first());
    urls << QUrl::fromLocalFile(
        QStandardPaths::standardLocations(QStandardPaths::DesktopLocation)
            .first());
    urls << QUrl::fromLocalFile(
        QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)
            .first());
    urls << QUrl::fromLocalFile(
        QStandardPaths::standardLocations(QStandardPaths::MoviesLocation)
            .first());
    urls << QUrl::fromLocalFile(
        QStandardPaths::standardLocations(QStandardPaths::MusicLocation).first());

    QString default_folder;

    if (mIsDownload) {
      default_folder =
          (settings->value("Settings/defaultDownloadDir").toString());
    } else {
      default_folder =
    (settings->value("Settings/defaultUploadDir").toString());
    }

    if (!default_folder.isEmpty()) {

      urls << QUrl::fromLocalFile(default_folder);
    }

    this->setSidebarUrls(urls);

  */
}

bool FileDialog::eventFilter(QObject *watched, QEvent *event) {

#if defined(Q_OS_MACOS)
  // macOS implenmetation has a bug in multi item drag & drop - it crashes. Only
  // allow to drag and drop one folder
  if (qobject_cast<QTreeView *>(watched)) {
    if (m_listView->selectionModel()->selectedRows().count() > 1) {
      m_listView->setDragEnabled(false);
      m_treeView->setDragEnabled(false);
    } else {
      m_listView->setDragEnabled(true);
      m_treeView->setDragEnabled(true);
    }
  }
#endif

  // keep button on for directories and folders
  QPushButton *btn = qobject_cast<QPushButton *>(watched);
  if (btn) {
    if (event->type() == QEvent::EnabledChange) {
      if (!mIsDownload) {
        if (!btn->isEnabled()) {
          btn->setEnabled(true);
        }
      }
    }
  }

  return QWidget::eventFilter(watched, event);
}

void FileDialog::chooseClicked() {
  // return list of selected files
  // does not work well - does contain nothing if only sidebar item was selected
  // we will use selectedUrls() - which works better

  QModelIndexList indexList = m_listView->selectionModel()->selectedIndexes();
  foreach (QModelIndex index, indexList) {
    if (index.column() == 0) {
      m_selectedFiles.append(this->directory().absolutePath() + "????" +
                             index.data().toString());
    }
  }

  QDialog::accept();
}

QStringList FileDialog::selectedFiles() { return m_selectedFiles; }

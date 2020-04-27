#include "file_dialog.h"
#include "utils.h"

/*

// it is a bit hack of existing QFileDialog class
// but otherwise will require implemeting own QFileDialog like class

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

  auto settings = GetSettings();
  if (settings->contains("FileDialog/geometry")) {
    restoreGeometry(settings->value("FileDialog/geometry").toByteArray());
  }

  mIsDownload = isDownload;

  m_btnOpen = NULL;
  m_listView = NULL;
  m_treeView = NULL;
  m_selectedFiles.clear();

  this->setFileMode(QFileDialog::Directory);
  this->setOption(QFileDialog::DontUseNativeDialog, true);

  QList<QPushButton *> btns = this->findChildren<QPushButton *>();
  if (!btns.isEmpty()) {
    // find accept button
    for (int i = 0; i < btns.size(); ++i) {
      QString text = btns[i]->text();
      if (text.toLower().contains("open") ||
          text.toLower().contains("choose")) {
        m_btnOpen = btns[i];
        break;
      }
    }

    if (!mIsDownload) {
      // for uploads we take control of accept button
      m_btnOpen->installEventFilter(this);
      m_btnOpen->disconnect(SIGNAL(clicked()));
      connect(m_btnOpen, SIGNAL(clicked()), this, SLOT(chooseClicked()));
    }
  }

  if (!m_btnOpen)
    return;

  // change filter name
  QComboBox *fileTypeCombo = findChild<QComboBox *>("fileTypeCombo");
  if (fileTypeCombo) {
    fileTypeCombo->setItemText(fileTypeCombo->currentIndex(),
                               "All Files and Directories");
  }

  /*
    // remove "Delete" from contex menu
    QAction *deleteAction = findChild<QAction *>("qt_delete_action");
    if (deleteAction) {
      deleteAction->setVisible(false);
    }
  */

  if (findChild<QLineEdit *>("fileNameEdit")) {
    findChild<QLineEdit *>("fileNameEdit")->setReadOnly(true);
  }

  if (!mIsDownload) {
    // fileNameEdit lable
    this->setLabelText(QFileDialog::FileName,
                       QFileDialog::tr("Items to upload:"));
  }

  // enable multi select for both files and dirs
  m_listView = findChild<QListView *>("listView");
  if (m_listView) {

#ifdef Q_OS_WIN
    // as with Fusion style in Windows font size does not scale
    // with QApplication::font() changes we control it manually using style
    // sheet
    QFont defaultFont = QApplication::font();
    int fontSize = defaultFont.pointSize() + 3;

    QString fontStyleSheet =
        QString("QListView { font-size: %1px;}").arg(fontSize);
    m_listView->setStyleSheet(fontStyleSheet);
#endif

    // watch selection changes
    m_listView->installEventFilter(this);

    if (!mIsDownload) {
      m_listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }

    // don't allow drag and drop inside m_listView
    m_listView->setAcceptDrops(false);
#if defined(Q_OS_MACOS)
    // on macOS we later re-enable if single object selected
    m_listView->setDragEnabled(false);
#endif
  }

  m_treeView = findChild<QTreeView *>();
  if (m_treeView) {

#ifdef Q_OS_WIN
    // as with Fusion style in Windows font size does not scale
    // with QApplication::font() changes we control it manually using style
    // sheet
    QFont defaultFont = QApplication::font();
    int fontSize = defaultFont.pointSize() + 3;

    QString fontStyleSheet =
        QString("QTreeView { font-size: %1px;}").arg(fontSize);
    m_treeView->setStyleSheet(fontStyleSheet);
#endif

    // watch selection changes
    m_treeView->installEventFilter(this);

    QTimer::singleShot(0, this, SLOT(resizeColums()));

    // don't allow drag and drop inside m_treeView
    m_treeView->setAcceptDrops(false);
#if defined(Q_OS_MACOS)
    // on macOS we later re-enable if single object selected
    m_treeView->setDragEnabled(false);
#endif

    if (!mIsDownload) {
      m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }
  }

  // customise buttons
  QString iconsColour = settings->value("Settings/iconsColour").toString();

  QString img_add = "";

  if (iconsColour == "white") {
    img_add = "_inv";
  }

  // customise buttons

  int iconSize = 24;

  QToolButton *toParentButton = findChild<QToolButton *>("toParentButton");
  if (toParentButton) {
    toParentButton->setIcon(
        QIcon(":media/images/qbutton_icons/vuparrow" + img_add + ".png"));
    toParentButton->setStyleSheet(
        "QToolButton { border: 0; } QToolButton:pressed { border: 4; "
        "border-radius: 10px; border-style: inset; border-color: rgba(1, 1, 1, "
        "0);}");
    toParentButton->setIconSize(QSize(iconSize, iconSize));
    connect(toParentButton, SIGNAL(clicked()), this, SLOT(clearSelection()));
  }

  QToolButton *forwardButton = findChild<QToolButton *>("forwardButton");
  if (forwardButton) {
    forwardButton->setIcon(
        QIcon(":media/images/qbutton_icons/vrightarrow" + img_add + ".png"));
    forwardButton->setStyleSheet(
        "QToolButton { border: 0; } QToolButton:pressed { border: 4; "
        "border-radius: 10px; border-style: inset; border-color: rgba(1, 1, 1, "
        "0);}");
    forwardButton->setIconSize(QSize(iconSize, iconSize));
    connect(forwardButton, SIGNAL(clicked()), this, SLOT(clearSelection()));
  }

  QToolButton *backButton = findChild<QToolButton *>("backButton");
  if (backButton) {
    backButton->setIcon(
        QIcon(":media/images/qbutton_icons/vleftarrow" + img_add + ".png"));
    backButton->setStyleSheet("QToolButton { border: 0; } QToolButton:pressed "
                              "{ border: 4; border-radius: 10px; border-style: "
                              "inset; border-color: rgba(1, 1, 1, 0);}");
    backButton->setIconSize(QSize(iconSize, iconSize));
    connect(backButton, SIGNAL(clicked()), this, SLOT(clearSelection()));
  }

  QToolButton *newFolderButton = findChild<QToolButton *>("newFolderButton");
  if (newFolderButton) {
    newFolderButton->setIcon(
        QIcon(":media/images/qbutton_icons/mkdir2" + img_add + ".png"));
    newFolderButton->setStyleSheet(
        "QToolButton { border: 0; } QToolButton:pressed { border: 4; "
        "border-radius: 10px; border-style: inset; border-color: rgba(1, 1, 1, "
        "0);}");

    newFolderButton->setIconSize(QSize(iconSize, iconSize));
  }

  QToolButton *detailModeButton = findChild<QToolButton *>("detailModeButton");
  if (detailModeButton) {
    detailModeButton->setIcon(
        QIcon(":media/images/qbutton_icons/detailed_view" + img_add + ".png"));
    detailModeButton->setStyleSheet(
        "QToolButton { border: 0; } QToolButton:pressed { border: 4; "
        "border-radius: 10px; border-style: inset; border-color: rgba(1, 1, 1, "
        "0);}");
    detailModeButton->setIconSize(QSize(iconSize, iconSize));
  }

  QToolButton *listModeButton = findChild<QToolButton *>("listModeButton");
  if (listModeButton) {
    listModeButton->setIcon(
        QIcon(":media/images/qbutton_icons/list_view" + img_add + ".png"));
    listModeButton->setStyleSheet(
        "QToolButton { border: 0; } QToolButton:pressed { border: 4; "
        "border-radius: 10px; border-style: inset; border-color: rgba(1, 1, 1, "
        "0);}");
    listModeButton->setIconSize(QSize(iconSize, iconSize));
  }

  // custom sidebar URLs - if never saved before create initial list

  QSettings settingsFileDialog(QSettings::UserScope,
                               QLatin1String("QtProject"));

  if (!settingsFileDialog.childGroups().contains(QLatin1String("FileDialog"))) {

    QList<QUrl> urls;

    urls = this->sidebarUrls();

    //   urls.clear();

    //   urls << QUrl("file:");
    //   urls << QUrl::fromLocalFile(
    //       QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first());

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
        QStandardPaths::standardLocations(QStandardPaths::MusicLocation)
            .first());

    // add also default Download and Upload directories
    QString default_folder;

    default_folder =
        (settings->value("Settings/defaultDownloadDir").toString());

    if (!default_folder.isEmpty()) {

      urls << QUrl::fromLocalFile(default_folder);
    }

    default_folder = (settings->value("Settings/defaultUploadDir").toString());

    if (!default_folder.isEmpty()) {

      urls << QUrl::fromLocalFile(default_folder);
    }

    this->setSidebarUrls(urls);
  }

  // sidebar font size
  QWidget *sidebar = findChild<QWidget *>("sidebar");
  if (sidebar) {
#ifdef Q_OS_WIN
    // as with Fusion style in Windows font size does not scale
    // with QApplication::font() changes we control it manually using style
    // sheet
    QFont defaultFont = QApplication::font();
    int fontSize = defaultFont.pointSize() + 3;

    QString fontStyleSheet =
        QString("QWidget { font-size: %1px;}").arg(fontSize);
    sidebar->setStyleSheet(fontStyleSheet);
#endif
  }
}

FileDialog::~FileDialog() {
  auto settings = GetSettings();
  settings->setValue("FileDialog/geometry", saveGeometry());
}

bool FileDialog::eventFilter(QObject *watched, QEvent *event) {

#if defined(Q_OS_MACOS)
  // macOS implenmetation has a bug in multi item drag & drop - it crashes. Only
  // allow to drag and drop one folder
  if (qobject_cast<QTreeView *>(watched)) {
    if (m_treeView->selectionModel()->selectedRows().count() == 1) {
      m_treeView->setDragEnabled(true);
    } else {
      m_treeView->setDragEnabled(false);
    }
  }

  if (qobject_cast<QListView *>(watched)) {
    if (m_listView->selectionModel()->selectedRows().count() == 1) {
      m_listView->setDragEnabled(true);
    } else {
      m_listView->setDragEnabled(false);
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

void FileDialog::clearSelection() {
  // when forward, back or up pressed clear if anything already selected
  static_cast<QTreeView *>(m_treeView)->selectionModel()->clearSelection();
  static_cast<QListView *>(m_listView)->selectionModel()->clearSelection();

  QLineEdit *fileNameEdit = findChild<QLineEdit *>("fileNameEdit");
  if (fileNameEdit) {
    fileNameEdit->clear();
  }
}

void FileDialog::resizeColums() {
  m_treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
  m_treeView->resizeColumnToContents(1);
  m_treeView->resizeColumnToContents(2);
  m_treeView->resizeColumnToContents(3);
}

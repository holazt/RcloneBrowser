#include "remote_widget.h"
#include "check_dialog.h"
#include "export_dialog.h"
#include "icon_cache.h"
#include "item_model.h"
#include "list_of_job_options.h"
#include "progress_dialog.h"
#include "transfer_dialog.h"
#include "utils.h"

RemoteWidget::RemoteWidget(IconCache *iconCache, const QString &remote,
                           const QString &remoteType, QWidget *parent)
    : QWidget(parent) {

  ui.setupUi(this);

  bool isLocal = remoteType == "local";
  bool isGoogle = remoteType == "drive";

  QString root = isLocal ? "/" : QString();

  QString remoteMode = "main";

  //  QString remoteMode_ = "main";

#ifndef Q_OS_WIN
  isLocal = false;
#endif

#ifdef Q_OS_WIN
  // as with Fusion style in Windows QTreeView font size does not scale
  // with QApplication::font() changes we control it manually using style sheet

  QFont defaultFont = QApplication::font();
  int fontSize = defaultFont.pointSize() + 3;

  QString fontStyleSheet =
      QString("QTreeView { font-size: %1px;}").arg(fontSize);
  ui.tree->setStyleSheet(fontStyleSheet);
#endif

  auto settings = GetSettings();

  QString buttonStyle = settings->value("Settings/buttonStyle").toString();
  QString buttonSize = settings->value("Settings/buttonSize").toString();
  QString iconsColour = settings->value("Settings/iconsColour").toString();
  QString rcloneVersion = settings->value("Settings/rcloneVersion").toString();
  settings->setValue("Settings/remoteMode", "main");
  ui.tree->setAlternatingRowColors(
      settings->value("Settings/rowColors", false).toBool());

  ui.cb_GoogleDriveMode->setDisabled(!isGoogle);
  // hide cb_GoogleDriveMode for non Google remotes
  if (!isGoogle) {
    ui.cb_GoogleDriveMode->hide();
  }

  QString img_add = "";

  if (iconsColour == "white") {
    img_add = "_inv";
  }

  ui.refresh->setIcon(
      QIcon(":remotes/images/qbutton_icons/refresh" + img_add + ".png"));
  ui.mkdir->setIcon(
      QIcon(":remotes/images/qbutton_icons/mkdir" + img_add + ".png"));
  ui.rename->setIcon(
      QIcon(":remotes/images/qbutton_icons/rename" + img_add + ".png"));
  ui.move->setIcon(
      QIcon(":remotes/images/qbutton_icons/move" + img_add + ".png"));
  ui.purge->setIcon(
      QIcon(":remotes/images/qbutton_icons/purge" + img_add + ".png"));
  ui.mount->setIcon(
      QIcon(":remotes/images/qbutton_icons/mount" + img_add + ".png"));
  ui.stream->setIcon(
      QIcon(":remotes/images/qbutton_icons/stream" + img_add + ".png"));
  ui.upload->setIcon(
      QIcon(":remotes/images/qbutton_icons/upload" + img_add + ".png"));
  ui.download->setIcon(
      QIcon(":remotes/images/qbutton_icons/download" + img_add + ".png"));
  ui.actionCheck->setIcon(
      QIcon(":remotes/images/qbutton_icons/check" + img_add + ".png"));
  ui.getSize->setIcon(
      QIcon(":remotes/images/qbutton_icons/getsize" + img_add + ".png"));
  ui.getTree->setIcon(
      QIcon(":remotes/images/qbutton_icons/gettree" + img_add + ".png"));
  ui.link->setIcon(
      QIcon(":remotes/images/qbutton_icons/link" + img_add + ".png"));
  ui.export_->setIcon(
      QIcon(":remotes/images/qbutton_icons/export" + img_add + ".png"));
  ui.getInfo->setIcon(
      QIcon(":remotes/images/qbutton_icons/info" + img_add + ".png"));

  ui.buttonRefresh->setDefaultAction(ui.refresh);
  ui.buttonMkdir->setDefaultAction(ui.mkdir);
  ui.buttonRename->setDefaultAction(ui.rename);
  ui.buttonMove->setDefaultAction(ui.move);
  ui.buttonPurge->setDefaultAction(ui.purge);
  ui.buttonMount->setDefaultAction(ui.mount);
  ui.buttonStream->setDefaultAction(ui.stream);
  ui.buttonUpload->setDefaultAction(ui.upload);
  ui.buttonDownload->setDefaultAction(ui.download);
  ui.buttonCheck->setDefaultAction(ui.actionCheck);
  ui.buttonSize->setDefaultAction(ui.getSize);
  ui.buttonTree->setDefaultAction(ui.getTree);
  ui.buttonLink->setDefaultAction(ui.link);
  ui.buttonExport->setDefaultAction(ui.export_);
  ui.buttonInfo->setDefaultAction(ui.getInfo);

  // buttons and icons size
  int icon_w = 16;
  int icon_h = 16;
  if (buttonSize == "0") {
    icon_w = 24;
  }
  if (buttonSize == "1") {
    icon_w = 32;
  }
  if (buttonSize == "2") {
    icon_w = 48;
  }
  if (buttonSize == "3") {
    icon_w = 72;
  }
  if (buttonSize == "4") {
    icon_w = 96;
  }
  icon_h = icon_w;

  if (buttonStyle == "textandicon") {
    ui.buttonRefresh->setIconSize(QSize(icon_w, icon_h));
    ui.buttonMkdir->setIconSize(QSize(icon_w, icon_h));
    ui.buttonRename->setIconSize(QSize(icon_w, icon_h));
    ui.buttonMove->setIconSize(QSize(icon_w, icon_h));
    ui.buttonPurge->setIconSize(QSize(icon_w, icon_h));
    ui.buttonMount->setIconSize(QSize(icon_w, icon_h));
    ui.buttonStream->setIconSize(QSize(icon_w, icon_h));
    ui.buttonUpload->setIconSize(QSize(icon_w, icon_h));
    ui.buttonDownload->setIconSize(QSize(icon_w, icon_h));
    ui.buttonSize->setIconSize(QSize(icon_w, icon_h));
    ui.buttonTree->setIconSize(QSize(icon_w, icon_h));
    ui.buttonLink->setIconSize(QSize(icon_w, icon_h));
    ui.buttonExport->setIconSize(QSize(icon_w, icon_h));
    ui.buttonCheck->setIconSize(QSize(icon_w, icon_h));
    ui.buttonInfo->setIconSize(QSize(icon_w, icon_h));

  } else {
    if (buttonStyle == "textonly") {

      ui.buttonRefresh->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonMkdir->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonRename->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonMove->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonPurge->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonMount->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonStream->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonUpload->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonDownload->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonSize->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonTree->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonLink->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonExport->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonCheck->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonInfo->setToolButtonStyle(Qt::ToolButtonTextOnly);

    } else {
      // button style - icononly
      ui.buttonRefresh->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonRefresh->setIconSize(QSize(icon_w, icon_h));
      ui.buttonMkdir->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonMkdir->setIconSize(QSize(icon_w, icon_h));
      ui.buttonRename->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonRename->setIconSize(QSize(icon_w, icon_h));
      ui.buttonMove->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonMove->setIconSize(QSize(icon_w, icon_h));
      ui.buttonPurge->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonPurge->setIconSize(QSize(icon_w, icon_h));
      ui.buttonMount->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonMount->setIconSize(QSize(icon_w, icon_h));
      ui.buttonStream->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonStream->setIconSize(QSize(icon_w, icon_h));
      ui.buttonUpload->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonUpload->setIconSize(QSize(icon_w, icon_h));
      ui.buttonDownload->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonDownload->setIconSize(QSize(icon_w, icon_h));
      ui.buttonSize->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonSize->setIconSize(QSize(icon_w, icon_h));
      ui.buttonTree->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonTree->setIconSize(QSize(icon_w, icon_h));
      ui.buttonLink->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonLink->setIconSize(QSize(icon_w, icon_h));
      ui.buttonExport->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonExport->setIconSize(QSize(icon_w, icon_h));
      ui.buttonCheck->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonCheck->setIconSize(QSize(icon_w, icon_h));
      ui.buttonInfo->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonInfo->setIconSize(QSize(icon_w, icon_h));
    }
  }

  ui.tree->sortByColumn(0, Qt::AscendingOrder);
  ui.tree->header()->setSectionsMovable(false);

  ItemModel *model = new ItemModel(iconCache, remote, this);
  ui.tree->setModel(model);
  QTimer::singleShot(0, ui.tree, SLOT(setFocus()));

  QObject::connect(model, &QAbstractItemModel::layoutChanged, this, [=]() {
    ui.tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui.tree->resizeColumnToContents(1);
    ui.tree->resizeColumnToContents(2);
  });

  QObject::connect(
      ui.tree->selectionModel(), &QItemSelectionModel::selectionChanged, this,
      [=](const QItemSelection &selection) {
        for (auto child : findChildren<QAction *>()) {
          child->setDisabled(selection.isEmpty());
        }

        if (selection.isEmpty()) {
          ui.path->clear();
          return;
        }

        QModelIndex index = selection.indexes().front();

        bool topLevel = model->isTopLevel(index);
        bool isFolder = model->isFolder(index);

        QDir path;
        if (model->isLoading(index)) {
          ui.refresh->setDisabled(true);
          ui.move->setDisabled(true);
          ui.rename->setDisabled(true);
          ui.purge->setDisabled(true);
          ui.mount->setDisabled(true);
          ui.stream->setDisabled(true);
          ui.upload->setDisabled(true);
          ui.download->setDisabled(true);
          ui.cb_GoogleDriveMode->setDisabled(true);
          path = model->path(model->parent(index));
        } else {
          ui.refresh->setDisabled(false);

          bool driveModeButtons = false;

          if (remoteType == "drive") {
            // for Google Drive --drive-shared-with-me is read only
            driveModeButtons = (ui.cb_GoogleDriveMode->currentIndex() == 1);
          }

          ui.mkdir->setDisabled(driveModeButtons);
          ui.rename->setDisabled(topLevel || driveModeButtons);
          ui.move->setDisabled(topLevel || driveModeButtons);
          ui.purge->setDisabled(topLevel || driveModeButtons);
          ui.upload->setDisabled(driveModeButtons);

#if defined(Q_OS_WIN32)
          // check if required version
          unsigned int result =
              compareVersion(rcloneVersion.toStdString(), "1.50");
          if (result == 2) {
            ui.mount->setDisabled(true);
          } else {
            ui.mount->setDisabled(!isFolder);
          };
#else
// mount is not supported by rclone on these systems
#if defined(Q_OS_OPENBSD) || defined(Q_OS_NETBSD)
          ui.mount->setDisabled(true);
#else
          if (remoteType == "local") {
            ui.mount->setDisabled(true);
          } else {
            ui.mount->setDisabled(!isFolder);
          }
#endif
#endif

          ui.stream->setDisabled(isFolder);
          ui.cb_GoogleDriveMode->setDisabled(!isGoogle);
          path = model->path(index);
        }

        ui.getSize->setDisabled(!isFolder);
        ui.getTree->setDisabled(!isFolder);
        ui.export_->setDisabled(!isFolder);
        ui.actionCheck->setDisabled(!isFolder);
        ui.path->setText(isLocal ? QDir::toNativeSeparators(path.path())
                                 : path.path());
      });

  QObject::connect(ui.refresh, &QAction::triggered, this, [=]() {
    setRemoteMode(ui.cb_GoogleDriveMode->currentIndex(), remoteType);

    QModelIndex index = ui.tree->selectionModel()->selectedRows().front();
    model->refresh(index);
  });

  QObject::connect(ui.mkdir, &QAction::triggered, this, [=]() {
    setRemoteMode(ui.cb_GoogleDriveMode->currentIndex(), remoteType);

    QModelIndex index = ui.tree->selectionModel()->selectedRows().front();

    if (!model->isFolder(index)) {
      index = index.parent();
    }
    QDir path = model->path(index);
    QString pathMsg =
        isLocal ? QDir::toNativeSeparators(path.path()) : path.path();
    QString name = QInputDialog::getText(
        this, "New Folder", QString("Create folder in %1").arg(pathMsg));
    if (!name.isEmpty()) {
      QString folder = path.filePath(name);
      QString folderMsg = isLocal ? QDir::toNativeSeparators(folder) : folder;

      QProcess process;
      UseRclonePassword(&process);
      process.setProgram(GetRclone());
      process.setArguments(QStringList() << "mkdir" << GetRcloneConf()
                                         << GetRemoteModeRcloneOptions()
                                         << GetDefaultRcloneOptionsList()
                                         << remote + ":" + folder);
      process.setProcessChannelMode(QProcess::MergedChannels);

      ProgressDialog progress("New Folder", "Creating...", folderMsg, &process,
                              this);
      if (progress.exec() == QDialog::Accepted) {
        model->refresh(index);
      }
    }
  });

  QObject::connect(ui.rename, &QAction::triggered, this, [=]() {
    setRemoteMode(ui.cb_GoogleDriveMode->currentIndex(), remoteType);

    QModelIndex index = ui.tree->selectionModel()->selectedRows().front();

    QString path = model->path(index).path();
    QString pathMsg = isLocal ? QDir::toNativeSeparators(path) : path;

    QString name = model->data(index, Qt::DisplayRole).toString();
    name = QInputDialog::getText(this, "Rename",
                                 QString("New name for %1").arg(pathMsg),
                                 QLineEdit::Normal, name);
    if (!name.isEmpty()) {
      QProcess process;
      UseRclonePassword(&process);
      process.setProgram(GetRclone());
      process.setArguments(
          QStringList() << "moveto" << GetRcloneConf()
                        << GetRemoteModeRcloneOptions()
                        << GetDefaultRcloneOptionsList() << remote + ":" + path
                        << remote + ":" +
                               model->path(index.parent()).filePath(name));
      process.setProcessChannelMode(QProcess::MergedChannels);

      ProgressDialog progress("Rename", "Renaming...", pathMsg, &process, this);
      if (progress.exec() == QDialog::Accepted) {
        model->rename(index, name);
      }
    }
  });

  QObject::connect(ui.move, &QAction::triggered, this, [=]() {
    setRemoteMode(ui.cb_GoogleDriveMode->currentIndex(), remoteType);

    QModelIndex index = ui.tree->selectionModel()->selectedRows().front();

    QString path = model->path(index).path();
    QString pathMsg = isLocal ? QDir::toNativeSeparators(path) : path;

    QString name = model->path(index.parent()).path() + "/";
    name = QInputDialog::getText(this, "Move",
                                 QString("New location for %1").arg(pathMsg),
                                 QLineEdit::Normal, name);
    if (!name.isEmpty()) {
      QProcess process;
      UseRclonePassword(&process);
      process.setProgram(GetRclone());
      process.setArguments(QStringList()
                           << "move" << GetRcloneConf()
                           << GetRemoteModeRcloneOptions()
                           << GetDefaultRcloneOptionsList()
                           << remote + ":" + path << remote + ":" + name);
      process.setProcessChannelMode(QProcess::MergedChannels);

      ProgressDialog progress("Move", "Moving...", pathMsg, &process, this);
      if (progress.exec() == QDialog::Accepted) {
        model->refresh(index);
      }
    }
  });

  QObject::connect(ui.purge, &QAction::triggered, this, [=]() {
    setRemoteMode(ui.cb_GoogleDriveMode->currentIndex(), remoteType);

    QModelIndex index = ui.tree->selectionModel()->selectedRows().front();

    QString path = model->path(index).path();
    QString pathMsg = isLocal ? QDir::toNativeSeparators(path) : path;

    int button = QMessageBox::question(
        this, "Delete",
        QString("Are you sure you want to delete %1 ?").arg(pathMsg),
        QMessageBox::Yes | QMessageBox::No);
    if (button == QMessageBox::Yes) {
      QProcess process;
      UseRclonePassword(&process);
      process.setProgram(GetRclone());
      process.setArguments(QStringList()
                           << (model->isFolder(index) ? "purge" : "delete")
                           << GetRcloneConf() << GetRemoteModeRcloneOptions()
                           << GetDefaultRcloneOptionsList()
                           << remote + ":" + path);
      process.setProcessChannelMode(QProcess::MergedChannels);

      ProgressDialog progress("Delete", "Deleting...", pathMsg, &process, this);
      if (progress.exec() == QDialog::Accepted) {
        QModelIndex parent = index.parent();
        QModelIndex next = parent.model()->index(index.row() + 1, 0);
        ui.tree->selectionModel()->select(next.isValid() ? next : parent,
                                          QItemSelectionModel::SelectCurrent);
        model->removeRow(index.row(), parent);
      }
    }
  });

  QObject::connect(ui.mount, &QAction::triggered, this, [=]() {
    setRemoteMode(ui.cb_GoogleDriveMode->currentIndex(), remoteType);

    QModelIndex index = ui.tree->selectionModel()->selectedRows().front();

    QString path = model->path(index).path();
    QString pathMsg = isLocal ? QDir::toNativeSeparators(path) : path;

#if defined(Q_OS_WIN32)
    QString folder =
        QInputDialog::getText(this, "Mount",
                              QString("(Make sure you have WinFsp-FUSE "
                                      "installed)\n\nDrive to mount %1 to")
                                  .arg(remote),
                              QLineEdit::Normal, "Z:");
#else
        QString folder = QFileDialog::getExistingDirectory(this, QString("Mount %1").arg(remote));
#endif

    if (!folder.isEmpty()) {
      emit addMount(remote + ":" + path, folder, remoteType);
    }
  });

  QObject::connect(ui.stream, &QAction::triggered, this, [=]() {
    setRemoteMode(ui.cb_GoogleDriveMode->currentIndex(), remoteType);

    QModelIndex index = ui.tree->selectionModel()->selectedRows().front();
    QString path = model->path(index).path();

    auto settings = GetSettings();
    bool streamConfirmed =
        settings->value("Settings/streamConfirmed", false).toBool();
    QString stream = settings->value("Settings/stream", "vlc -").toString();
    if (!streamConfirmed) {
      QString result = QInputDialog::getText(
          this, "Stream",
          "Enter stream command (file will be passed in STDIN):",
          QLineEdit::Normal, stream);
      if (result.isEmpty()) {
        return;
      }

      stream = result;

      settings->setValue("Settings/stream", stream);
      settings->setValue("Settings/streamConfirmed", true);
    }

    emit addStream(remote + ":" + path, stream);
  });

  QObject::connect(ui.link, &QAction::triggered, this, [=]() {
    setRemoteMode(ui.cb_GoogleDriveMode->currentIndex(), remoteType);

    QModelIndex index = ui.tree->selectionModel()->selectedRows().front();

    QString path = model->path(index).path();
    QString pathMsg = isLocal ? QDir::toNativeSeparators(path) : path;

    QProcess *process = new QProcess;
    UseRclonePassword(process);
    process->setProgram(GetRclone());
    process->setArguments(QStringList() << "link" << GetRcloneConf()
                                        << GetRemoteModeRcloneOptions()
                                        << GetDefaultRcloneOptionsList()
                                        << remote + ":" + path);
    process->setProcessChannelMode(QProcess::MergedChannels);
    ProgressDialog *progress =
        new ProgressDialog("Fetch Public Link", "Running... ",
                           "Public link for: " + remote + ":" + pathMsg,
                           process, this, false, true);
    progress->expand();
    progress->allowToClose();
    progress->show();
  });

  QObject::connect(ui.upload, &QAction::triggered, this, [=]() {
    QString _remoteMode =
        setRemoteMode(ui.cb_GoogleDriveMode->currentIndex(), remoteType);

    QModelIndex index = ui.tree->selectionModel()->selectedRows().front();

    if (!model->isFolder(index)) {
      index = index.parent();
    }
    QDir path = model->path(index);

    TransferDialog t(false, false, remote, path, true, remoteType, _remoteMode,
                     this);
    if (t.exec() == QDialog::Accepted) {
      QString src = t.getSource();
      QString dst = t.getDest();

      QStringList args = t.getOptions();
      QString info;

      if (args.value(1) == "--dry-run") {
        info = QString("Dry run, %1 from %2").arg(t.getMode()).arg(src);
      } else {
        info = QString("%1 from %2").arg(t.getMode()).arg(src);
      }
      emit addTransfer(info, src, dst, args);
    }
  });

  QObject::connect(ui.download, &QAction::triggered, this, [=]() {
    QString _remoteMode =
        setRemoteMode(ui.cb_GoogleDriveMode->currentIndex(), remoteType);

    QModelIndex index = ui.tree->selectionModel()->selectedRows().front();
    QDir path = model->path(index);

    TransferDialog t(true, false, remote, path, model->isFolder(index),
                     remoteType, _remoteMode, this);
    if (t.exec() == QDialog::Accepted) {
      QString src = t.getSource();
      QString dst = t.getDest();

      QStringList args = t.getOptions();

      QString info;

      if (args.value(1) == "--dry-run") {
        info = QString("Dry run, %1 from %2").arg(t.getMode()).arg(src);
      } else {
        info = QString("%1 from %2").arg(t.getMode()).arg(src);
      }

      emit addTransfer(info, src, dst, args);
    }
  });

  QObject::connect(ui.getTree, &QAction::triggered, this, [=]() {
    setRemoteMode(ui.cb_GoogleDriveMode->currentIndex(), remoteType);

    QModelIndex index = ui.tree->selectionModel()->selectedRows().front();

    QString path = model->path(index).path();
    QString pathMsg = isLocal ? QDir::toNativeSeparators(path) : path;

    QProcess *process = new QProcess;
    UseRclonePassword(process);
    process->setProgram(GetRclone());
    process->setArguments(
        QStringList() << "tree"
                      << "-d" << GetRcloneConf() << GetRemoteModeRcloneOptions()
                      << GetDefaultRcloneOptionsList() << remote + ":" + path);
    process->setProcessChannelMode(QProcess::MergedChannels);
    ProgressDialog *progress = new ProgressDialog(
        "Show directories tree", "Running... ",
        "rclone tree -d " + remote + ":" + pathMsg, process, this, false);
    progress->expand();
    progress->allowToClose();
    progress->resize(1000, 600);
    progress->show();
  });

  QObject::connect(ui.getSize, &QAction::triggered, this, [=]() {
    setRemoteMode(ui.cb_GoogleDriveMode->currentIndex(), remoteType);

    QModelIndex index = ui.tree->selectionModel()->selectedRows().front();

    QString path = model->path(index).path();
    QString pathMsg = isLocal ? QDir::toNativeSeparators(path) : path;

    QProcess *process = new QProcess;
    UseRclonePassword(process);
    process->setProgram(GetRclone());
    process->setArguments(QStringList() << "size" << GetRcloneConf()
                                        << GetRemoteModeRcloneOptions()
                                        << GetDefaultRcloneOptionsList()
                                        << remote + ":" + path);
    process->setProcessChannelMode(QProcess::MergedChannels);
    ProgressDialog *progress = new ProgressDialog(
        "Get Size", "Running... ", "Size of: " + remote + ":" + pathMsg,
        process, this, false);

    progress->expand();
    progress->allowToClose();
    progress->show();
  });

  QObject::connect(ui.export_, &QAction::triggered, this, [=]() {
    setRemoteMode(ui.cb_GoogleDriveMode->currentIndex(), remoteType);

    QModelIndex index = ui.tree->selectionModel()->selectedRows().front();

    QString path_info = model->path(index).path();
    QString pathMsg = isLocal ? QDir::toNativeSeparators(path_info) : path_info;

    QDir path = model->path(index);
    ExportDialog e(remote, path, this);

    if (e.exec() == QDialog::Accepted) {
      QString dst = e.getDestination();
      bool txt = e.onlyFilenames();

      QFile *file = new QFile(dst);
      if (!file->open(QFile::WriteOnly)) {
        QMessageBox::warning(
            this, "Error",
            QString("Cannot open file '%1' for writing!").arg(dst));
        delete file;
        return;
      }

      QRegExp re(R"(^(\d+) (\d\d\d\d-\d\d-\d\d \d\d:\d\d:\d\d)\.\d+ (.+)$)");

      QProcess *process = new QProcess;
      UseRclonePassword(process);
      process->setProgram(GetRclone());
      process->setArguments(QStringList()
                            << GetRcloneConf() << GetRemoteModeRcloneOptions()
                            << GetDefaultRcloneOptionsList() << e.getOptions());
      process->setProcessChannelMode(QProcess::MergedChannels);

      ProgressDialog *progress = new ProgressDialog(
          "Export", "Running... ",
          "Exporting content of " + remote + ":" + pathMsg + "\nto " + dst,
          process, this, false);

      file->setParent(progress);

      QObject::connect(progress, &ProgressDialog::outputAvailable, this,
                       [=](const QString &output) {
                         QTextStream out(file);
                         out.setCodec("UTF-8");

                         for (const auto &line : output.split('\n')) {
                           if (re.exactMatch(line.trimmed())) {
                             QStringList cap = re.capturedTexts();

                             if (txt) {
                               out << cap[3] << '\n';
                             } else {
                               QString name = cap[3];
                               if (name.contains(' ') || name.contains(',') ||
                                   name.contains('"')) {
                                 name = '"' + name.replace("\"", "\"\"") + '"';
                               }
                               out << name << ',' << '"' << cap[2] << '"' << ','
                                   << cap[1].toULongLong() << '\n';
                             }
                           }
                         }
                       });

      progress->allowToClose();
      progress->show();
    }
  });

  QObject::connect(ui.actionCheck, &QAction::triggered, this, [=]() {
    setRemoteMode(ui.cb_GoogleDriveMode->currentIndex(), remoteType);

    QModelIndex index = ui.tree->selectionModel()->selectedRows().front();

    QString path_info = model->path(index).path();
    QString pathMsg = isLocal ? QDir::toNativeSeparators(path_info) : path_info;

    QDir path = model->path(index);
    CheckDialog e(remote, path, remoteType, this);

    if (e.exec() == QDialog::Accepted) {
      QString source = e.getSource();

      QProcess *process = new QProcess;
      UseRclonePassword(process);
      process->setProgram(GetRclone());

      process->setArguments(QStringList() << GetRcloneConf() << e.getOptions()
                                          << GetRemoteModeRcloneOptions()
                                          << GetDefaultRcloneOptionsList());
      process->setProcessChannelMode(QProcess::MergedChannels);

      QString checkcommand = "Integity check";
      if (!e.isCheck()) {
        checkcommand = "Integrity cryptcheck";
      }

      ProgressDialog *progress =
          new ProgressDialog(checkcommand, "Running... ",
                             "rclone " + e.getOptions().join(" ") + " " +
                                 GetRemoteModeRcloneOptions().join(" ") + " " +
                                 GetDefaultRcloneOptionsList().join(" "),
                             process, this, false);

      progress->expand();
      progress->allowToClose();
      progress->show();
    }
  });

  QObject::connect(ui.getInfo, &QAction::triggered, this, [=]() {
    setRemoteMode(ui.cb_GoogleDriveMode->currentIndex(), remoteType);

    QProcess *process = new QProcess;
    UseRclonePassword(process);
    process->setProgram(GetRclone());
    process->setArguments(QStringList()
                          << "about" << GetRcloneConf()
                          << GetRemoteModeRcloneOptions()
                          << GetDefaultRcloneOptionsList() << remote + ":");
    process->setProcessChannelMode(QProcess::MergedChannels);

    ProgressDialog *progress = new ProgressDialog(
        "Get remote Info", "Runnning... ", "rclone about " + remote + ":",
        process, this, false);

    progress->expand();
    progress->allowToClose();
    progress->show();
  });

  QObject::connect(
      model, &ItemModel::drop, this,
      [=](const QDir &path, const QModelIndex &parent) {
        setRemoteMode(ui.cb_GoogleDriveMode->currentIndex(), remoteType);

        qApp->setActiveWindow(this);
        QDir destPath = model->path(parent);
        QString dest = QFileInfo(path.path()).isDir()
                           ? destPath.filePath(path.dirName())
                           : destPath.path();

        TransferDialog t(false, true, remote, dest, true, remoteType,
                         remoteMode, this);
        t.setSource(path.path());

        if (t.exec() == QDialog::Accepted) {
          QString src = t.getSource();
          QString dst = t.getDest();

          QStringList args = t.getOptions();
          emit addTransfer(QString("%1 from %2").arg(t.getMode()).arg(src), src,
                           dst, args);
        }
      });

  QObject::connect(ui.tree, &QWidget::customContextMenuRequested, this,
                   [=](const QPoint &pos) {
                     setRemoteMode(ui.cb_GoogleDriveMode->currentIndex(),
                                   remoteType);

                     QMenu menu;
                     menu.addAction(ui.refresh);
                     menu.addSeparator();
                     menu.addAction(ui.mkdir);
                     menu.addAction(ui.rename);
                     menu.addAction(ui.move);
                     menu.addAction(ui.purge);
                     menu.addSeparator();
                     menu.addAction(ui.mount);
                     menu.addAction(ui.stream);
                     menu.addAction(ui.upload);
                     menu.addAction(ui.download);
                     menu.addSeparator();
                     menu.addAction(ui.getSize);
                     menu.addAction(ui.getTree);
                     menu.addAction(ui.link);
                     menu.addAction(ui.export_);
                     menu.addAction(ui.actionCheck);
                     menu.exec(ui.tree->viewport()->mapToGlobal(pos));
                   });

  if (isLocal) {
    QHash<QString, QPersistentModelIndex> drives;

    // QDir::drives is fast
    for (const auto &drive : QDir::drives()) {
      QString path = drive.path();
      QModelIndex index = model->addRoot(QDir::toNativeSeparators(path), path);
      drives.insert(path, index);
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)) && !(defined Q_OS_WIN)
    QThread *thread = new QThread(this);
    thread->start();

    QObject *worker = new QObject();
    worker->moveToThread(thread);

    QTimer::singleShot(0, worker, [=]() {
      QStorageInfo info;
      info.refresh();

      // QStorageInfo::mountedVolumes is slow :(
      for (const auto &volume : info.mountedVolumes()) {
        QString name = volume.name();
        if (!name.isEmpty()) {
          QString path = volume.rootPath();
          QString item =
              QString("%1 (%2)").arg(QDir::toNativeSeparators(path)).arg(name);
          QTimer::singleShot(0, this,
                             [=]() { model->rename(drives[path], item); });
        }
      }

      thread->quit();
      thread->deleteLater();
      worker->deleteLater();
    });
#endif

    ui.tree->selectionModel()->selectionChanged(QItemSelection(),
                                                QItemSelection());
  } else {
    QModelIndex index = model->addRoot("/", root);
    ui.tree->selectionModel()->select(
        index, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
    ui.tree->expand(index);
  }

  QShortcut *close = new QShortcut(QKeySequence::Close, this);
  QObject::connect(close, &QShortcut::activated, this, [=]() {
    auto tabs = qobject_cast<QTabWidget *>(parent);
    tabs->removeTab(tabs->indexOf(this));
  });

  QObject::connect(ui.shared, &QAction::triggered, [=]() {
    /*
      // connected when QComboBox cb_GoogleDriveMode emits signal
     currentIndexChanged(int)
      // in remote_widget.ui configured via in Qt Creator
     <connections>
      <connection>
       <sender>cb_GoogleDriveMode</sender>
       <signal>currentIndexChanged(int)</signal>
       <receiver>shared</receiver>
       <slot>trigger()</slot>
       <hints>
        <hint type="sourcelabel">
         <x>881</x>
         <y>27</y>
        </hint>
        <hint type="destinationlabel">
         <x>-1</x>
         <y>-1</y>
        </hint>
       </hints>
      </connection>
     </connections>
    */

    setRemoteMode(ui.cb_GoogleDriveMode->currentIndex(), remoteType);

    QModelIndex index = ui.tree->selectionModel()->selectedRows().front();
    QModelIndex top = index;
    while (!model->isTopLevel(top)) {
      top = top.parent();
    }
    ui.tree->selectionModel()->clear();
    ui.tree->selectionModel()->select(top, QItemSelectionModel::Select |
                                               QItemSelectionModel::Rows);
    model->refresh(top);
  });
}

RemoteWidget::~RemoteWidget() {}

QString setRemoteMode(int index, QString remoteType) {

  QString mode = "main";

  if (remoteType == "drive") {

    auto settings = GetSettings();
    switch (index) {

    case 0:
      settings->setValue("Settings/remoteMode", "main");
      mode = "main";
      break;
    case 1:
      settings->setValue("Settings/remoteMode", "shared");
      mode = "shared";
      break;
    case 2:
      settings->setValue("Settings/remoteMode", "trash");
      mode = "trash";
      break;
    }
  }

  return mode;
}

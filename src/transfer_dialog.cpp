#include "transfer_dialog.h"
#include "list_of_job_options.h"
#include "utils.h"

TransferDialog::TransferDialog(bool isDownload, bool isDrop,
                               const QString &remote, const QDir &path,
                               bool isFolder, const QString &remoteType,
                               const QString &remoteMode, QWidget *parent,
                               JobOptions *task, bool editMode)
    : QDialog(parent), mIsDownload(isDownload), mIsFolder(isFolder),
      mIsEditMode(editMode), mRemoteMode(remoteMode), mRemoteType(remoteType),
      mJobOptions(task) {

  ui.setupUi(this);

  auto settings = GetSettings();

  // set minimumWidth based on font size
  int fontsize = 0;
  fontsize = (settings->value("Settings/fontSize").toInt());
  setMinimumWidth(minimumWidth() + (fontsize * 50));

  // lock vertical resizing
  resize(0, 0);
  adjustSize();

  setMaximumHeight(this->height());
  setMinimumHeight(this->height());
  QTimer::singleShot(0, this, SLOT(size()));

  setWindowTitle(isDownload ? "Download" : "Upload");

  QString iconsColour = settings->value("Settings/iconsColour").toString();

  QString img_add = "";

  if (iconsColour == "white") {
    img_add = "_inv";
  }

  ui.buttonSourceFile->setIcon(
      QIcon(":media/images/qbutton_icons/file" + img_add + ".png"));
  ui.buttonSourceFolder->setIcon(
      QIcon(":media/images/qbutton_icons/folder" + img_add + ".png"));
  ui.buttonDest->setIcon(
      QIcon(":media/images/qbutton_icons/folder" + img_add + ".png"));
  ui.buttonDefaultSource->setIcon(
      QIcon(":media/images/qbutton_icons/restore" + img_add + ".png"));
  ui.buttonDefaultDest->setIcon(
      QIcon(":media/images/qbutton_icons/restore" + img_add + ".png"));

  ui.buttonSourceFile->setIconSize(QSize(24, 24));
  ui.buttonSourceFolder->setIconSize(QSize(24, 24));
  ui.buttonDest->setIconSize(QSize(24, 24));
  ui.buttonDefaultSource->setIconSize(QSize(24, 24));
  ui.buttonDefaultDest->setIconSize(QSize(24, 24));

  // only used for layout
  ui.comboBoxSpacer1->hide();
  ui.comboBoxSpacer2->hide();
  ui.comboBoxSpacer3->hide();
  ui.comboBoxSpacer4->hide();

  if (!mIsEditMode) {
    QPushButton *dryRun =
        ui.buttonBox->addButton("&Dry run", QDialogButtonBox::AcceptRole);
    ui.buttonBox->addButton("&Run", QDialogButtonBox::AcceptRole);
    QObject::connect(dryRun, &QPushButton::clicked, this,
                     [=]() { mDryRun = true; });
  }

  QPushButton *saveTask = ui.buttonBox->addButton(
      "&Save task", QDialogButtonBox::ButtonRole::ActionRole);

  QObject::connect(
      ui.buttonBox->button(QDialogButtonBox::RestoreDefaults),
      &QPushButton::clicked, this, [=]() {
        ui.cbSyncDelete->setCurrentIndex(0);
        // set combobox tooltips
        ui.cbSyncDelete->setItemData(0, "--delete-during", Qt::ToolTipRole);
        ui.cbSyncDelete->setItemData(1, "--delete-after", Qt::ToolTipRole);
        ui.cbSyncDelete->setItemData(2, "--delete-before", Qt::ToolTipRole);
        ui.checkSkipNewer->setChecked(false);
        ui.checkSkipNewer->setChecked(false);
        ui.checkSkipExisting->setChecked(false);
        ui.checkCompare->setChecked(true);
        ui.cbCompare->setCurrentIndex(0);
        // set combobox tooltips
        ui.cbCompare->setItemData(0, "default", Qt::ToolTipRole);
        ui.cbCompare->setItemData(1, "--checksum", Qt::ToolTipRole);
        ui.cbCompare->setItemData(2, "--ignore-size", Qt::ToolTipRole);
        ui.cbCompare->setItemData(3, "--size-only", Qt::ToolTipRole);
        ui.cbCompare->setItemData(4, "--checksum --ignore-size",
                                  Qt::ToolTipRole);
        //      ui.checkVerbose->setChecked(false);
        ui.checkSameFilesystem->setChecked(false);
        ui.checkDontUpdateModified->setChecked(false);
        ui.spinTransfers->setValue(4);
        ui.spinCheckers->setValue(8);
        ui.textBandwidth->clear();
        ui.textMinSize->clear();
        ui.textMinAge->clear();
        ui.textMaxAge->clear();
        ui.spinMaxDepth->setValue(0);
        ui.spinConnectTimeout->setValue(60);
        ui.spinIdleTimeout->setValue(300);
        ui.spinRetries->setValue(3);
        ui.spinLowLevelRetries->setValue(10);
        ui.checkDeleteExcluded->setChecked(false);
        ui.textExclude->clear();
        auto settings = GetSettings();
        if (isDownload) {
          // download
          ui.pte_textExtra->setPlainText(
              settings->value("Settings/defaultDownloadOptions").toString());
        } else {
          // upload
          ui.pte_textExtra->setPlainText(
              settings->value("Settings/defaultUploadOptions").toString());
        }
      });

  ui.buttonBox->button(QDialogButtonBox::RestoreDefaults)->click();

  QObject::connect(saveTask, &QPushButton::clicked, this, [=]() {
    // validate before saving task...
    if (ui.textDescription->text().isEmpty()) {
      QMessageBox::warning(this, "Warning",
                           "Please enter task description to Save!");
      ui.textDescription->setFocus(Qt::FocusReason::OtherFocusReason);
      return;
    }
    // even though the below does not match the condition on the Run buttons
    // it SEEMS like blanking either one would be a problem, right?
    if (ui.textDest->text().isEmpty() || ui.textSource->text().isEmpty()) {
      QMessageBox::warning(this, "Error",
                           "Invalid Task, source and destination required!");
      return;
    }
    JobOptions *jobo = getJobOptions();
    ListOfJobOptions::getInstance()->Persist(jobo);
    // always close on save
    // if (mIsEditMode)
    this->close();
  });

  QObject::connect(ui.buttonBox, &QDialogButtonBox::accepted, this,
                   &QDialog::accept);
  QObject::connect(ui.buttonBox, &QDialogButtonBox::rejected, this,
                   &QDialog::reject);

  QObject::connect(ui.buttonSourceFile, &QToolButton::clicked, this, [=]() {
    QString file = QFileDialog::getOpenFileName(this, "Choose file to upload");
    if (!file.isEmpty()) {
      if (!mIsEditMode) {
        ui.textSource->setText(QDir::toNativeSeparators(file));
        ui.textDest->setText(path.path());
      } else {
        ui.textSource->setText(QDir::toNativeSeparators(file));
      }
    }
  });

  QObject::connect(ui.buttonSourceFolder, &QToolButton::clicked, this, [=]() {
    auto settings = GetSettings();
    QString last_used_source_folder =
        (settings->value("Settings/lastUsedSourceFolder").toString());
    QString folder = QFileDialog::getExistingDirectory(
        this, "Choose folder to upload", last_used_source_folder,
        QFileDialog::ShowDirsOnly);

    if (!folder.isEmpty()) {

      // store new folder in lastUsedSourceFolder
      settings->setValue("Settings/lastUsedSourceFolder", folder);
      ui.textSource->setText(QDir::toNativeSeparators(folder));

      if (!mIsEditMode) {
        ui.textDest->setText(path.filePath(QFileInfo(folder).fileName()));
      }
    }
  });

  QObject::connect(ui.buttonDefaultSource, &QToolButton::clicked, this, [=]() {
    auto settings = GetSettings();
    QString default_folder =
        (settings->value("Settings/defaultUploadDir").toString());
    // store default folder in lastUsedSourceFolder
    settings->setValue("Settings/lastUsedSourceFolder", default_folder);
    ui.textSource->setText(QDir::toNativeSeparators(default_folder));
    if (!default_folder.isEmpty()) {
      if (!mIsEditMode) {
        ui.textDest->setText(
            path.filePath(QFileInfo(default_folder).fileName()));
      }
    } else {
      if (!mIsEditMode) {
        ui.textDest->setText(path.path());
      }
    };
  });

  QObject::connect(ui.buttonDest, &QToolButton::clicked, this, [=]() {
    auto settings = GetSettings();
    QString last_used_dest_folder =
        (settings->value("Settings/lastUsedDestFolder").toString());
    QString folder = QFileDialog::getExistingDirectory(
        this, "Choose destination folder", last_used_dest_folder,
        QFileDialog::ShowDirsOnly);

    if (!folder.isEmpty()) {
      // store new folder in lastUsedDestFolder
      settings->setValue("Settings/lastUsedDestFolder", folder);

      if (!mIsEditMode) {
        if (isFolder) {
          ui.textDest->setText(
              QDir::toNativeSeparators(folder + "/" + path.dirName()));
        } else {
          ui.textDest->setText(QDir::toNativeSeparators(folder));
        }

      } else {

        ui.textDest->setText(QDir::toNativeSeparators(folder));
      }
    }
  });

  QObject::connect(ui.buttonDefaultDest, &QToolButton::clicked, this, [=]() {
    auto settings = GetSettings();
    QString default_folder =
        (settings->value("Settings/defaultDownloadDir").toString());
    // store default_folder in lastUsedDestFolder
    settings->setValue("Settings/lastUsedDestFolder", default_folder);
    if (!default_folder.isEmpty()) {
      if (!mIsEditMode) {

        if (isFolder) {
          ui.textDest->setText(
              QDir::toNativeSeparators(default_folder + "/" + path.dirName()));
        } else {
          ui.textDest->setText(QDir::toNativeSeparators(default_folder));
        }
      } else {
        ui.textDest->setText(QDir::toNativeSeparators(default_folder));
      }
    } else {
      ui.textDest->setText("");
    };
  });

  settings->beginGroup("Transfer");
  ReadSettings(settings.get(), this);
  settings->endGroup();

  ui.buttonSourceFile->setVisible(!isDownload);
  ui.buttonSourceFolder->setVisible(!isDownload);
  ui.buttonDefaultSource->setVisible(!isDownload);

  ui.buttonDest->setVisible(isDownload);
  ui.buttonDefaultDest->setVisible(isDownload);

  if (mRemoteType != "drive") {
    ui.remoteMode->setText(mRemoteType);
  } else {
    if (mRemoteMode == "main") {
      ui.remoteMode->setText("drive");
    }
    if (mRemoteMode == "trash") {
      ui.remoteMode->setText("drive, --drive-trashed-only");
    }
    if (mRemoteMode == "shared") {
      ui.remoteMode->setText("drive, --drive-shared-with-me");
    }
  }

  ui.textDescription->clear();

  if (mIsEditMode && mJobOptions != nullptr) {
    // it's not really valid for only one of these things to be true.
    // when operating on an existing instance i.e. a saved task,

    if (isDownload) {
      ui.l_destRemote->hide();
      ui.buttonSourceFile->setVisible(false);
      ui.buttonSourceFolder->setVisible(false);
      ui.buttonDefaultSource->setVisible(false);
      ui.l_sourceRemote->setEnabled(false);
    } else {
      ui.l_sourceRemote->hide();
      ui.buttonDefaultDest->setVisible(false);
      ui.buttonDest->setVisible(false);
      ui.l_destRemote->setEnabled(false);
    }

    putJobOptions();

  } else {

    // set source and destination using defaults
    if (isDownload) {
      // download
      ui.l_destRemote->hide();
      ui.l_sourceRemote->setEnabled(false);
      ui.l_sourceRemote->setText(remote + ":");

      ui.pte_textExtra->setPlainText(
          settings->value("Settings/defaultDownloadOptions").toString());
      ui.textSource->setText(path.path());
      QString folder;
      QString default_folder =
          (settings->value("Settings/defaultDownloadDir").toString());
      QString last_used_dest_folder =
          (settings->value("Settings/lastUsedDestFolder").toString());

      if (last_used_dest_folder.isEmpty()) {
        folder = default_folder;
      } else {
        folder = last_used_dest_folder;
      };

      if (!folder.isEmpty()) {
        if (isFolder) {
          ui.textDest->setText(
              QDir::toNativeSeparators(folder + "/" + path.dirName()));
        } else {
          ui.textDest->setText(QDir::toNativeSeparators(folder));
        }
      }

    } else {
      // upload
      ui.l_sourceRemote->hide();
      ui.l_destRemote->setEnabled(false);
      ui.l_destRemote->setText(remote + ":");

      ui.pte_textExtra->setPlainText(
          settings->value("Settings/defaultUploadOptions").toString());
      QString folder;
      QString default_folder =
          (settings->value("Settings/defaultUploadDir").toString());
      QString last_used_source_folder =
          (settings->value("Settings/lastUsedSourceFolder").toString());

      if (last_used_source_folder.isEmpty()) {
        folder = default_folder;
      } else {
        folder = last_used_source_folder;
      };

      // if upload initiated from drag and drop we dont use default upload
      // folder
      if (!isDrop) {
        ui.textSource->setText(QDir::toNativeSeparators(folder));
        if (!folder.isEmpty()) {
          ui.textDest->setText(path.filePath(QFileInfo(folder).fileName()));
        } else {
          ui.textDest->setText(path.path());
        }
      } else {
        // when dropping to root folder
        if (path.path() == ".") {
          ui.textDest->setText("");
        } else {
          ui.textDest->setText(path.path());
        }
      };
    };
  }
}

TransferDialog::~TransferDialog() {
  if (result() == QDialog::Accepted) {
    auto settings = GetSettings();
    settings->beginGroup("Transfer");
    WriteSettings(settings.get(), this);
    settings->remove("textSource");
    settings->remove("textDest");
    settings->endGroup();
  }
}

void TransferDialog::size() { resize(0, maximumHeight()); }

void TransferDialog::setSource(const QString &path) {
  ui.textSource->setText(QDir::toNativeSeparators(path));
}

QString TransferDialog::getMode() const {
  if (ui.rbCopy->isChecked()) {
    return "Copy";
  } else if (ui.rbMove->isChecked()) {
    return "Move";
  } else if (ui.rbSync->isChecked()) {
    return "Sync";
  }

  return QString();
}

QString TransferDialog::getSource() const { return ui.textSource->text(); }

QString TransferDialog::getDest() const { return ui.textDest->text(); }

QStringList TransferDialog::getOptions() {
  JobOptions *jobo = getJobOptions();
  QStringList newWay = jobo->getOptions();
  return newWay;
}

/*
 * Apply the displayed/edited values on the UI to the
 * JobOptions object.
 *
 * This needs to be edited whenever options are added or changed.
 */
JobOptions *TransferDialog::getJobOptions() {
  if (mJobOptions == nullptr)
    mJobOptions = new JobOptions(mIsDownload);

  if (ui.rbCopy->isChecked()) {
    mJobOptions->operation = JobOptions::Copy;
    mJobOptions->sync = false;
  } else if (ui.rbMove->isChecked()) {
    mJobOptions->operation = JobOptions::Move;
    mJobOptions->sync = false;
  } else if (ui.rbSync->isChecked()) {
    mJobOptions->operation = JobOptions::Sync;
  }

  mJobOptions->dryRun = mDryRun;
  ;

  if (ui.rbSync->isChecked()) {
    mJobOptions->sync = true;
    switch (ui.cbSyncDelete->currentIndex()) {
    case 0:
      mJobOptions->syncTiming = JobOptions::During;
      break;
    case 1:
      mJobOptions->syncTiming = JobOptions::After;
      break;
    case 2:
      mJobOptions->syncTiming = JobOptions::Before;
      break;
    }
  }

  mJobOptions->skipNewer = ui.checkSkipNewer->isChecked();
  mJobOptions->skipExisting = ui.checkSkipExisting->isChecked();

  if (ui.checkCompare->isChecked()) {
    mJobOptions->compare = true;
    switch (ui.cbCompare->currentIndex()) {
    case 0:
      mJobOptions->compareOption = JobOptions::SizeAndModTime;
      break;
    case 1:
      mJobOptions->compareOption = JobOptions::Checksum;
      break;
    case 2:
      mJobOptions->compareOption = JobOptions::IgnoreSize;
      break;
    case 3:
      mJobOptions->compareOption = JobOptions::SizeOnly;
      break;
    case 4:
      mJobOptions->compareOption = JobOptions::ChecksumIgnoreSize;
      break;
    }
  } else {
    mJobOptions->compare = false;
  };

  //    mJobOptions->verbose = ui.checkVerbose->isChecked();
  mJobOptions->sameFilesystem = ui.checkSameFilesystem->isChecked();
  mJobOptions->dontUpdateModified = ui.checkDontUpdateModified->isChecked();

  mJobOptions->transfers = ui.spinTransfers->text();
  mJobOptions->checkers = ui.spinCheckers->text();
  mJobOptions->bandwidth = ui.textBandwidth->text();
  mJobOptions->minSize = ui.textMinSize->text();
  mJobOptions->minAge = ui.textMinAge->text();
  mJobOptions->maxAge = ui.textMaxAge->text();
  mJobOptions->maxDepth = ui.spinMaxDepth->value();

  mJobOptions->connectTimeout = ui.spinConnectTimeout->text();
  mJobOptions->idleTimeout = ui.spinIdleTimeout->text();
  mJobOptions->retries = ui.spinRetries->text();
  mJobOptions->lowLevelRetries = ui.spinLowLevelRetries->text();
  mJobOptions->deleteExcluded = ui.checkDeleteExcluded->isChecked();

  mJobOptions->excluded = ui.textExclude->toPlainText().trimmed();
  mJobOptions->extra = ui.pte_textExtra->toPlainText().trimmed();

  mJobOptions->isFolder = mIsFolder;

  if (mIsDownload) {
    mJobOptions->source = ui.l_sourceRemote->text() + ui.textSource->text();
    mJobOptions->dest = ui.textDest->text();
  } else {
    mJobOptions->source = ui.textSource->text();
    mJobOptions->dest = ui.l_destRemote->text() + ui.textDest->text();
  }

  mJobOptions->description = ui.textDescription->text();
  mJobOptions->remoteType = mRemoteType;
  mJobOptions->remoteMode = mRemoteMode;

  return mJobOptions;
}

/*
 * Apply the JobOptions object to the displayed widget values.
 *
 * It could be "better" to use a two-way binding mechanism, but
 * if used that should be global to the app; and anyway doing
 * it this old primitive way makes it easier when the user wants
 * to not save changes...
 */
void TransferDialog::putJobOptions() {
  ui.rbCopy->setChecked(mJobOptions->operation == JobOptions::Copy);
  ui.rbMove->setChecked(mJobOptions->operation == JobOptions::Move);
  ui.rbSync->setChecked(mJobOptions->operation == JobOptions::Sync);

  mDryRun = mJobOptions->dryRun;
  ui.rbSync->setChecked(mJobOptions->sync);

  ui.cbSyncDelete->setCurrentIndex((int)mJobOptions->syncTiming);
  // set combobox tooltips
  ui.cbSyncDelete->setItemData(0, "--delete-during", Qt::ToolTipRole);
  ui.cbSyncDelete->setItemData(1, "--delete-after", Qt::ToolTipRole);
  ui.cbSyncDelete->setItemData(2, "--delete-before", Qt::ToolTipRole);

  ui.checkSkipNewer->setChecked(mJobOptions->skipNewer);
  ui.checkSkipExisting->setChecked(mJobOptions->skipExisting);

  ui.checkCompare->setChecked(mJobOptions->compare);

  ui.cbCompare->setCurrentIndex(mJobOptions->compareOption);
  // set combobox tooltips
  ui.cbCompare->setItemData(0, "default", Qt::ToolTipRole);
  ui.cbCompare->setItemData(1, "--checksum", Qt::ToolTipRole);
  ui.cbCompare->setItemData(2, "--ignore-size", Qt::ToolTipRole);
  ui.cbCompare->setItemData(3, "--size-only", Qt::ToolTipRole);
  ui.cbCompare->setItemData(4, "--checksum --ignore-size", Qt::ToolTipRole);
  // ui.checkVerbose->setChecked(mJobOptions->verbose);
  ui.checkSameFilesystem->setChecked(mJobOptions->sameFilesystem);
  ui.checkDontUpdateModified->setChecked(mJobOptions->dontUpdateModified);

  ui.spinTransfers->setValue(mJobOptions->transfers.toInt());
  ui.spinCheckers->setValue(mJobOptions->checkers.toInt());
  ui.textBandwidth->setText(mJobOptions->bandwidth);
  ui.textMinSize->setText(mJobOptions->minSize);
  ui.textMinAge->setText(mJobOptions->minAge);
  ui.textMaxAge->setText(mJobOptions->maxAge);
  ui.spinMaxDepth->setValue(mJobOptions->maxDepth);

  ui.spinConnectTimeout->setValue(mJobOptions->connectTimeout.toInt());
  ui.spinIdleTimeout->setValue(mJobOptions->idleTimeout.toInt());
  ui.spinRetries->setValue(mJobOptions->retries.toInt());
  ui.spinLowLevelRetries->setValue(mJobOptions->lowLevelRetries.toInt());
  ui.checkDeleteExcluded->setChecked(mJobOptions->deleteExcluded);

  ui.textExclude->setPlainText(mJobOptions->excluded);
  ui.pte_textExtra->setPlainText(mJobOptions->extra);

  if (mJobOptions->jobType == JobOptions::JobType::Download) {
    ui.l_sourceRemote->setText(
        (mJobOptions->source).left((mJobOptions->source).indexOf(":") + 1));
    ui.textSource->setText((mJobOptions->source)
                               .right((mJobOptions->source).length() -
                                      (mJobOptions->source).indexOf(":") - 1));
    ui.textDest->setText(mJobOptions->dest);
  } else {
    ui.textSource->setText(mJobOptions->source);
    ui.l_destRemote->setText(
        (mJobOptions->dest).left((mJobOptions->dest).indexOf(":") + 1));
    ui.textDest->setText((mJobOptions->dest)
                             .right((mJobOptions->dest).length() -
                                    (mJobOptions->dest).indexOf(":") - 1));
  }

  ui.textDescription->setText(mJobOptions->description);

  if (mJobOptions->remoteMode == "") {
    // task saved before googleDriveMode was added
    if (mJobOptions->DriveSharedWithMe) {
      ui.remoteMode->setText("drive, --drive-shared-with-me");
    } else {
      ui.remoteMode->setText("drive");
    }
  } else {
    if (mJobOptions->remoteType != "drive") {
      ui.remoteMode->setText(mJobOptions->remoteType);
    } else {
      if (mJobOptions->remoteMode == "main") {
        ui.remoteMode->setText("drive");
      }
      if (mJobOptions->remoteMode == "trash") {
        ui.remoteMode->setText("drive, --drive-trashed-only");
      }
      if (mJobOptions->remoteMode == "shared") {
        ui.remoteMode->setText("drive, --drive-shared-with-me");
      }
    }
  }
}

void TransferDialog::done(int r) {
  if (r == QDialog::Accepted) {
    if (mIsDownload) {
      if (ui.textDest->text().isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter destination!");
        return;
      }
    } else {
      if (ui.textSource->text().isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter source!");
        return;
      }
    }
  }
  QDialog::done(r);
}

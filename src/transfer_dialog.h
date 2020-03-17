#pragma once

#include "job_options.h"
#include "pch.h"
#include "ui_transfer_dialog.h"

class TransferDialog : public QDialog {
  Q_OBJECT

public:
  TransferDialog(bool isDownload, bool isDrop, const QString &remote,
                 const QDir &path, bool isFolder, const QString &remoteType,
                 const QString &remoteMode, QWidget *parent = nullptr,
                 JobOptions *task = nullptr, bool editMode = false);
  ~TransferDialog();

  void setSource(const QString &path);

  QString getMode() const;
  QString getSource() const;
  QString getDest() const;
  QStringList getOptions();

  JobOptions *getJobOptions();

private:
  Ui::TransferDialog ui;

  bool mIsDownload;
  bool mDryRun = false;
  bool mIsFolder;
  bool mIsEditMode;
  QString mRemoteMode;
  QString mRemoteType;

  JobOptions *mJobOptions;

  void putJobOptions();

  void done(int r) override;

signals:
  void tasksListChanged();
};

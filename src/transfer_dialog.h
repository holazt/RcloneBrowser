#pragma once

#include "job_options.h"
#include "pch.h"
#include "ui_transfer_dialog.h"

class TransferDialog : public QDialog {
  Q_OBJECT

public:
  TransferDialog(bool isDownload, bool isDrop, const QString &remote,
                 const QDir &path, bool isFolder, const QString &remoteType,
                 const QString &remoteMode, bool isMultiItem,
                 const QStringList &includeItems, QWidget *parent = nullptr,
                 JobOptions *task = nullptr, bool editMode = false);
  ~TransferDialog();

  void setSource(const QString &path);

  QString getMode() const;
  QString getSource() const;
  QString getDest() const;
  QStringList getOptions();
  bool getDryRun();
  QString getTaskId();
  bool getAddToQueue();

  JobOptions *getJobOptions(JobOptions *taskOptions);

private:
  Ui::TransferDialog ui;

  bool mIsDownload;
  bool mDryRun = false;
  bool mIsFolder;
  bool mIsEditMode;
  QString mRemote;
  QString mRemoteMode;
  QString mRemoteType;
  QString mTaskId = "";
  bool mAddToQueue = false;

  bool mIsMultiItem;

  bool mFirstRun = true;

  JobOptions *mJobOptions;
  JobOptions *mJobOptionsRcloneCmd;

  void save_AutoName_AddToQueue();
  bool saveTaskToFile();
  void transferWriteSettings();
  void generateAutoTaskName();

  void rcloneCmdUpdate();

  void putJobOptions();

  void done(int r) override;

private slots:
  void size();
  void showToolTip();

signals:
  void tasksListChanged();
};

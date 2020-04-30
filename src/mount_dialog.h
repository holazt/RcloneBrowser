#pragma once

#include "job_options.h"
#include "pch.h"
#include "ui_mount_dialog.h"

class MountDialog : public QDialog {
  Q_OBJECT

public:
  MountDialog(const QString &remote, const QDir &path,
              const QString &remoteType, const QString &remoteMode,
              QWidget *parent = nullptr, JobOptions *task = nullptr,
              bool editMode = false);
  ~MountDialog();

  QString getMountPoint() const;
  QStringList getOptions();
  QString getScript() const;

private:
  bool validateOptions();
  Ui::MountDialog ui;
  QString mTarget;

  void done(int r) override;
  void putJobOptions();

  QString mRemoteMode;
  QString mRemoteType;
  int mRcPort;
  bool mIsEditMode;

  JobOptions *mJobOptions;

  void mountOptionsWriteSettings();

private slots:
  void shrink();

signals:
  void tasksListChanged();
};

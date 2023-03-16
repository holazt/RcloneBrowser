#pragma once

#include "pch.h"
#include "ui_mount_widget.h"

class MountWidget : public QWidget {
  Q_OBJECT

public:
  MountWidget(QProcess *process, const QString &remote, const QString &folder,
              const QStringList &args, const QString &script,
              const QString &uniqueID, const QString &info,
              QWidget *parent = nullptr);
  ~MountWidget();
  bool isRunning = true;
  QDateTime getStartDateTime();
  QString getStatus();

public slots:
  void cancel();
  QString getUniqueID();
  QString getUnmountingError();

signals:
  void finished();
  void closed();

private:
  Ui::MountWidget ui;

  bool mScriptRunning = false;
  bool mScriptStarted = false;

  QProcess *mProcess;
  QProcess mScriptProcess;

  QString mUnmountingError = "0";
  QString mRcPort = "0";
  QStringList mArgs;
  QString mUniqueID = "";

  // 0 - running, 1 - finished, 2 - error
  // we add "z" to make mounts listed after transfers
  QString mStatus = "0_zmount_mounted";

  QDateTime mStartDateTime = QDateTime::currentDateTime();
  QDateTime mFinishDateTime;
  void updateStartFinishInfo();
};

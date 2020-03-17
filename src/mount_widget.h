#pragma once

#include "pch.h"
#include "ui_mount_widget.h"

class MountWidget : public QWidget {
  Q_OBJECT

public:
  MountWidget(QProcess *process, const QString &remote, const QString &folder,
              const QStringList &args, const QString &script,
              const QString &uniqueID, QWidget *parent = nullptr);
  ~MountWidget();
  bool isRunning = true;

public slots:
  void cancel();
  QString getUniqueID();

signals:
  void finished();
  void closed();

private:
  Ui::MountWidget ui;

  bool mScriptRunning = false;
  bool mScriptStarted = false;
  QProcess *mProcess;
  QProcess *mScriptProcess;
  QString mRcPort = "0";
  QStringList mArgs;
  QString mUniqueID = "";
};

#pragma once

#include "icon_cache.h"
#include "job_options.h"
#include "pch.h"
#include "ui_main_window.h"

class JobWidget;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow();
  ~MainWindow();

private slots:
  void rcloneGetVersion();
  void rcloneConfig();
  void rcloneListRemotes();
  void listTasks();

  void addTransfer(const QString &message, const QString &source,
                   const QString &dest, const QStringList &args, const QString &uniqueId, const QString &transferMode);
  void addMount(const QString &remote, const QString &folder, const QString &remoteType);
  void addStream(const QString &remote, const QString &stream);

  void slotCloseTab(int index);

  void saveQueueFile(void);

private:
  Ui::MainWindow ui;

  QSystemTrayIcon mSystemTray;
  JobWidget *mLastFinished = nullptr;

  bool mAlwaysShowInTray;
  bool mCloseToTray;
  bool mNotifyFinishedTransfers;

  QLabel *mStatusMessage;

  IconCache mIcons;

  bool mFirstTime = true;
  int mJobCount = 0;

  // keep track of number of active transfers
  int mTransferJobCount = 0;

  // false = Queue Paused, true = Queue running
  int mQueueStatus = false;

  // number of queued tasks
  int mQueueCount = 0;

  // make queue logic aware that app is quiting
  // so job is not removed from the queue
  bool mAppQuittingStatus = false;

  bool canClose();
  void closeEvent(QCloseEvent *ev) override;
  bool getConfigPassword(QProcess *p);

  void addEmptyJobsMessage();

  void runItem(JobOptionsListWidgetItem *item, const QString &transferMode, bool dryrun = false);
  void editSelectedTask();
  QIcon mUploadIcon;
  QIcon mDownloadIcon;

  void addTasksToQueue();
};

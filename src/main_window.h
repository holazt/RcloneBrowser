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
                   const QString &dest, const QStringList &args,
                   const QString &uniqueId, const QString &transferMode);
  void addStream(const QString &remote, const QString &stream,
                 const QString &remoteType);

  void addNewMount(const QString &remote, const QString &folder,
                   const QString &remoteType, const QStringList &args,
                   const QString &script, const QString &uniqueId);

  void runQueueScript(const QString &script);

  void slotCloseTab(int index);

  void saveQueueFile(void);

  void autoStartMounts(void);

  // quit RB but only when all processes finished
  void quitApp(void);

private:
  Ui::MainWindow ui;

  QSystemTrayIcon mSystemTray;
  JobWidget *mLastFinished = nullptr;

  bool mAlwaysShowInTray;
  bool mCloseToTray;
  bool mNotifyFinishedTransfers;
  bool mSoundNotif;

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

  // sort QListWidget view/selection
  QList<QListWidgetItem *> sortListWidget(const QList<QListWidgetItem *> &list,
                                          bool sortOrder = false);

  // set screen buttons logic mess in one place
  void setQueueButtons(void);
  void setTasksButtons(void);

  void addEmptyJobsMessage();

  void runItem(JobOptionsListWidgetItem *item, const QString &transferMode,
               bool dryrun = false);
  void editSelectedTask();
  QIcon mUploadIcon;
  QIcon mDownloadIcon;
  QIcon mMountIcon;
  // if waiting for processes we show dialog - this is used to calculate delay
  int mQuitInfoDelay = 0;

  void addTasksToQueue();
};

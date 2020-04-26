#pragma once

#include "item_model.h"
#include "pch.h"
#include "ui_remote_widget.h"

QString setRemoteMode(int, QString);

class IconCache;

class RemoteWidget : public QWidget {
  Q_OBJECT

public:
  RemoteWidget(IconCache *icons, const QString &remote,
               const QString &remoteType, QWidget *parent = nullptr);
  ~RemoteWidget();

signals:
  void addTransfer(const QString &message, const QString &source,
                   const QString &remote, const QStringList &args,
                   const QString &uniqueId, const QString &transferMode,
                   const QString &requestId);
  void addStream(const QString &remote, const QString &stream,
                 const QString &remoteType);
  void addNewMount(const QString &remote, const QString &folder,
                   const QString &remoteType, const QStringList &args,
                   const QString &script, const QString &uniqueId,
                   const QString &info);
  void addSavedTransfer(const QString &uniqueId, bool dryRun, bool addToQueue);

private slots:

  void initialModelLoading();
  void preemptiveLoadingProcessor();

  void switchRemoteType();

  void processSelection(const QItemSelection &selected,
                        const QItemSelection &deselected);

  void refreshAfterMove();

private:
  Ui::RemoteWidget ui;
  bool mButtonToolsState = false;

  QString mRemoteType;

  ItemModel *model;
  QModelIndex mRootIndex;

  // get include patterns from selection
  QStringList
  getSelectionFilteringPatterns(const QModelIndexList &multiSelection);

  // folders' indexes with already preloaded subfolders
  QModelIndexList mPreemptiveLoadingListDone;
  // folders already refreshed itself
  QModelIndexList mPreemptiveLoadingListDoneNodes;
  // queue of folders to refresh (preload)
  QModelIndexList mPreemptiveLoadingList;
  // queue of expanded folders still refreshing to refresh later all subfolders
  QModelIndexList mPreemptiveLoadingListPending;

  // mPreemptiveLoadingList deduplication status (run dedup when true)
  bool mPreemptiveLoadingListDups = true;
  // preemtive actions mutex
  QMutex preemptiveLoadingProcessorMutex;
  // preemptive loading on/off (true/false)
  bool mPreemptiveLoading = true;

  // Qtimer count for flashing info
  int mCount = 0;

  // Qtimer count for preeemptive loading level update
  int mCountLevel = 0;

  // how many rclone lsl/lsd processes for preeemptive loading
  int mMaxRcloneLsProcessCount = 10;

  // two indexes to refresh after move
  QModelIndex mSrcIndex;
  QModelIndex mDestIndex;
};

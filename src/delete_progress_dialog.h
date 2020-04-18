#pragma once

#include "pch.h"
#include "ui_delete_progress_dialog.h"

class DeleteProgressDialog : public QDialog {
  Q_OBJECT

public:
  DeleteProgressDialog(const QList<QStringList> &pDataList,
                       QWidget *parent = nullptr, bool close = true);

  ~DeleteProgressDialog();

  void expand();
  void allowToClose();

signals:
  void outputAvailable(const QString &output) const;

private slots:

  // start single rclone purge/delete
  void addNewDeleteProcess(const QStringList &args);

  // process queue of items to delete
  void deleteProcessor();

  // quit dialog via delayed slot so for a second final result is visible
  void emitAccept();

private:
  void closeEvent(QCloseEvent *ev) override;

  Ui::DeleteProgressDialog ui;
  int mWidth;
  int mMinimumWidth;
  int mMinimumHeight;
  int mHeight;
  bool mIsRunning = true;

  QPushButton *mCancelButton;

  QList<QStringList> mItemsToDelete;

  int mTotalToDelete = 0;
  int mDeleting = 0;
  int mDeleted = 0;

  int mMaxRcloneLsProcessCount;
  bool mPreemptiveLoading;

  bool mClose = true;
  bool mError = false;
  bool mCancelled = false;
  bool mQuitting = false;
  int mDeletingProcessesCount = 0;

  void updateInfo();

  // deleteProcessor actions mutex
  QMutex deleteProcessorMutex;
  QMutex deleteProcessMutex;
};

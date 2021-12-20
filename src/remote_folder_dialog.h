#pragma once

#include "item_model.h"
#include "job_options.h"
#include "pch.h"
#include "ui_remote_folder_dialog.h"

QString setRemoteMode(int, QString);

class IconCache;
class QWidget;

class RemoteFolderDialog : public QDialog {
  Q_OBJECT

public:
  RemoteFolderDialog(bool isMove, bool isMultiSelect,
                     const QStringList includedList,
                     const QModelIndex &sourceIndex, const QString &remote,
                     const QString &remoteType, const QString &remoteMode,
                     bool isLocal, const QModelIndex &rootIndex,
                     ItemModel *model = nullptr, QWidget *parent = nullptr);

  ~RemoteFolderDialog();

  //  QString getMountPoint() const;
  //  QStringList getOptions();
  //  QString getScript() const;

  QModelIndex pDestIndex;
  QModelIndex pSourceIndex;

  QStringList getOptions();
  QStringList getFilterList();

private:
  //  bool validateOptions();
  Ui::RemoteFolderDialog ui;
  QModelIndex mRootIndex;

private slots:
  //  void shrink();

signals:
  //  void tasksListChanged();
};

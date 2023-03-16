#pragma once
#include "pch.h"

// Subclass QFileDialog for customise allow select both file/folder
class FileDialog : public QFileDialog {
  Q_OBJECT
private:
  QListView *m_listView;
  QTreeView *m_treeView;
  QPushButton *m_btnOpen;
  QStringList m_selectedFiles;
  bool mIsDownload;

private slots:
  void clearSelection();
  void resizeColums();

public slots:
  void chooseClicked();

public:
  // true: Download dialog - select one folder, false: Upload dialog - select
  // mutiple files and folders
  FileDialog(bool isDownload);

  ~FileDialog();

  QStringList selectedFiles();

  bool eventFilter(QObject *watched, QEvent *event);
};

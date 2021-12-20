#pragma once

#include "pch.h"
#include "ui_progress_dialog.h"

class ProgressDialog : public QDialog {
  Q_OBJECT

public:
  ProgressDialog(const QString &title, const QString &operation,
                 const QString &message, QProcess *process,
                 QWidget *parent = nullptr, bool close = true,
                 bool trim = false, QString toolTip = "");
  ~ProgressDialog();

  void expand();
  void allowToClose();

signals:
  void outputAvailable(const QString &output) const;

private:
  void closeEvent(QCloseEvent *ev) override;

  Ui::ProgressDialog ui;
  int mWidth;
  int mMinimumWidth;
  int mMinimumHeight;
  int mHeight;
  bool mIsRunning = true;
};

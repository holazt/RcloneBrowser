#pragma once

#include "pch.h"
#include "ui_job_widget.h"

class JobWidget : public QWidget {
  Q_OBJECT

public:
  JobWidget(QProcess *process, const QString &info, const QStringList &args,
            const QString &source, const QString &dest, const QString &uniqueID,
            const QString &transferMode, QWidget *parent = nullptr);
  ~JobWidget();

  void showDetails();
  bool isRunning = true;

public slots:
  void cancel();
  QString getUniqueID();
  QString getTransferMode();

signals:
  void finished(const QString &info);
  void closed();

private:
  Ui::JobWidget ui;

  QProcess *mProcess;
  int mLines = 0;

  QStringList mArgs;
  QHash<QString, QLabel *> mActive;
  QSet<QLabel *> mUpdated;

  QString mUniqueID = "";
  QString mTransferMode = "";
};

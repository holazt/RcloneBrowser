#pragma once

#include "pch.h"
#include "ui_stream_widget.h"

class StreamWidget : public QWidget {
  Q_OBJECT

public:
  StreamWidget(QProcess *rclone, QProcess *player, const QString &remote,
               const QString &stream, const QStringList &args,
               QWidget *parent = nullptr);
  ~StreamWidget();
  bool isRunning = true;
  QDateTime getStartDateTime();
  QString getStatus();

public slots:
  void cancel();

signals:
  void finished();
  void closed();

private:
  Ui::StreamWidget ui;

  QProcess *mRclone;
  QProcess *mPlayer;

  QStringList mArgs;

  // 0 - running, 1 - finished, 2 - error
  QString mStatus = "0_stream_streaming";

  QDateTime mStartDateTime = QDateTime::currentDateTime();
  QDateTime mFinishDateTime;
  void updateStartFinishInfo();
};

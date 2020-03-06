#pragma once

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
                   const QString &remote, const QStringList &args, const QString &uniqueId, const QString &transferMode);
  void addMount(const QString &remote, const QString &folder,
                const QString &remoteType);
  void addStream(const QString &remote, const QString &stream);

private:
  Ui::RemoteWidget ui;
};

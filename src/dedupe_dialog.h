#pragma once

#include "pch.h"
#include "ui_dedupe_dialog.h"

class DedupeDialog : public QDialog {
  Q_OBJECT

public:
  DedupeDialog(const QString &remote, const QDir &path,
               const QString &remoteType, QWidget *parent = nullptr);
  ~DedupeDialog();

  //  QString getSource() const;
  //  bool isCheck() const;
  QStringList getOptions() const;
  bool isDryRun() const;

private:
  Ui::DedupeDialog ui;
  QString mTarget;
  bool mDryRun = false;

  void done(int r) override;
};

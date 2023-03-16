#pragma once

#include "pch.h"
#include "ui_check_dialog.h"

class CheckDialog : public QDialog {
  Q_OBJECT

public:
  CheckDialog(const QString &remote, const QDir &path,
              const QString &remoteType, QWidget *parent = nullptr);
  ~CheckDialog();

  QString getSource() const;
  bool isCheck() const;
  QStringList getOptions() const;

private:
  Ui::CheckDialog ui;
  QString mTarget;

  void done(int r) override;
};

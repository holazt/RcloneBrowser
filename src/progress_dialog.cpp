#include "progress_dialog.h"

ProgressDialog::ProgressDialog(const QString &title, const QString &operation,
                               const QString &message, QProcess *process,
                               QWidget *parent, bool close, bool trim)
    : QDialog(parent) {
  ui.setupUi(this);
  resize(width(), 0);

  setWindowTitle(title);

  ui.labelOperation->setText(operation);

  ui.labelOperation->setStyleSheet(
      "QLabel { color: green; font-weight: bold; }");

  ui.labelInfo->setText(message);

  ui.output->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

  ui.output->setVisible(false);

  // set default arrow
  ui.buttonShowOutput->setIcon(
      QIcon(":remotes/images/qbutton_icons/vrightarrow.png"));
  ui.buttonShowOutput->setIconSize(QSize(24, 24));

  QObject::connect(ui.buttonBox, &QDialogButtonBox::rejected, this,
                   &QDialog::reject);

  QObject::connect(
      ui.buttonShowOutput, &QPushButton::toggled, this, [=](bool checked) {
        ui.output->setVisible(checked);

        if (checked) {
          ui.buttonShowOutput->setIcon(
              QIcon(":remotes/images/qbutton_icons/vdownarrow.png"));
          ui.buttonShowOutput->setIconSize(QSize(24, 24));
        } else {
          ui.buttonShowOutput->setIcon(
              QIcon(":remotes/images/qbutton_icons/vrightarrow.png"));
          ui.buttonShowOutput->setIconSize(QSize(24, 24));
        }

        if (!checked) {
          adjustSize();
        }
      });

  QObject::connect(process,
                   static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
                       &QProcess::finished),
                   this, [=](int code, QProcess::ExitStatus status) {
                     if (status == QProcess::NormalExit && code == 0) {
                       if (close) {
                         emit accept();
                       }
                     } else {
                       ui.buttonShowOutput->setChecked(true);
                       ui.buttonBox->setEnabled(true);
                     }
                   });

  QObject::connect(process, &QProcess::readyRead, this, [=]() {
    QString output = process->readAll();
    if (trim) {
      output = output.trimmed();
    }
    ui.output->appendPlainText(output);

    ui.labelOperation->setStyleSheet(
        "QLabel { color: black; font-weight: bold; }");
    ui.labelOperation->setText("DONE!  ");

    emit outputAvailable(output);
  });

  process->setProcessChannelMode(QProcess::MergedChannels);
  process->start(QIODevice::ReadOnly);
}

ProgressDialog::~ProgressDialog() {}

void ProgressDialog::expand() { ui.buttonShowOutput->setChecked(true); }

void ProgressDialog::allowToClose() { ui.buttonBox->setEnabled(true); }
//
// QString ProgressDialog::getOutput() const
//{
//    return ui.output->toPlainText();
//}

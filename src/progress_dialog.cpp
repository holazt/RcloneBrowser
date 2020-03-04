#include "progress_dialog.h"
#include "utils.h"

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
  ui.labelInfo->setTextInteractionFlags(Qt::TextSelectableByMouse);
  ui.labelInfo->setWordWrap(true);

  ui.output->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

  // apply font size preferences
  auto settings = GetSettings();

  int fontsize = 0;

  if (settings->value("Settings/fontSize").toString() == "0") {
    fontsize = 0;
  }
  if (settings->value("Settings/fontSize").toString() == "1") {
    fontsize = 1;
  }
  if (settings->value("Settings/fontSize").toString() == "2") {
    fontsize = 2;
  }
  if (settings->value("Settings/fontSize").toString() == "3") {
    fontsize = 3;
  }
  if (settings->value("Settings/fontSize").toString() == "4") {
    fontsize = 4;
  }
  if (settings->value("Settings/fontSize").toString() == "5") {
    fontsize = 5;
  }
  if (settings->value("Settings/fontSize").toString() == "6") {
    fontsize = 6;
  }
  if (settings->value("Settings/fontSize").toString() == "7") {
    fontsize = 7;
  }
  if (settings->value("Settings/fontSize").toString() == "8") {
    fontsize = 8;
  }
  if (settings->value("Settings/fontSize").toString() == "9") {
    fontsize = 9;
  }

  QFont font = ui.output->font();
  QFontMetrics fm(font);
  font.setPointSize(font.pointSize() + fontsize);
  ui.output->setFont(font);

  // icons style
  QString iconsColour = settings->value("Settings/iconsColour").toString();

  QString img_add = "";

  if (iconsColour == "white") {
    img_add = "_inv";
  }

  ui.output->setVisible(false);

  // set default arrow
  ui.buttonShowOutput->setIcon(
      QIcon(":remotes/images/qbutton_icons/vrightarrow" + img_add + ".png"));
  ui.buttonShowOutput->setIconSize(QSize(24, 24));

  QObject::connect(ui.buttonBox, &QDialogButtonBox::rejected, this,
                   &QDialog::reject);

  QObject::connect(
      ui.buttonShowOutput, &QPushButton::toggled, this, [=](bool checked) {
        ui.output->setVisible(checked);

        if (checked) {
          ui.buttonShowOutput->setIcon(QIcon(
              ":remotes/images/qbutton_icons/vdownarrow" + img_add + ".png"));
          ui.buttonShowOutput->setIconSize(QSize(24, 24));
        } else {
          ui.buttonShowOutput->setIcon(QIcon(
              ":remotes/images/qbutton_icons/vrightarrow" + img_add + ".png"));
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

    if (iconsColour == "white") {
      ui.labelOperation->setStyleSheet("QLabel { font-weight: bold; }");
    } else {
      ui.labelOperation->setStyleSheet(
          "QLabel { color: black; font-weight: bold; }");
    }

    ui.labelOperation->setText("Finished ");

    //    ui.labelOperation->setVisible(false);

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

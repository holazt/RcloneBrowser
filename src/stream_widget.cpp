#include "stream_widget.h"
#include "utils.h"

StreamWidget::StreamWidget(QProcess *rclone, QProcess *player,
                           const QString &remote, const QString &stream,
                           const QStringList &args, QWidget *parent)
    : QWidget(parent), mRclone(rclone), mPlayer(player) {
  ui.setupUi(this);

  QString remoteTrimmed;

  auto settings = GetSettings();
  mArgs.append(QDir::toNativeSeparators(GetRclone()));
  mArgs.append(args);
  mArgs.append(" | ");
  mArgs.append(settings->value("Settings/stream").toString());

  if (remote.length() > 140) {
    remoteTrimmed = remote.left(57) + "..." + remote.right(80);
  } else {
    remoteTrimmed = remote;
  }

  ui.info->setText(remoteTrimmed);
  ui.info->setCursorPosition(0);

  ui.stream->setText(stream);
  ui.stream->setCursorPosition(0);
  ui.stream->setToolTip(stream);

  ui.remote->setText(remote);
  ui.remote->setCursorPosition(0);
  ui.remote->setToolTip(remote);

  ui.details->setVisible(false);

  ui.output->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  ui.output->setVisible(false);

  QString iconsColour = settings->value("Settings/iconsColour").toString();

  QString img_add = "";

  if (iconsColour == "white") {
    img_add = "_inv";
  }

  ui.showDetails->setIcon(
      QIcon(":remotes/images/qbutton_icons/vrightarrow" + img_add + ".png"));
  ui.showDetails->setIconSize(QSize(24, 24));

  ui.showOutput->setIcon(
      QIcon(":remotes/images/qbutton_icons/vrightarrow" + img_add + ".png"));
  ui.showOutput->setIconSize(QSize(24, 24));

  ui.cancel->setToolTip("Stop streaming");
  ui.cancel->setStatusTip("Stop streaming");

  ui.copy->setIcon(
      QIcon(":remotes/images/qbutton_icons/copy" + img_add + ".png"));
  ui.copy->setIconSize(QSize(24, 24));

  QObject::connect(ui.copy, &QToolButton::clicked, this, [=]() {
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(mArgs.join(" "));
  });

  QObject::connect(
      ui.showDetails, &QToolButton::toggled, this, [=](bool checked) {
        ui.details->setVisible(checked);
        if (checked) {
          ui.showDetails->setIcon(QIcon(
              ":remotes/images/qbutton_icons/vdownarrow" + img_add + ".png"));
          ui.showDetails->setIconSize(QSize(24, 24));
        } else {
          ui.showDetails->setIcon(QIcon(
              ":remotes/images/qbutton_icons/vrightarrow" + img_add + ".png"));
          ui.showDetails->setIconSize(QSize(24, 24));
        }
      });

  QObject::connect(
      ui.showOutput, &QToolButton::toggled, this, [=](bool checked) {
        ui.output->setVisible(checked);
        if (checked) {
          ui.showOutput->setIcon(QIcon(
              ":remotes/images/qbutton_icons/vdownarrow" + img_add + ".png"));
          ui.showOutput->setIconSize(QSize(24, 24));
        } else {
          ui.showOutput->setIcon(QIcon(
              ":remotes/images/qbutton_icons/vrightarrow" + img_add + ".png"));
          ui.showOutput->setIconSize(QSize(24, 24));
        }
      });

  ui.cancel->setIcon(
      QIcon(":remotes/images/qbutton_icons/cancel" + img_add + ".png"));
  ui.cancel->setIconSize(QSize(24, 24));

  QObject::connect(ui.cancel, &QToolButton::clicked, this, [=]() {
    if (isRunning) {
      int button = QMessageBox::question(
          this, "Stop", QString("Do you want to stop %1 stream?").arg(remote),
          QMessageBox::Yes | QMessageBox::No);
      if (button == QMessageBox::Yes) {
        cancel();
      }
    } else {
      emit closed();
    }
  });

  QObject::connect(mRclone, &QProcess::readyRead, this, [=]() {
    while (mRclone->canReadLine()) {
      ui.output->appendPlainText(mRclone->readLine().trimmed());
    }
  });

  QObject::connect(
      mRclone,
      static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
          &QProcess::finished),
      this, [=](int status, QProcess::ExitStatus) {
        mRclone->deleteLater();
        isRunning = false;

        QString info = "Streaming " + ui.info->text();
        QString infoTrimmed;
        if (info.length() > 140) {
          infoTrimmed = info.left(57) + "..." + info.right(80);
        } else {
          infoTrimmed = info;
        }
        ui.info->setText(infoTrimmed);
        ui.info->setCursorPosition(0);

        if (status == 0 || status == 9) {
          if (iconsColour == "white") {
            ui.showDetails->setStyleSheet(
                "QToolButton { border: 0; font-weight: normal;}");
          } else {
            ui.showDetails->setStyleSheet(
                "QToolButton { border: 0; color: black; font-weight: normal;}");
          }
          ui.showDetails->setText("  Finished");
        } else {
          ui.showDetails->setStyleSheet(
              "QToolButton { border: 0; color: red; font-weight: normal;}");
          ui.showDetails->setText("  Error");
        }

        ui.cancel->setToolTip("Close");
        ui.cancel->setStatusTip("Close");

        emit finished();
        //          emit closed();
      });

  ui.showDetails->setStyleSheet(
      "QToolButton { border: 0; color: green; font-weight: bold;}");
  ui.showDetails->setText("  Streaming");
}

StreamWidget::~StreamWidget() {}

void StreamWidget::cancel() {
  if (!isRunning) {
    return;
  }

  mPlayer->terminate();
  mRclone->kill();
  mRclone->waitForFinished();
}

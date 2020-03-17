#include "mount_widget.h"
#include "global.h"
#include "utils.h"

MountWidget::MountWidget(QProcess *process, const QString &remote,
                         const QString &folder, const QStringList &args,
                         const QString &script, const QString &uniqueID,
                         QWidget *parent)
    : QWidget(parent), mProcess(process) {
  ui.setupUi(this);

  mUniqueID = uniqueID;
  QProcess *mScriptProcess = new QProcess();
  mArgs.append(QDir::toNativeSeparators(GetRclone()));
  mArgs.append(args);

  QString info = QString("%1 on %2").arg(remote).arg(folder);
  QString infoTrimmed;

  if (info.length() > 140) {
    infoTrimmed = info.left(57) + "..." + info.right(80);
  } else {
    infoTrimmed = info;
  }

  ui.info->setText(infoTrimmed);
  ui.info->setCursorPosition(0);

  ui.remote->setText(remote);
  ui.remote->setCursorPosition(0);
  ui.remote->setToolTip(remote);

  ui.folder->setText(folder);
  ui.folder->setCursorPosition(0);
  ui.folder->setToolTip(folder);

  ui.details->setVisible(false);

  ui.output->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  ui.output->setVisible(false);

  ui.sOutput->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  ui.sOutput->setVisible(false);
  ui.l_script->setVisible(false);

  if (script.isEmpty()) {
    ui.showScriptOutput->setVisible(false);
    ui.l_script->setVisible(false);
  } else {
    ui.l_script->setStyleSheet("QLabel { color: green; font-weight: bold;}");
    ui.l_script->setText("Running");
  }

  auto settings = GetSettings();
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

  ui.showScriptOutput->setIcon(
      QIcon(":remotes/images/qbutton_icons/vrightarrow" + img_add + ".png"));
  ui.showScriptOutput->setIconSize(QSize(24, 24));

  ui.cancel->setIcon(
      QIcon(":remotes/images/qbutton_icons/cancel" + img_add + ".png"));
  ui.cancel->setIconSize(QSize(24, 24));

  ui.copy->setIcon(
      QIcon(":remotes/images/qbutton_icons/copy" + img_add + ".png"));
  ui.copy->setIconSize(QSize(24, 24));

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

  QObject::connect(mScriptProcess, &QProcess::readyRead, this, [=]() {
    QString line;

    while (mScriptProcess->canReadLine()) {
      line = mScriptProcess->readLine().trimmed();
      ui.sOutput->appendPlainText(line);
    }
  });

  QObject::connect(mScriptProcess, &QProcess::started, this,
                   [=]() { mScriptRunning = true; });

  QObject::connect(
      mScriptProcess,
      static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
          &QProcess::finished),
      this, [=](int status, QProcess::ExitStatus) {
        mScriptRunning = false;
        mScriptProcess->deleteLater();
        if (status == 0) {

          if (iconsColour == "white") {

            ui.l_script->setStyleSheet("QLabel {font-weight: bold;}");
            ui.l_script->setText("Finished");

          } else {

            ui.l_script->setStyleSheet(
                "QLabel { color: black; font-weight: bold;}");
            ui.l_script->setText("Finished (" + QString::number(status) + ")");
          }

        } else {

          ui.l_script->setStyleSheet(
              "QLabel { color: red; font-weight: bold;}");
          ui.l_script->setText("Error (" + QString::number(status) + ")");
        }
      });

  QObject::connect(ui.copy, &QToolButton::clicked, this, [=]() {
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(mArgs.join(" "));
  });

  QObject::connect(
      ui.showScriptOutput, &QToolButton::toggled, this, [=](bool checked) {
        ui.sOutput->setVisible(checked);
        ui.l_script->setVisible(checked);

        if (checked) {
          ui.showScriptOutput->setIcon(QIcon(
              ":remotes/images/qbutton_icons/vdownarrow" + img_add + ".png"));
          ui.showScriptOutput->setIconSize(QSize(24, 24));
        } else {
          ui.showScriptOutput->setIcon(QIcon(
              ":remotes/images/qbutton_icons/vrightarrow" + img_add + ".png"));
          ui.showScriptOutput->setIconSize(QSize(24, 24));
        }
      });

  QObject::connect(
      ui.showOutput, &QToolButton::toggled, this, [=](bool checked) {
        ui.output->setVisible(checked);
        // ui.l_rclone->setVisible(checked);

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

  QObject::connect(ui.cancel, &QToolButton::clicked, this, [=]() {
    if (isRunning) {
      int button = QMessageBox::question(
          this, "Unmount",
          QString("Do you want to unmount?\n\n %2 \n\n mounted to \n\n %1")
              .arg(folder)
              .arg(remote),
          QMessageBox::Yes | QMessageBox::No);
      if (button == QMessageBox::Yes) {
        cancel();
      }
    } else {

      emit closed();
    }
  });

  QObject::connect(mProcess, &QProcess::readyRead, this, [=]() {
    QString line;
    QRegExp rx("^.+Serving\\sremote\\scontrol\\son\\s\\S+$");

    while (mProcess->canReadLine()) {
      line = mProcess->readLine().trimmed();
      ui.output->appendPlainText(line);

      // we capture RC port here from rclone output
      if (rx.exactMatch(line)) {
        line.replace("/", "");
        mRcPort = line.right(line.length() - line.lastIndexOf(":") - 1);
      }

      // we only start custome script when rclone reports RC port
      if (mRcPort != "0" && mScriptStarted == false) {

        mScriptStarted = true;

        QStringList sargs;
        sargs << GetRclone();
        sargs << mRcPort;

        QString user;
        QString password;
        int index;

        index = mArgs.indexOf(QRegExp("^--rc-user\\S+"));
        user = mArgs.at(index);
        user.replace("--rc-user=", "");

        index = mArgs.indexOf(QRegExp("^--rc-pass\\S+"));
        password = mArgs.at(index);
        password.replace("--rc-pass=", "");

        sargs << user;
        sargs << password;
        sargs << folder;

        mScriptProcess->start(script, sargs, QIODevice::ReadOnly);
      }
    }
  });

  QObject::connect(
      mProcess,
      static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
          &QProcess::finished),
      this, [=](int status, QProcess::ExitStatus) {
        mProcess->deleteLater();
        isRunning = false;

        // we terminate script as well
        if (mScriptRunning) {
          mScriptProcess->kill();
        }

        QString info = "Mounted " + ui.info->text();
        QString infoTrimmed;
        if (info.length() > 140) {
          infoTrimmed = info.left(57) + "..." + info.right(80);
        } else {
          infoTrimmed = info;
        }
        ui.info->setText(infoTrimmed);
        ui.info->setCursorPosition(0);

        if (status == 0) {
          if (iconsColour == "white") {
            ui.showDetails->setStyleSheet(
                "QToolButton { border: 0; font-weight: bold;}");
          } else {
            ui.showDetails->setStyleSheet(
                "QToolButton { border: 0; color: black; font-weight: bold;}");
          }
          ui.showDetails->setText("  Finished");
        } else {
          ui.showDetails->setStyleSheet(
              "QToolButton { border: 0; color: red; font-weight: bold;}");
          ui.showDetails->setText("  Error");
        }
        ui.cancel->setToolTip("Close");
        ui.cancel->setStatusTip("Close");
        emit finished();
      });

  ui.showDetails->setStyleSheet(
      "QToolButton { border: 0; color: green; font-weight: bold;}");
  ui.showDetails->setText("  Mounted");
}

MountWidget::~MountWidget() {}

void MountWidget::cancel() {
  if (!isRunning) {
    return;
  }

  QString cmd;

#if defined(Q_OS_MACOS) || defined(Q_OS_FREEBSD)
  QProcess::startDetached("umount", QStringList() << ui.folder->text());
#else
#if defined(Q_OS_WIN32)
  QProcess *p = new QProcess();
  QStringList unmountArgs;
  unmountArgs << "rc";

  // requires rlone version at least 1.50
  unmountArgs << "core/quit";

  // get RC parameters from mArgs
  int index = 0;
  QString user;
  QString password;

  unmountArgs << "--rc-addr";

  unmountArgs << "localhost:" + mRcPort;

  index = mArgs.indexOf(QRegExp("^--rc-user\\S+"));
  user = mArgs.at(index);

  index = mArgs.indexOf(QRegExp("^--rc-pass\\S+"));
  password = mArgs.at(index);

  unmountArgs << user << password;

  UseRclonePassword(p);
  p->start(GetRclone(), unmountArgs, QIODevice::ReadOnly);

#else
  QProcess::startDetached("fusermount", QStringList()
                                            << "-u" << ui.folder->text());
#endif
#endif

  //!!! it ok?
  //  mProcess->waitForFinished();

  //  auto settings = GetSettings();
  //  QString iconsColour = settings->value("Settings/iconsColour").toString();

  //  if (iconsColour == "white") {
  //    ui.showDetails->setStyleSheet(
  //        "QToolButton { border: 0; font-weight: normal; }");
  //  } else {
  ui.showDetails->setStyleSheet(
      "QToolButton { border: 0; color: green; font-weight: bold;}");
  //  }

  ui.showDetails->setText("  Unmounting");
  //  ui.cancel->setToolTip("Close");
  //  ui.cancel->setStatusTip("Close");
}

QString MountWidget::getUniqueID() { return mUniqueID; }

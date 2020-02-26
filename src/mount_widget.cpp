#include "mount_widget.h"
#include "utils.h"

MountWidget::MountWidget(QProcess *process, const QString &remote,
                         const QString &folder, QWidget *parent)
    : QWidget(parent), mProcess(process) {
  ui.setupUi(this);


  QString info = QString("%1 on %2").arg(remote).arg(folder);
  QString infoTrimmed;

  if (info.length() > 140) {
    infoTrimmed = info.left(57) + "..." + info.right(80);
  } else {
     infoTrimmed = info;
  }

  ui.info->setText(infoTrimmed);
  ui.info->setToolTip(info);

  ui.remote->setText(remote);
  ui.remote->setToolTip(remote);

  ui.folder->setText(folder);
  ui.folder->setToolTip(folder);

  ui.details->setVisible(false);

  ui.output->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  ui.output->setVisible(false);

  ui.showDetails->setIcon(QIcon(":remotes/images/qbutton_icons/vrightarrow.png"));
  ui.showDetails->setIconSize(QSize(24, 24));
  ui.showOutput->setIcon(QIcon(":remotes/images/qbutton_icons/vrightarrow.png"));
  ui.showOutput->setIconSize(QSize(24, 24));

  QObject::connect(
      ui.showDetails, &QToolButton::toggled, this, [=](bool checked) {
        ui.details->setVisible(checked);
        if (checked) {
          ui.showDetails->setIcon(QIcon(":remotes/images/qbutton_icons/vdownarrow.png"));
          ui.showDetails->setIconSize(QSize(24, 24));
        } else {
          ui.showDetails->setIcon(QIcon(":remotes/images/qbutton_icons/vrightarrow.png"));
          ui.showDetails->setIconSize(QSize(24, 24));
        }
      });

  QObject::connect(
      ui.showOutput, &QToolButton::toggled, this, [=](bool checked) {
        ui.output->setVisible(checked);
        if (checked) {
          ui.showOutput->setIcon(QIcon(":remotes/images/qbutton_icons/vdownarrow.png"));
          ui.showOutput->setIconSize(QSize(24, 24));
        } else {
          ui.showOutput->setIcon(QIcon(":remotes/images/qbutton_icons/vrightarrow.png"));
          ui.showOutput->setIconSize(QSize(24, 24));
        }
      });

  ui.cancel->setIcon(QIcon(":remotes/images/qbutton_icons/cancel.png"));
  ui.cancel->setIconSize(QSize(24, 24));

  QObject::connect(ui.cancel, &QToolButton::clicked, this, [=]() {
    if (mRunning) {
      int button = QMessageBox::question(
          this, "Unmount",
#if defined(Q_OS_WIN)
          QString("Do you want to unmount\n\n %1 drive?").arg(folder),
#else
          QString("Do you want to unmount\n\n %1 folder?").arg(folder),
#endif
          QMessageBox::Yes | QMessageBox::No);
      if (button == QMessageBox::Yes) {
        cancel();
      }
    } else {

      emit closed();
    }
  });

  QObject::connect(mProcess, &QProcess::readyRead, this, [=]() {
    while (mProcess->canReadLine()) {
      ui.output->appendPlainText(mProcess->readLine().trimmed());
    }
  });

  QObject::connect(mProcess,
                   static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
                       &QProcess::finished),
                   this, [=](int status, QProcess::ExitStatus) {
                     mProcess->deleteLater();
                     mRunning = false;

                     QString info = "Mounted "  + ui.info->text();
                     QString infoTrimmed;
                     if (info.length() > 140) {
                       infoTrimmed = info.left(57) + "..." + info.right(80);
                     } else {
                       infoTrimmed = info;
                     }
                     ui.info->setText(infoTrimmed);

                     if (status == 0) {
                       ui.showDetails->setStyleSheet(
                           "QToolButton { border: 0; color: black; }");
                       ui.showDetails->setText("Finished");
                     } else {
                       ui.showDetails->setStyleSheet(
                           "QToolButton { border: 0; color: red; }");
                       ui.showDetails->setText("Error");
                     }
                     ui.cancel->setToolTip("Close");
                     emit finished();
                   });

  ui.showDetails->setStyleSheet("QToolButton { border: 0; color: green; }");
  ui.showDetails->setText("Mounted");
}

MountWidget::~MountWidget() {}

void MountWidget::cancel() {
  if (!mRunning) {
    return;
  }

  QString cmd;

#if defined(Q_OS_MACOS) || defined(Q_OS_FREEBSD)
  QProcess::startDetached("umount", QStringList() << ui.folder->text());
#else
#if defined(Q_OS_WIN32)
  QProcess *p = new QProcess();
  QStringList args;
  args << "rc";
  // requires rlone version at least 1.50
  args << "core/quit";

  args << "--rc-addr";
  QString folder = ui.folder->text();

  int port_offset = folder[0].toLatin1();
  unsigned short int rclone_rc_port_base = 19000;
  unsigned short int rclone_rc_port = rclone_rc_port_base + port_offset;
  args << "localhost:" + QVariant(rclone_rc_port).toString();

  UseRclonePassword(p);
  p->start(GetRclone(), args, QIODevice::ReadOnly);
#else
  QProcess::startDetached("fusermount", QStringList()
                                            << "-u" << ui.folder->text());
#endif
#endif

  mProcess->waitForFinished();

  ui.showDetails->setStyleSheet(
      "QToolButton { border: 0; color: black; }");
  ui.showDetails->setText("Finished");
  ui.cancel->setToolTip("Close");

}

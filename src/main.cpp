#include "main_window.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  app.setApplicationDisplayName("Rclone Browser");
  app.setApplicationName("rclone-browser");
  app.setOrganizationName("rclone-browser");
  app.setWindowIcon(QIcon(":/icons/icon.png"));

  // enforce one instance of Rclone Browser per user
  QString tmpDir = QDir::tempPath();
  QLockFile lockFile(tmpDir + "/RcloneBrowser_4q6RRpbJA.lock");

  if (!lockFile.tryLock(100)) {
    // if already running display warning and quit
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("Rclone Browser is already running."
                   "\r\n\nOnly one instance is allowed.");
    msgBox.exec();
    return static_cast<int>(
        0x80004004); // exit immediately if another instance is running
  }

  MainWindow w;
  w.show();

  return app.exec();
}

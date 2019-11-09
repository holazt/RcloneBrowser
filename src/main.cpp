#include "main_window.h"
#include "qdebug.h"
#include "utils.h"

int main(int argc, char *argv[]) {

#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
static const char ENV_VAR_QT_DEVICE_PIXEL_RATIO[] = "QT_DEVICE_PIXEL_RATIO";
if (!qEnvironmentVariableIsSet(ENV_VAR_QT_DEVICE_PIXEL_RATIO)
        && !qEnvironmentVariableIsSet("QT_AUTO_SCREEN_SCALE_FACTOR")
        && !qEnvironmentVariableIsSet("QT_SCALE_FACTOR")
        && !qEnvironmentVariableIsSet("QT_SCREEN_SCALE_FACTORS")) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
}
#endif

  QApplication app(argc, argv);

//  app.setApplicationDisplayName("Rclone Browser");
  app.setApplicationName("rclone-browser");
  app.setOrganizationName("rclone-browser");
  app.setWindowIcon(QIcon(":/icons/icon.png"));

  // enforce one instance of Rclone Browser per user
  qDebug() << QString("main.cpp currentPath: " + QDir::currentPath());
  qDebug() << QString("main.cpp tempPath:  " + QDir::tempPath());

  QString tmpDir;
  QString applicationNameBase;
  QFileInfo applicationPath;
  QFileInfo applicationUserPath;

  QString xdg_config_home = qgetenv("XDG_CONFIG_HOME");
  qDebug() << QString("main.cpp $XDG_CONFIG_HOME: " + xdg_config_home);

  QString APPIMAGE = qgetenv("APPIMAGE");
  qDebug() << QString("main.cpp $APPIMAGE: " + APPIMAGE);

  QFileInfo appBundlePath;

  if (IsPortableMode()) {

    qDebug() << QString("isPortable is true");
    applicationPath = qApp->applicationFilePath();
#ifdef Q_OS_OSX
    // on macOS excecutable file is located in
    // ./rclone-browser.app/Contents/MasOS/
    // to get actual bundle folder we have
    // to traverse three levels up
    tmpDir = applicationPath.absolutePath() + "/../../..";

    // get bundle name
    QFileInfo MacOSPath = applicationPath.dir().path();
    QFileInfo ContentsPath = MacOSPath.dir().path();
    appBundlePath = ContentsPath.dir().path();

#else
    // not macOS
#ifdef Q_OS_WIN
    tmpDir = applicationPath.absolutePath();
#else
    tmpDir = xdg_config_home + "/rclone-browser";
    // create ./rclone-browser folder
    if (!QDir(tmpDir).exists()) {
      QDir().mkdir(tmpDir);
    }
#endif
#endif
  } else {
    // not portable mode
    // get tmp folder from Qt  - OS dependend
    tmpDir = QDir::tempPath();
  }

  // check if tmpDir writable
  // as isWritable does weird things on Windows
  // we do this old fashioned way by creating temp file
  QTemporaryFile tempfile(tmpDir + "/rclone-browserXXXXXX.test");

  if (tempfile.open()) {
    tempfile.close();
    tempfile.remove();
  } else {
    // folder has no write access
    if (IsPortableMode()) {
      QMessageBox msgBox;
      msgBox.setIcon(QMessageBox::Warning);
      msgBox.setText("You need write "
                     "access to this folder:\n\n"
#ifdef Q_OS_OSX
                     + appBundlePath.absolutePath() +
#else
#ifdef Q_OS_WIN
                     + tmpDir +
#else
                     + tmpDir.left(tmpDir.length() - 15) +
#endif
#endif
                     "\n\n"
#ifdef Q_OS_OSX
                     "Or remove file:\n\n" +
                     appBundlePath.baseName() +
                     ".ini \n\nfrom the above folder "
#else
#ifdef Q_OS_WIN
                     "Or remove file:\n\n" +
                     applicationPath.baseName() +
                     ".ini \n\nfrom the above folder "
#else
                     "Or remove folder:\n\n" +
                     tmpDir.left(tmpDir.length() - 15) +
                     "\n\n"
#endif
#endif
                     "to disable portable mode.");
      msgBox.exec();
    } else {
      QMessageBox msgBox;
      msgBox.setIcon(QMessageBox::Warning);
      msgBox.setText("You need write "
                     "access to this folder: \n\n"
#ifdef Q_OS_OSX
                     + tmpDir
#else

#ifdef Q_OS_WIN
                     + tmpDir
#else

                     + tmpDir.left(tmpDir.length() - 15)
#endif
#endif
      );
      msgBox.exec();
    }
    return static_cast<int>(
        0x80004004); // exit immediately if folder not writable
  }

  qDebug() << QString("main.cpp tmpDir:  " + tmpDir);

  // not most elegant as fixed name but in reality not big deal
  QLockFile lockFile(tmpDir + "/.RcloneBrowser_4q6RgLs2RpbJA.lock");

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

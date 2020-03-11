#include "main_window.h"
#include "utils.h"

int main(int argc, char *argv[]) {

#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
  static const char ENV_VAR_QT_DEVICE_PIXEL_RATIO[] = "QT_DEVICE_PIXEL_RATIO";
  if (!qEnvironmentVariableIsSet(ENV_VAR_QT_DEVICE_PIXEL_RATIO) &&
      !qEnvironmentVariableIsSet("QT_AUTO_SCREEN_SCALE_FACTOR") &&
      !qEnvironmentVariableIsSet("QT_SCALE_FACTOR") &&
      !qEnvironmentVariableIsSet("QT_SCREEN_SCALE_FACTORS")) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  }
#endif

  QApplication app(argc, argv);

  //  app.setApplicationDisplayName("Rclone Browser");
  app.setApplicationName("rclone-browser");
  app.setOrganizationName("rclone-browser");
  app.setWindowIcon(QIcon(":/icons/icon.png"));

// initialize SSL libraries
// see: https://github.com/linuxdeploy/linuxdeploy-plugin-qt/issues/57
#if defined(Q_OS_LINUX)
  QString currentDir = QDir::currentPath();
  QDir::setCurrent(QCoreApplication::applicationDirPath());
  QSslSocket::supportsSsl();
  QDir::setCurrent(currentDir);
#endif

  auto settings = GetSettings();

  // initialize proxy settings
  if (!(settings->contains("Settings/useProxy"))) {
    settings->setValue("Settings/useProxy", "false");
  };
  if (!(settings->contains("Settings/http_proxy"))) {
    settings->setValue("Settings/http_proxy", "");
  };
  if (!(settings->contains("Settings/https_proxy"))) {
    settings->setValue("Settings/https_proxy", "");
  };
  if (!(settings->contains("Settings/no_proxy"))) {
    settings->setValue("Settings/no_proxy", "");
  };

  if (settings->value("Settings/useProxy").toBool()) {
    qputenv("HTTP_PROXY", settings->value("Settings/http_proxy").toByteArray());
    qputenv("http_proxy", settings->value("Settings/http_proxy").toByteArray());
    qputenv("HTTPS_PROXY",
            settings->value("Settings/https_proxy").toByteArray());
    qputenv("https_proxy",
            settings->value("Settings/https_proxy").toByteArray());
    qputenv("NO_PROXY", settings->value("Settings/no_proxy").toByteArray());
    qputenv("no_proxy", settings->value("Settings/no_proxy").toByteArray());
  }

  // remmber darkMode state on app startup
  // during first run the darkModeIni key might not exist
  if (!(settings->contains("Settings/darkModeIni"))) {
    // if darkModeIni does not exist create new key
    settings->setValue("Settings/darkModeIni", "true");
  };

  // during first run the darkMode key might not exist
  if (!(settings->contains("Settings/darkMode"))) {
    // if darkMode does not exist create new key
    settings->setValue("Settings/darkMode", "true");
  };

  bool darkMode = settings->value("Settings/darkMode").toBool();

  settings->setValue("Settings/darkModeIni", darkMode);

  // during first run the iconSize key might not exist
  if (!(settings->contains("Settings/iconSize"))) {
    // if iconSize does not exist create new key
    settings->setValue("Settings/iconSize", "S");
  };

  // during first run the iconsLayout key might not exist
  if (!(settings->contains("Settings/iconsLayout"))) {
    // if iconsLayout does not exist create new key
    settings->setValue("Settings/iconsLayout", "tiles");
  };

  // during first run the iconsColour key might not exist
  if (!(settings->contains("Settings/iconsColour"))) {
    // if iconsColour does not exist create new key
    settings->setValue("Settings/iconsColour", "black");
  };

  // during first run the buttonStyle key might not exist
  if (!(settings->contains("Settings/buttonStyle"))) {
    // if buttonstyle does not exist create new key
    settings->setValue("Settings/buttonStyle", "textandicon");
  };

  // during first run the fontSize key might not exist
  if (!(settings->contains("Settings/fontSize"))) {
    // if fontSize does not exist create new key
    settings->setValue("Settings/fontSize", "0");
  };

  // during first run the buttonSize key might not exist
  if (!(settings->contains("Settings/buttonSize"))) {
    // if buttonSize does not exist create new key
    settings->setValue("Settings/buttonSize", "0");
  };

  // during first run the sortTask key might not exist
  if (!(settings->contains("Settings/sortTask"))) {
    // if sortTask does not exist create new key
    settings->setValue("Settings/sortTask", "false");
  };

  // during first run the remoteMode key might not exist
  if (!(settings->contains("Settings/remoteMode"))) {
    // if remoteMode does not exist create new key
    settings->setValue("Settings/remoteMode", "main");
  };

  // during first run the remoteType key might not exist
  if (!(settings->contains("Settings/remoteType"))) {
    // if remoteType does not exist create new key
    settings->setValue("Settings/remoteType", "main");
  };

  // during first run the queueScript key might not exist
  if (!(settings->contains("Settings/queueScript"))) {
    // if queueScript does not exist create new key
    settings->setValue("Settings/queueScript", "");
  };

  // during first run the queueScriptRun key might not exist
  if (!(settings->contains("Settings/queueScriptRun"))) {
    // if queueScriptRun does not exist create new key
    settings->setValue("Settings/queueScriptRun", "false");
  };

  // set application font size
  int fontsize = 0;
  fontsize = (settings->value("Settings/fontSize").toInt());

  QFont defaultFont = QApplication::font();
  defaultFont.setPointSize(defaultFont.pointSize() + fontsize);
  qApp->setFont(defaultFont);

  // enforce one instance of Rclone Browser per user
  QString tmpDir;
  QString applicationNameBase;
  QFileInfo applicationPath;
  QFileInfo applicationUserPath;

  // QString xdg_config_home = qgetenv("XDG_CONFIG_HOME");
  // qDebug() << QString("main.cpp $XDG_CONFIG_HOME: " + xdg_config_home);

  // QString APPIMAGE = qgetenv("APPIMAGE");
  // qDebug() << QString("main.cpp $APPIMAGE: " + APPIMAGE);

  QFileInfo appBundlePath;

  if (IsPortableMode()) {

    //  qDebug() << QString("isPortable is true");
    //  applicationPath = qApp->applicationFilePath();
#ifdef Q_OS_MACOS
    // on macOS excecutable file is located in
    // ./rclone-browser.app/Contents/MasOS/
    // to get actual bundle folder we have
    // to traverse three levels up
    applicationPath = qApp->applicationFilePath();
    tmpDir = applicationPath.absolutePath() + "/../../..";

    // get bundle name
    QFileInfo MacOSPath = applicationPath.dir().path();
    QFileInfo ContentsPath = MacOSPath.dir().path();
    appBundlePath = ContentsPath.dir().path();

#else
    // not macOS
#ifdef Q_OS_WIN
    applicationPath = qApp->applicationFilePath();
    tmpDir = applicationPath.absolutePath();
#else
    QString xdg_config_home = qgetenv("XDG_CONFIG_HOME");
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
#ifdef Q_OS_MACOS
                     + appBundlePath.absolutePath() +
#else
#ifdef Q_OS_WIN
                     + tmpDir +
#else
                     + tmpDir.left(tmpDir.length() - 15) +
#endif
#endif
                     "\n\n"
#ifdef Q_OS_MACOS
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
#ifdef Q_OS_MACOS
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

  // qDebug() << QString("main.cpp tmpDir:  " + tmpDir);

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

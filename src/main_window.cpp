#include "main_window.h"
#include "job_options.h"
#include "job_widget.h"
#include "list_of_job_options.h"
#include "mount_widget.h"
#include "preferences_dialog.h"
#include "remote_widget.h"
#include "stream_widget.h"
#include "transfer_dialog.h"
#include "utils.h"
#ifdef Q_OS_MACOS
#include "osx_helper.h"
#endif

MainWindow::MainWindow() {

  ui.setupUi(this);
  if (IsPortableMode()) {
    this->setWindowTitle("Rclone Browser - portable mode");
  } else {
    this->setWindowTitle("Rclone Browser");
  }

  auto settings = GetSettings();

#if defined(Q_OS_WIN)
  // disable "?" WindowContextHelpButton
  QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);
#endif

#if !defined(Q_OS_MACOS)
  bool darkMode = settings->value("Settings/darkMode").toBool();

  // enable dark mode for Windows and Linux
  if (darkMode) {

    qApp->setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText,
                         QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Base, QColor(42, 42, 42));
    darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text,
                         QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Dark, QColor(35, 35, 35));
    darkPalette.setColor(QPalette::Shadow, QColor(20, 20, 20));
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText,
                         QColor(127, 127, 127));
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Disabled, QPalette::Highlight,
                         QColor(80, 80, 80));
    darkPalette.setColor(QPalette::HighlightedText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText,
                         QColor(127, 127, 127));
    qApp->setPalette(darkPalette);
    qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; "
                        "border: 1px solid white;}");
  }

#else

  // enable dark mode for older macOS
  QString sysInfo = QSysInfo::productVersion();

  if (sysInfo == "10.9" || sysInfo == "10.10" || sysInfo == "10.11" ||
      sysInfo == "10.12" || sysInfo == "10.13") {
    auto settings = GetSettings();
    bool darkMode = settings->value("Settings/darkMode").toBool();
    if (darkMode) {
      qApp->setStyle(QStyleFactory::create("Fusion"));

      QPalette darkPalette;
      darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
      darkPalette.setColor(QPalette::WindowText, Qt::white);
      darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
      darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
      darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
      darkPalette.setColor(QPalette::ToolTipText, Qt::white);
      darkPalette.setColor(QPalette::Text, Qt::white);
      darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
      darkPalette.setColor(QPalette::ButtonText, Qt::white);
      darkPalette.setColor(QPalette::BrightText, Qt::red);
      darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

      darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
      darkPalette.setColor(QPalette::HighlightedText, Qt::black);

      qApp->setPalette(darkPalette);

      qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: "
                          "#2a82da; border: 1px solid white; }");
    }
  }
#endif

  mSystemTray.setIcon(qApp->windowIcon());
  if (settings->contains("MainWindow/geometry")) {
    restoreGeometry(settings->value("MainWindow/geometry").toByteArray());
  }
  SetRclone(settings->value("Settings/rclone").toString());
  SetRcloneConf(settings->value("Settings/rcloneConf").toString());

  mAlwaysShowInTray =
      settings->value("Settings/alwaysShowInTray", false).toBool();
  mCloseToTray = settings->value("Settings/closeToTray", false).toBool();
  mNotifyFinishedTransfers =
      settings->value("Settings/notifyFinishedTransfers", true).toBool();

  mSystemTray.setVisible(mAlwaysShowInTray);

  // during first run the lastUsed keys might not exist
  if (!(settings->contains("Settings/lastUsedSourceFolder"))) {
    // if lastUsedSourceFolder does not exist create new empty key
    settings->setValue("Settings/lastUsedSourceFolder", "");
  };
  if (!(settings->contains("Settings/lastUsedDestFolder"))) {
    // if lastUsedDestFolder does not exist create new empty key
    settings->setValue("Settings/lastUsedDestFolder", "");
  };
  if (!(settings->contains("Settings/defaultDownloadOptions"))) {
    // if defaultDownloadOptions does not exist create new empty key
    settings->setValue("Settings/defaultDownloadOptions", "");
  };
#ifdef Q_OS_MACOS
  // for macOS by default exclude .DS_Store files from uploads
  if (!(settings->contains("Settings/defaultUploadOptions"))) {
    // if defaultDownloadOptions does not exist create new empty key
    settings->setValue("Settings/defaultUploadOptions", "--exclude .DS_Store");
  };
#else
  if (!(settings->contains("Settings/defaultUploadOptions"))) {
    // if defaultDownloadOptions does not exist create new empty key
    settings->setValue("Settings/defaultUploadOptions", "");
  };
#endif
  if (!(settings->contains("Settings/defaultRcloneOptions"))) {
    // if defaultRcloneOptions does not exist create new empty key
    settings->setValue("Settings/defaultRcloneOptions", "--fast-list");
  };

  QString buttonStyle = settings->value("Settings/buttonStyle").toString();
  QString buttonSize = settings->value("Settings/buttonSize").toString();
  QString iconsColour = settings->value("Settings/iconsColour").toString();

  QString img_add = "";

  if (iconsColour == "white") {
    img_add = "_inv";
  }

  ui.actionDryRun->setIcon(
      QIcon(":remotes/images/qbutton_icons/dryrun" + img_add + ".png"));
  ui.actionRun->setIcon(
      QIcon(":remotes/images/qbutton_icons/run" + img_add + ".png"));
  ui.actionEdit->setIcon(
      QIcon(":remotes/images/qbutton_icons/edit" + img_add + ".png"));
  ui.actionDelete->setIcon(
      QIcon(":remotes/images/qbutton_icons/purge" + img_add + ".png"));
  ui.actionRefresh->setIcon(
      QIcon(":remotes/images/qbutton_icons/refresh" + img_add + ".png"));
  ui.actionOpen->setIcon(
      QIcon(":remotes/images/qbutton_icons/open_remote" + img_add + ".png"));
  ui.actionConfig->setIcon(
      QIcon(":remotes/images/qbutton_icons/rclone_config" + img_add + ".png"));
  ui.actionStopAllTransfers->setIcon(
      QIcon(":remotes/images/qbutton_icons/stop" + img_add + ".png"));
  ui.actionCleanNotRunning->setIcon(
      QIcon(":remotes/images/qbutton_icons/purge" + img_add + ".png"));
  // Preferences button action is triggered via slot defined in ui file
  // as we dont want pref icon in the menu
  ui.buttonPrefs->setIcon(
      QIcon(":remotes/images/qbutton_icons/preferences" + img_add + ".png"));

  ui.actionAddToQueue->setIcon(
      QIcon(":remotes/images/qbutton_icons/addtoqueue" + img_add + ".png"));
  ui.actionStartQueue->setIcon(
      QIcon(":remotes/images/qbutton_icons/run" + img_add + ".png"));
  ui.actionStopQueue->setIcon(
      QIcon(":remotes/images/qbutton_icons/stop" + img_add + ".png"));
  ui.actionPurgeQueue->setIcon(
      QIcon(":remotes/images/qbutton_icons/purge" + img_add + ".png"));
  ui.actionRemoveFromQueue->setIcon(QIcon(
      ":remotes/images/qbutton_icons/removefromqueue" + img_add + ".png"));
  ui.actionUpQueue->setIcon(
      QIcon(":remotes/images/qbutton_icons/vuparrow" + img_add + ".png"));
  ui.actionDownQueue->setIcon(
      QIcon(":remotes/images/qbutton_icons/vdownarrow" + img_add + ".png"));

  QPixmap arrowDownPixmap(":remotes/images/qbutton_icons/arrowdown" + img_add +
                          ".png");
  QPixmap arrowUpPixmap(":remotes/images/qbutton_icons/arrowup" + img_add +
                        ".png");
  QPixmap sortZAPixmap(":remotes/images/qbutton_icons/sortZA" + img_add +
                       ".png");
  QPixmap sortAZPixmap(":remotes/images/qbutton_icons/sortAZ" + img_add +
                       ".png");

  QIcon arrowDownIcon(arrowDownPixmap);
  QIcon arrowUpIcon(arrowUpPixmap);
  QIcon sortZAIcon(sortZAPixmap);
  QIcon sortAZIcon(sortAZPixmap);

  ui.buttonDryrunTask->setDefaultAction(ui.actionDryRun);
  ui.buttonRunTask->setDefaultAction(ui.actionRun);
  ui.buttonEditTask->setDefaultAction(ui.actionEdit);
  ui.buttonDeleteTask->setDefaultAction(ui.actionDelete);
  ui.buttonSortTask->setDefaultAction(ui.actionSortTask);
  ui.refresh->setDefaultAction(ui.actionRefresh);
  ui.open->setDefaultAction(ui.actionOpen);
  ui.config->setDefaultAction(ui.actionConfig);
  //  ui.buttonPrefs->setDefaultAction(ui.preferences);
  ui.buttonStopAllJobs->setDefaultAction(ui.actionStopAllTransfers);
  ui.buttonCleanNotRunning->setDefaultAction(ui.actionCleanNotRunning);
  ui.buttonAddToQueue->setDefaultAction(ui.actionAddToQueue);
  ui.buttonStartQueue->setDefaultAction(ui.actionStartQueue);
  ui.buttonStopQueue->setDefaultAction(ui.actionStopQueue);
  ui.buttonPurgeQueue->setDefaultAction(ui.actionPurgeQueue);
  ui.buttonRemoveFromQueue->setDefaultAction(ui.actionRemoveFromQueue);
  ui.buttonDownQueue->setDefaultAction(ui.actionDownQueue);
  ui.buttonUpQueue->setDefaultAction(ui.actionUpQueue);

  // overwrite button text
  ui.buttonPrefs->setText("Prefs");

  // open remote should be not active when there is
  // no foucs on any e.g. after start
  ui.open->setEnabled(false);

  // both buttons inactive after start
  ui.buttonStopAllJobs->setEnabled(false);
  ui.buttonCleanNotRunning->setEnabled(false);

  int icon_w = 16;
  int icon_h = 16;
  //  int button_width = 32;

  if (buttonSize == "0") {
    icon_w = 24;
  }
  if (buttonSize == "1") {
    icon_w = 32;
  }
  if (buttonSize == "2") {
    icon_w = 48;
  }
  if (buttonSize == "3") {
    icon_w = 72;
  }
  if (buttonSize == "4") {
    icon_w = 96;
  }
  icon_h = icon_w;
  int button_width = 55;

  if (buttonStyle == "textandicon") {

    ui.buttonDryrunTask->setIconSize(QSize(icon_w, icon_h));
    ui.buttonRunTask->setIconSize(QSize(icon_w, icon_h));
    ui.buttonEditTask->setIconSize(QSize(icon_w, icon_h));
    ui.buttonDeleteTask->setIconSize(QSize(icon_w, icon_h));
    ui.buttonSortTask->setIconSize(QSize(icon_w, icon_h));
    ui.buttonPrefs->setIconSize(QSize(icon_w, icon_h));
    ui.refresh->setIconSize(QSize(icon_w, icon_h));
    ui.open->setIconSize(QSize(icon_w, icon_h));
    ui.config->setIconSize(QSize(icon_w, icon_h));
    ui.buttonStopAllJobs->setIconSize(QSize(icon_w, icon_h));
    ui.buttonCleanNotRunning->setIconSize(QSize(icon_w, icon_h));
    ui.buttonAddToQueue->setIconSize(QSize(icon_w, icon_h));
    ui.buttonStartQueue->setIconSize(QSize(icon_w, icon_h));
    ui.buttonStopQueue->setIconSize(QSize(icon_w, icon_h));
    ui.buttonPurgeQueue->setIconSize(QSize(icon_w, icon_h));
    ui.buttonRemoveFromQueue->setIconSize(QSize(icon_w, icon_h));
    ui.buttonDownQueue->setIconSize(QSize(icon_w, icon_h));
    ui.buttonUpQueue->setIconSize(QSize(icon_w, icon_h));

    ui.buttonDryrunTask->setMinimumWidth(button_width);
    ui.buttonRunTask->setMinimumWidth(button_width);
    ui.buttonEditTask->setMinimumWidth(button_width);
    ui.buttonDeleteTask->setMinimumWidth(button_width);
    ui.buttonSortTask->setMinimumWidth(button_width);
    ui.buttonPrefs->setMinimumWidth(button_width);
    ui.refresh->setMinimumWidth(button_width);
    ui.open->setMinimumWidth(button_width);
    ui.config->setMinimumWidth(button_width);
    ui.buttonStopAllJobs->setMinimumWidth(button_width);
    ui.buttonCleanNotRunning->setMinimumWidth(button_width);
    ui.buttonAddToQueue->setMinimumWidth(button_width);
    ui.buttonStartQueue->setMinimumWidth(button_width);
    ui.buttonStopQueue->setMinimumWidth(button_width);
    ui.buttonPurgeQueue->setMinimumWidth(button_width);
    ui.buttonRemoveFromQueue->setMinimumWidth(button_width);
    ui.buttonDownQueue->setMinimumWidth(button_width);

    ui.buttonUpQueue->setIconSize(QSize(icon_w, icon_h));
    ui.buttonUpQueue->setMinimumWidth(button_width);

  } else {
    if (buttonStyle == "textonly") {
      ui.buttonDryrunTask->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonRunTask->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonEditTask->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonDeleteTask->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonSortTask->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonPrefs->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.refresh->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.open->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.config->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonStopAllJobs->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonCleanNotRunning->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonAddToQueue->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonStartQueue->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonStopQueue->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonPurgeQueue->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonRemoveFromQueue->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonDownQueue->setToolButtonStyle(Qt::ToolButtonTextOnly);

      ui.buttonDryrunTask->setMinimumWidth(button_width);
      ui.buttonRunTask->setMinimumWidth(button_width);
      ui.buttonEditTask->setMinimumWidth(button_width);
      ui.buttonDeleteTask->setMinimumWidth(button_width);
      ui.buttonSortTask->setMinimumWidth(button_width);
      ui.buttonPrefs->setMinimumWidth(button_width);
      ui.refresh->setMinimumWidth(button_width);
      ui.open->setMinimumWidth(button_width);
      ui.config->setMinimumWidth(button_width);
      ui.buttonStopAllJobs->setMinimumWidth(button_width);
      ui.buttonCleanNotRunning->setMinimumWidth(button_width);
      ui.buttonAddToQueue->setMinimumWidth(button_width);
      ui.buttonStartQueue->setMinimumWidth(button_width);
      ui.buttonStopQueue->setMinimumWidth(button_width);
      ui.buttonPurgeQueue->setMinimumWidth(button_width);
      ui.buttonRemoveFromQueue->setMinimumWidth(button_width);
      ui.buttonDownQueue->setMinimumWidth(button_width);

      ui.buttonUpQueue->setToolButtonStyle(Qt::ToolButtonTextOnly);
      ui.buttonUpQueue->setMinimumWidth(button_width);

    } else {
      // button style - icononly
      ui.buttonDryrunTask->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonDryrunTask->setIconSize(QSize(icon_w, icon_h));
      ui.buttonRunTask->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonRunTask->setIconSize(QSize(icon_w, icon_h));
      ui.buttonEditTask->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonEditTask->setIconSize(QSize(icon_w, icon_h));
      ui.buttonDeleteTask->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonDeleteTask->setIconSize(QSize(icon_w, icon_h));
      ui.buttonSortTask->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonSortTask->setIconSize(QSize(icon_w, icon_h));
      ui.buttonPrefs->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonPrefs->setIconSize(QSize(icon_w, icon_h));
      ui.refresh->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.refresh->setIconSize(QSize(icon_w, icon_h));
      ui.open->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.open->setIconSize(QSize(icon_w, icon_h));
      ui.config->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.config->setIconSize(QSize(icon_w, icon_h));
      ui.buttonStopAllJobs->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonStopAllJobs->setIconSize(QSize(icon_w, icon_h));
      ui.buttonCleanNotRunning->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonCleanNotRunning->setIconSize(QSize(icon_w, icon_h));
      ui.buttonAddToQueue->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonAddToQueue->setIconSize(QSize(icon_w, icon_h));
      ui.buttonStartQueue->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonStartQueue->setIconSize(QSize(icon_w, icon_h));
      ui.buttonStopQueue->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonStopQueue->setIconSize(QSize(icon_w, icon_h));
      ui.buttonPurgeQueue->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonPurgeQueue->setIconSize(QSize(icon_w, icon_h));
      ui.buttonRemoveFromQueue->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonRemoveFromQueue->setIconSize(QSize(icon_w, icon_h));
      ui.buttonDownQueue->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonDownQueue->setIconSize(QSize(icon_w, icon_h));
      ui.buttonUpQueue->setToolButtonStyle(Qt::ToolButtonIconOnly);
      ui.buttonUpQueue->setIconSize(QSize(icon_w, icon_h));
    }
  }

  QObject::connect(ui.preferences, &QAction::triggered, this, [=]() {
    PreferencesDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
      auto settings = GetSettings();
      settings->setValue("Settings/rclone", dialog.getRclone().trimmed());
      settings->setValue("Settings/rcloneConf",
                         dialog.getRcloneConf().trimmed());
      settings->setValue("Settings/stream", dialog.getStream());
      settings->setValue("Settings/mount", dialog.getMount());
      settings->setValue("Settings/defaultDownloadDir",
                         dialog.getDefaultDownloadDir().trimmed());
      settings->setValue("Settings/defaultUploadDir",
                         dialog.getDefaultUploadDir().trimmed());
      settings->setValue("Settings/defaultDownloadOptions",
                         dialog.getDefaultDownloadOptions().trimmed());
      settings->setValue("Settings/defaultUploadOptions",
                         dialog.getDefaultUploadOptions().trimmed());
      settings->setValue("Settings/defaultRcloneOptions",
                         dialog.getDefaultRcloneOptions().trimmed());

      settings->setValue("Settings/checkRcloneBrowserUpdates",
                         dialog.getCheckRcloneBrowserUpdates());
      settings->setValue("Settings/checkRcloneUpdates",
                         dialog.getCheckRcloneUpdates());

      settings->setValue("Settings/alwaysShowInTray",
                         dialog.getAlwaysShowInTray());
      settings->setValue("Settings/closeToTray", dialog.getCloseToTray());
      settings->setValue("Settings/notifyFinishedTransfers",
                         dialog.getNotifyFinishedTransfers());

      settings->setValue("Settings/showFolderIcons",
                         dialog.getShowFolderIcons());
      settings->setValue("Settings/showFileIcons", dialog.getShowFileIcons());
      settings->setValue("Settings/rowColors", dialog.getRowColors());
      settings->setValue("Settings/showHidden", dialog.getShowHidden());
      settings->setValue("Settings/darkMode", dialog.getDarkMode());
      settings->setValue("Settings/buttonStyle",
                         dialog.getButtonStyle().trimmed());
      settings->setValue("Settings/iconsLayout",
                         dialog.getIconsLayout().trimmed());
      settings->setValue("Settings/iconsColour",
                         dialog.getIconsColour().trimmed());

      settings->setValue("Settings/fontSize", dialog.getFontSize().trimmed());
      settings->setValue("Settings/buttonSize",
                         dialog.getButtonSize().trimmed());
      settings->setValue("Settings/iconSize", dialog.getIconSize().trimmed());

      settings->setValue("Settings/useProxy", dialog.getUseProxy());
      settings->setValue("Settings/http_proxy",
                         dialog.getHttpProxy().trimmed());
      settings->setValue("Settings/https_proxy",
                         dialog.getHttpsProxy().trimmed());
      settings->setValue("Settings/no_proxy", dialog.getNoProxy().trimmed());

      SetRclone(dialog.getRclone());
      SetRcloneConf(dialog.getRcloneConf());
      mFirstTime = true;
      rcloneGetVersion();

      mAlwaysShowInTray = dialog.getAlwaysShowInTray();
      mCloseToTray = dialog.getCloseToTray();
      mNotifyFinishedTransfers = dialog.getNotifyFinishedTransfers();

      mSystemTray.setVisible(mAlwaysShowInTray);
    }
  });

  // intercept tab closure
  MainWindow::connect(ui.tabs, SIGNAL(tabCloseRequested(int)), this,
                      SLOT(slotCloseTab(int)));

  QObject::connect(ui.quit, &QAction::triggered, this, [=]() {
    mCloseToTray = false;
    close();
  });

  QObject::connect(ui.about, &QAction::triggered, this, [=]() {
    QMessageBox::about(
        this, "Rclone Browser",
        QString(
            R"(<h3>GUI for rclone, v)" RCLONE_BROWSER_VERSION "</h3>"
            R"(<p>Copyright &copy; 2019</p>)"

            R"(<p>Current development and maintenance<br /><a href="https://github.com/kapitainsky/RcloneBrowser">kapitainsky</a></p>)"

            R"(<p>New features and fixes<br /><a href="https://github.com/kapitainsky/RcloneBrowser/graphs/contributors">contributors</a></p>)"

            R"(<p>Original version<br /><a href="https://mmozeiko.github.io/RcloneBrowser">Martins Mozeiko</a></p>)"));
  });
  QObject::connect(ui.aboutQt, &QAction::triggered, qApp,
                   &QApplication::aboutQt);

  QObject::connect(
      ui.remotes, &QListWidget::currentItemChanged, this,
      [=](QListWidgetItem *current) { ui.open->setEnabled(current != NULL); });

  QObject::connect(ui.remotes, &QListWidget::itemActivated, ui.open,
                   &QPushButton::clicked);

  QObject::connect(ui.config, &QPushButton::clicked, this,
                   &MainWindow::rcloneConfig);

  QObject::connect(ui.refresh, &QPushButton::clicked, this,
                   &MainWindow::rcloneListRemotes);

  QObject::connect(ui.open, &QPushButton::clicked, this, [=]() {
    if (ui.remotes->selectedItems().size() != 0) {
      auto item = ui.remotes->selectedItems().front();
      QString type = item->data(Qt::UserRole).toString();
      QString name = item->text();
      QString remoteType = type;

      auto remote = new RemoteWidget(&mIcons, name, remoteType, ui.tabs);
      QObject::connect(remote, &RemoteWidget::addMount, this,
                       &MainWindow::addMount);
      QObject::connect(remote, &RemoteWidget::addStream, this,
                       &MainWindow::addStream);
      QObject::connect(remote, &RemoteWidget::addTransfer, this,
                       &MainWindow::addTransfer);

      int index = ui.tabs->addTab(remote, name);
      ui.tabs->setCurrentIndex(index);
    }
  });

  QObject::connect(ui.tabs, &QTabWidget::tabCloseRequested, ui.tabs,
                   &QTabWidget::removeTab);

  QObject::connect(ui.tasksListWidget, &QListWidget::itemClicked, this, [=]() {
    ui.buttonDeleteTask->setEnabled(true);
    ui.buttonEditTask->setEnabled(true);
    ui.buttonRunTask->setEnabled(true);
    ui.buttonDryrunTask->setEnabled(true);
    ui.buttonAddToQueue->setEnabled(true);
  });

  QObject::connect(ui.tasksListWidget, &QListWidget::currentItemChanged, this,
                   [=]() {
                     auto items = ui.tasksListWidget->selectedItems();

                     if (items.isEmpty()) {
                       ui.buttonDeleteTask->setEnabled(false);
                       ui.buttonEditTask->setEnabled(false);
                       ui.buttonRunTask->setEnabled(false);
                       ui.buttonDryrunTask->setEnabled(false);
                       ui.buttonAddToQueue->setEnabled(false);
                     } else {

                       ui.buttonDeleteTask->setEnabled(true);
                       ui.buttonEditTask->setEnabled(true);
                       ui.buttonRunTask->setEnabled(true);
                       ui.buttonDryrunTask->setEnabled(true);
                       ui.buttonAddToQueue->setEnabled(true);
                     }

                     if (ui.tasksListWidget->count() > 1) {
                       ui.buttonSortTask->setEnabled(true);
                     } else {
                       ui.buttonSortTask->setEnabled(false);
                     }
                   });

  QObject::connect(ui.tasksListWidget, &QListWidget::itemDoubleClicked, this,
                   [=]() { editSelectedTask(); });

  bool sortTask = settings->value("Settings/sortTask").toBool();

  if (sortTask) {
    ui.actionSortTask->setIcon(sortZAIcon);
    ui.buttonSortTask->setToolTip("Sort Descending");
    ui.tasksListWidget->setSortingEnabled(true);
    ui.tasksListWidget->sortItems(Qt::AscendingOrder);
  } else {
    ui.actionSortTask->setIcon(sortAZIcon);
    ui.buttonSortTask->setToolTip("Sort Ascending");
    ui.tasksListWidget->setSortingEnabled(true);
    ui.tasksListWidget->sortItems(Qt::DescendingOrder);
  }

  QObject::connect(ui.tasksListWidget, &QWidget::customContextMenuRequested,
                   this, [=](const QPoint &pos) {
                     QMenu menu;
                     menu.addAction(ui.actionAddToQueue);
                     menu.addSeparator();
                     menu.addAction(ui.actionDryRun);
                     menu.addAction(ui.actionRun);
                     menu.addSeparator();
                     menu.addAction(ui.actionEdit);
                     menu.addAction(ui.actionDelete);
                     menu.exec(
                         ui.tasksListWidget->viewport()->mapToGlobal(pos));
                   });

  QObject::connect(ui.queueListWidget, &QWidget::customContextMenuRequested,
                   this, [=](const QPoint &pos) {
                     QMenu menu;
                     menu.addAction(ui.actionUpQueue);
                     menu.addAction(ui.actionDownQueue);
                     menu.addSeparator();
                     menu.addAction(ui.actionRemoveFromQueue);
                     menu.exec(
                         ui.queueListWidget->viewport()->mapToGlobal(pos));
                   });

  QObject::connect(ui.actionCleanNotRunning, &QAction::triggered, this, [=]() {
    int jobsCount = ((ui.jobs->count() - 2) / 2 - mJobCount);

    int button = QMessageBox::question(this, "Rclone Browser",
                                       QString("There are %1 inactive job(s).\n"
                                               "\nDo you want to clean them?")
                                           .arg(jobsCount),
                                       QMessageBox::Yes | QMessageBox::No);

    if (button == QMessageBox::Yes) {
      for (int j = 0; j < ui.jobs->count(); j++) {
        for (int i = 0; i < ui.jobs->count(); i++) {
          QWidget *widget = ui.jobs->itemAt(i)->widget();
          if (auto mount = qobject_cast<MountWidget *>(widget)) {
            if (!(mount->isRunning)) {
              emit mount->closed();
            }
          } else if (auto transfer = qobject_cast<JobWidget *>(widget)) {
            if (!(transfer->isRunning)) {
              emit transfer->closed();
            }
          } else if (auto stream = qobject_cast<StreamWidget *>(widget)) {
            if (!(stream->isRunning)) {
              emit stream->closed();
            }
          }
        }
      }
    }
  });

  QObject::connect(ui.actionStopAllTransfers, &QAction::triggered, this, [=]() {
    // we only stop transfer jobs - others are intact
    if (mTransferJobCount != 0) {

      int button = QMessageBox::question(
          this, "Rclone Browser",
          QString("There are %1 transfer job(s) running.\n"
                  "\nDo you want to stop  them?")
              .arg(mTransferJobCount),
          QMessageBox::Yes | QMessageBox::No);

      if (button == QMessageBox::Yes) {

        for (int i = 0; i < ui.jobs->count(); i++) {
          QWidget *widget = ui.jobs->itemAt(i)->widget();
          if (auto transfer = qobject_cast<JobWidget *>(widget)) {
            emit transfer->cancel();
          }
        }
      }
    }
  });

  QObject::connect(ui.actionSortTask, &QAction::triggered, this, [=]() {
    auto settings = GetSettings();
    bool sortTask = settings->value("Settings/sortTask").toBool();

    if (sortTask) {
      ui.actionSortTask->setIcon(sortAZIcon);
      ui.buttonSortTask->setToolTip("Sort Ascending");
      ui.tasksListWidget->setSortingEnabled(true);
      ui.tasksListWidget->sortItems(Qt::DescendingOrder);
      settings->setValue("Settings/sortTask", "false");
    } else {
      ui.actionSortTask->setIcon(sortZAIcon);
      settings->setValue("Settings/sortTask", "true");
      ui.tasksListWidget->setSortingEnabled(true);
      ui.tasksListWidget->sortItems(Qt::AscendingOrder);
      ui.buttonSortTask->setToolTip("Sort Descending");
    }
  });

  QObject::connect(ui.actionEdit, &QAction::triggered, this, [=]() {
    auto items = ui.tasksListWidget->selectedItems();

    if (items.count() != 1) {
      QMessageBox::information(this, tr("Edit task"),
                               tr("Please select one task to edit"));
    }

    else {
      editSelectedTask();
    }
  });

  QObject::connect(ui.actionDryRun, &QAction::triggered, this, [=]() {
    auto selection = ui.tasksListWidget->selectedItems();

    auto settings = GetSettings();
    bool sortTask = settings->value("Settings/sortTask").toBool();

    auto items = sortListWidget(selection, sortTask);

    QString itemsToRun;

    foreach (auto i, items) {
      JobOptionsListWidgetItem *item =
          static_cast<JobOptionsListWidgetItem *>(i);
      JobOptions *jo = item->GetData();
      itemsToRun = itemsToRun + jo->description + "\n";
    }

    if (items.count() > 0) {
      int button = QMessageBox::question(
          this, "Dry run",
          QString(
              "Are you sure you want to dry run the following task(s)?\n\n" +
              itemsToRun),
          QMessageBox::Yes | QMessageBox::No);
      if (button == QMessageBox::Yes) {
        foreach (auto i, items) {
          JobOptionsListWidgetItem *item =
              static_cast<JobOptionsListWidgetItem *>(i);
          runItem(item, "task", true);
        }
      }
    }
    ui.tasksListWidget->setFocus();
  });

  QObject::connect(ui.actionRun, &QAction::triggered, this, [=]() {
    auto selection = ui.tasksListWidget->selectedItems();

    auto settings = GetSettings();
    bool sortTask = settings->value("Settings/sortTask").toBool();

    auto items = sortListWidget(selection, sortTask);

    QString itemsToRun;
    QString itemsAlreadyRunning;

    foreach (auto i, items) {
      JobOptionsListWidgetItem *item =
          static_cast<JobOptionsListWidgetItem *>(i);
      JobOptions *jo = item->GetData();
      itemsToRun = itemsToRun + jo->description + "\n";

      // check if task already running
      for (int j = 0; j < ui.jobs->count(); j++) {
        QWidget *widget = ui.jobs->itemAt(j)->widget();
        if (auto transfer = qobject_cast<JobWidget *>(widget)) {
          if ((transfer->getUniqueID() == jo->uniqueId.toString()) &&
              (transfer->isRunning)) {
            itemsAlreadyRunning = itemsAlreadyRunning + jo->description + "\n";
          }
        }
      }
    }

    if (!itemsAlreadyRunning.isEmpty()) {
      QMessageBox::critical(
          this, "Run",
          QString("Failed as the following task(s) are already running.\n\n" +
                  itemsAlreadyRunning),
          QMessageBox::Ok);
      return;
    }

    if (items.count() > 0) {
      int button = QMessageBox::question(
          this, "Run",
          QString("Are you sure you want to run the following task(s)?\n\n" +
                  itemsToRun),
          QMessageBox::No | QMessageBox::Yes);
      if (button == QMessageBox::Yes) {
        foreach (auto i, items) {
          JobOptionsListWidgetItem *item =
              static_cast<JobOptionsListWidgetItem *>(i);
          runItem(item, "task");
        }
      }
    }
    ui.tasksListWidget->setFocus();
  });

  QObject::connect(ui.actionDelete, &QAction::triggered, this, [=]() {
    auto selection = ui.tasksListWidget->selectedItems();

    auto settings = GetSettings();
    bool sortTask = settings->value("Settings/sortTask").toBool();

    auto items = sortListWidget(selection, sortTask);

    QString itemsToDelete;
    QString itemsAlreadyRunning;

    foreach (auto i, items) {
      JobOptionsListWidgetItem *item =
          static_cast<JobOptionsListWidgetItem *>(i);
      JobOptions *jo = item->GetData();
      itemsToDelete = itemsToDelete + jo->description + "\n";

      // check if any tasks already running
      for (int j = 0; j < ui.jobs->count(); j++) {
        QWidget *widget = ui.jobs->itemAt(j)->widget();
        if (auto transfer = qobject_cast<JobWidget *>(widget)) {
          if ((transfer->getUniqueID() == jo->uniqueId.toString()) &&
              (transfer->isRunning)) {
            itemsAlreadyRunning = itemsAlreadyRunning + jo->description + "\n";
          }
        }
      }
    }

    // dont delete running tasks
    if (!itemsAlreadyRunning.isEmpty()) {
      QMessageBox::critical(
          this, "Run",
          QString("Failed as the following task(s) are running:\n\n" +
                  itemsAlreadyRunning +
                  "\n\nYou can't delete running tasks.\nStop them first."),
          QMessageBox::Ok);
      ui.tasksListWidget->setFocus();
      return;
    }

    if (items.count() > 0) {
      // all clear - delete
      int button = QMessageBox::warning(
          this, "Delete",
          QString("Are you sure you want to delete the following task(s)?\n\n" +
                  itemsToDelete),
          QMessageBox::No | QMessageBox::Yes);

      if (button == QMessageBox::Yes) {
        foreach (auto i, items) {
          JobOptionsListWidgetItem *item =
              static_cast<JobOptionsListWidgetItem *>(i);
          JobOptions *jo = item->GetData();
          ListOfJobOptions::getInstance()->Forget(jo);
        }
      } else {
        ui.tasksListWidget->setFocus();
      }
    }
  });

  QObject::connect(ui.actionAddToQueue, &QAction::triggered, this, [=]() {
    auto selection = ui.tasksListWidget->selectedItems();

    auto settings = GetSettings();
    bool sortTask = settings->value("Settings/sortTask").toBool();

    auto items = sortListWidget(selection, sortTask);

    QString itemsToAdd;

    // create list of selected tasks' names
    foreach (auto i, items) {
      JobOptionsListWidgetItem *item =
          static_cast<JobOptionsListWidgetItem *>(i);
      JobOptions *jo = item->GetData();
      itemsToAdd = itemsToAdd + jo->description + "\n";
    }

    // if queue is empty we have to start first task
    bool isQueueEmpty = (ui.queueListWidget->count() == 0);

    if (items.count() > 0) {
      int button = QMessageBox::question(
          this, "Add to queue",
          QString("Are you sure you want to add the following "
                  "task(s) to the queue?\n\n" +
                  itemsToAdd),
          QMessageBox::No | QMessageBox::Yes);

      if (button == QMessageBox::Yes) {

        mQueueCount = mQueueCount + items.count();
        if (mQueueStatus) {
          if (mQueueCount == 0) {
            ui.tabs->setTabText(3, QString("Queue (%1)>>(0)").arg(mQueueCount));
          } else {
            ui.tabs->setTabText(
                3, QString("Queue (%1)>>(1)").arg(mQueueCount - 1));
          }
        } else {
          ui.tabs->setTabText(3, QString("Queue (%1)").arg(mQueueCount));
        }

        foreach (auto i, items) {
          JobOptionsListWidgetItem *item =
              static_cast<JobOptionsListWidgetItem *>(i);

          JobOptions *jo = item->GetData();

          JobOptionsListWidgetItem *newitem = new JobOptionsListWidgetItem(
              jo,
              jo->jobType == JobOptions::JobType::Download ? mDownloadIcon
                                                           : mUploadIcon,
              jo->description);

          ui.queueListWidget->addItem(newitem);
        }

        if (ui.queueListWidget->count() > 0) {
          ui.buttonPurgeQueue->setEnabled(true);
        } else {
          ui.buttonPurgeQueue->setEnabled(false);
        }
        // save new queue to file
        saveQueueFile();
        ui.queueListWidget->setFocus();
      } else {
        // user pressed No
        return;
      }
    }

    // if queue was empty we start first taks if queue is running
    if (mQueueStatus && isQueueEmpty) {

      if (mQueueCount > 0) {

        JobOptionsListWidgetItem *item =
            static_cast<JobOptionsListWidgetItem *>(
                ui.queueListWidget->item(0));

        JobOptions *jo = item->GetData();

        // check first if maybe already running (manually by user??)
        // in that case we only mark it as processing
        bool isAlreadyRunning = false;
        for (int j = 0; j < ui.jobs->count(); j++) {
          QWidget *widget = ui.jobs->itemAt(j)->widget();
          if (auto transfer = qobject_cast<JobWidget *>(widget)) {
            if ((transfer->getUniqueID() == jo->uniqueId.toString()) &&
                (transfer->isRunning)) {
              isAlreadyRunning = true;
              break;
            }
          }
        }
        if (!isAlreadyRunning) {
          runItem(item, "queue");
        }
        ui.queueListWidget->item(0)->setBackground(Qt::darkGreen);
      }
    }
  });

  QObject::connect(ui.actionStartQueue, &QAction::triggered, this, [=]() {
    mQueueStatus = true;

    if (mQueueCount == 0) {
      ui.tabs->setTabText(3, QString("Queue (%1)>>(0)").arg(mQueueCount));
    } else {
      ui.tabs->setTabText(3, QString("Queue (%1)>>(1)").arg(mQueueCount - 1));
    }

    ui.buttonStopQueue->setEnabled(true);
    ui.buttonStartQueue->setEnabled(false);

    ui.buttonPurgeQueue->setEnabled(false);

    if (mQueueCount > 1) {
      ui.buttonPurgeQueue->setEnabled(true);
    } else {
      ui.buttonPurgeQueue->setEnabled(false);
    }

    if (ui.queueListWidget->currentRow() == 0 ||
        ui.queueListWidget->currentRow() == ui.queueListWidget->count() - 1) {
      ui.buttonDownQueue->setEnabled(false);
    } else {
      ui.buttonDownQueue->setEnabled(true);
    }

    if (ui.queueListWidget->currentRow() == 1 ||
        ui.queueListWidget->currentRow() == 0) {
      ui.buttonUpQueue->setEnabled(false);
    } else {
      ui.buttonUpQueue->setEnabled(true);
    }

    if (ui.queueListWidget->currentRow() == 0) {

      ui.buttonRemoveFromQueue->setEnabled(false);
      ui.buttonUpQueue->setEnabled(false);
      ui.buttonDownQueue->setEnabled(false);

    } else {
      ui.buttonRemoveFromQueue->setEnabled(true);
    }

    if (mQueueCount == 0) {
      ui.buttonRemoveFromQueue->setEnabled(false);
      ui.buttonPurgeQueue->setEnabled(false);
      ui.buttonUpQueue->setEnabled(false);
      ui.buttonDownQueue->setEnabled(false);
    }

    ui.labelQueueInfoStart->setText("Queue is running.");
    ui.labelQueueInfoStart->show();
    ui.labelQueueInfoStop->hide();

    // if not empty start first task
    if (mQueueCount > 0) {
      JobOptionsListWidgetItem *item =
          static_cast<JobOptionsListWidgetItem *>(ui.queueListWidget->item(0));
      JobOptions *jo = item->GetData();

      // check first if maybe already running (manually by user??)
      // in that case we just wait
      bool isAlreadyRunning = false;
      for (int j = 0; j < ui.jobs->count(); j++) {
        QWidget *widget = ui.jobs->itemAt(j)->widget();
        if (auto transfer = qobject_cast<JobWidget *>(widget)) {
          if ((transfer->getUniqueID() == jo->uniqueId.toString()) &&
              (transfer->isRunning)) {
            isAlreadyRunning = true;
            break;
          }
        }
      }
      if (!isAlreadyRunning) {
        runItem(item, "queue");
      }
      ui.queueListWidget->item(0)->setBackground(Qt::darkGreen);
      ui.queueListWidget->item(0)->setSelected(false);
    }
  });

  QObject::connect(ui.actionStopQueue, &QAction::triggered, this, [=]() {
    mQueueStatus = false;

    if (ui.queueListWidget->count() > 0) {
      ui.queueListWidget->item(0)->setBackground(QBrush());
    }

    ui.buttonStopQueue->setEnabled(false);
    ui.buttonStartQueue->setEnabled(true);
    ui.buttonPurgeQueue->setEnabled(true);
    ui.buttonRemoveFromQueue->setEnabled(true);

    if (mQueueCount == 0) {
      ui.buttonRemoveFromQueue->setEnabled(false);
      ui.buttonPurgeQueue->setEnabled(false);
      ui.buttonUpQueue->setEnabled(false);
      ui.buttonDownQueue->setEnabled(false);
      ui.tabs->setTabText(3, QString("Queue"));
    } else {
      ui.tabs->setTabText(3, QString("Queue (%1)").arg(mQueueCount));
    }

    if (!(ui.queueListWidget->item(0) == nullptr)) {
      JobOptionsListWidgetItem *item =
          static_cast<JobOptionsListWidgetItem *>(ui.queueListWidget->item(0));

      JobOptions *jo = item->GetData();

      for (int j = 0; j < ui.jobs->count(); j++) {
        QWidget *widget = ui.jobs->itemAt(j)->widget();
        if (auto transfer = qobject_cast<JobWidget *>(widget)) {

          if ((transfer->getUniqueID() == jo->uniqueId.toString()) &&
              (transfer->isRunning)) {
            emit transfer->cancel();
          }
        }
      }
    }

    ui.labelQueueInfoStop->setText("Queue is not running.");
    ui.labelQueueInfoStop->show();
    ui.labelQueueInfoStart->hide();
  });

  QObject::connect(ui.actionPurgeQueue, &QAction::triggered, this, [=]() {
    if (ui.queueListWidget->count() > 0) {

      int button = QMessageBox::warning(
          this, "Clean the queue",
          QString("Are you sure you want to remove all\nnot running tasks from "
                  "the queue?"),
          QMessageBox::No | QMessageBox::Yes);

      if (button == QMessageBox::Yes) {

        int itemsCount = ui.queueListWidget->count();
        for (int i = 0; i < itemsCount; i++) {

          if (mQueueStatus) {
            if (i != 0) {
              --mQueueCount;
              ui.queueListWidget->takeItem(1);
            }
          } else {
            --mQueueCount;
            ui.queueListWidget->takeItem(0);
          }

          ui.buttonRemoveFromQueue->setEnabled(false);
          ui.buttonPurgeQueue->setEnabled(false);
          ui.buttonUpQueue->setEnabled(false);
          ui.buttonDownQueue->setEnabled(false);
        }
      } else {
        ui.queueListWidget->setFocus();
      }
    }

    if (mQueueStatus) {

      if (mQueueCount == 0) {
        ui.tabs->setTabText(3, QString("Queue (%1)>>(0)").arg(mQueueCount));
      } else {
        ui.tabs->setTabText(3, QString("Queue (%1)>>(1)").arg(mQueueCount - 1));
      }

    } else {
      if (mQueueCount != 0) {
        ui.tabs->setTabText(3, QString("Queue (%1)").arg(mQueueCount));
      } else {
        ui.tabs->setTabText(3, QString("Queue"));
      }
    }

    saveQueueFile();
  });

  QObject::connect(ui.actionRemoveFromQueue, &QAction::triggered, this, [=]() {
    if (mQueueStatus) {

      if (mQueueCount > 0) {
        --mQueueCount;
        ui.queueListWidget->takeItem(ui.queueListWidget->currentRow());

        if (ui.queueListWidget->currentRow() == 0 ||
            ui.queueListWidget->currentRow() ==
                ui.queueListWidget->count() - 1) {
          ui.buttonDownQueue->setEnabled(false);
        } else {
          ui.buttonDownQueue->setEnabled(true);
        }

        if (ui.queueListWidget->currentRow() == 0 ||
            ui.queueListWidget->currentRow() == 1) {
          ui.buttonUpQueue->setEnabled(false);
        } else {
          ui.buttonUpQueue->setEnabled(true);
        }
      }
      if (ui.queueListWidget->count() == 1) {
        ui.buttonRemoveFromQueue->setEnabled(false);
      }
      if (mQueueCount == 0) {
        ui.tabs->setTabText(3, QString("Queue (%1)>>(0)").arg(mQueueCount));
      } else {
        ui.tabs->setTabText(3, QString("Queue (%1)>>(1)").arg(mQueueCount - 1));
      }

    } else {

      --mQueueCount;
      ui.queueListWidget->takeItem(ui.queueListWidget->currentRow());

      if (ui.queueListWidget->currentRow() == ui.queueListWidget->count() - 1) {
        ui.buttonDownQueue->setEnabled(false);
      } else {
        ui.buttonDownQueue->setEnabled(true);
      }
      if (ui.queueListWidget->currentRow() == 0) {
        ui.buttonUpQueue->setEnabled(false);
      } else {
        ui.buttonUpQueue->setEnabled(true);
      }

      if (ui.queueListWidget->count() == 0) {

        ui.buttonRemoveFromQueue->setEnabled(false);
        ui.buttonPurgeQueue->setEnabled(false);
        ui.buttonUpQueue->setEnabled(false);
        ui.buttonDownQueue->setEnabled(false);
        ui.tabs->setTabText(3, QString("Queue"));

      } else {
        ui.tabs->setTabText(3, QString("Queue (%1)").arg(mQueueCount));
      }
    }

    saveQueueFile();
  });

  QObject::connect(ui.actionDownQueue, &QAction::triggered, this, [=]() {
    auto item = ui.queueListWidget->takeItem(ui.queueListWidget->currentRow());
    ui.queueListWidget->insertItem(ui.queueListWidget->currentRow() + 1, item);
    ui.queueListWidget->setCurrentRow(ui.queueListWidget->row(item));

    saveQueueFile();
  });

  QObject::connect(ui.actionUpQueue, &QAction::triggered, this, [=]() {
    if (ui.queueListWidget->currentRow() == ui.queueListWidget->count() - 1) {
      // last row needs special treatment
      auto item =
          ui.queueListWidget->takeItem(ui.queueListWidget->currentRow());
      ui.queueListWidget->insertItem(ui.queueListWidget->currentRow(), item);
      ui.queueListWidget->setCurrentRow(ui.queueListWidget->row(item));
    } else {
      auto item =
          ui.queueListWidget->takeItem(ui.queueListWidget->currentRow());
      ui.queueListWidget->insertItem(ui.queueListWidget->currentRow() - 1,
                                     item);
      ui.queueListWidget->setCurrentRow(ui.queueListWidget->row(item));
    }

    saveQueueFile();
  });

  QObject::connect(ui.queueListWidget, &QListWidget::itemChanged, this,
                   [=]() { setQueueButtons(); });

  QObject::connect(ui.queueListWidget, &QListWidget::itemClicked, this,
                   [=]() { setQueueButtons(); });

  QObject::connect(ui.queueListWidget, &QListWidget::currentItemChanged, this,
                   [=]() { setQueueButtons(); });

  QObject::connect(ui.queueListWidget, &QListWidget::itemSelectionChanged, this,
                   [=]() { setQueueButtons(); });

  QObject::connect(ListOfJobOptions::getInstance(),
                   &ListOfJobOptions::tasksListUpdated, this,
                   &MainWindow::listTasks);

  mUploadIcon = arrowUpIcon;
  mDownloadIcon = arrowDownIcon;

  // remove close button from these tabs
  ui.tabs->tabBar()->setTabButton(0, QTabBar::RightSide, nullptr);
  ui.tabs->tabBar()->setTabButton(0, QTabBar::LeftSide, nullptr);
  ui.tabs->tabBar()->setTabButton(1, QTabBar::RightSide, nullptr);
  ui.tabs->tabBar()->setTabButton(1, QTabBar::LeftSide, nullptr);
  ui.tabs->tabBar()->setTabButton(2, QTabBar::RightSide, nullptr);
  ui.tabs->tabBar()->setTabButton(2, QTabBar::LeftSide, nullptr);
  ui.tabs->tabBar()->setTabButton(3, QTabBar::RightSide, nullptr);
  ui.tabs->tabBar()->setTabButton(3, QTabBar::LeftSide, nullptr);

  ui.tabs->setCurrentIndex(0);

  addTasksToQueue();
  listTasks();

  QObject::connect(&mSystemTray, &QSystemTrayIcon::activated, this,
                   [=](QSystemTrayIcon::ActivationReason reason) {
                     if (reason == QSystemTrayIcon::DoubleClick ||
                         reason == QSystemTrayIcon::Trigger) {
                       showNormal();
                       mSystemTray.setVisible(mAlwaysShowInTray);
#ifdef Q_OS_MACOS
                       osxShowDockIcon();
#endif
                     }
                   });

  QObject::connect(&mSystemTray, &QSystemTrayIcon::messageClicked, this, [=]() {
    showNormal();
    mSystemTray.setVisible(mAlwaysShowInTray);
#ifdef Q_OS_MACOS
    osxShowDockIcon();
#endif

    ui.tabs->setCurrentIndex(1);
    if (mLastFinished) {
      mLastFinished->showDetails();
      ui.jobsArea->ensureWidgetVisible(mLastFinished);
    }
  });

  QMenu *trayMenu = new QMenu(this);
  QObject::connect(
      trayMenu->addAction("&Show"), &QAction::triggered, this, [=]() {
        MainWindow::setWindowState((windowState() & ~Qt::WindowMinimized) |
                                   Qt::WindowActive);
        MainWindow::show();  // bring window to top on macOS
        MainWindow::raise(); // bring window from minimized state on macOS
        MainWindow::activateWindow(); // bring window to front/unminimize on
                                      // windows
        mSystemTray.setVisible(mAlwaysShowInTray);
#ifdef Q_OS_MACOS
        osxShowDockIcon();
#endif
      });

  QObject::connect(trayMenu->addAction("&Quit"), &QAction::triggered, this,
                   [=]() {
                     if (canClose()) {
                       QApplication::quit();
                     }
                   });

  mSystemTray.setContextMenu(trayMenu);

  mStatusMessage = new QLabel();
  ui.statusBar->addWidget(mStatusMessage);
  ui.statusBar->setStyleSheet("QStatusBar::item { border: 0; }");

  QTimer::singleShot(0, ui.remotes, SLOT(setFocus()));

  QString rclone = GetRclone();
  if (rclone.isEmpty()) {
    rclone = QStandardPaths::findExecutable("rclone");
    if (rclone.isEmpty()) {
      QMessageBox::information(
          this, "Error",
          "Cannot check rclone version!\nPlease verify rclone location.");
      emit ui.preferences->trigger();
    } else {
      auto settings = GetSettings();
      settings->setValue("Settings/rclone", rclone);
      SetRclone(rclone);
    }
  } else {
    rcloneGetVersion();
  }
}

MainWindow::~MainWindow() {
  auto settings = GetSettings();
  settings->setValue("MainWindow/geometry", saveGeometry());
}

QList<QListWidgetItem *>
MainWindow::sortListWidget(const QList<QListWidgetItem *> &list,
                           bool sortOrder) {

  QList<QListWidgetItem *> sortedList = list;

  int n = sortedList.size();
  int min_idx;

  // basic selection sort algorithm
  for (int i = 0; i < (n - 1); i++) {
    min_idx = i;
    for (int j = i + 1; j < n; j++) {

      JobOptionsListWidgetItem *item_j =
          static_cast<JobOptionsListWidgetItem *>(sortedList.at(j));
      JobOptions *jo_j = item_j->GetData();

      JobOptionsListWidgetItem *item_min_idx =
          static_cast<JobOptionsListWidgetItem *>(sortedList.at(min_idx));
      JobOptions *jo_min_idx = item_min_idx->GetData();

      if (sortOrder) {
        if ((jo_j->description).toUpper() < (jo_min_idx->description).toUpper()) {
          min_idx = j;
        }
      } else {
        if ((jo_j->description).toUpper() > (jo_min_idx->description).toUpper() ) {
          min_idx = j;
        }
      }
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    sortedList.swapItemsAt(min_idx, i);
#else
    sortedList.swap(min_idx, i);
#endif
  }
  return sortedList;
}

void MainWindow::setQueueButtons() {

  if (ui.queueListWidget->selectedItems().empty()) {

    ui.buttonDownQueue->setEnabled(false);
    ui.buttonUpQueue->setEnabled(false);
    ui.buttonRemoveFromQueue->setEnabled(false);
    ui.actionDownQueue->setEnabled(false);
    ui.actionUpQueue->setEnabled(false);
    ui.actionRemoveFromQueue->setEnabled(false);

    if (mQueueStatus) {

      if (ui.queueListWidget->count() > 1) {
        ui.buttonPurgeQueue->setEnabled(true);
      } else {
        ui.buttonPurgeQueue->setEnabled(false);
      }

      ui.queueListWidget->item(0)->setSelected(false);

    } else {

      if (ui.queueListWidget->count() > 0) {
        ui.buttonPurgeQueue->setEnabled(true);
      } else {
        ui.buttonPurgeQueue->setEnabled(false);
      }
    }
    return;
  }

  if (mQueueStatus) {

    if (ui.queueListWidget->count() > 1) {
      ui.buttonPurgeQueue->setEnabled(true);
    } else {
      ui.buttonPurgeQueue->setEnabled(false);
    }

    if (ui.queueListWidget->currentRow() == 0 ||
        ui.queueListWidget->currentRow() == ui.queueListWidget->count() - 1) {
      ui.buttonDownQueue->setEnabled(false);
      ui.actionDownQueue->setEnabled(false);
    } else {
      ui.buttonDownQueue->setEnabled(true);
      ui.actionDownQueue->setEnabled(true);
    }

    if (ui.queueListWidget->currentRow() == 1 ||
        ui.queueListWidget->currentRow() == 0) {
      ui.buttonUpQueue->setEnabled(false);
      ui.actionUpQueue->setEnabled(false);
    } else {
      ui.buttonUpQueue->setEnabled(true);
      ui.actionUpQueue->setEnabled(true);
    }

    if (ui.queueListWidget->currentRow() == 0) {
      ui.buttonRemoveFromQueue->setEnabled(false);
      ui.actionRemoveFromQueue->setEnabled(false);
    } else {
      ui.buttonRemoveFromQueue->setEnabled(true);
      ui.actionRemoveFromQueue->setEnabled(true);
    }

    ui.queueListWidget->item(0)->setSelected(false);

  } else {

    if (ui.queueListWidget->count() > 0) {
      ui.buttonPurgeQueue->setEnabled(true);
    } else {
      ui.buttonPurgeQueue->setEnabled(false);
    }

    if (ui.queueListWidget->currentRow() == 0) {
      ui.buttonUpQueue->setEnabled(false);
      ui.actionUpQueue->setEnabled(false);
    } else {
      ui.buttonUpQueue->setEnabled(true);
      ui.actionUpQueue->setEnabled(true);
    }

    if (ui.queueListWidget->currentRow() == ui.queueListWidget->count() - 1) {
      ui.buttonDownQueue->setEnabled(false);
      ui.actionDownQueue->setEnabled(false);
    } else {
      ui.buttonDownQueue->setEnabled(true);
      ui.actionDownQueue->setEnabled(true);
    }

    if (ui.queueListWidget->count() > 0) {
      ui.buttonPurgeQueue->setEnabled(true);
    } else {
      ui.buttonPurgeQueue->setEnabled(false);
    }

    ui.buttonRemoveFromQueue->setEnabled(true);
    ui.actionRemoveFromQueue->setEnabled(true);
  }
}

void MainWindow::rcloneGetVersion() {
  bool firstTime = mFirstTime;
  mFirstTime = false;

  QProcess *p = new QProcess();

  QObject::connect(
      p,
      static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
          &QProcess::finished),
      this, [=](int code, QProcess::ExitStatus) {
        if (code == 0) {
          QString version = p->readAllStandardOutput().trimmed();

          // extract rclone version - numbers only
          QString rclone_info1 = version;
          QString rclone_version_no;
          int lineBreak = rclone_info1.indexOf('\n');
          if (lineBreak != -1) {
            rclone_info1.remove(lineBreak, rclone_info1.length() - lineBreak);
            rclone_version_no = rclone_info1;
            rclone_version_no.replace("rclone v", "");
            rclone_version_no.replace("-DEV", "");
          } else {
            // for very old rclone versions format was one line only
            rclone_version_no = rclone_info1.trimmed();
            rclone_version_no.replace("rclone v", "");
            rclone_version_no.replace("-DEV", "");
          }
          // save current version no in settings
          auto settings = GetSettings();
          settings->setValue("Settings/rcloneVersion", rclone_version_no);

#if defined(Q_OS_WIN32)
          // check if required version
          unsigned int result =
              compareVersion(rclone_version_no.toStdString(), "1.50");

          if (result == 2) {
            QMessageBox::warning(
                this, "",
                "For mount functionality to work you need "
                "rclone version at least v1.50 "
                "and your current version is v" +
                    rclone_version_no +
                    ". Mount will be disabled. \n\nPlease consider upgrading.");
          };
#endif

          QStringList lines = version.split("\n", QString::SkipEmptyParts);
          QString rclone_info2;
          QString rclone_info3;

          int counter = 0;
          foreach (QString line, lines) {
            line = line.trimmed();
            if (counter == 1)
              rclone_info2 = line.replace("- ", "");
            if (counter == 2)
              rclone_info3 = line.replace("- ", "");
            counter++;
          };

          QFileInfo appBundlePath;
#ifdef Q_OS_MACOS
          if (IsPortableMode()) {

            QFileInfo applicationPath = qApp->applicationFilePath();
            QFileInfo MacOSPath = applicationPath.dir().path();
            QFileInfo ContentsPath = MacOSPath.dir().path();
            appBundlePath = ContentsPath.dir().path();

            mStatusMessage->setText(rclone_info1 + ", " + rclone_info2 + ", " +
                                    rclone_info3);

            mStatusMessage->setToolTip(
                rclone_info1 + " in " +
                QDir::toNativeSeparators(GetRclone().replace(
                    appBundlePath.fileName() + "/Contents/MacOS/../../../",
                    "")) +
                ", " + rclone_info2 + ", " + rclone_info3);

          } else {

            mStatusMessage->setText(rclone_info1 + ", " + rclone_info2 + ", " +
                                    rclone_info3);

            mStatusMessage->setToolTip(
                rclone_info1 + " in " + QDir::toNativeSeparators(GetRclone()) +
                ", " + rclone_info2 + ", " + rclone_info3);
          }
#else
#ifdef Q_OS_WIN
          mStatusMessage->setText(rclone_info1 + ", " +
                                  rclone_info2 + ", " + rclone_info3);


          mStatusMessage->setToolTip(rclone_info1 + " in " +
                                  QDir::toNativeSeparators(GetRclone()) + ", " +
                                  rclone_info2 + ", " + rclone_info3);

#else
          if (IsPortableMode()) {
            QString xdg_config_home = qgetenv("XDG_CONFIG_HOME");
            QString appImageConfigFolder = xdg_config_home.right(xdg_config_home.length()-xdg_config_home.lastIndexOf("/"));

            mStatusMessage->setText(rclone_info1 + ", " +
                                  rclone_info2 + ", " + rclone_info3);

            mStatusMessage->setToolTip(rclone_info1 + " in " +
                                  QDir::toNativeSeparators(GetRclone().replace(appImageConfigFolder + "/..",  "")) + ", " +
                                  rclone_info2 + ", " + rclone_info3);


          } else {
            mStatusMessage->setText(rclone_info1 + ", " +
                                  rclone_info2 + ", " + rclone_info3);

            mStatusMessage->setToolTip(rclone_info1 + " in " +
                                  QDir::toNativeSeparators(GetRclone()) + ", " +
                                  rclone_info2 + ", " + rclone_info3);


         }
#endif
#endif

          rcloneListRemotes();
        } else {
          if (p->error() != QProcess::FailedToStart) {
            if (getConfigPassword(p)) {
              rcloneGetVersion();
            } else {
              close();
            }
            p->deleteLater();
            return;
          }

          if (firstTime) {
            if (p->error() == QProcess::FailedToStart) {
              QMessageBox::information(
                  this, "Error",
                  "Wrong rclone executable or rclone not found!\nPlease select "
                  "its location in next dialog.");
            } else {
              QMessageBox::information(this, "Error",
                                       "Cannot check rclone version!\nPlease "
                                       "verify rclone location.");
            }
            emit ui.preferences->trigger();
          }
        }

        auto settings = GetSettings();

        /// check rclone version

        // get already stored rclone version no
        QString rclone_version_no =
            settings->value("Settings/rcloneVersion").toString();

        // during first run the key might not exist yet
        if (!(settings->contains("Settings/checkRcloneUpdates"))) {
          // if checkRcloneUpdates does not exist create new key
          settings->setValue("Settings/checkRcloneUpdates", true);
        };

        bool checkRcloneUpdates =
            settings->value("Settings/checkRcloneUpdates").toBool();

        // if check updates enabled in settings
        if (checkRcloneUpdates) {
          QString last_check;
          QString current_date = QDate::currentDate().toString();

          if (!(settings->contains("Settings/lastRcloneUpdateCheck"))) {
            // if lastRcloneUpdateCheck does not exist create new key
            settings->setValue("Settings/lastRcloneUpdateCheck", current_date);
          } else { // read last check date
            last_check =
                settings->value("Settings/lastRcloneUpdateCheck").toString();
          };

          // dont check if already checked today (once per day only)
          if (!(last_check == current_date)) {
            // remmber when last checked
            settings->setValue("Settings/lastRcloneUpdateCheck", current_date);

            QString url =
                "https://api.github.com/repos/rclone/rclone/releases/latest";
            QNetworkAccessManager manager;
            QNetworkReply *response = manager.get(QNetworkRequest(QUrl(url)));
            QEventLoop event;
            connect(response, SIGNAL(finished()), &event, SLOT(quit()));
            event.exec();
            QByteArray content = response->readAll();
            QJsonParseError jsonError;

            QJsonDocument document = QJsonDocument::fromJson(
                content, &jsonError); // parse and capture the error flag

            if (jsonError.error == QJsonParseError::NoError) {

              if (document.object().contains("tag_name")) {

                QJsonValue tag_name = document.object().value("tag_name");

                QString rclone_latest_version_no = tag_name.toString(QString());

                rclone_latest_version_no.replace("v", "");
                rclone_latest_version_no.replace("-DEV", "");
                rclone_latest_version_no = rclone_latest_version_no.trimmed();

                // check if new version available and if yes display information
                unsigned int result =
                    compareVersion(rclone_latest_version_no.toStdString(),
                                   rclone_version_no.toStdString());
                // latest version is greater than current
                if (result == 1) {

                  QMessageBox::information(
                      this, "",
                      QString(
                          R"(<p>New rclone version is available</p>)"
                          R"(<p>You have: v)" +
                          rclone_version_no +
                          "<br />"
                          R"(New version: v)" +
                          rclone_latest_version_no +
                          "</p>"
                          R"(<p>Visit rclone <a href="https://rclone.org/downloads/">downloads</a> page to upgrade</p>)"));
                };
              };
            };
          };
        };

        /// check rclone browser version

        // during first run the key might not exist yet
        if (!(settings->contains("Settings/checkRcloneBrowserUpdates"))) {
          // if checkRcloneBrowserUpdates does not exist create new key
          settings->setValue("Settings/checkRcloneBrowserUpdates", true);
        };

        bool checkRcloneBrowserUpdates =
            settings->value("Settings/checkRcloneBrowserUpdates").toBool();

        // if check updates enabled in settings
        if (checkRcloneBrowserUpdates) {
          QString last_check;
          QString current_date = QDate::currentDate().toString();

          if (!(settings->contains("Settings/lastRcloneBrowserUpdateCheck"))) {
            // if lastRcloneBrowserUpdateCheck does not exist create new key
            settings->setValue("Settings/lastRcloneBrowserUpdateCheck",
                               current_date);
          } else { // read last check date
            last_check =
                settings->value("Settings/lastRcloneBrowserUpdateCheck")
                    .toString();
          };

          // dont check if already checked today (once per day only)
          if (!(last_check == current_date)) {
            // remmber when last checked
            settings->setValue("Settings/lastRcloneBrowserUpdateCheck",
                               current_date);

            // get latest version available
            QString url = "https://api.github.com/repos/kapitainsky/"
                          "rclonebrowser/releases/latest";
            QNetworkAccessManager manager;
            QNetworkReply *response = manager.get(QNetworkRequest(QUrl(url)));
            QEventLoop event;
            connect(response, SIGNAL(finished()), &event, SLOT(quit()));
            event.exec();
            QByteArray content = response->readAll();

            QJsonParseError jsonError;
            QJsonDocument document = QJsonDocument::fromJson(
                content, &jsonError); // parse and capture the error flag

            if (jsonError.error == QJsonParseError::NoError) {
              if (document.object().contains("tag_name")) {
                QJsonValue tag_name = document.object().value("tag_name");
                QString rclone_browser_latest_version_no =
                    tag_name.toString(QString());
                rclone_browser_latest_version_no =
                    rclone_browser_latest_version_no.trimmed();

                // check if new version available and if yes display information
                unsigned int result = compareVersion(
                    rclone_browser_latest_version_no.toStdString(),
                    RCLONE_BROWSER_VERSION);
                // latest version is greater than current
                if (result == 1) {
                  QMessageBox::information(
                      this, "",
                      QString(
                          R"(<p>New Rclone Browser version is available</p>)"
                          R"(<p>You have: v)" RCLONE_BROWSER_VERSION "<br />"
                          R"(New version: v)" +
                          rclone_browser_latest_version_no +
                          "</p>"
                          R"(<p>Visit <a href="https://github.com/kapitainsky/RcloneBrowser/releases/latest">releases</a> page to download</p>)"));
                };
              };
            };
          };
        };

        p->deleteLater();
      });

  UseRclonePassword(p);
  p->start(GetRclone(),
           QStringList() << "version"
                         << "--ask-password=false",
           QIODevice::ReadOnly);
}

void MainWindow::rcloneConfig() {

  // for macOS and Linux we have to take care of possible spaces in rclone and
  // rclone.conf paths by using "" around them
  QString terminalRcloneCmd;
  if (!GetRcloneConf().isEmpty()) {
    terminalRcloneCmd = "\"" + GetRclone() + "\"" + " config" + " --config " +
                        "\"" + GetRcloneConf().at(1) + "\"";
  } else {
    terminalRcloneCmd = "\"" + GetRclone() + "\"" + " config";
  }

#if defined(Q_OS_WIN32) && (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
  QProcess::startDetached(GetRclone(), QStringList()
                                           << "config" << GetRcloneConf());
  return;
#else

  QProcess *p = new QProcess(this);

  QObject::connect(p,
                   static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
                       &QProcess::finished),
                   this, [=](int code, QProcess::ExitStatus) {
                     if (code == 0) {
                       emit rcloneListRemotes();
                     }
                     p->deleteLater();
                   });
#endif

#if defined(Q_OS_WIN32)
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
  p->setCreateProcessArgumentsModifier(
      [](QProcess::CreateProcessArguments *args) {
        args->flags |= CREATE_NEW_CONSOLE;
        args->startupInfo->dwFlags &= ~STARTF_USESTDHANDLES;
      });
  p->setProgram(GetRclone());
  p->setArguments(QStringList() << "config" << GetRcloneConf());
#endif

#elif defined(Q_OS_MACOS)
  auto tmp = new QFile("/tmp/rclone_config.command");
  tmp->open(QIODevice::WriteOnly);
  QTextStream(tmp) << "#!/bin/sh\n" << terminalRcloneCmd << "\n";
  tmp->close();
  tmp->setPermissions(QFileDevice::ReadUser | QFileDevice::WriteUser |
                      QFileDevice::ExeUser | QFileDevice::ReadGroup |
                      QFileDevice::ExeGroup | QFileDevice::ReadOther |
                      QFileDevice::ExeOther);
  p->setProgram("open");
  p->setArguments(QStringList() << tmp->fileName());
#else
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  QString terminal = env.value("TERMINAL");
  if (terminal.isEmpty()) {
    terminal = QStandardPaths::findExecutable("gnome-terminal");
    if (terminal.isEmpty()) {
      terminal = QStandardPaths::findExecutable("xfce4-terminal");
      if (terminal.isEmpty()) {
        terminal = QStandardPaths::findExecutable("xterm");
        if (terminal.isEmpty()) {
          terminal = QStandardPaths::findExecutable("x-terminal-emulator");
          if (terminal.isEmpty()) {
            terminal = QStandardPaths::findExecutable("konsole");
            if (terminal.isEmpty()) {
              QMessageBox::critical(this, "Error",
                                    "Not sure how to launch terminal!\n"
                                    "Please set path to terminal executable in "
                                    "$TERMINAL environment variable.",
                                    QMessageBox::Ok);
              return;
            }
          }
        }
      }
    }
  }

  p->setArguments(QStringList() << "-e" << terminalRcloneCmd);
  p->setProgram(terminal);
#endif

#if !defined(Q_OS_WIN32) || (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
  UseRclonePassword(p);
  p->start(QIODevice::NotOpen);
#endif
}

void MainWindow::rcloneListRemotes() {

  ui.remotes->clear();

  auto settings = GetSettings();
  QString iconsLayout = settings->value("Settings/iconsLayout").toString();

  if (iconsLayout == "tiles") {
    ui.remotes->setViewMode(QListWidget::IconMode);
    // disable drag and drop
    ui.remotes->setMovement(QListView::Static);
    // always adjust icons after the window is resized
    ui.remotes->setResizeMode(QListView::Adjust);
    ui.remotes->setWrapping(true);
    ui.remotes->setSpacing(10);
  }
  if (iconsLayout == "longlist") {
    ui.remotes->setViewMode(QListWidget::ListMode);
    ui.remotes->setResizeMode(QListView::Adjust);
    ui.remotes->setWrapping(false);
  }
  if (iconsLayout == "list") {
    ui.remotes->setViewMode(QListWidget::ListMode);
    ui.remotes->setResizeMode(QListView::Adjust);
    ui.remotes->setWrapping(true);
  }

  QProcess *p = new QProcess();

  QObject::connect(
      p,
      static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
          &QProcess::finished),
      this, [=](int code, QProcess::ExitStatus) {
        if (code == 0) {
          QStyle *style = qApp->style();

          QString bytes = p->readAllStandardOutput().trimmed();
          QStringList items = bytes.split('\n');

          auto settings = GetSettings();
          bool darkModeIni = settings->value("Settings/darkModeIni").toBool();
          QString iconSize = settings->value("Settings/iconSize").toString();
          QString iconsColour =
              settings->value("Settings/iconsColour").toString();

          for (const QString &line : items) {
            if (line.isEmpty()) {
              continue;
            }

            QStringList parts = line.split(':');
            if (parts.count() != 2) {
              continue;
            }

            QString name = parts[0].trimmed();
            QString type = parts[1].trimmed();
            QString tooltip = type;

            QString img_add = "";
            int size;

            // medium scale by default
            double darkModeIconScale = 1.333;
            double lightModeiconScale = 2;
            // to avoid "variable not used" compiler error
            if (darkModeIconScale == lightModeiconScale) {
            };

            // set icons scale based on iconSize value
            if (iconSize == "S") {
              lightModeiconScale = 3;
              darkModeIconScale = 2;
            }

            if (iconSize == "M") {
              lightModeiconScale = 4;
              darkModeIconScale = 2.666;
            }

            if (iconSize == "L") {
              lightModeiconScale = 6;
              darkModeIconScale = 4;
            }

            if (iconSize == "XL") {
              lightModeiconScale = 8;
              darkModeIconScale = 5.333;
            }

            if (iconSize == "XXL") {
              lightModeiconScale = 15;
              darkModeIconScale = 10;
            }

#if !defined(Q_OS_MACOS)
            // _inv only for dark mode
            // we use darkModeIni to apply mode active at startup
            if (darkModeIni) {
              if (iconsColour == "white") {
                img_add = "_inv";
              } else {
                img_add = "";
              }
            } else {
              img_add = "";
            }
#if defined(Q_OS_WIN)
            // on Windows dark theme changes PM_ListViewIconSize size
            // so we have to adjust
            if (darkModeIni) {
              size = darkModeIconScale *
                     style->pixelMetric(QStyle::PM_ListViewIconSize);
            } else {
              size = lightModeiconScale *
                     style->pixelMetric(QStyle::PM_ListViewIconSize);
            }
#else
             // for Linux/BSD PM_ListViewIconSize stays the same
             size = lightModeiconScale * style->pixelMetric(QStyle::PM_ListViewIconSize);
#endif
#else
             QString sysInfo = QSysInfo::productVersion();
             // dark mode on older macOS
             if (sysInfo == "10.9" ||
                 sysInfo == "10.10" ||
                 sysInfo == "10.11" ||
                 sysInfo == "10.12" ||
                 sysInfo == "10.13") {

               // on older macOS we also have to adjust icon size per mode
               if (darkModeIni) {
                 size = darkModeIconScale * style->pixelMetric(QStyle::PM_ListViewIconSize);
                 if (iconsColour == "white") {
                   img_add = "_inv";
                 } else {
                   img_add = "";
                 }
               } else {
                 size = lightModeiconScale * style->pixelMetric(QStyle::PM_ListViewIconSize);
                 img_add = "";
               }

             } else {
               // for macOS > 10.13 native dark mode does not change IconSize base
               size = 1.5 * lightModeiconScale * style->pixelMetric(QStyle::PM_ListViewIconSize);
               if (iconsColour == "white") {
                  img_add = "_inv";
               } else {
                   img_add = "";
               }
             }
#endif
            ui.remotes->setIconSize(QSize(size, size));

            QString path =
                ":/remotes/images/" + type.replace(' ', '_') + img_add + ".png";
            QIcon icon(QFile(path).exists()
                           ? path
                           : ":/remotes/images/unknown" + img_add + ".png");

            QListWidgetItem *item = new QListWidgetItem(icon, name);
            item->setData(Qt::UserRole, type);
            item->setToolTip(tooltip);
            ui.remotes->addItem(item);
          }
        } else {
          if (p->error() != QProcess::FailedToStart) {
            if (getConfigPassword(p)) {
              rcloneListRemotes();
            }
          }
        }
        p->deleteLater();
      });

  UseRclonePassword(p);
  p->start(GetRclone(),
           QStringList() << "listremotes" << GetRcloneConf()
                         << GetDefaultRcloneOptionsList() << "--long"
                         << "--ask-password=false",
           QIODevice::ReadOnly);
}

bool MainWindow::getConfigPassword(QProcess *p) {
  QString output = p->readAllStandardError().trimmed();
  if (output.indexOf("RCLONE_CONFIG_PASS") > 0) {
    bool ok;
    QString password = QInputDialog::getText(
        this, qApp->applicationDisplayName(),
        "Enter password for .rclone.conf configuration file:",
        QLineEdit::Password, QString(), &ok);
    if (ok) {
      SetRclonePassword(password);
      return true;
    }
  } else if (output.indexOf("unknown command \"listremotes\"") > 0) {
    QMessageBox::critical(this, qApp->applicationDisplayName(),
                          "It seems rclone version you are using is too "
                          "old.\nPlease upgrade to the latest version");
    return false;
  }
  return false;
}

bool MainWindow::canClose() {
  if (mJobCount == 0) {
    return true;
  }

  bool wasVisible = isVisible();

  ui.tabs->setCurrentIndex(1);
  showNormal();

  int button =
      QMessageBox::question(this, "Rclone Browser",
                            QString("There are %1 job(s) running.\n"
                                    "\nDo you want to stop them and quit?")
                                .arg(mJobCount),
                            QMessageBox::Yes | QMessageBox::No);

  if (!wasVisible) {
    hide();
  }

  if (button == QMessageBox::Yes) {

    // make sure terminated job is not removed from the queue
    // we make close process aware that it is quitting
    mAppQuittingStatus = true;

    for (int i = 0; i < ui.jobs->count(); i++) {
      QWidget *widget = ui.jobs->itemAt(i)->widget();
      if (auto mount = qobject_cast<MountWidget *>(widget)) {
        emit mount->cancel();
      } else if (auto transfer = qobject_cast<JobWidget *>(widget)) {
        emit transfer->cancel();
      } else if (auto stream = qobject_cast<StreamWidget *>(widget)) {
        emit stream->cancel();
      }
    }
    return true;
  }

  return false;
}

void MainWindow::closeEvent(QCloseEvent *ev) {
  if (mCloseToTray && isVisible()) {
#ifdef Q_OS_MACOS
    osxHideDockIcon();
#endif
    mSystemTray.show();
    hide();
    ev->ignore();
    return;
  }

  if (canClose()) {
    QApplication::quit();
  } else {
    ev->ignore();
  }
}

void MainWindow::addTasksToQueue() {

  // restore queue from file
  // ignore no more existing

  ui.queueListWidget->clear();

  auto items = ui.tasksListWidget->selectedItems();

  ListOfJobOptions *ljo = ListOfJobOptions::getInstance();

  QString filePath = GetConfigDir().absoluteFilePath("queue.conf");
  QFile file(filePath);
  QTextStream in(&file);

  if (!file.open(QIODevice::ReadOnly)) {
    return;
  } else {

    while (!in.atEnd()) {

      QString line = in.readLine();

      for (JobOptions *jo : ljo->getTasks()) {
        JobOptionsListWidgetItem *item = new JobOptionsListWidgetItem(
            jo,
            jo->jobType == JobOptions::JobType::Download ? mDownloadIcon
                                                         : mUploadIcon,
            jo->description);

        if (jo->uniqueId.toString() == line) {
          ++mQueueCount;
          ui.queueListWidget->addItem(item);
        }
      }
    }

    file.close();

    if (mQueueCount == 0) {
      ui.tabs->setTabText(3, QString("Queue"));
    } else {
      ui.tabs->setTabText(3, QString("Queue (%1)").arg(mQueueCount));
    }

    ui.labelQueueInfoStop->setText("Queue is not running.");
    ui.labelQueueInfoStop->show();
    ui.labelQueueInfoStart->hide();
  }

  ui.buttonStopQueue->setEnabled(false);
  ui.buttonStartQueue->setEnabled(true);
  ui.buttonDownQueue->setEnabled(false);
  ui.buttonUpQueue->setEnabled(false);
  ui.buttonRemoveFromQueue->setEnabled(false);

  if (ui.queueListWidget->count() > 0) {
    ui.buttonPurgeQueue->setEnabled(true);
  } else {
    ui.buttonPurgeQueue->setEnabled(false);
  }
}

void MainWindow::listTasks() {

  ui.tasksListWidget->clear();

  // make tasks list flow (wrap) in its window
  ui.tasksListWidget->setViewMode(QListWidget::ListMode);
  ui.tasksListWidget->setResizeMode(QListView::Adjust);
  ui.tasksListWidget->setSpacing(5);
  ui.tasksListWidget->setWrapping(true);

  // enable drag and drop reordering (there is no persistence of order
  // implemented yet)
  // ui.tasksListWidget->setDragDropMode(QAbstractItemView::InternalMove);

  auto settings = GetSettings();
  bool sortTask = settings->value("Settings/sortTask").toBool();
  if (sortTask) {
    //    ui.actionSortTask->setIcon(sortZAIcon);
    //    ui.buttonSortTask->setToolTip("Sort Descending");
    ui.tasksListWidget->setSortingEnabled(true);
    ui.tasksListWidget->sortItems(Qt::AscendingOrder);
  } else {
    //    ui.actionSortTask->setIcon(sortAZIcon);
    //    ui.buttonSortTask->setToolTip("Sort Ascending");
    ui.tasksListWidget->setSortingEnabled(true);
    ui.tasksListWidget->sortItems(Qt::DescendingOrder);
  }

  ListOfJobOptions *ljo = ListOfJobOptions::getInstance();

  for (JobOptions *jo : ljo->getTasks()) {
    JobOptionsListWidgetItem *item = new JobOptionsListWidgetItem(
        jo,
        jo->jobType == JobOptions::JobType::Download ? mDownloadIcon
                                                     : mUploadIcon,
        jo->description);
    ui.tasksListWidget->addItem(item);
  }

  ui.buttonDeleteTask->setEnabled(false);
  ui.buttonEditTask->setEnabled(false);
  ui.buttonRunTask->setEnabled(false);
  ui.buttonDryrunTask->setEnabled(false);
  ui.buttonAddToQueue->setEnabled(false);

  if (ui.tasksListWidget->count() > 1) {
    ui.buttonSortTask->setEnabled(true);
  } else {
    ui.buttonSortTask->setEnabled(false);
  }

  // update ui.queueListWidget when task changed
  QString uniqueId_queue;
  QString uniqueId_task;
  bool itemFound = false;

  // for every item in tasks
  for (int i = 0; i < ui.queueListWidget->count(); i++) {

    ui.queueListWidget->item(i);

    JobOptionsListWidgetItem *item_queue =
        static_cast<JobOptionsListWidgetItem *>(ui.queueListWidget->item(i));
    JobOptions *jo_queue = item_queue->GetData();
    uniqueId_queue = jo_queue->uniqueId.toString();

    // if no corresponding item found in the queue means task has been deleted
    // and have to be removed from the queue as well
    itemFound = false;

    // check if corresponding item in the queue
    for (int j = 0; j < ui.tasksListWidget->count(); j++) {

      JobOptionsListWidgetItem *item_task =
          static_cast<JobOptionsListWidgetItem *>(ui.tasksListWidget->item(j));
      JobOptions *jo_task = item_task->GetData();
      uniqueId_task = jo_task->uniqueId.toString();

      // uniqueId never changes, name can be edited so we check Id
      if (uniqueId_queue == uniqueId_task) {
        itemFound = true;
        // update ui.queueListWidget

        ui.queueListWidget->takeItem(i);

        JobOptionsListWidgetItem *item_insert = new JobOptionsListWidgetItem(
            jo_task,
            jo_task->jobType == JobOptions::JobType::Download ? mDownloadIcon
                                                              : mUploadIcon,
            jo_task->description);

        ui.queueListWidget->insertItem(i, item_insert);

        if (i == 0 && mQueueStatus) {
          ui.queueListWidget->item(0)->setBackground(Qt::darkGreen);
        }
      }
    } // for j

    // remove item from ui.queueListWidget if removed from tasks
    if (mQueueStatus) {
      // running queue
      if (!itemFound) {
        // only if already running leave it
        // never should happen - as not possible to delete already running
        if (i != 0) {
          --mQueueCount;
          ui.queueListWidget->takeItem(i);
          if (mQueueCount == 0) {
            ui.tabs->setTabText(3, QString("Queue (%1)>>(0)").arg(mQueueCount));
          } else {
            ui.tabs->setTabText(
                3, QString("Queue (%1)>>(1)").arg(mQueueCount - 1));
          }
        }
      }

    } else {
      // stopped queue
      if (!itemFound) {
        --mQueueCount;
        ui.queueListWidget->takeItem(i);
        if (mQueueCount == 0) {
          ui.tabs->setTabText(3, QString("Queue"));
        } else {
          ui.tabs->setTabText(3, QString("Queue (%1)").arg(mQueueCount));
        }
      }

    } // mQueueStatus
  }   // for i

  saveQueueFile();
  ui.queueListWidget->setFocus();
  setQueueButtons();

} // MainWindow::listTasks()

void MainWindow::runItem(JobOptionsListWidgetItem *item,
                         const QString &transferMode, bool dryrun) {
  if (item == nullptr)
    return;
  JobOptions *jo = item->GetData();
  jo->dryRun = dryrun;
  QStringList args = jo->getOptions();

  QString info;
  QString operation;

  if (int(jo->operation) == 1)
    operation = "Copy";
  if (int(jo->operation) == 2)
    operation = "Move";
  if (int(jo->operation) == 3)
    operation = "Sync";

  if (dryrun) {
    info = QString("Dry run, task: \"%1\", %2 from %3")
               .arg(jo->description)
               .arg(operation.toLower())
               .arg(jo->source);
  } else {
    if (transferMode == "queue") {
      info = QString("Queue task: \"%1\", %2 from %3")
                 .arg(jo->description)
                 .arg(operation)
                 .arg(jo->source);
    } else {

      info = QString("Task: \"%1\", %2 from %3")
                 .arg(jo->description)
                 .arg(operation)
                 .arg(jo->source);
    }
  }

  addTransfer(info, jo->source, jo->dest, args, jo->uniqueId.toString(),
              transferMode);
}

void MainWindow::editSelectedTask() {

  auto items = ui.tasksListWidget->selectedItems();

  // remember which item we edit
  int selection = (ui.tasksListWidget->selectionModel()->currentIndex()).row();

  foreach (auto i, items) {

    JobOptionsListWidgetItem *item = static_cast<JobOptionsListWidgetItem *>(i);

    JobOptions *jo = item->GetData();
    bool isDownload = (jo->jobType == JobOptions::Download);
    QString remoteType = (jo->remoteType);
    QString remoteMode = (jo->remoteMode);

    QString remote = isDownload ? jo->source : jo->dest;
    QString path = isDownload ? jo->dest : jo->source;
    // qDebug() << "remote:" + remote;
    // qDebug() << "path:" + path;

    TransferDialog td(isDownload, false, remote, path, jo->isFolder, remoteType,
                      remoteMode, this, jo, true);
    td.exec();
  }

  // restore the selection to help user keep track of what s/he was doing
  ui.tasksListWidget->setCurrentItem(ui.tasksListWidget->item(selection));
  ui.tasksListWidget->setFocus();

  // edit mode on the TransferDialog suppresses the usual Accept buttons
  // and the Save Task button closes it... so there is nothing more to do here
}

void MainWindow::saveQueueFile(void) {

  QString filePath = GetConfigDir().absoluteFilePath("queue.conf");
  QFile file(filePath);
  QTextStream out(&file);
  file.open(QIODevice::WriteOnly);

  // loop over ui.queueListWidget
  for (int i = 0; i < ui.queueListWidget->count(); ++i) {
    QListWidgetItem *item = ui.queueListWidget->item(i);

    JobOptionsListWidgetItem *jobItem =
        static_cast<JobOptionsListWidgetItem *>(item);

    JobOptions *jo = jobItem->GetData();

    out << jo->uniqueId.toString() << endl;
  }

  file.close();
}

void MainWindow::addTransfer(const QString &message, const QString &source,
                             const QString &dest, const QStringList &args,
                             const QString &uniqueId,
                             const QString &transferMode) {

  QProcess *transfer = new QProcess(this);
  transfer->setProcessChannelMode(QProcess::MergedChannels);

  auto widget = new JobWidget(transfer, message, args, source, dest, uniqueId,
                              transferMode);

  auto line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);

  QObject::connect(
      widget, &JobWidget::finished, this, [=](const QString &info) {
        if (mNotifyFinishedTransfers) {
          qApp->alert(this);
          mLastFinished = widget;
#if defined(Q_OS_WIN)
          mSystemTray.showMessage(
              "Rclone Browser - Transfer finished", info,
              QIcon(":remotes/images/program_icons/rclone-browser512.png"));
#else
#if defined(Q_OS_MACOS)
          mSystemTray.showMessage(
              "Rclone Browser - Transfer finished", info);
#else
          mSystemTray.showMessage(
              "Rclone Browser - Transfer finished", info, QSystemTrayIcon::Information);
#endif
#endif
        }

        --mTransferJobCount;
        if (--mJobCount == 0) {
          ui.tabs->setTabText(1, "Jobs");
        } else {
          ui.tabs->setTabText(1, QString("Jobs (%1)").arg(mJobCount));
        }

        ui.buttonStopAllJobs->setEnabled(mTransferJobCount != 0);
        ui.buttonCleanNotRunning->setEnabled(mJobCount !=
                                             (ui.jobs->count() - 2) / 2);

        // if job finished try to run next one from the queue
        // only when not quitting  and when queue is active

        if (!mAppQuittingStatus && mQueueStatus) {

          auto QueueRunningTask = ui.queueListWidget->item(0);

          JobOptionsListWidgetItem *item =
              static_cast<JobOptionsListWidgetItem *>(QueueRunningTask);
          JobOptions *jo = item->GetData();

          // check if finished task the same as running from the queue
          auto transfer = qobject_cast<JobWidget *>(widget);
          if (transfer->getUniqueID() == jo->uniqueId.toString()) {

            --mQueueCount;
            // queue is still running, even if mQueueCount is 0
            if (mQueueCount == 0) {
              ui.tabs->setTabText(3,
                                  QString("Queue (%1)>>(0)").arg(mQueueCount));
            } else {
              ui.tabs->setTabText(
                  3, QString("Queue (%1)>>(1)").arg(mQueueCount - 1));
            }

            ui.queueListWidget->takeItem(0);
            auto nextTask = ui.queueListWidget->item(0);

            // if there is still task to run
            if ((ui.queueListWidget->count()) > 0) {

              // check if task is not already running (user could run it
              // manually)

              bool isAlreadyRunning = false;

              JobOptionsListWidgetItem *item =
                  static_cast<JobOptionsListWidgetItem *>(nextTask);
              JobOptions *jo = item->GetData();

              for (int j = 0; j < ui.jobs->count(); j++) {
                QWidget *widget = ui.jobs->itemAt(j)->widget();
                if (auto transfer = qobject_cast<JobWidget *>(widget)) {
                  if ((transfer->getUniqueID() == jo->uniqueId.toString()) &&
                      (transfer->isRunning)) {
                    isAlreadyRunning = true;
                    break;
                  }
                }
              }

              if (!isAlreadyRunning) {
                runItem(item, "queue");
              }
              ui.queueListWidget->item(0)->setBackground(Qt::darkGreen);
            }
            saveQueueFile();
          }
        }
      });

  QObject::connect(widget, &JobWidget::closed, this, [=]() {
    if (widget == mLastFinished) {
      mLastFinished = nullptr;
    }

    ui.jobs->removeWidget(widget);
    ui.jobs->removeWidget(line);
    widget->deleteLater();
    delete line;
    if (ui.jobs->count() == 2) {
      ui.noJobsAvailable->show();
    }
    ui.buttonStopAllJobs->setEnabled(mTransferJobCount != 0);
    ui.buttonCleanNotRunning->setEnabled(mJobCount !=
                                         (ui.jobs->count() - 2) / 2);
  });

  if (ui.jobs->count() == 2) {
    ui.noJobsAvailable->hide();
  }

  ui.jobs->insertWidget(0, widget);
  ui.jobs->insertWidget(1, line);
  ++mTransferJobCount;
  ui.tabs->setTabText(1, QString("Jobs (%1)").arg(++mJobCount));

  ui.buttonStopAllJobs->setEnabled(mTransferJobCount != 0);
  ui.buttonCleanNotRunning->setEnabled(mJobCount != (ui.jobs->count() - 2) / 2);

  UseRclonePassword(transfer);
  transfer->start(GetRclone(), GetRcloneConf() + args, QIODevice::ReadOnly);
}

void MainWindow::addMount(const QString &remote, const QString &folder,
                          const QString &remoteType) {
  QProcess *mount = new QProcess(this);
  mount->setProcessChannelMode(QProcess::MergedChannels);

  auto widget = new MountWidget(mount, remote, folder);

  auto line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);

  QObject::connect(widget, &MountWidget::finished, this, [=]() {
    if (--mJobCount == 0) {
      ui.tabs->setTabText(1, "Jobs");
    } else {
      ui.tabs->setTabText(1, QString("Jobs (%1)").arg(mJobCount));
    }

    ui.buttonStopAllJobs->setEnabled(mTransferJobCount != 0);
    ui.buttonCleanNotRunning->setEnabled(mJobCount !=
                                         (ui.jobs->count() - 2) / 2);
  });

  QObject::connect(widget, &MountWidget::closed, this, [=]() {
    ui.jobs->removeWidget(widget);
    ui.jobs->removeWidget(line);
    widget->deleteLater();
    delete line;
    if (ui.jobs->count() == 2) {
      ui.noJobsAvailable->show();
    }
    ui.buttonStopAllJobs->setEnabled(mTransferJobCount != 0);
    ui.buttonCleanNotRunning->setEnabled(mJobCount !=
                                         (ui.jobs->count() - 2) / 2);
  });

  if (ui.jobs->count() == 2) {
    ui.noJobsAvailable->hide();
  }

  ui.jobs->insertWidget(0, widget);
  ui.jobs->insertWidget(1, line);
  ui.tabs->setTabText(1, QString("Jobs (%1)").arg(++mJobCount));

  ui.buttonStopAllJobs->setEnabled(mTransferJobCount != 0);
  ui.buttonCleanNotRunning->setEnabled(mJobCount != (ui.jobs->count() - 2) / 2);

  auto settings = GetSettings();
  QString opt = settings->value("Settings/mount").toString();
  QString driveSharedMode = settings->value("Settings/remoteMode").toString();

  QStringList args;
  args << "mount";

#if defined(Q_OS_WIN32)
  args << "--rc";
  args << "--rc-addr";

  // calculate remote control interface port based on mount drive letter
  // this way every mount will have unique port assigned
  int port_offset = folder[0].toLatin1();
  unsigned short int rclone_rc_port_base = 19000;
  unsigned short int rclone_rc_port = rclone_rc_port_base + port_offset;
  args << "localhost:" + QVariant(rclone_rc_port).toString();
#endif

  // for google drive "shared with me" without --read-only writes go created in
  // main google drive it is more logical to mount it as read only so there is
  // no confusion

  if (remoteType == "drive") {

    if (driveSharedMode == "shared") {
      args << "--drive-shared-with-me";
      args << "--read-only";
    }

    if (driveSharedMode == "trash") {
      args << "--drive-trashed-only";
    }
  };

  //	 default mount is now more generic. all options can be passed via
  // preferences mount field
  //       args << "--vfs-cache-mode";
  //       args << "writes";

  args.append(GetRcloneConf());

  if (!opt.isEmpty()) {
    // split on spaces but not if inside quotes e.g. --option-1 --option-2="arg1
    // arg2" --option-3 arg3 should generate "--option-1" "--option-2=\"arg1
    // arg2\"" "--option-3" "arg3"
    for (QString arg : opt.split(QRegExp(" (?=[^\"]*(\"[^\"]*\"[^\"]*)*$)"))) {
      if (!arg.isEmpty()) {
        args << arg.replace("\"", "");
      }
    }
  }

  args << remote << folder;

  UseRclonePassword(mount);
  mount->start(GetRclone(), args, QIODevice::ReadOnly);
}

void MainWindow::addStream(const QString &remote, const QString &stream,
                           const QString &remoteType) {
  auto player = new QProcess();
  auto rclone = new QProcess();
  rclone->setStandardOutputProcess(player);

  QObject::connect(
      player,
      static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
          &QProcess::finished),
      this, [=](int status, QProcess::ExitStatus) {
        player->deleteLater();
        if (status != 0 && player->error() == QProcess::FailedToStart) {
          QMessageBox::critical(
              this, "Error",
              QString("Failed to start '%1' player process").arg(stream));
          auto settings = GetSettings();
          settings->remove("Settings/streamConfirmed");
        }
      });

  auto widget = new StreamWidget(rclone, player, remote, stream);

  auto line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);

  QObject::connect(widget, &StreamWidget::finished, this, [=]() {
    if (--mJobCount == 0) {
      ui.tabs->setTabText(1, "Jobs");
    } else {
      ui.tabs->setTabText(1, QString("Jobs (%1)").arg(mJobCount));
    }

    ui.buttonStopAllJobs->setEnabled(mTransferJobCount != 0);
    ui.buttonCleanNotRunning->setEnabled(mJobCount !=
                                         (ui.jobs->count() - 2) / 2);
  });

  QObject::connect(widget, &StreamWidget::closed, this, [=]() {
    ui.jobs->removeWidget(widget);
    ui.jobs->removeWidget(line);
    widget->deleteLater();
    delete line;
    if (ui.jobs->count() == 2) {
      ui.noJobsAvailable->show();
    }
    ui.buttonStopAllJobs->setEnabled(mTransferJobCount != 0);
    ui.buttonCleanNotRunning->setEnabled(mJobCount !=
                                         (ui.jobs->count() - 2) / 2);
  });

  if (ui.jobs->count() == 2) {
    ui.noJobsAvailable->hide();
  }

  ui.jobs->insertWidget(0, widget);
  ui.jobs->insertWidget(1, line);
  ui.tabs->setTabText(1, QString("Jobs (%1)").arg(++mJobCount));

  ui.buttonStopAllJobs->setEnabled(mTransferJobCount != 0);
  ui.buttonCleanNotRunning->setEnabled(mJobCount != (ui.jobs->count() - 2) / 2);

  auto settings = GetSettings();
  QString opt = settings->value("Settings/mount").toString();
  QString driveSharedMode = settings->value("Settings/remoteMode").toString();

  QStringList args;
  args << "cat";

  if (remoteType == "drive") {

    if (driveSharedMode == "shared") {
      args << "--drive-shared-with-me";
    }

    if (driveSharedMode == "trash") {
      args << "--drive-trashed-only";
    }
  };

  player->start(stream, QProcess::ReadOnly);
  UseRclonePassword(rclone);
  rclone->start(GetRclone(),
                QStringList() << args << GetRcloneConf()
                              << GetDefaultRcloneOptionsList() << remote,
                QProcess::WriteOnly);
}

void MainWindow::slotCloseTab(int index) {

  // only when last remote tab is closed return to remote list tab
  // only when closing current tab
  if (ui.tabs->currentIndex() == index) {

    if (ui.tabs->count() == 5) {
      if (index == 4) {
        ui.tabs->setCurrentIndex(0);
      }
    } else {

      if (ui.tabs->count() == 6) {
        ui.tabs->setCurrentIndex(4);
      }
    }
  }
}

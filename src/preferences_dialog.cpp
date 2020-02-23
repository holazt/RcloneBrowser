#include "preferences_dialog.h"
#include "utils.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent) {
  ui.setupUi(this);

  QObject::connect(ui.rcloneBrowse, &QPushButton::clicked, this, [=]() {
    QString rclone = QFileDialog::getOpenFileName(
        this, "Select rclone executable", ui.rclone->text());
    if (rclone.isEmpty()) {
      return;
    }

    if (!QFileInfo(rclone).isExecutable()) {
      QMessageBox::critical(this, "Error",
                            QString("File %1 is not executable").arg(rclone));
      return;
    }

    if (QFileInfo(rclone) == QFileInfo(qApp->applicationFilePath())) {
      QMessageBox::critical(this, "Error",
                            "You selected RcloneBrowser executable!\nPlease "
                            "select rclone executable instead.");
      return;
    }

    ui.rclone->setText(rclone);
  });

  QObject::connect(ui.rcloneConfBrowse, &QPushButton::clicked, this, [=]() {
    QString rcloneConf = QFileDialog::getOpenFileName(
        this, "Select .rclone.conf location", ui.rcloneConf->text());
    if (rcloneConf.isEmpty()) {
      return;
    }

    ui.rcloneConf->setText(rcloneConf);
  });

  QObject::connect(
      ui.defaultDownloadDirBrowse, &QPushButton::clicked, this, [=]() {
        QString defaultDownloadDir = QFileDialog::getExistingDirectory(
            this, "Select default download directory",
            ui.defaultDownloadDir->text());

        if (defaultDownloadDir.isEmpty()) {
          return;
        }

        ui.defaultDownloadDir->setText(defaultDownloadDir);
      });

  QObject::connect(
      ui.defaultUploadDirBrowse, &QPushButton::clicked, this, [=]() {
        QString defaultUploadDir = QFileDialog::getExistingDirectory(
            this, "Select default upload directory",
            ui.defaultUploadDir->text());

        if (defaultUploadDir.isEmpty()) {
          return;
        }

        ui.defaultUploadDir->setText(defaultUploadDir);
      });

  auto settings = GetSettings();
  ui.rclone->setText(
      QDir::toNativeSeparators(settings->value("Settings/rclone").toString()));
  ui.rcloneConf->setText(QDir::toNativeSeparators(
      settings->value("Settings/rcloneConf").toString()));
  ui.stream->setText(settings->value("Settings/stream").toString());

#if defined(Q_OS_OPENBSD) || defined(Q_OS_NETBSD)
  ui.mount->setText(
      settings
          ->value("Settings/mount",
                  "* mount is not supported by rclone on this system *")
          .toString());
  ui.mount->setDisabled(true);
#else
  ui.mount->setText(
      settings->value("Settings/mount", "--vfs-cache-mode writes").toString());
#endif

  ui.defaultDownloadDir->setText(QDir::toNativeSeparators(
      settings->value("Settings/defaultDownloadDir").toString()));
  ui.defaultUploadDir->setText(QDir::toNativeSeparators(
      settings->value("Settings/defaultUploadDir").toString()));
  ui.defaultDownloadOptions->setText(
      settings->value("Settings/defaultDownloadOptions").toString());
  ui.defaultUploadOptions->setText(
      settings->value("Settings/defaultUploadOptions").toString());
  ui.defaultRcloneOptions->setText(
      settings->value("Settings/defaultRcloneOptions").toString());

  ui.checkRcloneBrowserUpdates->setChecked(
      settings->value("Settings/checkRcloneBrowserUpdates", true).toBool());
  ui.checkRcloneUpdates->setChecked(
      settings->value("Settings/checkRcloneUpdates", true).toBool());

  if (QSystemTrayIcon::isSystemTrayAvailable()) {
    ui.alwaysShowInTray->setChecked(
        settings->value("Settings/alwaysShowInTray", false).toBool());
    ui.closeToTray->setChecked(
        settings->value("Settings/closeToTray", false).toBool());
    ui.notifyFinishedTransfers->setChecked(
        settings->value("Settings/notifyFinishedTransfers", true).toBool());
  } else {
    ui.alwaysShowInTray->setChecked(false);
    ui.alwaysShowInTray->setDisabled(true);
    ui.closeToTray->setChecked(false);
    ui.closeToTray->setDisabled(true);
    ui.notifyFinishedTransfers->setChecked(false);
    ui.notifyFinishedTransfers->setDisabled(true);
  }

  ui.showFolderIcons->setChecked(
      settings->value("Settings/showFolderIcons", true).toBool());
  ui.showFileIcons->setChecked(
      settings->value("Settings/showFileIcons", true).toBool());
  ui.rowColors->setChecked(
      settings->value("Settings/rowColors", true).toBool());
  ui.showHidden->setChecked(
      settings->value("Settings/showHidden", true).toBool());
  ui.darkMode->setChecked(settings->value("Settings/darkMode", true).toBool());

// dark mode option for all systems but latest macOS
// on macOS Mojave or newer dark mode is managed by OS
#if defined(Q_OS_MACOS)
  QString sysInfo = QSysInfo::productVersion();
  if (sysInfo == "10.9" || sysInfo == "10.10" || sysInfo == "10.11" ||
      sysInfo == "10.12" || sysInfo == "10.13") {
  } else {
    ui.darkMode->hide();
    ui.darkMode_info->hide();
  }
#endif

  if ((settings->value("Settings/buttonStyle").toString()) == "icononly") {
    ui.cb_icononly->setChecked(true);
  } else {
    if (settings->value("Settings/buttonStyle").toString() == "textonly") {
      ui.cb_textonly->setChecked(true);
    } else {
      ui.cb_textandicon->setChecked(true);
    }
  }

  if ((settings->value("Settings/iconsLayout").toString()) == "list") {
    ui.cb_list->setChecked(true);
  } else {
    ui.cb_tiles->setChecked(true);
  }

  if ((settings->value("Settings/fontSize").toString()) == "S") {
    ui.cb_font_s->setChecked(true);
  } else {
      if (settings->value("Settings/fontSize").toString() == "L") {
        ui.cb_font_l->setChecked(true);
    } else {
        if (settings->value("Settings/fontSize").toString() == "XL") {
          ui.cb_font_xl->setChecked(true);
      } else {
          if (settings->value("Settings/fontSize").toString() == "XXL") {
            ui.cb_font_xxl->setChecked(true);
        } else {
          ui.cb_font_m->setChecked(true);
        }
      }
    }
  }

  if ((settings->value("Settings/buttonSize").toString()) == "S") {
    ui.cb_button_s->setChecked(true);
  } else {
      if (settings->value("Settings/buttonSize").toString() == "L") {
        ui.cb_button_l->setChecked(true);
    } else {
        if (settings->value("Settings/buttonSize").toString() == "XL") {
          ui.cb_button_xl->setChecked(true);
      } else {
          if (settings->value("Settings/buttonSize").toString() == "XXL") {
            ui.cb_button_xxl->setChecked(true);
        } else {
          ui.cb_button_m->setChecked(true);
        }
      }
    }
  }

  if ((settings->value("Settings/iconSize").toString()) == "S") {
    ui.cb_remote_s->setChecked(true);
  } else {
      if (settings->value("Settings/iconSize").toString() == "L") {
        ui.cb_remote_l->setChecked(true);
    } else {
        if (settings->value("Settings/iconSize").toString() == "XL") {
          ui.cb_remote_xl->setChecked(true);
      } else {
          if (settings->value("Settings/iconSize").toString() == "XXL") {
            ui.cb_remote_xxl->setChecked(true);
        } else {
          ui.cb_remote_m->setChecked(true);
        }
      }
    }
  }

  ui.info_2->setText(
      "See rclone <a "
      "href=\"https://github.com/rclone/rclone/blob/master/docs/content/"
      "faq.md#can-i-use-rclone-with-an-http-proxy\">FAQ</a> for details.");
  ui.info_2->setTextFormat(Qt::RichText);
  ui.info_2->setTextInteractionFlags(Qt::TextBrowserInteraction);
  ui.info_2->setOpenExternalLinks(true);

  if (settings->value("Settings/useProxy").toBool()) {
    ui.useProxy->setChecked(true);
  } else {
    ui.useSystemSettings->setChecked(true);
  }
  ui.http_proxy->setText(settings->value("Settings/http_proxy").toString());
  ui.https_proxy->setText(settings->value("Settings/https_proxy").toString());
  ui.no_proxy->setText(settings->value("Settings/no_proxy").toString());
}

PreferencesDialog::~PreferencesDialog() {}

QString PreferencesDialog::getRclone() const {
  return QDir::fromNativeSeparators(ui.rclone->text());
}

QString PreferencesDialog::getRcloneConf() const {
  return QDir::fromNativeSeparators(ui.rcloneConf->text());
}

QString PreferencesDialog::getStream() const { return ui.stream->text(); }

QString PreferencesDialog::getMount() const { return ui.mount->text(); }

QString PreferencesDialog::getDefaultDownloadDir() const {
  return QDir::fromNativeSeparators(ui.defaultDownloadDir->text());
}

QString PreferencesDialog::getDefaultUploadDir() const {
  return QDir::fromNativeSeparators(ui.defaultUploadDir->text());
}

QString PreferencesDialog::getDefaultDownloadOptions() const {
  return ui.defaultDownloadOptions->text();
}

QString PreferencesDialog::getDefaultUploadOptions() const {
  return ui.defaultUploadOptions->text();
}

QString PreferencesDialog::getDefaultRcloneOptions() const {
  return ui.defaultRcloneOptions->text();
}

bool PreferencesDialog::getCheckRcloneBrowserUpdates() const {
  return ui.checkRcloneBrowserUpdates->isChecked();
}

bool PreferencesDialog::getCheckRcloneUpdates() const {
  return ui.checkRcloneUpdates->isChecked();
}

bool PreferencesDialog::getAlwaysShowInTray() const {
  return ui.alwaysShowInTray->isChecked();
}

bool PreferencesDialog::getCloseToTray() const {
  return ui.closeToTray->isChecked();
}

bool PreferencesDialog::getNotifyFinishedTransfers() const {
  return ui.notifyFinishedTransfers->isChecked();
}

bool PreferencesDialog::getShowFolderIcons() const {
  return ui.showFolderIcons->isChecked();
}

bool PreferencesDialog::getShowFileIcons() const {
  return ui.showFileIcons->isChecked();
}

bool PreferencesDialog::getRowColors() const {
  return ui.rowColors->isChecked();
}

bool PreferencesDialog::getShowHidden() const {
  return ui.showHidden->isChecked();
}

bool PreferencesDialog::getDarkMode() const { return ui.darkMode->isChecked(); }

QString PreferencesDialog::getButtonStyle() const {
  if (ui.cb_icononly->isChecked()) {
    return "icononly";
  } else {
    if (ui.cb_textandicon->isChecked()) {
      return "textandicon";
    } else {
      return "textonly";
    }
  }
}

QString PreferencesDialog::getIconsLayout() const {
  if (ui.cb_list->isChecked()) {
    return "list";
  } else {
      return "tiles";
  }
}

QString PreferencesDialog::getFontSize() const {
  if (ui.cb_font_s->isChecked()) {
    return "S";
  } else {
    if (ui.cb_font_l->isChecked()) {
      return "L";
    } else {
      if (ui.cb_font_xl->isChecked()) {
        return "XL";
      } else {
        if (ui.cb_font_xxl->isChecked()) {
          return "XXL";
        } else {
          return "M";
        }
      }
    }
  }
}

QString PreferencesDialog::getButtonSize() const {
  if (ui.cb_button_s->isChecked()) {
    return "S";
  } else {
    if (ui.cb_button_l->isChecked()) {
      return "L";
    } else {
      if (ui.cb_button_xl->isChecked()) {
        return "XL";
      } else {
        if (ui.cb_button_xxl->isChecked()) {
          return "XXL";
        } else {
          return "M";
        }
      }
    }
  }
}

QString PreferencesDialog::getIconSize() const {
  if (ui.cb_remote_s->isChecked()) {
    return "S";
  } else {
    if (ui.cb_remote_l->isChecked()) {
      return "L";
    } else {
      if (ui.cb_remote_xl->isChecked()) {
        return "XL";
      } else {
        if (ui.cb_remote_xxl->isChecked()) {
          return "XXL";
        } else {
          return "M";
        }
      }
    }
  }
}

QString PreferencesDialog::getHttpProxy() const {
  return ui.http_proxy->text();
}

QString PreferencesDialog::getHttpsProxy() const {
  return ui.https_proxy->text();
}

QString PreferencesDialog::getNoProxy() const { return ui.no_proxy->text(); }

bool PreferencesDialog::getUseProxy() const {
  if (ui.useSystemSettings->isChecked()) {
    return false;
  } else {
    return true;
  }
}

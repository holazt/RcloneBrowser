#include "dedupe_dialog.h"
#include "utils.h"

DedupeDialog::DedupeDialog(const QString &remote, const QDir &path,
                         const QString &remoteType, QWidget *parent)
    : QDialog(parent) {

  ui.setupUi(this);
  //  resize(0, 0);

  adjustSize();
  setMaximumHeight(this->height());
  setMinimumHeight(this->height());

  QPushButton *dryRun =
      ui.buttonBox->addButton("&Dry run", QDialogButtonBox::AcceptRole);
  ui.buttonBox->addButton("&Run", QDialogButtonBox::AcceptRole);

  QObject::connect(dryRun, &QPushButton::clicked, this,
                   [=]() { mDryRun = true; });

  if (remoteType == "drive") {
  }

  setWindowTitle("rclone dedupe");

  mTarget = remote + ":" + path.path();

  ui.remoteToCheck->setText(mTarget);
  ui.remoteToCheck->setCursorPosition(0);

  QObject::connect(ui.buttonBox, &QDialogButtonBox::accepted, this,
                   &QDialog::accept);

  QObject::connect(ui.buttonBox, &QDialogButtonBox::rejected, this,
                   &QDialog::reject);
}

DedupeDialog::~DedupeDialog() {}

bool DedupeDialog::isDryRun() const { return mDryRun; }

QStringList DedupeDialog::getOptions() const {

  QStringList list;

  list << "dedupe";

  list << "--dedupe-mode";

  if (ui.cb_skip->isChecked()) {
    list << "skip";
  }

  if (ui.cb_first->isChecked()) {
    list << "first";
  }

  if (ui.cb_newest->isChecked()) {
    list << "newest";
  }

  if (ui.cb_oldest->isChecked()) {
    list << "oldest";
  }

  if (ui.cb_largest->isChecked()) {
    list << "largest";
  }

  if (ui.cb_smallest->isChecked()) {
    list << "smallest";
  }

  if (ui.cb_rename->isChecked()) {
    list << "rename";
  }

  QString extra = ui.textExtra->text().trimmed();
  if (!extra.isEmpty()) {
    for (QString arg :
         extra.split(QRegExp(" (?=[^\"]*(\"[^\"]*\"[^\"]*)*$)"))) {
      if (!arg.isEmpty()) {
        list << arg.replace("\"", "");
      }
    }
  }

  list << mTarget;

  if (mDryRun) {
    list << "--dry-run";
  }

  return list;
}

void DedupeDialog::done(int r) {
  if (r == QDialog::Accepted) {
  }
  QDialog::done(r);
}

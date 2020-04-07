#include "hours_spinbox.h"

HoursSpinBox::HoursSpinBox(QWidget *parent) : QSpinBox(parent) {}

HoursSpinBox::~HoursSpinBox() {}

int HoursSpinBox::valueFromText(const QString &text) const {
  bool ok;
  int num = text.toInt(&ok);
  if (ok) {
    return num;
  } else {
    return 0;
  }
}

QString HoursSpinBox::textFromValue(int value) const {

  if (value == 0) {
    return "00";
  }
  if (value == 1) {
    return "01";
  }
  if (value == 2) {
    return "02";
  }
  if (value == 3) {
    return "03";
  }
  if (value == 4) {
    return "04";
  }
  if (value == 5) {
    return "05";
  }
  if (value == 6) {
    return "06";
  }
  if (value == 7) {
    return "07";
  }
  if (value == 8) {
    return "08";
  }
  if (value == 9) {
    return "09";
  }

  return QString::number(value);
}

#pragma once
#include "pch.h"

class MinutesSpinBox : public QSpinBox {
  Q_OBJECT

public:
  MinutesSpinBox(QWidget *parent = Q_NULLPTR);

  ~MinutesSpinBox();

public slots:
  int valueFromText(const QString &text) const;
  QString textFromValue(int value) const;
};

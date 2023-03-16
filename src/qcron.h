/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Mankalas
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * Source: https://github.com/mankalas/qcron
 */

#ifndef _QCRON_H
#define _QCRON_H

#include "qcronfield.h"
#include <QDateTime>
#include <QObject>

class QCron : public QObject {
  Q_OBJECT

public:
  QCron();
  QCron(QString &pattern);
  ~QCron();

  // Accessors.
  void setBeginning(const QDateTime &date_time) { _beginning = date_time; }

  bool isValid() const { return _is_valid; }

  const QString &error() const { return _error; }

  // Features.

  QDateTime next(int n = 1);
  QDateTime next(QDateTime dt);
  void catchUp(QDateTime &dt, EField field, int value);
  bool match(const QDateTime &dt) const;
  void add(QDateTime &dt, EField field, int value);

signals:
  void activated();
  void deactivated();

private:
  bool _is_valid;
  QString _error;
  QCronField _fields[6];
  QDateTime _beginning;

  void _init();
  void _setError(const QString &error);
  void _parsePattern(QString &pattern);
  void _parseField(QString &field_str, EField field);
  QString _validCharacters(EField field);
  void chiche(QDateTime &dt, EField field);
};

#endif

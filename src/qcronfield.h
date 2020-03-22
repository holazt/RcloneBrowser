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

#ifndef _QCRONFIELD_H
#define _QCRONFIELD_H

#include <QDateTime>
#include <QList>
#include <QString>

enum EField { MINUTE, HOUR, DOM, MONTH, DOW, YEAR };

class QCronIntNode;
class QCronRangeNode;
class QCronNode;
class QCronEveryNode;
class QCronListNode;

class QCronFieldException {
public:
  QCronFieldException(const QString &msg) : _msg(msg) {}

  const QString &msg() const { return _msg; }

private:
  QString _msg;
};

class QCronField {
public:
  QCronField();

  // Accessors.
  void setField(EField field) {
    _field = field;
    switch (_field) {
    case MINUTE:
      _min = 0;
      _max = 59;
      break;
    case HOUR:
      _min = 0;
      _max = 23;
      break;
    case DOM:
      _min = 1;
      _max = 31;
      break;
    case MONTH:
      _min = 1;
      _max = 12;
      break;
    case DOW:
      _min = 1;
      _max = 7;
      break;
    case YEAR:
      _min = 1;
      _max = 2099;
      break;
    default:
      throw 42;
    }
  }
  EField getField() const { return _field; }

  int getMax() const { return _max; }

  int getMin() const { return _min; }

  // Features.
  void parse(QString &str);

  bool isValid() const { return _is_valid; }
  bool match(const QDateTime &dt) const;

  QCronNode *getRoot() const { return _root; }

  int next(QDateTime &dt);

  int getDateTimeSection(const QDateTime &dt) const;
  void applyOffset(QDateTime &dt, int &offset) const;

  void reset(QDateTime &dt);

private:
  int _min;
  int _max;
  EField _field;
  bool _is_valid;
  QCronNode *_last_node;
  QCronNode *_root;

  QCronIntNode *_parseInt(QString &str);
  QCronRangeNode *_parseRange(QString &str);
  QCronEveryNode *_parseEvery(QString &str);
  QCronListNode *_parseList(QString &str);
  QCronNode *_parseNode(QString &str);
  int getTimeSection(QDateTime &dt) const;
};

#endif

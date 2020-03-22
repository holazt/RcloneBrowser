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

#include "qcronfield.h"
#include "qcronnode.h"

#include <QDebug>

/******************************************************************************/

QCronField::
QCronField()
    : _is_valid(false)
{}

/******************************************************************************/

QCronIntNode *
QCronField::
_parseInt(QString & str)
{
    int value = 0;
    int char_idx = 0;
    QChar c = str[char_idx];
    while (c.isDigit())
    {
        value = value * 10 + c.digitValue();
        c = str[++char_idx];
    }
    if (char_idx == 0)
    {
        throw QCronFieldException(QString("%1 contains an invalid integer")
                                  .arg(str));
    }
    str.remove(0, char_idx);
    if (value < _min || _max < value)
    {
        throw QCronFieldException(QString("Value %1 out of range [%2;%3]")
                                  .arg(value).arg(_min).arg(_max));
    }
    return new QCronIntNode(value);
}

/******************************************************************************/

QCronRangeNode *
QCronField::
_parseRange(QString & str)
{
    if (_last_node == NULL)
    {
        throw QCronFieldException(QString("Syntax error at %1: range has no beginning")
                                  .arg(str));
    }
    QCronIntNode * begin = dynamic_cast<QCronIntNode *>(_last_node);
    if (begin == NULL)
    {
        throw QCronFieldException(QString("Syntax error: expected an interger at %1")
                                  .arg(str));
    }
    str.remove(0, 1);
    QCronIntNode * end = dynamic_cast<QCronIntNode *>(_parseInt(str));
    if (end == NULL)
    {
        throw QCronFieldException(QString("Syntax error: expected an interger at %1")
                                  .arg(str));
    }
    if (begin->value() > end->value())
    {
        throw QCronFieldException(QString("Invalid range: end is before start at %1")
                                  .arg(str));
    }
    return new QCronRangeNode(begin, end);
}

/******************************************************************************/

QCronEveryNode *
QCronField::
_parseEvery(QString & str)
{
    str.remove(0, 1);
    return new QCronEveryNode(_last_node, _parseInt(str));
}

/******************************************************************************/

QCronListNode *
QCronField::
_parseList(QString & str)
{
    QCronListNode * list = new QCronListNode();
    list->nodes() << _last_node;
    _last_node = list;
    while (!str.isEmpty())
    {
        if (str[0] == ',')
        {
            str.remove(0, 1);
        }
        QCronNode * node = _parseNode(str);
        list->nodes() << node;
        _last_node = node;
    }
    return list;
}

/******************************************************************************/

QCronNode *
QCronField::
_parseNode(QString & str)
{
    QCronNode * node = NULL;
    QChar c = str[0];
    if (c.isDigit())
    {
        node = _parseInt(str);
    }
    else if ("-" == QString(c))
    {
        node = _parseRange(str);
    }
    else if ("/" == QString(c))
    {
        node = _parseEvery(str);
    }
    else if ("*" == QString(c))
    {
        str.remove(0, 1);
        node = new QCronAllNode;
    }
    else if ("," == QString(c))
    {
        node = _parseList(str);
    }
    if (NULL == node)
    {
        throw QCronFieldException(QString("Unexpected character %1").arg(c));
    }
    node->setField(this);
    return node;
}

/******************************************************************************/

void
QCronField::
parse(QString & str)
{
    _last_node = NULL;
    _root = _parseNode(str);
    while (!str.isEmpty())
    {
        _last_node = _root;
        _root = _parseNode(str);
    }
    _is_valid = true;
}

/******************************************************************************/

int
QCronField::
getDateTimeSection(const QDateTime & dt) const
{
    switch (_field)
    {
        case MINUTE:
            return dt.time().minute();
        case HOUR:
            return dt.time().hour();
        case DOM:
            return dt.date().day();
        case MONTH:
            return dt.date().month();
        case DOW:
            return dt.date().dayOfWeek();
        case YEAR:
            return dt.date().year();
        default:
            qFatal("Shouldn't be here");
    }
}

/******************************************************************************/

void
QCronField::
applyOffset(QDateTime & dt, int & offset) const
{
    switch (_field)
    {
        case MINUTE:
        {
            dt = dt.addSecs(60 * offset);
            break;
        }
        case HOUR:
        {
            dt = dt.addSecs(3600 * offset);
            break;
        }
        case DOM:
        case DOW:
        {
            dt = dt.addDays(offset);
            break;
        }
        case MONTH:
        {
            dt = dt.addMonths(offset);
            break;
        }
        case YEAR:
        {
            dt = dt.addYears(offset);
            break;
        }
        default:
        {
            qFatal("Shouldn't be here");
        }
    }
}

/******************************************************************************/

int
QCronField::
next(QDateTime & dt)
{
    int time_section = getDateTimeSection(dt);
    return _root->next(time_section);
}

/******************************************************************************/

void
QCronField::
reset(QDateTime & dt)
{
    int value = _min;
    QDate date = dt.date();
    QTime time = dt.time();

    switch (_field)
    {
        case YEAR:
            dt.setDate(QDate(value, date.month(), date.day()));
            break;
        case MONTH:
            dt.setDate(QDate(date.year(), value, date.day()));
            break;
        case DOW:
        case DOM:
            dt.setDate(QDate(date.year(), date.month(), value));
            break;
        case HOUR:
            dt.setTime(QTime(value, time.minute(), 0));
            break;
        case MINUTE:
            dt.setTime(QTime(time.hour(), value, 0));
            break;
        default:
            qFatal("Unknown value in add");
    }
}

/******************************************************************************/

bool
QCronField::
match(const QDateTime & dt) const
{
    return _root->match(getDateTimeSection(dt));
}

/******************************************************************************/

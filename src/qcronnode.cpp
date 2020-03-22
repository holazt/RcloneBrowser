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

#include "qcronnode.h"
#include "qcronfield.h"
#include "qcron.h"

#include <QDebug>

#include <QDebug>

/******************************************************************************/

QCronNode::
~QCronNode()
{
}

/******************************************************************************/

void
QCronNode::
setField(QCronField * field)
{
    _field = field;
}

/******************************************************************************/
/******************************************************************************/

QCronIntNode::
QCronIntNode(int v)
    : _value(v)
{
}

/******************************************************************************/

int
QCronIntNode::
value() const
{
    return _value;
}

/******************************************************************************/

int
QCronIntNode::
next(int t) const
{
    return _value - t;
}

/******************************************************************************/

void
QCronIntNode::
process(QCron * cron,
        QDateTime & dt,
        EField field)
{
    cron->catchUp(dt, field, _value);
}

/******************************************************************************/

bool
QCronIntNode::
match(int tu) const
{
    return tu == _value;
}

/******************************************************************************/
/******************************************************************************/

int
QCronStrNode::
next(int t) const
{
    return t - 1;
}

/******************************************************************************/

void
QCronStrNode::
process(QCron *,
        QDateTime &,
        EField )
{
    // TODO
}

/******************************************************************************/

bool
QCronStrNode::
match(int tu) const
{
    return tu;
}

/******************************************************************************/
/******************************************************************************/

int
QCronAllNode::
next(int t) const
{
    Q_UNUSED(t);
    return 0;
}

/******************************************************************************/

void
QCronAllNode::
process(QCron * cron,
        QDateTime & dt,
        EField field)
{
    Q_UNUSED(cron);
    Q_UNUSED(dt);
    Q_UNUSED(field);
}

/******************************************************************************/

bool
QCronAllNode::
match(int tu) const
{
    Q_UNUSED(tu);
    return true;
}

/******************************************************************************/
/******************************************************************************/

QCronRangeNode::
QCronRangeNode(const QCronIntNode * begin,
               const QCronIntNode * end)
    : _begin(begin)
    , _end(end)
{
}

/******************************************************************************/

void
QCronRangeNode::
process(QCron * cron,
        QDateTime & dt,
        EField field)
{
    int begin = beginValue();
    int end = endValue();
    int current_time_unit = _field->getDateTimeSection(dt);
    if (current_time_unit < begin || end < current_time_unit)
    {
        cron->catchUp(dt, field, begin);
    }
    else
    {
        if (field == MINUTE)
        {
            cron->add(dt, field, 1);
        }
    }
}

/******************************************************************************/

int
QCronRangeNode::
beginValue() const
{
    return _begin->value();
}

/******************************************************************************/

int
QCronRangeNode::
endValue() const
{
    return _end->value();
}

/******************************************************************************/

int
QCronRangeNode::
next(int t) const
{
    if (_begin->value() <= t && t <= _end->value())
    {
        return 1;
    }
    return _begin->value() - t;
}

/******************************************************************************/

bool
QCronRangeNode::
match(int tu) const
{
    return _begin->value() <= tu && tu <= _end->value();
}

/******************************************************************************/
/******************************************************************************/

QCronEveryNode::
QCronEveryNode(QCronNode * what,
               QCronIntNode * freq)
    : _what(what)
    , _freq(freq)
{
}

/******************************************************************************/

void
QCronEveryNode::
process(QCron * cron ,
        QDateTime & dt,
        EField field)
{
    int freq = _freq->value();
    _what->process(cron, dt, field);
    while (_field->getDateTimeSection(dt) % freq)
    {
        cron->add(dt, field, 1);
        _what->process(cron, dt, field);
    }
}


/******************************************************************************/

int
QCronEveryNode::
next(int t) const
{
    int what_next = _what->next(t);
    if (what_next == 1)
    {
        /* what_next == 1 if we're in the right range AND if we're one
         * time unit ahead of it */
        int next = t % _freq->value();
        return _freq->value() - next;
    }
    return what_next;
}

/******************************************************************************/

bool
QCronEveryNode::
match(int tu) const
{
    return _what->match(tu) && tu % _freq->value() == 0;
}

/******************************************************************************/
/******************************************************************************/

QList<QCronNode *> &
QCronListNode::
nodes()
{
    return _nodes;
}

/******************************************************************************/

void
QCronListNode::
process(QCron * cron,
        QDateTime & dt,
        EField field)
{
    foreach (QCronNode * node, _nodes)
    {
        int node_next = node->next(_field->getDateTimeSection(dt));
        if (node_next >= 0)
        {
            node->process(cron, dt, field);
            return;
        }
    }
    _nodes[0]->process(cron, dt, field);
}

/******************************************************************************/

int
QCronListNode::
next(int t) const
{
    foreach (const QCronNode * node, _nodes)
    {
        int node_next = node->next(t);
        if (node_next > 0 && node_next != _field->getMax() + 1)
        {
            return node_next;
        }
    }
    return _nodes[0]->next(t);
}

/******************************************************************************/

bool
QCronListNode::
match(int tu) const
{
    foreach (const QCronNode * node, _nodes)
    {
        if (node->match(tu))
        {
            return true;
        }
    }
    return false;
}

/******************************************************************************/
/******************************************************************************/

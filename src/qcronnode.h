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

#ifndef _QCRONNODE_H
#define _QCRONNODE_H

#include <QList>
#include <QtGlobal>

#include "qcronfield.h"

class QCron;
class QCronField;

class QCronNode
{
public:
    virtual ~QCronNode();

    void setField(QCronField * field);

    virtual int next(int t) const = 0;

    virtual bool match(int tu) const = 0;
    virtual void process(QCron * cron,
                         QDateTime & dt,
                         EField field) = 0;

protected:
    QCronField * _field;
};

class QCronValueNode : public QCronNode
{
};

class QCronIntNode : public QCronValueNode
{
public:
    QCronIntNode(int v);

    int value() const;

    virtual int next(int t) const Q_DECL_OVERRIDE;
    virtual bool match(int tu) const Q_DECL_OVERRIDE;

    virtual void process(QCron * cron,
                         QDateTime & dt,
                         EField field) Q_DECL_OVERRIDE;

private:
    int _value;
};

class QCronStrNode : public QCronValueNode
{
public:
    virtual int next(int t) const Q_DECL_OVERRIDE;
    virtual bool match(int tu) const Q_DECL_OVERRIDE;
    virtual void process(QCron * cron,
                         QDateTime & dt,
                         EField field) Q_DECL_OVERRIDE;
};

class QCronAllNode : public QCronValueNode
{
public:
    virtual int next(int t) const Q_DECL_OVERRIDE;
    virtual bool match(int tu) const Q_DECL_OVERRIDE;
    virtual void process(QCron * cron,
                         QDateTime & dt,
                         EField field) Q_DECL_OVERRIDE;
};

class QCronRangeNode : public QCronNode
{
public:
    QCronRangeNode(const QCronIntNode * begin,
                   const QCronIntNode * end);

    int beginValue() const;
    int endValue() const;

    virtual int next(int t) const Q_DECL_OVERRIDE;
    virtual bool match(int tu) const Q_DECL_OVERRIDE;

    virtual void process(QCron * cron,
                         QDateTime & dt,
                         EField field) Q_DECL_OVERRIDE;

private:
    const QCronIntNode * _begin;
    const QCronIntNode * _end;
};

class QCronEveryNode : public QCronNode
{
public:
    QCronEveryNode(QCronNode *, QCronIntNode *);

    virtual int next(int t) const Q_DECL_OVERRIDE;
    virtual bool match(int tu) const Q_DECL_OVERRIDE;

    virtual void process(QCron * cron,
                         QDateTime & dt,
                         EField field) Q_DECL_OVERRIDE;

private:
    QCronNode * _what;
    QCronIntNode * _freq;
};

class QCronListNode : public QCronNode
{
public:
    QList<QCronNode *> & nodes();

    virtual int next(int t) const Q_DECL_OVERRIDE;
    virtual bool match(int tu) const Q_DECL_OVERRIDE;
    virtual void process(QCron * cron,
                         QDateTime & dt,
                         EField field) Q_DECL_OVERRIDE;

private:
    QList<QCronNode*> _nodes;
};

#endif

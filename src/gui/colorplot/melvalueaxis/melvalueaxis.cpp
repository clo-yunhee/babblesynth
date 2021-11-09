/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qmath.h>
#include <private/abstractdomain_p.h>
#include <private/chartaxiselement_p.h>
#include <private/chartpresenter_p.h>
#include <private/chartvalueaxisx_p.h>
#include <private/chartvalueaxisy_p.h>
#include <qexception.h>

#include <QRegularExpression>

#include "../access_private.hpp"
#include "chartmelvalueaxisx_p.h"
#include "chartmelvalueaxisy_p.h"
#include "melvalueaxis_p.h"

ACCESS_PRIVATE_FUN(QtCharts::ChartAxisElement,
                   QString(const QString &formatSpec, const QByteArray &array,
                           qreal value, int precision, const QString &preStr,
                           const QString &postStr) const,
                   formatLabel)

QT_CHARTS_BEGIN_NAMESPACE

static const char *labelFormatMatchString =
    "%[\\-\\+#\\s\\d\\.\\'lhjztL]*([dicuoxfegXFEG])";
static const char *labelFormatMatchLocalizedString =
    "^([^%]*)%\\.(\\d+)([defgiEG])(.*)$";
static QRegularExpression *labelFormatMatcher = 0;
static QRegularExpression *labelFormatMatcherLocalized = 0;
class StaticLabelFormatMatcherDeleter {
   public:
    StaticLabelFormatMatcherDeleter() {}
    ~StaticLabelFormatMatcherDeleter() {
        delete labelFormatMatcher;
        delete labelFormatMatcherLocalized;
    }
};
static StaticLabelFormatMatcherDeleter staticLabelFormatMatcherDeleter;

/*!
    Constructs an axis object that is a child of \a parent.
*/
MelValueAxis::MelValueAxis(QObject *parent)
    : QLogValueAxis(*new MelValueAxisPrivate(this), parent) {}

/*!
    \internal
*/
MelValueAxis::MelValueAxis(MelValueAxisPrivate &d, QObject *parent)
    : QLogValueAxis(d, parent) {}

/*!
    Destroys the object.
*/
MelValueAxis::~MelValueAxis() {
    Q_D(MelValueAxis);
    if (d->m_chart) d->m_chart->removeAxis(this);
}

void MelValueAxis::setMin(qreal min) {
    Q_D(MelValueAxis);
    setRange(min, qMax(d->m_max, min));
}

qreal MelValueAxis::min() const {
    Q_D(const MelValueAxis);
    return d->m_min;
}

void MelValueAxis::setMax(qreal max) {
    Q_D(MelValueAxis);
    setRange(qMin(d->m_min, max), max);
}

qreal MelValueAxis::max() const {
    Q_D(const MelValueAxis);
    return d->m_max;
}

/*!
  Sets the range from \a min to \a max on the axis.
  If \a min is greater than \a max, this function returns without making any
  changes.
*/
void MelValueAxis::setRange(qreal min, qreal max) {
    Q_D(MelValueAxis);

    if (min > max) return;

    if (min > 0) {
        bool changed = false;

        if (!qFuzzyCompare(d->m_min, min)) {
            d->m_min = min;
            changed = true;
            emit minChanged(min);
        }

        if (!qFuzzyCompare(d->m_max, max)) {
            d->m_max = max;
            changed = true;
            emit maxChanged(max);
        }

        if (changed) {
            d->updateTickCount();
            emit rangeChanged(min, max);
            emit d->rangeChanged(min, max);
        }
    }
}

void MelValueAxis::setTickCount(int count) {
    Q_D(MelValueAxis);
    if (d->m_tickCount != count && count >= 2) {
        d->m_tickCount = count;
        emit tickCountChanged(count);
    }
}

int MelValueAxis::tickCount() const {
    Q_D(const MelValueAxis);
    return d->m_tickCount;
}

void MelValueAxis::setMinorTickCount(int count) {
    Q_D(MelValueAxis);
    if (d->m_minorTickCount != count && count >= 0) {
        d->m_minorTickCount = count;
        emit minorTickCountChanged(count);
    }
}

int MelValueAxis::minorTickCount() const {
    Q_D(const MelValueAxis);
    return d->m_minorTickCount;
}

void MelValueAxis::setTickInterval(qreal interval) {
    Q_D(MelValueAxis);
    if (d->m_tickInterval != interval) {
        d->m_tickInterval = interval;
        emit tickIntervalChanged(interval);
    }
}

qreal MelValueAxis::tickInterval() const {
    Q_D(const MelValueAxis);
    return d->m_tickInterval;
}

void MelValueAxis::setTickAnchor(qreal anchor) {
    Q_D(MelValueAxis);
    if (d->m_tickAnchor != anchor) {
        d->m_tickAnchor = anchor;
        emit tickAnchorChanged(anchor);
    }
}

qreal MelValueAxis::tickAnchor() const {
    Q_D(const MelValueAxis);
    return d->m_tickAnchor;
}

void MelValueAxis::setTickType(QValueAxis::TickType type) {
    Q_D(MelValueAxis);
    if (d->m_tickType != type) {
        d->m_tickType = type;
        emit tickTypeChanged(type);
    }
}

QValueAxis::TickType MelValueAxis::tickType() const {
    Q_D(const MelValueAxis);
    return d->m_tickType;
}

void MelValueAxis::setLabelFormat(const QString &format) {
    Q_D(MelValueAxis);
    d->m_format = format;
    emit labelFormatChanged(format);
}

QString MelValueAxis::labelFormat() const {
    Q_D(const MelValueAxis);
    return d->m_format;
}

/*!
  Returns the type of the axis.
*/
QAbstractAxis::AxisType MelValueAxis::type() const { return AxisTypeLogValue; }

///////////////////////////////////////////////////////////////////////////////H////////////////////

MelValueAxisPrivate::MelValueAxisPrivate(MelValueAxis *q)
    : QLogValueAxisPrivate(q),
      m_min(0),
      m_max(0),
      m_tickCount(5),
      m_minorTickCount(0),
      m_format(),
      m_applying(false),
      m_tickInterval(0.0),
      m_tickAnchor(0.0),
      m_tickType(QValueAxis::TicksFixed) {}

MelValueAxisPrivate::~MelValueAxisPrivate() {}

void MelValueAxisPrivate::setMin(const QVariant &min) {
    Q_Q(MelValueAxis);
    bool ok;
    qreal value = min.toReal(&ok);
    if (ok) q->setMin(value);
}

void MelValueAxisPrivate::setMax(const QVariant &max) {
    Q_Q(MelValueAxis);
    bool ok;
    qreal value = max.toReal(&ok);
    if (ok) q->setMax(value);
}

void MelValueAxisPrivate::setRange(const QVariant &min, const QVariant &max) {
    Q_Q(MelValueAxis);
    bool ok1;
    bool ok2;
    qreal value1 = min.toReal(&ok1);
    qreal value2 = max.toReal(&ok2);
    if (ok1 && ok2) q->setRange(value1, value2);
}

void MelValueAxisPrivate::setRange(qreal min, qreal max) {
    Q_Q(MelValueAxis);

    if (min > max) return;

    if (min > 0) {
        bool changed = false;

        if (!qFuzzyCompare(m_min, min)) {
            m_min = min;
            changed = true;
            emit q->minChanged(min);
        }

        if (!qFuzzyCompare(m_max, max)) {
            m_max = max;
            changed = true;
            emit q->maxChanged(max);
        }

        if (changed) {
            updateTickCount();
            emit rangeChanged(min, max);
            emit q->rangeChanged(min, max);
        }
    }
}

void MelValueAxisPrivate::updateTickCount() {
    Q_Q(MelValueAxis);

    const qreal melMax = hz2mel(m_max);
    const qreal melMin = hz2mel(m_min);
    int tickCount = qAbs(qCeil(melMax) - qCeil(melMin));

    // If the high edge sits exactly on the tick value, add a tick
    qreal highValue = melMin < melMax ? melMax : melMin;
    if (qFuzzyCompare(highValue, qreal(qCeil(highValue)))) ++tickCount;

    if (m_tickCount == tickCount) return;

    m_tickCount = tickCount;
    emit q->tickCountChanged(m_tickCount);
}

void MelValueAxisPrivate::initializeGraphics(QGraphicsItem *parent) {
    Q_Q(MelValueAxis);
    ChartAxisElement *axis(0);

    if (m_chart->chartType() == QChart::ChartTypeCartesian) {
        if (orientation() == Qt::Vertical)
            axis = new ChartMelValueAxisY(q, parent);
        if (orientation() == Qt::Horizontal)
            axis = new ChartMelValueAxisX(q, parent);
    }

    if (m_chart->chartType() == QChart::ChartTypePolar) {
        qDebug() << "Polar mel chart not supported";
    }

    m_item.reset(axis);
    QAbstractAxisPrivate::initializeGraphics(parent);
}

void MelValueAxisPrivate::initializeDomain(AbstractDomain *domain) {
    if (orientation() == Qt::Vertical) {
        if (!qFuzzyCompare(m_max, m_min)) {
            domain->setRangeY(m_min, m_max);
        } else if (domain->minY() > 0) {
            setRange(domain->minY(), domain->maxY());
        } else if (domain->maxY() > 0) {
            domain->setRangeY(m_min, domain->maxY());
        } else {
            domain->setRangeY(1, 10);
        }
    }
    if (orientation() == Qt::Horizontal) {
        if (!qFuzzyCompare(m_max, m_min)) {
            domain->setRangeX(m_min, m_max);
        } else if (domain->minX() > 0) {
            setRange(domain->minX(), domain->maxX());
        } else if (domain->maxX() > 0) {
            domain->setRangeX(m_min, domain->maxX());
        } else {
            domain->setRangeX(1, 10);
        }
    }
}

QStringList createMelValueLabels(const ChartAxisElement *element, qreal min,
                                 qreal max, int ticks, qreal tickInterval,
                                 qreal tickAnchor,
                                 QValueAxis::TickType tickType,
                                 const QString &format) {
    return element->createLogValueLabels(min, max, 2, ticks, format);

    QStringList labels;

    for (int i = 0; i < ticks; ++i) {
        labels << QString("%1").arg(i);
    }
    return labels;

    int n = qMax(int(-qFloor(std::log10((max - min) / (ticks - 1)))), 0) + 1;

    qreal value = tickAnchor;
    if (value > min)
        value = value - int((value - min) / tickInterval) * tickInterval;
    else
        value = value + qCeil((min - value) / tickInterval) * tickInterval;
    while (value <= max || qFuzzyCompare(value, max)) {
        labels << element->presenter()->numberToString(value, 'f', n);
        value += tickInterval;
    }

    return labels;

    /*
        QStringList labels;
        if (max <= min || ticks < 1) return labels;
        if (format.isEmpty()) {
            int n =
                qMax(int(-qFloor(std::log10((max - min) / (ticks - 1)))), 0) +
        1; if (tickType == QValueAxis::TicksFixed) { for (int i = 0; i < ticks;
        i++) { qreal value = min + (i * (max - min) / (ticks - 1)); labels <<
        element->presenter()->numberToString(value, 'f', n);
                }
            } else {
                qreal value = tickAnchor;
                if (value > min)
                    value =
                        value - int((value - min) / tickInterval) *
        tickInterval; else value = value + qCeil((min - value) / tickInterval) *
        tickInterval; while (value <= max || qFuzzyCompare(value, max)) { labels
        << element->presenter()->numberToString(value, 'f', n); value +=
        tickInterval;
                }
            }
        } else {
            QByteArray array = format.toLatin1();
            QString formatSpec;
            QString preStr;
            QString postStr;
            int precision = 6;  // Six is the default precision in Qt API
            if (element->presenter()->localizeNumbers()) {
                if (!labelFormatMatcherLocalized)
                    labelFormatMatcherLocalized = new QRegularExpression(
                        QString::fromLatin1(labelFormatMatchLocalizedString));
                QRegularExpressionMatch rmatch;
                if (format.indexOf(*labelFormatMatcherLocalized, 0, &rmatch) !=
                    -1) {
                    preStr = rmatch.captured(1);
                    if (!rmatch.captured(2).isEmpty())
                        precision = rmatch.captured(2).toInt();
                    formatSpec = rmatch.captured(3);
                    postStr = rmatch.captured(4);
                }
            } else {
                if (!labelFormatMatcher)
                    labelFormatMatcher = new QRegularExpression(
                        QString::fromLatin1(labelFormatMatchString));
                QRegularExpressionMatch rmatch;
                if (format.indexOf(*labelFormatMatcher, 0, &rmatch) != -1)
                    formatSpec = rmatch.captured(1);
            }
            if (tickType == QValueAxis::TicksFixed) {
                for (int i = 0; i < ticks; i++) {
                    qreal value = min + (i * (max - min) / (ticks - 1));
                    labels << call_private::formatLabel(
                        *const_cast<ChartAxisElement *>(element), formatSpec,
        array, value, precision, preStr, postStr);
                }
            } else {
                qreal value = tickAnchor;
                if (value > min)
                    value =
                        value - int((value - min) / tickInterval) *
        tickInterval; else value = value + qCeil((min - value) / tickInterval) *
        tickInterval; while (value <= max || qFuzzyCompare(value, max)) { labels
        << call_private::formatLabel( *const_cast<ChartAxisElement *>(element),
        formatSpec, array, value, precision, preStr, postStr); value +=
        tickInterval;
                }
            }
        }
        return labels;
    */
}

QT_CHARTS_END_NAMESPACE

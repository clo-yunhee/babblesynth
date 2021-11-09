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
#include <QtWidgets/qgraphicslayout.h>
#include <private/abstractchartlayout_p.h>
#include <private/chartlogvalueaxisx_p.h>
#include <private/chartpresenter_p.h>

#include "chartmelvalueaxisy_p.h"
#include "melvalueaxis.h"

QT_CHARTS_BEGIN_NAMESPACE

ChartMelValueAxisY::ChartMelValueAxisY(MelValueAxis *axis, QGraphicsItem *item)
    : ChartLogValueAxisY(axis, item), m_axis(axis) {
    QObject::connect(m_axis, SIGNAL(tickCountChanged(int)), this,
                     SLOT(handleTickCountChanged(int)));
    QObject::connect(m_axis, SIGNAL(minorTickCountChanged(int)), this,
                     SLOT(handleMinorTickCountChanged(int)));
    QObject::connect(m_axis, SIGNAL(labelFormatChanged(QString)), this,
                     SLOT(handleLabelFormatChanged(QString)));
    QObject::connect(m_axis, SIGNAL(tickIntervalChanged(qreal)), this,
                     SLOT(handleTickIntervalChanged(qreal)));
    QObject::connect(m_axis, SIGNAL(tickAnchorChanged(qreal)), this,
                     SLOT(handleTickAnchorChanged(qreal)));
    QObject::connect(m_axis, SIGNAL(tickTypeChanged(QValueAxis::TickType)),
                     this, SLOT(handleTickTypeChanged(QValueAxis::TickType)));
}

ChartMelValueAxisY::~ChartMelValueAxisY() {}

QVector<qreal> ChartMelValueAxisY::calculateLayout() const {
    QVector<qreal> points;
    points.resize(m_axis->tickCount());

    const qreal melMax = hz2mel(m_axis->max());
    const qreal melMin = hz2mel(m_axis->min());
    const qreal leftEdge = qMin(melMin, melMax);
    const qreal ceilEdge = qCeil(leftEdge);

    const QRectF &gridRect = gridGeometry();
    const qreal deltaY = gridRect.height() / qAbs(melMax - melMin);
    for (int i = 0; i < m_axis->tickCount(); ++i)
        points[i] = (ceilEdge + qreal(i)) * -deltaY - leftEdge * -deltaY +
                    gridRect.bottom();

    return points;
}

void ChartMelValueAxisY::updateGeometry() {
    const QVector<qreal> &layout = ChartAxisElement::layout();
    const QVector<qreal> &dynamicMinorTicklayout =
        ChartAxisElement::dynamicMinorTicklayout();
    if (layout.isEmpty() && dynamicMinorTicklayout.isEmpty()) return;
    setLabels(createMelValueLabels(this, min(), max(), layout.size(),
                                   m_axis->tickInterval(), m_axis->tickAnchor(),
                                   m_axis->tickType(), m_axis->labelFormat()));
    VerticalAxis::updateGeometry();
}

void ChartMelValueAxisY::handleTickCountChanged(int tick) {
    Q_UNUSED(tick);
    QGraphicsLayoutItem::updateGeometry();
    if (presenter()) presenter()->layout()->invalidate();
}

void ChartMelValueAxisY::handleMinorTickCountChanged(int tick) {
    Q_UNUSED(tick);
    QGraphicsLayoutItem::updateGeometry();
    if (presenter()) presenter()->layout()->invalidate();
}

void ChartMelValueAxisY::handleLabelFormatChanged(const QString &format) {
    Q_UNUSED(format);
    QGraphicsLayoutItem::updateGeometry();
    if (presenter()) presenter()->layout()->invalidate();
}

void ChartMelValueAxisY::handleTickIntervalChanged(qreal interval) {
    Q_UNUSED(interval)
    QGraphicsLayoutItem::updateGeometry();
    if (presenter()) presenter()->layout()->invalidate();
}

void ChartMelValueAxisY::handleTickAnchorChanged(qreal anchor) {
    Q_UNUSED(anchor)
    QGraphicsLayoutItem::updateGeometry();
    if (presenter()) presenter()->layout()->invalidate();
}

void ChartMelValueAxisY::handleTickTypeChanged(QValueAxis::TickType type) {
    Q_UNUSED(type)
    QGraphicsLayoutItem::updateGeometry();
    if (presenter()) presenter()->layout()->invalidate();
}

QSizeF ChartMelValueAxisY::sizeHint(Qt::SizeHint which,
                                    const QSizeF &constraint) const {
    Q_UNUSED(constraint)

    QSizeF sh;

    QSizeF base = VerticalAxis::sizeHint(which, constraint);
    QStringList ticksList;
    qreal melMax = hz2mel(m_axis->max());
    qreal melMin = hz2mel(m_axis->min());
    int tickCount = qAbs(qCeil(melMax) - qCeil(melMin));

    // If the high edge sits exactly on the tick value, add a tick
    qreal highValue = melMin < melMax ? melMax : melMin;
    if (qFuzzyCompare(highValue, qreal(qCeil(highValue)))) tickCount++;

    if (m_axis->max() > m_axis->min() && tickCount > 0)
        ticksList =
            createMelValueLabels(this, m_axis->min(), m_axis->max(), tickCount,
                                 m_axis->tickInterval(), m_axis->tickAnchor(),
                                 m_axis->tickType(), m_axis->labelFormat());
    else
        ticksList.append(QStringLiteral(" "));
    qreal width = 0;
    // Height of vertical axis sizeHint indicates the maximum distance labels
    // can extend past first and last ticks. Base height is irrelevant.
    qreal height = 0;

    switch (which) {
        case Qt::MinimumSize: {
            QRectF boundingRect = ChartPresenter::textBoundingRect(
                axis()->labelsFont(), QStringLiteral("..."),
                axis()->labelsAngle());
            width = boundingRect.width() + labelPadding() + base.width() + 1.0;
            height = boundingRect.height() / 2.0;
            sh = QSizeF(width, height);
            break;
        }
        case Qt::PreferredSize: {
            qreal labelWidth = 0.0;
            qreal firstHeight = -1.0;
            foreach (const QString &s, ticksList) {
                QRectF rect = ChartPresenter::textBoundingRect(
                    axis()->labelsFont(), s, axis()->labelsAngle());
                labelWidth = qMax(rect.width(), labelWidth);
                height = rect.height();
                if (firstHeight < 0.0) firstHeight = height;
            }
            width = labelWidth + labelPadding() + base.width() +
                    2.0;  // two pixels of tolerance
            height = qMax(height, firstHeight) / 2.0;
            sh = QSizeF(width, height);
            break;
        }
        default:
            break;
    }

    return sh;
}

QT_CHARTS_END_NAMESPACE

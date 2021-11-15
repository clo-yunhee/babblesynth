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
#include <private/chartpresenter_p.h>
#include <private/horizontalaxis_p.h>

#include "chartmelvalueaxisx_p.h"
#include "melvalueaxis.h"

QT_CHARTS_BEGIN_NAMESPACE

ChartMelValueAxisX::ChartMelValueAxisX(MelValueAxis *axis, QGraphicsItem *item)
    : ChartLogValueAxisX(axis, item), m_axis(axis) {
    QObject::connect(m_axis, SIGNAL(labelFormatChanged(QString)), this,
                     SLOT(handleLabelFormatChanged(QString)));
}

ChartMelValueAxisX::~ChartMelValueAxisX() {}

QVector<qreal> ChartMelValueAxisX::calculateLayout() const {
    QVector<qreal> points;
    points.resize(m_axis->tickCount());

    const qreal melMax = hz2mel(m_axis->max());
    const qreal melMin = hz2mel(m_axis->min());
    const qreal leftEdge = qMin(melMin, melMax);
    const qreal ceilEdge = qCeil(leftEdge);

    const QRectF &gridRect = gridGeometry();
    const qreal deltaX = gridRect.width() / qAbs(melMax - melMin);
    for (int i = 0; i < m_axis->tickCount(); ++i)
        points[i] = (ceilEdge + qreal(i)) * deltaX - leftEdge * deltaX +
                    gridRect.left();

    return points;
}

void ChartMelValueAxisX::updateGeometry() {
    const QVector<qreal> &layout = ChartAxisElement::layout();

    setLabels(createMelValueLabels(this, m_axis->min(), m_axis->max(),
                                   layout.size(), m_axis->tickInterval(),
                                   m_axis->tickAnchor(), m_axis->tickType(),
                                   m_axis->labelFormat()));
    HorizontalAxis::updateGeometry();
}

void ChartMelValueAxisX::handleLabelFormatChanged(const QString &format) {
    Q_UNUSED(format);
    QGraphicsLayoutItem::updateGeometry();
    if (presenter()) presenter()->layout()->invalidate();
}

QSizeF ChartMelValueAxisX::sizeHint(Qt::SizeHint which,
                                    const QSizeF &constraint) const {
    Q_UNUSED(constraint)

    QSizeF sh;

    QSizeF base = HorizontalAxis::sizeHint(which, constraint);
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
    // Width of horizontal axis sizeHint indicates the maximum distance labels
    // can extend past first and last ticks. Base width is irrelevant.
    qreal width = 0;
    qreal height = 0;

    switch (which) {
        case Qt::MinimumSize: {
            QRectF boundingRect = ChartPresenter::textBoundingRect(
                axis()->labelsFont(), QStringLiteral("..."),
                axis()->labelsAngle());
            width = boundingRect.width() / 2.0;
            height =
                boundingRect.height() + labelPadding() + base.height() + 1.0;
            sh = QSizeF(width, height);
            break;
        }
        case Qt::PreferredSize: {
            qreal labelHeight = 0.0;
            qreal firstWidth = -1.0;
            foreach (const QString &s, ticksList) {
                QRectF rect = ChartPresenter::textBoundingRect(
                    axis()->labelsFont(), s, axis()->labelsAngle());
                labelHeight = qMax(rect.height(), labelHeight);
                width = rect.width();
                if (firstWidth < 0.0) firstWidth = width;
            }
            height = labelHeight + labelPadding() + base.height() + 1.0;
            width = qMax(width, firstWidth) / 2.0;
            sh = QSizeF(width, height);
            break;
        }
        default:
            break;
    }

    return sh;
}

QT_CHARTS_END_NAMESPACE
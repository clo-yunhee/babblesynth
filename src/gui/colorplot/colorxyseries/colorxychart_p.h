/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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

//  W A R N I N G
//  -------------
//
// This file is not part of the Qt Chart API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef COLORXYCHART_H
#define COLORXYCHART_H

#include <QtCharts/private/qchartglobal_p.h>
#include <private/chartitem_p.h>
#include <private/xyanimation_p.h>

#include <QtCharts/QChartGlobal>
#include <QtCharts/QValueAxis>
#include <QtGui/QPen>

#include "qcolorxyseries.h"

QT_CHARTS_BEGIN_NAMESPACE

class ChartPresenter;

class ColorXYChart : public ChartItem {
    Q_OBJECT
   public:
    explicit ColorXYChart(QColorXYSeries *series, QGraphicsItem *item = 0);
    ~ColorXYChart() {}

    void setGeometryPoints(const QVector<QPointF> &points);
    QVector<QPointF> geometryPoints() const { return m_points; }

    void setAnimation(XYAnimation *animation);
    ChartAnimation *animation() const override { return m_animation; }
    virtual void updateGeometry() = 0;

    bool isDirty() const { return m_dirty; }
    void setDirty(bool dirty);

    void getSeriesRanges(qreal &minX, qreal &maxX, qreal &minY, qreal &maxY);
    QVector<bool> offGridStatusVector();

   public Q_SLOTS:
    void handlePointAdded(int index);
    void handlePointRemoved(int index);
    void handlePointsRemoved(int index, int count);
    void handlePointReplaced(int index);
    void handlePointsReplaced();
    void handleDomainUpdated() override;

    virtual void handleSeriesUpdated();

   Q_SIGNALS:
    void clicked(const QPointF &point);
    void hovered(const QPointF &point, bool state);
    void pressed(const QPointF &point);
    void released(const QPointF &point);
    void doubleClicked(const QPointF &point);

   protected:
    virtual void updateChart(const QVector<QPointF> &oldPoints,
                             const QVector<QPointF> &newPoints, int index = -1);
    virtual void updateGlChart();
    virtual void refreshGlChart();

   private:
    inline bool isEmpty();

   protected:
    QColorXYSeries *m_series;
    QVector<QPointF> m_points;
    // QColor m_selectedColor;
    XYAnimation *m_animation;
    bool m_dirty;

    QHash<int, QHash<QColorXYSeries::PointConfiguration, QVariant>>
        m_pointsConfiguration;
    bool m_pointsConfigurationDirty;
};

QT_CHARTS_END_NAMESPACE

#endif
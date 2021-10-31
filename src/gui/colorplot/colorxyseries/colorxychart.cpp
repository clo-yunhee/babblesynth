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

#include <private/abstractdomain_p.h>
#include <private/chartdataset_p.h>
#include <private/chartpresenter_p.h>
#include <private/glxyseriesdata_p.h>
#include <private/qabstractaxis_p.h>

#include <QtCharts/QXYModelMapper>
#include <QtCore/QAbstractItemModel>
#include <QtGui/QPainter>

#include "colorxychart_p.h"
#include "qcolorxyseries.h"
#include "qcolorxyseries_p.h"

QT_CHARTS_BEGIN_NAMESPACE

ColorXYChart::ColorXYChart(QColorXYSeries *series, QGraphicsItem *item)
    : ChartItem(series->d_func(), item),
      m_series(series),
      m_animation(0),
      m_dirty(true) {
    connect(series->d_func(), &QXYSeriesPrivate::updated, this,
            &ColorXYChart::handleSeriesUpdated);
    connect(series, &QXYSeries::pointReplaced, this,
            &ColorXYChart::handlePointReplaced);
    connect(series, &QXYSeries::pointsReplaced, this,
            &ColorXYChart::handlePointsReplaced);
    connect(series, &QXYSeries::pointAdded, this,
            &ColorXYChart::handlePointAdded);
    connect(series, &QXYSeries::pointRemoved, this,
            &ColorXYChart::handlePointRemoved);
    connect(series, &QXYSeries::pointsRemoved, this,
            &ColorXYChart::handlePointsRemoved);
    connect(this, &ColorXYChart::clicked, series, &QXYSeries::clicked);
    connect(this, &ColorXYChart::hovered, series, &QXYSeries::hovered);
    connect(this, &ColorXYChart::pressed, series, &QXYSeries::pressed);
    connect(this, &ColorXYChart::released, series, &QXYSeries::released);
    connect(this, &ColorXYChart::doubleClicked, series,
            &QXYSeries::doubleClicked);
    connect(series, &QAbstractSeries::useOpenGLChanged, this,
            &ColorXYChart::handleDomainUpdated);
}

void ColorXYChart::setGeometryPoints(const QVector<QPointF> &points) {
    m_points = points;
}

void ColorXYChart::setAnimation(XYAnimation *animation) {
    m_animation = animation;
}

void ColorXYChart::setDirty(bool dirty) { m_dirty = dirty; }

// Returns a list with same size as geometryPoints list, indicating
// the off grid status of points.
QVector<bool> ColorXYChart::offGridStatusVector() {
    qreal minX = domain()->minX();
    qreal maxX = domain()->maxX();
    qreal minY = domain()->minY();
    qreal maxY = domain()->maxY();

    QVector<bool> returnVector;
    returnVector.resize(m_points.size());
    // During remove animation series may have different number of points,
    // so ensure we don't go over the index. No need to check for zero points,
    // this will not be called in such a situation.
    const int seriesLastIndex = m_series->count() - 1;

    for (int i = 0; i < m_points.size(); i++) {
        const QPointF &seriesPoint = m_series->at(qMin(seriesLastIndex, i));
        if (seriesPoint.x() < minX || seriesPoint.x() > maxX ||
            seriesPoint.y() < minY || seriesPoint.y() > maxY) {
            returnVector[i] = true;
        } else {
            returnVector[i] = false;
        }
    }
    return returnVector;
}

void ColorXYChart::updateChart(const QVector<QPointF> &oldPoints,
                               const QVector<QPointF> &newPoints, int index) {
    if (m_animation) {
        m_animation->setup(oldPoints, newPoints, index);
        m_points = newPoints;
        setDirty(false);
        presenter()->startAnimation(m_animation);
    } else {
        m_points = newPoints;
        updateGeometry();
    }
}

void ColorXYChart::updateGlChart() {
    dataSet()->glXYSeriesDataManager()->setPoints(m_series, domain());
    presenter()->updateGLWidget();
    updateGeometry();
}

// Doesn't update gl geometry, but refreshes the chart
void ColorXYChart::refreshGlChart() {
    if (presenter()) presenter()->updateGLWidget();
}

// handlers

void ColorXYChart::handlePointAdded(int index) {
    Q_ASSERT(index < m_series->count());
    Q_ASSERT(index >= 0);

    if (m_series->useOpenGL()) {
        updateGlChart();
    } else {
        QVector<QPointF> points;
        if (m_dirty || m_points.isEmpty()) {
            points =
                domain()->calculateGeometryPoints(m_series->pointsVector());
        } else {
            points = m_points;
            QPointF point = domain()->calculateGeometryPoint(
                m_series->points().at(index), m_validData);
            if (!m_validData)
                m_points.clear();
            else
                points.insert(index, point);
        }
        updateChart(m_points, points, index);
    }
}

void ColorXYChart::handlePointRemoved(int index) {
    Q_ASSERT(index <= m_series->count());
    Q_ASSERT(index >= 0);

    if (m_series->useOpenGL()) {
        updateGlChart();
    } else {
        QVector<QPointF> points;
        if (m_dirty || m_points.isEmpty()) {
            points =
                domain()->calculateGeometryPoints(m_series->pointsVector());
        } else {
            points = m_points;
            points.remove(index);
        }
        updateChart(m_points, points, index);
    }
}

void ColorXYChart::handlePointsRemoved(int index, int count) {
    Q_ASSERT(index <= m_series->count());
    Q_ASSERT(index >= 0);

    if (m_series->useOpenGL()) {
        updateGlChart();
    } else {
        QVector<QPointF> points;
        if (m_dirty || m_points.isEmpty()) {
            points =
                domain()->calculateGeometryPoints(m_series->pointsVector());
        } else {
            points = m_points;
            points.remove(index, count);
        }
        updateChart(m_points, points, index);
    }
}

void ColorXYChart::handlePointReplaced(int index) {
    Q_ASSERT(index < m_series->count());
    Q_ASSERT(index >= 0);

    if (m_series->useOpenGL()) {
        updateGlChart();
    } else {
        QVector<QPointF> points;
        if (m_dirty || m_points.isEmpty()) {
            points =
                domain()->calculateGeometryPoints(m_series->pointsVector());
        } else {
            QPointF point = domain()->calculateGeometryPoint(
                m_series->points().at(index), m_validData);
            if (!m_validData) m_points.clear();
            points = m_points;
            if (m_validData) points.replace(index, point);
        }
        updateChart(m_points, points, index);
    }
}

void ColorXYChart::handlePointsReplaced() {
    if (m_series->useOpenGL()) {
        updateGlChart();
    } else {
        // All the points were replaced -> recalculate
        QVector<QPointF> points =
            domain()->calculateGeometryPoints(m_series->pointsVector());
        updateChart(m_points, points, -1);
    }
}

void ColorXYChart::handleDomainUpdated() {
    if (m_series->useOpenGL()) {
        updateGlChart();
    } else {
        if (isEmpty()) return;
        QVector<QPointF> points =
            domain()->calculateGeometryPoints(m_series->pointsVector());
        updateChart(m_points, points);
    }
}

void ColorXYChart::handleSeriesUpdated() {}

bool ColorXYChart::isEmpty() {
    return domain()->isEmpty() || m_series->points().isEmpty();
}

QT_CHARTS_END_NAMESPACE

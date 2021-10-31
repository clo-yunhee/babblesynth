/*
 * BabbleSynth
 * Copyright (C) 2021  Clo Yun-Hee Dufour
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "chart_view.h"

#include "qnamespace.h"

using namespace babblesynth::gui;

using MatchingPointsValueType = QPair<QPointF, int>;

Q_DECLARE_METATYPE(MatchingPointsValueType);

ChartView::ChartView(QChart* chart, QWidget* parent)
    : QChartView(chart, parent) {
    setMouseTracking(true);
}

void ChartView::mouseMoveEvent(QMouseEvent* event) {
    if (!checkThrottle()) return;

    auto pointsPerSeries = findMatchingPoints(event->pos());

    auto mousePoint = pointFromPos(event->pos());

    for (auto it = pointsPerSeries.begin(); it != pointsPerSeries.end(); ++it) {
        auto series = it.key();
        const auto& variant = it.value();

        if (variant.isValid()) {
            const auto& [point, index] =
                variant.value<MatchingPointsValueType>();
            emit mouseHovered(series, point, index);
        } else {
            emit mouseLeft(series);
        }

        if (m_seriesDragging.value(series)) {
            emit mouseDragging(series, mousePoint);
        }
    }
}

void ChartView::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton) return;

    auto pointsPerSeries = findMatchingPoints(event->pos());

    for (auto it = pointsPerSeries.begin(); it != pointsPerSeries.end(); ++it) {
        auto series = it.key();
        const auto& variant = it.value();

        if (variant.isValid()) {
            const auto& [point, index] =
                variant.value<MatchingPointsValueType>();
            emit mouseDoubleClicked(series, point, index);
        }
    }
}

void ChartView::mousePressEvent(QMouseEvent* event) {
    auto pointsPerSeries = findMatchingPoints(event->pos());

    for (auto it = pointsPerSeries.begin(); it != pointsPerSeries.end(); ++it) {
        auto series = it.key();
        const auto& variant = it.value();

        if (variant.isValid()) {
            const auto& [point, index] =
                variant.value<MatchingPointsValueType>();

            if (event->button() != Qt::RightButton) {
                emit mousePressed(series, point, index);
                m_seriesDragging[series] = true;
            } else if (!m_seriesRightClicking.value(series, false)) {
                emit mouseRightClicked(series, point, index);
                m_seriesRightClicking[series] = true;
            }
        }
    }
}

void ChartView::mouseReleaseEvent(QMouseEvent* event) {
    auto seriesList = chart()->series();

    for (const auto series : seriesList) {
        if (auto xySeries = dynamic_cast<QColorXYSeries*>(series);
            xySeries != nullptr) {
            if (event->button() != Qt::RightButton) {
                emit mouseReleased(xySeries->name());
                m_seriesDragging[xySeries->name()] = false;
            } else {
                m_seriesRightClicking[xySeries->name()] = false;
            }
        }
    }
}

QPointF ChartView::pointFromPos(const QPointF& pos) const {
    const auto scenePos = mapToScene(pos.toPoint());
    const auto chartPos = chart()->mapFromScene(scenePos);
    const auto point = chart()->mapToValue(chartPos);

    return point;
}

QPointF ChartView::posFromPoint(const QPointF& point) const {
    const auto chartPos = chart()->mapToPosition(point);
    const auto scenePos = chart()->mapToScene(chartPos);
    const auto pos = mapFromScene(scenePos);

    return pos;
}

QHash<QString, QVariant> ChartView::findMatchingPoints(
    const QPointF& mousePos) const {
    QHash<QString, QVariant> pointsPerSeries;

    auto seriesList = chart()->series();

    auto floatNaN = std::numeric_limits<float>::quiet_NaN();
    auto floatInf = std::numeric_limits<float>::infinity();

    int seriesIndex = 0;

    for (const auto series : seriesList) {
        if (auto xySeries = dynamic_cast<QColorXYSeries*>(series);
            xySeries != nullptr) {
            auto points = xySeries->pointsVector();
            if (points.empty()) continue;

            auto pointsConfiguration = xySeries->pointsConfiguration();

            auto closestPoint = QPointF(floatNaN, floatNaN);
            auto closestPointIndex = -1;
            auto minDistanceSq = floatInf;

            for (int index = 0; index < points.size(); ++index) {
                auto diff = mousePos - posFromPoint(points[index]);
                auto distanceSq = QPointF::dotProduct(diff, diff);

                // Check if this point is closer to the mouse than any we found
                // so far.
                if (distanceSq < minDistanceSq) {
                    auto pointSize =
                        pointsConfiguration
                            [index][QColorXYSeries::PointConfiguration::Size]
                                .toReal();

                    // Check if the mouse point is in the point's circle.
                    if (distanceSq < 4 * pointSize * pointSize) {
                        // Set this point as the closest point.
                        closestPoint = points[index];
                        closestPointIndex = index;
                        minDistanceSq = distanceSq;
                    }
                }
            }

            if (closestPointIndex >= 0) {
                // If we found at least one point matching, fill in the table
                // entry.
                pointsPerSeries[xySeries->name()] =
                    QVariant::fromValue(QPair(closestPoint, closestPointIndex));
            } else {
                // If we didn't find any, put an empty variant in the table.
                pointsPerSeries[xySeries->name()] = QVariant();
            }
        }

        ++seriesIndex;
    }

    return pointsPerSeries;
}

bool ChartView::checkThrottle() {
    // Throttle the mouse move events to 30 Hz max.
    if (!m_eventThrottle.isValid() || m_eventThrottle.hasExpired(33)) {
        m_eventThrottle.start();
        return true;
    }
    return false;
}
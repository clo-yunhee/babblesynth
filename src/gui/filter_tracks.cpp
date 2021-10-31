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

#include "filter_tracks.h"

#include <iostream>
#include <numeric>
#include <stdexcept>

#include "app_state.h"
#include "clickable_label.h"

using namespace babblesynth::gui;

FilterTracks::FilterTracks(int nFormants, QWidget* parent)
    : QWidget(parent),
      m_points(nFormants),
      m_nFormants(nFormants),
      m_formantGraph(nFormants),
      m_formantPoints(nFormants),
      m_isDragging(false),
      m_isDraggingPoint(false) {
    setObjectName("FilterTracks");

    m_timeAxis = new QValueAxis(this);
    m_timeAxis->setRange(0, 1);

    m_valueAxis = new QValueAxis(this);
    m_valueAxis->setRange(0, 6000);
    m_valueAxis->setTickInterval(1000);
    m_valueAxis->setTickType(QValueAxis::TicksDynamic);

    QChart* chart = new QChart;
    chart->legend()->hide();
    chart->addAxis(m_timeAxis, Qt::AlignBottom);
    chart->addAxis(m_valueAxis, Qt::AlignLeft);
    chart->setTitle(tr("Pitch and formants"));
    chart->setTheme(QChart::ChartThemeBlueIcy);

    for (int n = 0; n < nFormants; ++n) {
        QColorLineSeries* graph = new QColorLineSeries(this);
        graph->setName(QString("graph%1").arg(n));
        graph->setProperty("formantNumber", n);
        graph->setPen(QPen(Qt::transparent, 12));
        graph->setPointsVisible(true);
        chart->addSeries(graph);
        graph->attachAxis(m_timeAxis);
        graph->attachAxis(m_valueAxis);
        m_formantGraph[n] = graph;
    }

    for (int n = 0; n < nFormants; ++n) {
        QColorLineSeries* points = new QColorLineSeries(this);
        points->setName(QString("points%1").arg(n));
        points->setProperty("formantNumber", n);
        points->setPen(QPen(Qt::transparent, 12));
        points->setPointsVisible(true);
        chart->addSeries(points);
        points->attachAxis(m_timeAxis);
        points->attachAxis(m_valueAxis);
        m_formantPoints[n] = points;
    }

    m_pitchGraph = new QColorLineSeries(this);
    m_pitchGraph->setColor(Qt::transparent);
    m_pitchGraph->setPointsVisible(true);
    chart->addSeries(m_pitchGraph);
    m_pitchGraph->attachAxis(m_timeAxis);
    m_pitchGraph->attachAxis(m_valueAxis);

    m_chartView = new ChartView(chart, this);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    QObject::connect(m_chartView, &ChartView::mouseHovered, this,
                     &FilterTracks::onSeriesHovered);
    QObject::connect(m_chartView, &ChartView::mouseLeft, this,
                     &FilterTracks::onSeriesLeft);
    QObject::connect(m_chartView, &ChartView::mouseDoubleClicked, this,
                     &FilterTracks::onSeriesDoubleClicked);
    QObject::connect(m_chartView, &ChartView::mouseRightClicked, this,
                     &FilterTracks::onSeriesRightClicked);
    QObject::connect(m_chartView, &ChartView::mousePressed, this,
                     &FilterTracks::onSeriesPressed);
    QObject::connect(m_chartView, &ChartView::mouseReleased, this,
                     &FilterTracks::onSeriesReleased);
    QObject::connect(m_chartView, &ChartView::mouseDragging, this,
                     &FilterTracks::onSeriesDragging);

    QVBoxLayout* rootLayout = new QVBoxLayout;
    rootLayout->addWidget(m_chartView);
    setLayout(rootLayout);

    std::array freqsA{1000, 1500, 2900, 3950, 5200};
    std::array freqsE{400, 2500, 3000, 4000, 5000};
    std::array freqsI{280, 2900, 3300, 4000, 4800};
    std::array freqsO{500, 800, 2400, 3800, 4900};
    std::array freqsU{250, 800, 2700, 4000, 5100};

    for (int n = 0; n < nFormants; ++n) {
        m_points[n].append(QPointF(0.0, freqsA[n]));
        m_points[n].append(QPointF(0.3, freqsA[n]));
        m_points[n].append(QPointF(0.5, freqsE[n]));
        m_points[n].append(QPointF(0.7, freqsE[n]));
        m_points[n].append(QPointF(0.9, freqsI[n]));
        m_points[n].append(QPointF(1.1, freqsI[n]));
        m_points[n].append(QPointF(1.3, freqsO[n]));
        m_points[n].append(QPointF(1.5, freqsO[n]));
        m_points[n].append(QPointF(1.7, freqsU[n]));
        m_points[n].append(QPointF(2.0, freqsU[n]));
    }

    updatePlans();
}

void FilterTracks::redrawGraph() {
    const double duration = appState->pitchPlan()->duration();

    m_timeAxis->setRange(0, duration);

    constexpr int nPoints = 400;

    QVector<QPointF> pitch(nPoints);
    QVector<qreal> amplitude(nPoints + 2);

    QVector<QVector<QPointF>> formants(m_nFormants, QVector<QPointF>(nPoints));

    for (int i = 0; i < nPoints; ++i) {
        const double t = duration * i / double(nPoints - 1);

        pitch[i].setX(t);
        pitch[i].setY(appState->pitchPlan()->evaluateAtTime(t));

        amplitude[i] = appState->amplitudePlan()->evaluateAtTime(t);

        for (int n = 0; n < m_nFormants; ++n) {
            formants[n][i].setX(t);
            formants[n][i].setY(appState->formantPlan(n)->evaluateAtTime(t));
        }
    }

    amplitude[nPoints] = 0;
    amplitude[nPoints + 1] = 1;

    m_pitchGraph->replace(pitch);

    QLinearGradient pitchGradient(QPointF(50, 0), QPointF(50, 100));
    pitchGradient.setColorAt(0, Qt::white);
    pitchGradient.setColorAt(0.6, Qt::darkGray);
    pitchGradient.setColorAt(1, Qt::black);

    m_pitchGraph->colorBy(amplitude, pitchGradient);
    m_pitchGraph->sizeBy(amplitude, 0, 5);

    for (int n = 0; n < m_nFormants; ++n) {
        m_formantGraph[n]->replace(formants[n]);

        for (int i = 0; i < nPoints; ++i) {
            setGraphPointStyle(n, i, Normal);
        }

        m_formantPoints[n]->replace(m_points[n]);

        for (int i = 0; i < m_points[n].size(); ++i) {
            setPointStyle(n, i, Normal);
        }
    }
}

void FilterTracks::updatePlans() {
    for (int n = 0; n < m_nFormants; ++n) {
        appState->formantPlan(n)->reset(m_points[n][0].y());

        for (int i = 1; i < m_points[n].size(); ++i) {
            appState->formantPlan(n)->cubicToValueAtTime(m_points[n][i].y(),
                                                         m_points[n][i].x());
        }
    }

    appState->updatePlans();

    redrawGraph();
}

void FilterTracks::onSeriesHovered(const QString& series, const QPointF& point,
                                   int index) {
    if (!series.startsWith("graph") && !series.startsWith("points")) {
        return;
    }

    const int n = series.right(1).toInt();

    const int nGraphPoints = m_formantGraph[n]->count();

    for (int i = 0; i < nGraphPoints; ++i) {
        setGraphPointStyle(n, i, Hover);
    }

    const int nPoints = m_points[n].size();

    for (int i = 0; i < nPoints; ++i) {
        setPointStyle(n, i, Hover);
    }

    if (!m_isDragging) {
        int leftPointIndex = 0;
        while (leftPointIndex < nPoints &&
               point.x() >= m_points[n][leftPointIndex].x()) {
            ++leftPointIndex;
        }

        int rightPointIndex = nPoints - 1;
        while (rightPointIndex >= 0 &&
               point.x() <= m_points[n][rightPointIndex].x()) {
            --rightPointIndex;
        }

        if (leftPointIndex > 0) --leftPointIndex;
        if (rightPointIndex < nPoints - 1) ++rightPointIndex;

        if (leftPointIndex < nPoints && rightPointIndex >= 0) {
            if (leftPointIndex <
                rightPointIndex) {  // Strictly increasing interval: hovering a
                                    // segment
                auto leftPos =
                    m_chartView->posFromPoint(m_points[n][leftPointIndex]);
                leftPos -= m_chartView->posFromPoint(point);
                if (QPointF::dotProduct(leftPos, leftPos) <= 10 * 10) {
                    return;
                }

                auto rightPos =
                    m_chartView->posFromPoint(m_points[n][rightPointIndex]);
                rightPos -= m_chartView->posFromPoint(point);
                if (QPointF::dotProduct(rightPos, rightPos) <= 10 * 10) {
                    return;
                }

                setGraphStyleBetweenPoints(n, leftPointIndex, rightPointIndex,
                                           HoverSegment);
            } else if (leftPointIndex ==
                       rightPointIndex) {  // Same index: hovering a point
                // Nothing.
            }
        }
    }
}

void FilterTracks::onSeriesLeft(const QString& series) {
    if (!series.startsWith("graph")) {
        return;
    }

    const int n = series.right(1).toInt();

    const int nGraphPoints = m_formantGraph[n]->count();

    for (int i = 0; i < nGraphPoints; ++i) {
        setGraphPointStyle(n, i, Normal);
    }

    const int nPoints = m_points[n].size();

    for (int i = 0; i < nPoints; ++i) {
        setPointStyle(n, i, Normal);
    }
}

void FilterTracks::onSeriesDoubleClicked(const QString& series,
                                         const QPointF& point, int index) {
    if (!series.startsWith("graph")) {
        return;
    }

    const int n = series.right(1).toInt();

    m_points[n].append(point);

    std::sort(m_points[n].begin(), m_points[n].end(),
              [](auto a, auto b) { return a.x() < b.x(); });

    updatePlans();
}

void FilterTracks::onSeriesRightClicked(const QString& series,
                                        const QPointF& point, int index) {
    if (!series.startsWith("points")) {
        return;
    }

    const int n = series.right(1).toInt();

    m_points[n].removeAt(index);

    updatePlans();
}

void FilterTracks::onSeriesPressed(const QString& series, const QPointF& point,
                                   int index) {
    if (!series.startsWith("points") && !series.startsWith("graph")) {
        return;
    }

    const int n = series.right(1).toInt();

    if (!m_isDragging) {
        if (series.startsWith("points")) {
            setPointStyle(n, index, Drag);

            m_isDragging = true;
            m_isDraggingPoint = true;
            m_formantBeingDragged = n;
            m_pointIndexBeingDragged = index;
        } else if (series.startsWith("graph")) {
            const int nPoints = m_points[n].size();

            int leftPointIndex = 0;
            while (leftPointIndex < nPoints &&
                   point.x() >= m_points[n][leftPointIndex].x()) {
                ++leftPointIndex;
            }

            int rightPointIndex = nPoints - 1;
            while (rightPointIndex >= 0 &&
                   point.x() <= m_points[n][rightPointIndex].x()) {
                --rightPointIndex;
            }

            if (leftPointIndex > 0) --leftPointIndex;
            if (rightPointIndex < nPoints - 1) ++rightPointIndex;

            if (leftPointIndex < nPoints && rightPointIndex >= 0) {
                if (leftPointIndex <
                    rightPointIndex) {  // Strictly increasing interval:
                                        // hovering a segment

                    // Add checks for proximity to the two ends.
                    auto leftPos =
                        m_chartView->posFromPoint(m_points[n][leftPointIndex]);
                    leftPos -= m_chartView->posFromPoint(point);
                    if (QPointF::dotProduct(leftPos, leftPos) <= 10 * 10) {
                        return;
                    }

                    auto rightPos =
                        m_chartView->posFromPoint(m_points[n][rightPointIndex]);
                    rightPos -= m_chartView->posFromPoint(point);
                    if (QPointF::dotProduct(rightPos, rightPos) <= 10 * 10) {
                        return;
                    }

                    setPointStyle(n, leftPointIndex, Drag);
                    setPointStyle(n, rightPointIndex, Drag);
                    setGraphStyleBetweenPoints(n, leftPointIndex,
                                               rightPointIndex, Drag);

                    m_isDragging = true;
                    m_isDraggingPoint = false;
                    m_formantBeingDragged = n;
                    m_pointIndexBeingDragged = leftPointIndex;
                    m_secondPointIndexBeingDragged = rightPointIndex;
                    m_dragPointOriginY = point.y();
                    m_firstPointOriginY = m_points[n][leftPointIndex].y();
                    m_secondPointOriginY = m_points[n][rightPointIndex].y();
                }
            }
        }
    }
}

void FilterTracks::onSeriesReleased(const QString& series) {
    if (m_isDragging) {
        setPointStyle(m_formantBeingDragged, m_pointIndexBeingDragged, Normal);

        if (!m_isDraggingPoint) {
            setPointStyle(m_formantBeingDragged, m_secondPointIndexBeingDragged,
                          Normal);
            setGraphStyleBetweenPoints(m_formantBeingDragged,
                                       m_pointIndexBeingDragged,
                                       m_secondPointIndexBeingDragged, Normal);
        }

        m_isDragging = false;
    }
}

void FilterTracks::onSeriesDragging(const QString& series, QPointF point) {
    if (!series.startsWith("points") && !series.startsWith("graph")) {
        return;
    }

    const int n = series.right(1).toInt();

    if (m_isDragging) {
        if (series.startsWith("points")) {
            point.setX(std::max(point.x(), 0.001));

            // Don't finish this dragging event if there is already a point with
            // that x position.
            if (std::find_if(m_points[n].begin(), m_points[n].end(),
                             [point](auto p) {
                                 return qFuzzyCompare(p.x(), point.x());
                             }) == m_points[n].end()) {
                m_points[n][m_pointIndexBeingDragged] = point;

                if (m_pointIndexBeingDragged == 0) {
                    m_points[n][0].setX(0);
                } else {
                    std::vector<int> idx(m_points[n].size());
                    std::iota(idx.begin(), idx.end(), 0);

                    std::stable_sort(
                        idx.begin(), idx.end(), [this, n](int i, int j) {
                            return m_points[n][i].x() < m_points[n][j].x();
                        });

                    m_pointIndexBeingDragged = idx[m_pointIndexBeingDragged];

                    QList<QPointF> newPoints;

                    for (int i = 0; i < idx.size(); ++i) {
                        newPoints.append(m_points[n][idx[i]]);
                    }

                    m_points[n] = newPoints;
                }
            } else if (series.startsWith("graph") && !m_isDraggingPoint) {
                double dy = point.y() - m_dragPointOriginY;

                m_points[n][m_pointIndexBeingDragged].ry() =
                    m_firstPointOriginY + dy;
                m_points[n][m_secondPointIndexBeingDragged].ry() =
                    m_secondPointOriginY + dy;
            }

            updatePlans();

            const int nGraphPoints = m_formantGraph[n]->count();

            for (int i = 0; i < nGraphPoints; ++i) {
                setGraphPointStyle(n, i, Hover);
            }

            const int nPoints = m_points[n].size();

            for (int i = 0; i < nPoints; ++i) {
                setPointStyle(n, i, Hover);
            }

            setPointStyle(n, m_pointIndexBeingDragged, Drag);

            if (!m_isDraggingPoint) {
                setPointStyle(n, m_secondPointIndexBeingDragged, Drag);
                setGraphStyleBetweenPoints(n, m_pointIndexBeingDragged,
                                           m_secondPointIndexBeingDragged,
                                           Drag);
            }
        }
    }
}

void FilterTracks::setPointStyle(int formant, int index, PointStyle style) {
    qreal size;
    QColor color;

    switch (style) {
        case Normal:
            size = 8;
            color = QColor(96, 96, 96);
            break;
        case Hover:
            size = 9;
            color = QColor(72, 72, 72);
            break;
        case Drag:
            size = 10;
            color = QColor(48, 48, 48);
            break;
        case HoverSegment:
            throw std::invalid_argument(
                "HoverSegment style is not applicable to point series");
    }

    m_formantPoints[formant]->setPointConfiguration(
        index, QColorXYSeries::PointConfiguration::Size, size);
    m_formantPoints[formant]->setPointConfiguration(
        index, QColorXYSeries::PointConfiguration::Color, color);
}

void FilterTracks::setGraphPointStyle(int formant, int index,
                                      PointStyle style) {
    qreal size;
    QColor color;

    switch (style) {
        case Normal:
            size = 4;
            color = QColor(255, 127, 80);
            break;
        case Hover:
            size = 5;
            color = QColor(255, 127, 80);
            break;
        case HoverSegment:
            size = 5;
            color = QColor(255, 167, 91);
            break;
        case Drag:
            size = 6;
            color = QColor(255, 97, 76);
            break;
    }

    m_formantGraph[formant]->setPointConfiguration(
        index, QColorXYSeries::PointConfiguration::Size, size);
    m_formantGraph[formant]->setPointConfiguration(
        index, QColorXYSeries::PointConfiguration::Color, color);
}

void FilterTracks::setGraphStyleBetweenPoints(int formant, int left, int right,
                                              PointStyle style) {
    QVector<QPointF> graph = m_formantGraph[formant]->pointsVector();

    int leftGraph = std::distance(
        graph.begin(),
        std::upper_bound(graph.begin(), graph.end(), m_points[formant][left],
                         [](auto a, auto b) { return a.x() < b.x(); }));

    int rightGraph = std::distance(
        graph.begin(),
        std::lower_bound(graph.begin(), graph.end(), m_points[formant][right],
                         [](auto a, auto b) { return a.x() < b.x(); }));

    for (int i = leftGraph; i <= rightGraph; ++i) {
        setGraphPointStyle(formant, i, style);
    }
}
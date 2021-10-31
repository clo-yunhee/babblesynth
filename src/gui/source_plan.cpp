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

#include "source_plan.h"

#include "app_state.h"
#include "clickable_label.h"

using namespace babblesynth::gui;

SourcePlan::SourcePlan(QWidget* parent)
    : QWidget(parent), m_isDragging(false), m_isDraggingSegment(false) {
    setObjectName("SourcePlan");
    // setMinimumSize(QSize(1024, 768));

    m_timeAxis = new QValueAxis(this);
    m_timeAxis->setRange(0, 1);

    m_valueAxis = new QValueAxis(this);
    m_valueAxis->setRange(0, 500);
    m_valueAxis->setTickInterval(100);
    m_valueAxis->setTickType(QValueAxis::TicksDynamic);

    QChart* chart = new QChart;
    chart->legend()->hide();
    chart->addAxis(m_timeAxis, Qt::AlignBottom);
    chart->addAxis(m_valueAxis, Qt::AlignLeft);
    chart->setTitle(tr("Pitch and amplitude"));
    chart->setTheme(QChart::ChartThemeBlueIcy);

    m_pitchGraph = new QColorLineSeries(this);
    m_pitchGraph->setName("graph");
    m_pitchGraph->setPen(QPen(Qt::transparent, 12));
    m_pitchGraph->setPointsVisible(true);
    chart->addSeries(m_pitchGraph);
    m_pitchGraph->attachAxis(m_timeAxis);
    m_pitchGraph->attachAxis(m_valueAxis);

    m_pointGraph = new QColorLineSeries(this);
    m_pointGraph->setName("points");
    m_pointGraph->setPen(QPen(Qt::transparent, 12));
    m_pointGraph->setPointsVisible(true);
    chart->addSeries(m_pointGraph);
    m_pointGraph->attachAxis(m_timeAxis);
    m_pointGraph->attachAxis(m_valueAxis);

    m_chartView = new ChartView(chart, this);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    QObject::connect(m_chartView, &ChartView::mouseHovered, this,
                     &SourcePlan::onSeriesHovered);
    QObject::connect(m_chartView, &ChartView::mouseLeft, this,
                     &SourcePlan::onSeriesLeft);
    QObject::connect(m_chartView, &ChartView::mouseDoubleClicked, this,
                     &SourcePlan::onSeriesDoubleClicked);
    QObject::connect(m_chartView, &ChartView::mouseRightClicked, this,
                     &SourcePlan::onSeriesRightClicked);
    QObject::connect(m_chartView, &ChartView::mousePressed, this,
                     &SourcePlan::onSeriesPressed);
    QObject::connect(m_chartView, &ChartView::mouseReleased, this,
                     &SourcePlan::onSeriesReleased);
    QObject::connect(m_chartView, &ChartView::mouseDragging, this,
                     &SourcePlan::onSeriesDragging);

    QVBoxLayout* rootLayout = new QVBoxLayout;
    rootLayout->addWidget(m_chartView);
    setLayout(rootLayout);

    m_pitch.append(QPointF(0.0, 210));
    m_amplitude.append(QPointF(0.0, 1.0));

    m_pitch.append(QPointF(2.0, 220));
    m_amplitude.append(QPointF(2.0, 1.0));

    updatePlans();
}

void SourcePlan::redrawGraph() {
    const double duration = appState->pitchPlan()->duration();

    m_timeAxis->setRange(0, duration);

    constexpr int nPoints = 400;

    QVector<QPointF> pitch(nPoints);
    QVector<qreal> amplitude(nPoints + 2);

    for (int i = 0; i < nPoints; ++i) {
        const double t = duration * i / double(nPoints - 1);

        pitch[i].setX(t);
        pitch[i].setY(appState->pitchPlan()->evaluateAtTime(t));

        amplitude[i] = appState->amplitudePlan()->evaluateAtTime(t);
    }

    amplitude[nPoints] = 0;
    amplitude[nPoints + 1] = 1;

    m_pitchGraph->replace(pitch);

    QLinearGradient gradient(QPointF(50, 0), QPointF(50, 100));
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(0.6, Qt::darkGray);
    gradient.setColorAt(1, Qt::black);

    m_pitchGraph->colorBy(amplitude, gradient);
    m_pitchGraph->sizeBy(amplitude, 0, 5);

    m_pitchGraph->replace(pitch);

    for (int i = 0; i < nPoints; ++i) {
        setGraphPointStyle(i, Normal);
    }

    m_pointGraph->replace(m_pitch);

    for (int i = 0; i < m_pitch.size(); ++i) {
        setPointStyle(i, Normal);
    }

    m_chartView->update();
}

void SourcePlan::updatePlans() {
    appState->pitchPlan()->reset(m_pitch[0].y());
    appState->amplitudePlan()->reset(m_amplitude[0].y());

    for (int i = 1; i < m_pitch.size(); ++i) {
        appState->pitchPlan()->cubicToValueAtTime(m_pitch[i].y(),
                                                  m_pitch[i].x());
        appState->amplitudePlan()->cubicToValueAtTime(m_amplitude[i].y(),
                                                      m_amplitude[i].x());
    }

    appState->updatePlans();

    redrawGraph();

    emit updated();
}

void SourcePlan::onSeriesHovered(const QString& series, const QPointF& point,
                                 int index) {
    const int nGraphPoints = m_pitchGraph->count();

    for (int i = 0; i < nGraphPoints; ++i) {
        setGraphPointStyle(i, Hover);
    }

    const int nPoints = m_pitch.size();

    for (int i = 0; i < nPoints; ++i) {
        setPointStyle(i, Hover);
    }

    if (!m_isDragging) {
        int leftPointIndex = 0;
        while (leftPointIndex < nPoints &&
               point.x() >= m_pitch[leftPointIndex].x()) {
            ++leftPointIndex;
        }

        int rightPointIndex = nPoints - 1;
        while (rightPointIndex >= 0 &&
               point.x() <= m_pitch[rightPointIndex].x()) {
            --rightPointIndex;
        }

        if (leftPointIndex > 0) --leftPointIndex;
        if (rightPointIndex < nPoints - 1) ++rightPointIndex;

        if (leftPointIndex < nPoints && rightPointIndex >= 0) {
            if (leftPointIndex <
                rightPointIndex) {  // Strictly increasing interval: hovering a
                                    // segment
                auto leftPos =
                    m_chartView->posFromPoint(m_pitch[leftPointIndex]);
                leftPos -= m_chartView->posFromPoint(point);
                if (QPointF::dotProduct(leftPos, leftPos) <= 10 * 10) {
                    return;
                }

                auto rightPos =
                    m_chartView->posFromPoint(m_pitch[rightPointIndex]);
                rightPos -= m_chartView->posFromPoint(point);
                if (QPointF::dotProduct(rightPos, rightPos) <= 10 * 10) {
                    return;
                }

                setGraphStyleBetweenPoints(leftPointIndex, rightPointIndex,
                                           HoverSegment);
            } else if (leftPointIndex ==
                       rightPointIndex) {  // Same index: hovering a point
                // Nothing.
            }
        }
    }

    m_chartView->update();
}

void SourcePlan::onSeriesLeft(const QString& series) {
    if (series != "graph") return;

    const int nGraphPoints = m_pitchGraph->count();

    for (int i = 0; i < nGraphPoints; ++i) {
        setGraphPointStyle(i, Normal);
    }

    const int nPoints = m_pitch.size();

    for (int i = 0; i < nPoints; ++i) {
        setPointStyle(i, Normal);
    }
}

void SourcePlan::onSeriesDoubleClicked(const QString& series,
                                       const QPointF& point, int index) {
    if (series != "graph") return;

    m_pitch.append(point);
    m_amplitude.append(QPointF(point.x(), 0.0));

    std::sort(m_pitch.begin(), m_pitch.end(),
              [](auto a, auto b) { return a.x() < b.x(); });
    std::sort(m_amplitude.begin(), m_amplitude.end(),
              [](auto a, auto b) { return a.x() < b.x(); });

    updatePlans();
}

void SourcePlan ::onSeriesRightClicked(const QString& series,
                                       const QPointF& point, int index) {
    if (series != "points") return;

    m_pitch.removeAt(index);
    m_amplitude.removeAt(index);

    updatePlans();
}

void SourcePlan::onSeriesPressed(const QString& series, const QPointF& point,
                                 int index) {
    if (!m_isDragging) {
        if (series == "points") {
            setPointStyle(index, Drag);

            m_isDragging = true;
            m_isDraggingSegment = false;
            m_firstPointIndexBeingDragged = index;
        } else if (series == "graph") {
            const int nPoints = m_pitch.size();

            int leftPointIndex = 0;
            while (leftPointIndex < nPoints &&
                   point.x() >= m_pitch[leftPointIndex].x()) {
                ++leftPointIndex;
            }

            int rightPointIndex = nPoints - 1;
            while (rightPointIndex >= 0 &&
                   point.x() <= m_pitch[rightPointIndex].x()) {
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
                        m_chartView->posFromPoint(m_pitch[leftPointIndex]);
                    leftPos -= m_chartView->posFromPoint(point);
                    if (QPointF::dotProduct(leftPos, leftPos) <= 4 * 4) {
                        return;
                    }

                    auto rightPos =
                        m_chartView->posFromPoint(m_pitch[rightPointIndex]);
                    rightPos -= m_chartView->posFromPoint(point);
                    if (QPointF::dotProduct(rightPos, rightPos) <= 4 * 4) {
                        return;
                    }

                    setPointStyle(leftPointIndex, Drag);
                    setPointStyle(rightPointIndex, Drag);
                    setGraphStyleBetweenPoints(leftPointIndex, rightPointIndex,
                                               Drag);

                    m_isDragging = true;
                    m_isDraggingSegment = true;
                    m_firstPointIndexBeingDragged = leftPointIndex;
                    m_secondPointIndexBeingDragged = rightPointIndex;
                    m_dragPointOriginY = point.y();
                    m_firstPointOriginY = m_pitch[leftPointIndex].y();
                    m_secondPointOriginY = m_pitch[rightPointIndex].y();
                }
            }
        }
    }
}

void SourcePlan::onSeriesReleased(const QString& series) {
    if (m_isDragging) {
        setPointStyle(m_firstPointIndexBeingDragged, Normal);

        if (m_isDraggingSegment) {
            setPointStyle(m_secondPointIndexBeingDragged, Normal);
            setGraphStyleBetweenPoints(m_firstPointIndexBeingDragged,
                                       m_secondPointIndexBeingDragged, Normal);
        }

        m_isDragging = false;
    }
}

void SourcePlan::onSeriesDragging(const QString& series, QPointF point) {
    if (m_isDragging) {
        if (series == "points") {
            point.setX(std::max(point.x(), 0.001));

            // Don't finish this dragging event if there is already a point with
            // that x position.
            if (std::find_if(m_pitch.begin(), m_pitch.end(), [point](auto p) {
                    return qFuzzyCompare(p.x(), point.x());
                }) == m_pitch.end()) {
                m_pitch[m_firstPointIndexBeingDragged] = point;
                m_amplitude[m_firstPointIndexBeingDragged].setX(point.x());

                if (m_firstPointIndexBeingDragged == 0) {
                    m_pitch[0].setX(0);
                } else {
                    if (m_pitch[m_firstPointIndexBeingDragged].x() <= 0) {
                        m_pitch[m_firstPointIndexBeingDragged].setX(1e-3);
                    }

                    std::vector<int> idx(m_pitch.size());
                    std::iota(idx.begin(), idx.end(), 0);

                    std::stable_sort(idx.begin(), idx.end(),
                                     [this](int i, int j) {
                                         return m_pitch[i].x() < m_pitch[j].x();
                                     });

                    m_firstPointIndexBeingDragged =
                        idx[m_firstPointIndexBeingDragged];

                    QList<QPointF> newPitch;
                    QList<QPointF> newAmplitude;

                    for (int i = 0; i < idx.size(); ++i) {
                        newPitch.append(m_pitch[idx[i]]);
                        newAmplitude.append(m_amplitude[idx[i]]);
                    }

                    m_pitch = newPitch;
                    m_amplitude = newAmplitude;
                }
            }
        } else if (series == "graph" && m_isDraggingSegment) {
            double dy = point.y() - m_dragPointOriginY;

            m_pitch[m_firstPointIndexBeingDragged].ry() =
                m_firstPointOriginY + dy;
            m_pitch[m_secondPointIndexBeingDragged].ry() =
                m_secondPointOriginY + dy;
        }

        updatePlans();

        const int nGraphPoints = m_pitchGraph->count();

        for (int i = 0; i < nGraphPoints; ++i) {
            setGraphPointStyle(i, Hover);
        }

        const int nPoints = m_pitch.size();

        for (int i = 0; i < nPoints; ++i) {
            setPointStyle(i, Hover);
        }

        setPointStyle(m_firstPointIndexBeingDragged, Drag);

        if (m_isDraggingSegment) {
            setPointStyle(m_secondPointIndexBeingDragged, Drag);
            setGraphStyleBetweenPoints(m_firstPointIndexBeingDragged,
                                       m_secondPointIndexBeingDragged, Drag);
        }
    }
}

void SourcePlan::setPointStyle(int index, PointStyle style) {
    qreal size;
    QColor color;

    switch (style) {
        case Normal:
            size = 9;
            color = QColor(96, 96, 96);
            break;
        case Hover:
            size = 10;
            color = QColor(72, 72, 72);
            break;
        case Drag:
            size = 12;
            color = QColor(48, 48, 48);
            break;
        case HoverSegment:
            throw std::invalid_argument(
                "HoverSegment style is not applicable to point series");
    }

    m_pointGraph->setPointConfiguration(
        index, QColorXYSeries::PointConfiguration::Size, size);
    m_pointGraph->setPointConfiguration(
        index, QColorXYSeries::PointConfiguration::Color, color);
}

void SourcePlan::setGraphPointStyle(int index, PointStyle style) {
    /*qreal size;

    switch (style) {
        case Normal:
            size = 5;
            break;
        case Hover:
            size = 6;
            break;
        case HoverSegment:
            size = 7;
            break;
        case Drag:
            size = 8;
            break;
    }

    m_pitchGraph->setPointConfiguration(
        index, QColorXYSeries::PointConfiguration::Size, size);*/
}

void SourcePlan::setGraphStyleBetweenPoints(int left, int right,
                                            PointStyle style) {
    QVector<QPointF> graph = m_pitchGraph->pointsVector();

    int leftGraph = std::distance(
        graph.begin(),
        std::upper_bound(graph.begin(), graph.end(), m_pitch[left],
                         [](auto a, auto b) { return a.x() < b.x(); }));

    int rightGraph = std::distance(
        graph.begin(),
        std::lower_bound(graph.begin(), graph.end(), m_pitch[right],
                         [](auto a, auto b) { return a.x() < b.x(); }));

    for (int i = leftGraph; i <= rightGraph; ++i) {
        setGraphPointStyle(i, style);
    }
}
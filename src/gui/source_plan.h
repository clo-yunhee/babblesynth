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

#ifndef BABBLESYNTH_SOURCE_PLAN_H
#define BABBLESYNTH_SOURCE_PLAN_H

#include <babblesynth.h>

#include <QtCharts>
#include <QtWidgets>

#include "chart_view.h"
#include "colorplot/colorplot.h"
#include "source_plan_item.h"

namespace babblesynth {
namespace gui {

class SourcePlan : public QWidget {
    Q_OBJECT

   public:
    SourcePlan(QWidget* parent = nullptr);

   signals:
    void updated();

   public slots:
    void redrawGraph();

   private slots:
    void updatePlans();

    void onSeriesHovered(const QString& series, const QPointF& point,
                         int index);
    void onSeriesLeft(const QString& series);

    void onSeriesDoubleClicked(const QString& series, const QPointF& point,
                               int index);
    void onSeriesRightClicked(const QString& series, const QPointF& point,
                              int index);

    void onSeriesPressed(const QString& series, const QPointF& point,
                         int index);
    void onSeriesReleased(const QString& series);

    void onSeriesDragging(const QString& series, QPointF point);

    void onSeriesScrolled(const QString& series, const QPointF& point,
                          int index, double dy);

   private:
    enum PointStyle { Normal, Hover, Drag, HoverSegment };

    void setPointStyle(int index, PointStyle style);
    void setGraphPointStyle(int index, PointStyle style);

    void setGraphStyleBetweenPoints(int left, int right, PointStyle style);

    QList<QPointF> m_pitch;
    QList<QPointF> m_amplitude;

    QColorLineSeries* m_pitchGraph;
    QColorLineSeries* m_pointGraph;
    QValueAxis* m_timeAxis;
    QLogValueAxis* m_valueAxis;

    ChartView* m_chartView;

    bool m_isDragging;
    bool m_isDraggingSegment;
    int m_firstPointIndexBeingDragged;
    int m_secondPointIndexBeingDragged;
    double m_dragPointOriginY;
    double m_firstPointOriginY;
    double m_secondPointOriginY;
};

}  // namespace gui
}  // namespace babblesynth

#endif  // BABBLESYNTH_SOURCE_PLAN_H
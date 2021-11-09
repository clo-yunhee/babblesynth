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

#ifndef BABBLESYNTH_FILTER_TRACKS_H
#define BABBLESYNTH_FILTER_TRACKS_H

#include <babblesynth.h>

#include <QtCharts>
#include <QtWidgets>

#include "chart_view.h"
#include "colorplot/colorplot.h"

namespace babblesynth {
namespace gui {

class FilterTracks : public QWidget {
    Q_OBJECT

   public:
    FilterTracks(int nFormants, QWidget* parent = nullptr);

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

   private:
    enum PointStyle { Normal, Hover, Drag, HoverSegment };

    void setPointStyle(int formant, int index, PointStyle style);
    void setGraphPointStyle(int formant, int index, PointStyle style);

    void setGraphStyleBetweenPoints(int formant, int left, int right,
                                    PointStyle style);

    QVector<QList<QPointF>> m_points;

    int m_nFormants;
    QVector<QColorLineSeries*> m_formantGraph;
    QVector<QColorLineSeries*> m_formantPoints;
    QColorLineSeries* m_pitchGraph;
    QValueAxis* m_timeAxis;
    QLogValueAxis* m_valueAxis;

    ChartView* m_chartView;

    bool m_isDragging;
    int m_formantBeingDragged;

    bool m_isDraggingPoint;
    int m_pointIndexBeingDragged;
    int m_secondPointIndexBeingDragged;
    double m_dragPointOriginY;
    double m_firstPointOriginY;
    double m_secondPointOriginY;
};

}  // namespace gui
}  // namespace babblesynth

#endif  // BABBLESYNTH_FILTER_TRACKS_H
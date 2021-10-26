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

#include <QtWidgets>
#include <QtCharts>
#include <babblesynth.h>

#include "colorplot/colorplot.h"

namespace babblesynth {
namespace gui {
    
class FilterTracks : public QWidget {
    Q_OBJECT
    
public:
    FilterTracks(int nFormants, QWidget *parent = nullptr);

private slots:
    void updatePlans();

public slots:
    void redrawGraph();

private:
    void newPoint(int n);

    QVector<QList<QPointF>> m_points;

    int m_nFormants;
    QVector<QColorLineSeries *> m_formantGraph;
    QColorLineSeries *m_pitchGraph;
    QValueAxis *m_timeAxis;

    QChartView *m_chartView;
};

}
}

#endif // BABBLESYNTH_FILTER_TRACKS_H
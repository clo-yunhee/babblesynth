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

#include <QtWidgets>
#include <QtCharts>
#include <babblesynth.h>

#include "colorplot/colorplot.h"
#include "source_plan_item.h"

namespace babblesynth {
namespace gui {
    
class SourcePlan : public QWidget {
    Q_OBJECT
    
public:
    SourcePlan(QWidget *parent = nullptr);

signals:
    void updated();

private slots:
    void newPlanItem();
    void removePlanItem(SourcePlanItem *item);
    void updatePlans();

private:
    void initialPlanItem();
    void redrawGraph();

    QVBoxLayout *m_segments;
    SourcePlanItem *m_initialPlanItem;
    QList<SourcePlanItem *> m_planItems;

    QColorLineSeries *m_pitchGraph;
    QValueAxis *m_timeAxis;

    QChartView *m_chartView;
};

}
}

#endif // BABBLESYNTH_SOURCE_PLAN_H
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


#ifndef BABBLESYNTH_SOURCE_PLAN_ITEM_H
#define BABBLESYNTH_SOURCE_PLAN_ITEM_H

#include <QtWidgets>
#include <babblesynth.h>

namespace babblesynth {
namespace gui {

class SourcePlanItem : public QFrame {
    Q_OBJECT
    
public:
    SourcePlanItem(int duration,
                   variable_plan::transition pitchTransition,
                   double targetPitch,
                   variable_plan::transition amplitudeTransiiton,
                   double targetAmplitude,
                   QWidget *parent = nullptr);

    SourcePlanItem(double initialPitch, double initialAmplitude, QWidget *parent = nullptr);

    void updatePlans();

    double duration() const;
    double pitch() const;
    double amplitude() const;

signals:
    void updated();
    void requestRemove(SourcePlanItem *item);

private:
    double m_duration;

    variable_plan::transition m_pitchTransition;
    double m_targetPitch;

    variable_plan::transition m_amplitudeTransition;
    double m_targetAmplitude;
};

}
}

#endif // BABBLESYNTH_SOURCE_PLAN_ITEM_H
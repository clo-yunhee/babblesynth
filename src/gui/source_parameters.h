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


#ifndef BABBLESYNTH_SOURCE_PARAMETERS_H
#define BABBLESYNTH_SOURCE_PARAMETERS_H

#include <QtWidgets>
#include <QtCharts>
#include <babblesynth.h>

namespace babblesynth {
namespace gui {
    
class SourceParameters : public QWidget {
    Q_OBJECT
    
public:
    SourceParameters(QWidget *parent = nullptr);

private slots:
    void onSourceTypeChanged(const QString&);

private:
    void updateFields();
    void addField(parameter& param);
    void redrawGraph();

    QFormLayout *m_sourceParams;
    QLineSeries *m_sourceGraph;
};

}
}

#endif // BABBLESYNTH_SOURCE_PARAMETERS_H
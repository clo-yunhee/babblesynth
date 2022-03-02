/*
 * BabbleSynth
 * Copyright (C) 2022  Clo Yun-Hee Dufour
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

#include <babblesynth.h>

#include <QFormLayout>
#include <QLineSeries>
#include <QPointF>
#include <QSplineSeries>
#include <QVector>
#include <QWidget>

namespace babblesynth {
namespace gui {

using namespace QtCharts;

class SourceParameters : public QWidget {
    Q_OBJECT

   public:
    SourceParameters(QWidget *parent = nullptr);

   private slots:
    void onSourceTypeChanged(const QString &);

   private:
    void updateFields();
    void addField(parameter &param);
    void redrawGraph();

    QVector<QPointF> calculateSpectrum(int fs, int f0, int nfft);

    QFormLayout *m_sourceParams;
    QSplineSeries *m_sourceGraph;

    QLineSeries *m_spectrum;
};

}  // namespace gui
}  // namespace babblesynth

#endif  // BABBLESYNTH_SOURCE_PARAMETERS_H
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
#include "clickable_label.h"
#include "app_state.h"
#include "qlayoutitem.h"
#include "qnamespace.h"
#include "qpoint.h"

using namespace babblesynth::gui;

FilterTracks::FilterTracks(int nFormants, QWidget *parent)
    : QWidget(parent),
      m_nFormants(nFormants)
{
    setObjectName("FilterTracks");
    setWindowTitle(tr("Filter tracks"));
    setMinimumSize(QSize(1024, 768));

    m_timeAxis = new QValueAxis(this);
    m_timeAxis->setRange(0, 1);

    QValueAxis *valueAxis = new QValueAxis(this);
    valueAxis->setRange(50, 5000);

    QChart *chart = new QChart;
    chart->legend()->hide();
    chart->addAxis(m_timeAxis, Qt::AlignBottom);
    chart->addAxis(valueAxis, Qt::AlignLeft);
    chart->setTitle(tr("Pitch and formants"));
    chart->setTheme(QChart::ChartThemeBlueIcy);

    m_formantGraph.resize(nFormants);
    for (int n = 0; n < nFormants; ++n) {
        QColorLineSeries *graph = new QColorLineSeries(this);
        graph->setColor(Qt::transparent);
        graph->setPointsVisible(true);
        chart->addSeries(graph);
        graph->attachAxis(m_timeAxis);
        graph->attachAxis(valueAxis);
        m_formantGraph[n] = graph;
    }

    m_pitchGraph = new QColorLineSeries(this);
    m_pitchGraph->setColor(Qt::transparent);
    m_pitchGraph->setPointsVisible(true);
    chart->addSeries(m_pitchGraph);
    m_pitchGraph->attachAxis(m_timeAxis);
    m_pitchGraph->attachAxis(valueAxis);

    m_chartView = new QChartView(chart, this);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    QHBoxLayout *addButtons = new QHBoxLayout;

    addButtons->addStretch();
    for (int n = 0; n < nFormants; ++n) {
        QToolButton *addButton = new QToolButton(this);
        addButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        addButton->setText(QString("  F%1").arg(n + 1));
        addButton->setIcon(QIcon(":/icons/plus.png"));

        QObject::connect(addButton, &QToolButton::pressed,
                         this, [this, n]() { newPoint(n); });
        
        addButtons->addWidget(addButton, 1);
        addButtons->addSpacing(2);
    }
    addButtons->addStretch();

    QVBoxLayout *rootLayout = new QVBoxLayout;
    rootLayout->addWidget(m_chartView, 1);
    rootLayout->addLayout(addButtons);
    setLayout(rootLayout);

    std::array freqsA {1000, 1800, 2900, 4200, 4950};
    std::array freqsE {900, 2200, 3100, 4200, 4950};
    std::array freqsI {550, 2700, 3200, 4100, 4950};
    std::array freqsO {700, 1200, 2950, 4100, 4950};
    std::array freqsU {500, 900, 2850, 4000, 4950};

    m_points.resize(nFormants);
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

void FilterTracks::newPoint(int n)
{
    const double duration = appState->pitchPlan()->duration();

    QPointF last = m_points[n].back();

    if (!qFuzzyCompare(last.x(), duration)) {
        const int i = m_points[n].size() - 2;
        m_points[n].insert(i, { (m_points[n][i].x() + duration) / 2, m_points[n][i].y() });
    }
    else {
        m_points[n].append({ (last.x() + duration) / 2, last.y() });
    }

    updatePlans();
}

void FilterTracks::updatePlans()
{
    for (int n = 0; n < m_nFormants; ++n) {
        appState->formantPlan(n)->reset(m_points[n][0].y());

        for (int i = 1; i < m_points[n].size(); ++i) {
            appState->formantPlan(n)->cubicToValueAtTime(m_points[n][i].y(), m_points[n][i].x());
        }
    }

    appState->updatePlans();

    redrawGraph();
}

void FilterTracks::redrawGraph()
{
    const double duration = appState->pitchPlan()->duration();

    m_timeAxis->setRange(0, duration);
    
    constexpr int nPoints = 600;

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

    QLinearGradient pitchGradient(QPointF(0, 0), QPointF(0, 100));
    pitchGradient.setColorAt(0, Qt::white);
    pitchGradient.setColorAt(0.6, Qt::darkGray);
    pitchGradient.setColorAt(1, Qt::black);

    m_pitchGraph->colorBy(amplitude, pitchGradient);
    m_pitchGraph->sizeBy(amplitude, 0, 5);

    QLinearGradient formantGradient(QPointF(0, 0), QPointF(0, 100));
    formantGradient.setColorAt(0, QColor::fromHsv(0, 0, 204));
    formantGradient.setColorAt(1, QColor::fromHsv(39, 255, 255));

    for (int n = 0; n < m_nFormants; ++n) {
        m_formantGraph[n]->replace(formants[n]);
        m_formantGraph[n]->colorBy(amplitude, formantGradient);
        m_formantGraph[n]->sizeBy(amplitude, 0, 5);

        for (const QPointF point : m_points[n]) {
            const int index = std::round((nPoints - 1) * point.x() / duration);
            m_formantGraph[n]->setPointConfiguration(index, QColorXYSeries::PointConfiguration::Size, 8);
        }
    }

    m_chartView->update();
}
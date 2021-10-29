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
#include "clickable_label.h"
#include "app_state.h"

using namespace babblesynth::gui;

SourcePlan::SourcePlan(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("SourcePlan");
    setWindowTitle(tr("Source plan"));
    setMinimumSize(QSize(1024, 768));

    QToolButton *addButton = new QToolButton(this);
    addButton->setIcon(QIcon(":/icons/plus.png"));

    QObject::connect(addButton, &QToolButton::pressed, this, &SourcePlan::newPlanItem);

    m_pitchGraph = new QColorLineSeries(this);
    m_pitchGraph->setColor(Qt::transparent);
    m_pitchGraph->setPointsVisible(true);

    m_timeAxis = new QValueAxis(this);
    m_timeAxis->setRange(0, 1);

    QValueAxis *valueAxis = new QValueAxis(this);
    valueAxis->setRange(0, 500);
    valueAxis->setTickInterval(100);
    valueAxis->setTickType(QValueAxis::TicksDynamic);

    QChart *chart = new QChart;
    chart->legend()->hide();
    chart->addSeries(m_pitchGraph);
    chart->addAxis(m_timeAxis, Qt::AlignBottom);
    chart->addAxis(valueAxis, Qt::AlignLeft);
    chart->setTitle(tr("Pitch and amplitude"));
    chart->setTheme(QChart::ChartThemeBlueIcy);
    
    m_pitchGraph->attachAxis(m_timeAxis);
    m_pitchGraph->attachAxis(valueAxis);

    m_chartView = new QChartView(chart, this);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    scrollArea->setWidgetResizable(true);
    scrollArea->setSizeAdjustPolicy(QScrollArea::AdjustToContentsOnFirstShow);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    QWidget *segmentsWidget = new QWidget;
    m_segments = new QVBoxLayout;
    m_segments->addStretch();
    segmentsWidget->setLayout(m_segments);
    scrollArea->setWidget(segmentsWidget);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(addButton, 0, Qt::AlignHCenter);
    leftLayout->addWidget(scrollArea, 1);
    
    QHBoxLayout *rootLayout = new QHBoxLayout;
    rootLayout->addLayout(leftLayout);
    rootLayout->addWidget(m_chartView, 1);
    setLayout(rootLayout);

    initialPlanItem();
    newPlanItem();
}

void SourcePlan::initialPlanItem()
{
    m_initialPlanItem = new SourcePlanItem(220, 1, this);

    QObject::connect(m_initialPlanItem, &SourcePlanItem::updated, this, &SourcePlan::updatePlans);

    m_segments->insertWidget(0, m_initialPlanItem);

    updatePlans();
}

void SourcePlan::newPlanItem()
{
    double duration;
    double pitch;
    double amplitude;

    if (m_planItems.empty()) {
        duration = 2000;
        pitch = 220;
        amplitude = 0.8;
    }
    else {
        duration = 400;
        pitch = m_planItems.back()->pitch();
        amplitude = m_planItems.back()->amplitude();
    }

    SourcePlanItem *item = new SourcePlanItem(duration,
                                              variable_plan::TransitionCubic, pitch,
                                              variable_plan::TransitionCubic, amplitude,
                                              this);

    QObject::connect(item, &SourcePlanItem::updated, this, &SourcePlan::updatePlans);
    QObject::connect(item, &SourcePlanItem::requestRemove, this, &SourcePlan::removePlanItem);

    m_segments->insertWidget(m_segments->count() - 1, item);
    m_planItems.append(item);

    updatePlans();
}

void SourcePlan::removePlanItem(SourcePlanItem *item)
{
    m_segments->removeWidget(item);
    m_planItems.removeAll(item);
    item->deleteLater();

    updatePlans();
}

void SourcePlan::updatePlans()
{
    appState->pitchPlan()->reset(m_initialPlanItem->pitch());
    appState->amplitudePlan()->reset(m_initialPlanItem->amplitude());

    for (SourcePlanItem *item : m_planItems) {
        item->updatePlans();
    }

    appState->updatePlans();

    redrawGraph();

    emit updated();
}

void SourcePlan::redrawGraph()
{
    const double duration = appState->pitchPlan()->duration();

    m_timeAxis->setRange(0, duration);
    
    constexpr int nPoints = 600;

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

    double t = 0;

    m_pitchGraph->setPointConfiguration(0, QColorXYSeries::PointConfiguration::Size, 8);

    for (const auto& item : m_planItems) {
        t += item->duration();

        const int index = std::round((nPoints - 1) * t / duration);
        
        m_pitchGraph->setPointConfiguration(index, QColorXYSeries::PointConfiguration::Size, 8);
    }

    m_chartView->update();
}
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

#include "undertale.h"

#include <QGroupBox>
#include <QSlider>
#include <QVBoxLayout>
#include <random>

#include "../app_state.h"
#include "frequency_scale.h"

using namespace babblesynth::gui::voicefx;

Undertale::Undertale()
    : m_isDraggingGraph(false), m_pauseRatio(0), m_F1(700), m_F2(1200) {
    QGroupBox *pitchBox = new QGroupBox(tr("Pitch"));
    {
        QSlider *pitchSlider = new QSlider(Qt::Horizontal);
        pitchSlider->setRange(hzToMel(50), hzToMel(500));

        m_pitchLabel = new QLabel("800 Hz");
        m_pitchLabel->setMinimumWidth(m_pitchLabel->sizeHint().width());
        m_pitchLabel->setAlignment(Qt::AlignRight);

        connect(pitchSlider, &QSlider::valueChanged, this,
                &Undertale::handlePitchChanged);

        pitchSlider->setValue(hzToMel(180));

        QHBoxLayout *pitchLayout = new QHBoxLayout;
        pitchLayout->addWidget(pitchSlider);
        pitchLayout->addWidget(m_pitchLabel);
        pitchBox->setLayout(pitchLayout);
    }

    QGroupBox *durationBox = new QGroupBox(tr("Duration"));
    {
        QSlider *durationSlider = new QSlider(Qt::Horizontal);
        durationSlider->setRange(50, 500);

        m_durationLabel = new QLabel("500 ms");
        m_durationLabel->setMinimumWidth(m_durationLabel->sizeHint().width());
        m_durationLabel->setAlignment(Qt::AlignRight);

        connect(durationSlider, &QSlider::valueChanged, this,
                &Undertale::handleDurationChanged);

        durationSlider->setValue(65);

        QHBoxLayout *durationLayout = new QHBoxLayout;
        durationLayout->addWidget(durationSlider);
        durationLayout->addWidget(m_durationLabel);
        durationBox->setLayout(durationLayout);
    }

    QGroupBox *pauseBox = new QGroupBox(tr("Pause ratio"));
    {
        QSlider *pauseSlider = new QSlider(Qt::Horizontal);
        pauseSlider->setRange(0, 50);

        m_pauseLabel = new QLabel("50 %");
        m_pauseLabel->setMinimumWidth(m_pauseLabel->sizeHint().width());
        m_pauseLabel->setAlignment(Qt::AlignRight);

        connect(pauseSlider, &QSlider::valueChanged, this,
                &Undertale::handlePauseChanged);

        m_pauseLabel->setText("0 %");
        pauseSlider->setValue(0);

        QHBoxLayout *pauseLayout = new QHBoxLayout;
        pauseLayout->addWidget(pauseSlider);
        pauseLayout->addWidget(m_pauseLabel);
        pauseBox->setLayout(pauseLayout);
    }

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(pitchBox);
    hLayout->addWidget(durationBox);
    hLayout->addWidget(pauseBox);
    hLayout->addStretch();

    QCustomPlot *customPlot = new QCustomPlot(this);

    QObject::connect(customPlot, &QCustomPlot::mousePress, this,
                     &Undertale::handlePlotPress);
    QObject::connect(customPlot, &QCustomPlot::mouseRelease, this,
                     &Undertale::handlePlotRelease);
    QObject::connect(customPlot, &QCustomPlot::mouseMove, this,
                     &Undertale::handlePlotMove);

    // Styling.
    QColor colorBg = QColor::fromRgb(0x19232D);
    QColor colorFg = QColor::fromRgb(0xE0E1E3);
    customPlot->xAxis->setBasePen(QPen(colorFg, 1));
    customPlot->yAxis->setBasePen(QPen(colorFg, 1));
    customPlot->xAxis->setTickPen(QPen(colorFg, 1));
    customPlot->yAxis->setTickPen(QPen(colorFg, 1));
    customPlot->xAxis->setSubTickPen(QPen(colorFg, 1));
    customPlot->yAxis->setSubTickPen(QPen(colorFg, 1));
    customPlot->xAxis->setTickLabelColor(colorFg);
    customPlot->yAxis->setTickLabelColor(colorFg);
    customPlot->xAxis->setLabelColor(colorFg);
    customPlot->yAxis->setLabelColor(colorFg);
    customPlot->setBackground(colorBg);
    customPlot->axisRect()->setBackground(colorBg);
    customPlot->setSizePolicy(QSizePolicy::MinimumExpanding,
                              QSizePolicy::Expanding);

    // set title of plot:
    customPlot->plotLayout()->insertRow(0);
    QFont boldFont(font());
    boldFont.setBold(true);
    auto plotTitle =
        new QCPTextElement(customPlot, tr("Vowel color"), boldFont);
    plotTitle->setTextColor(colorFg);
    customPlot->plotLayout()->addElement(0, 0, plotTitle);

    QCPGraph *F1F2graph = customPlot->addGraph();
    F1F2graph->setPen(QPen(colorFg, 1.5));
    F1F2graph->addData(0, 0);
    F1F2graph->addData(10000, 10000);

    m_formantGraph = customPlot->addGraph();
    m_formantGraph->setScatterStyle(
        QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(colorFg, 1.5),
                        QColor::fromRgb(0x455364), 14));
    m_formantGraph->addData(m_F1, m_F2);

    customPlot->xAxis->setLabel("F1 (Hz)");
    customPlot->yAxis->setLabel("F2 (Hz)");

    customPlot->rescaleAxes();
    customPlot->xAxis->setRange(300, 1300);
    customPlot->yAxis->setRange(500, 3000);

    QHBoxLayout *hLayout2 = new QHBoxLayout;
    hLayout2->addStretch(1);
    hLayout2->addWidget(customPlot, 4);
    hLayout2->addStretch(1);

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addLayout(hLayout);
    vLayout->addLayout(hLayout2, 1);

    setLayout(vLayout);
}

Undertale::~Undertale() {}

void Undertale::updateDialogueTextChanged(const QString &text) {
    m_chars.clear();
    m_chars.reserve(text.length());

    for (int i = 0; i < text.length(); ++i) {
        const auto &ch = text.at(i);

        if (ch.isPrint()) {
            if (ch.isLetterOrNumber()) {
                m_chars.push_back(CharTypeLetter);
            } else if (ch.isSpace()) {
                m_chars.push_back(CharTypeSpace);
            } else {
                m_chars.push_back(CharTypePunctuation);
            }
        }
    }

    m_chars.shrink_to_fit();

    updatePlans();
}

void Undertale::handlePitchChanged(int value) {
    m_pitch = melToHz(value);
    m_pitchLabel->setText(QString("%1 Hz").arg(m_pitch, 0, 'f', 1));

    updatePlans();
}

void Undertale::handleDurationChanged(int value) {
    m_duration = double(value) / 1000.0;
    m_durationLabel->setText(QString("%1 ms").arg(value));

    updatePlans();
}

void Undertale::handlePauseChanged(int value) {
    m_pauseRatio = double(value) / 100.0;
    m_pauseLabel->setText(QString("%1 %").arg(value));

    updatePlans();
}

void Undertale::handlePlotPress(QMouseEvent *event) {
    m_isDraggingGraph = true;
    checkAndSetFormants(event->pos());
}

void Undertale::handlePlotRelease(QMouseEvent *event) {
    if (m_isDraggingGraph) {
        m_isDraggingGraph = false;
        checkAndSetFormants(event->pos());
    }
}

void Undertale::handlePlotMove(QMouseEvent *event) {
    if (m_isDraggingGraph) {
        checkAndSetFormants(event->pos());
    }
}

void Undertale::checkAndSetFormants(const QPoint &pos) {
    double F1, F2;
    m_formantGraph->pixelsToCoords(pos.x(), pos.y(), F1, F2);

    const auto &rangeF1 = m_formantGraph->keyAxis()->range();
    const auto &rangeF2 = m_formantGraph->valueAxis()->range();

    F1 = std::clamp(F1, rangeF1.lower, rangeF1.upper);
    F2 = std::clamp(F2, rangeF2.lower, rangeF2.upper);

    // Make sure F2 > F1
    F2 = std::max(F1, F2);

    m_F1 = F1;
    m_F2 = F2;

    m_formantGraph->setData({m_F1}, {m_F2});
    m_formantGraph->parentPlot()->replot();

    updatePlans();
}

void Undertale::updatePlans() {
    /*qDebug() << "Rendering Undertale-type dialogue with parameters:";
    qDebug() << "- pitch: " << m_pitch;
    qDebug() << "- duration: " << m_duration;
    qDebug() << "- pause ratio: " << m_pauseRatio;
    qDebug() << "- F1: " << m_F1;
    qDebug() << "- F2: " << m_F2;*/

    appState->pitchPlan()->reset(m_pitch);
    appState->amplitudePlan()->reset(0);

    double time = 0.1;

    appState->amplitudePlan()->linearToValueAtTime(0, time);

    for (const auto &ch : m_chars) {
        if (ch == CharTypeLetter) {
            appState->amplitudePlan()->linearToValueAtTime(
                1, time + 15.0 / 1000.0);
            time += m_duration;
            appState->amplitudePlan()->linearToValueAtTime(1,
                                                           time - 5.0 / 1000.0);
            appState->amplitudePlan()->linearToValueAtTime(0, time);
            if (!qFuzzyIsNull(m_pauseRatio)) {
                time += m_pauseRatio * m_duration;
                appState->amplitudePlan()->linearToValueAtTime(0, time);
            }
        } else if (ch == CharTypeSpace) {
            time += 0.4 * m_duration;
            appState->amplitudePlan()->linearToValueAtTime(0, time);
        } else if (ch == CharTypePunctuation) {
            time += 0.6 * m_duration;
            appState->amplitudePlan()->linearToValueAtTime(0, time);
        }
    }

    appState->pitchPlan()->linearToValueAtTime(m_pitch, time);

    // const double factor = (m_pitch - 50) / 450;
    // const double F1 = m_F1 + factor * 600;
    // const double F2 = m_F2 + factor * 900;
    // const double F1 = 230 + factor * 600;
    // const double F2 = 3000 + factor * 900;

    const double F1 = m_F1;
    const double F2 = m_F2;

    appState->formantFrequencyPlan(0)->reset(F1);
    appState->formantFrequencyPlan(0)->linearToValueAtTime(F1, time);
    appState->formantFrequencyPlan(1)->reset(F2);
    appState->formantFrequencyPlan(1)->linearToValueAtTime(F2, time);

    appState->updatePlans();
}
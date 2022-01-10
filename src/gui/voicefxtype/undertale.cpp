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

#include "undertale.h"

#include <QGroupBox>
#include <QSlider>
#include <QVBoxLayout>
#include <random>

#include "../app_state.h"
#include "frequency_scale.h"

using namespace babblesynth::gui::voicefx;

Undertale::Undertale() {
    QGroupBox *pitchBox = new QGroupBox("Pitch");
    {
        QSlider *pitchSlider = new QSlider(Qt::Horizontal);
        pitchSlider->setRange(hzToMel(50), hzToMel(500));

        m_pitchLabel = new QLabel("800 Hz");
        m_pitchLabel->setMinimumWidth(m_pitchLabel->sizeHint().width());
        m_pitchLabel->setAlignment(Qt::AlignRight);

        connect(pitchSlider, &QSlider::valueChanged, this,
                &Undertale::handlePitchChanged);

        pitchSlider->setValue(hzToMel(120));

        QHBoxLayout *pitchLayout = new QHBoxLayout;
        pitchLayout->addWidget(pitchSlider);
        pitchLayout->addWidget(m_pitchLabel);
        pitchBox->setLayout(pitchLayout);
    }

    QGroupBox *durationBox = new QGroupBox("Duration");
    {
        QSlider *durationSlider = new QSlider(Qt::Horizontal);
        durationSlider->setRange(50, 500);

        m_durationLabel = new QLabel("500 ms");
        m_durationLabel->setMinimumWidth(m_durationLabel->sizeHint().width());
        m_durationLabel->setAlignment(Qt::AlignRight);

        connect(durationSlider, &QSlider::valueChanged, this,
                &Undertale::handleDurationChanged);

        durationSlider->setValue(90);

        QHBoxLayout *durationLayout = new QHBoxLayout;
        durationLayout->addWidget(durationSlider);
        durationLayout->addWidget(m_durationLabel);
        durationBox->setLayout(durationLayout);
    }

    QGroupBox *pauseBox = new QGroupBox("Pause ratio");
    {
        QSlider *pauseSlider = new QSlider(Qt::Horizontal);
        pauseSlider->setRange(0, 50);

        m_pauseLabel = new QLabel("50 %");
        m_pauseLabel->setMinimumWidth(m_pauseLabel->sizeHint().width());
        m_pauseLabel->setAlignment(Qt::AlignRight);

        connect(pauseSlider, &QSlider::valueChanged, this,
                &Undertale::handlePauseChanged);

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

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addLayout(hLayout);
    vLayout->addStretch();

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

void Undertale::updatePlans() {
    appState->pitchPlan()->reset(m_pitch);
    appState->amplitudePlan()->reset(0);

    double time = 0.1;

    appState->amplitudePlan()->linearToValueAtTime(0, time);

    for (const auto &ch : m_chars) {
        if (ch == CharTypeLetter) {
            time += m_duration;
            appState->amplitudePlan()->linearToValueAtTime(1,
                                                           time - 2.0 / 1000.0);
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

    const double factor = (m_pitch - 50) / 450;
    const double F1 = 700 + factor * 600;
    const double F2 = 1200 + factor * 900;

    appState->formantPlan(0)->reset(F1);
    appState->formantPlan(0)->linearToValueAtTime(F1, time);
    appState->formantPlan(1)->reset(F2);
    appState->formantPlan(1)->linearToValueAtTime(F2, time);

    appState->updatePlans();
}
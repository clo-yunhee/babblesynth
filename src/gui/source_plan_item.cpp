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

#include "source_plan_item.h"

#include "app_state.h"
#include "clickable_label.h"

using namespace babblesynth::gui;

SourcePlanItem::SourcePlanItem(int duration,
                               variable_plan::transition pitchTransition,
                               double targetPitch,
                               variable_plan::transition amplitudeTransition,
                               double targetAmplitude, QWidget *parent)
    : QFrame(parent),
      m_duration(duration / 1000.0),
      m_pitchTransition(pitchTransition),
      m_targetPitch(targetPitch),
      m_amplitudeTransition(amplitudeTransition),
      m_targetAmplitude(targetAmplitude) {
    setObjectName("SourcePlanItem");
    setStyleSheet(
        "QFrame#SourcePlanItem { border-style: solid; border-width: 2px; "
        "border-radius: 7px; }");

    QSpinBox *durationInput = new QSpinBox(this);
    durationInput->setRange(10, 5000);
    durationInput->setStepType(QDoubleSpinBox::AdaptiveDecimalStepType);
    durationInput->setSuffix(" ms");
    durationInput->setValue(duration);

    QSpinBox *pitch = new QSpinBox(this);
    pitch->setRange(20, 800);
    pitch->setSuffix(" Hz");
    pitch->setValue(targetPitch);

    QComboBox *pitchType = new QComboBox(this);
    pitchType->addItem(tr("Step"));
    pitchType->addItem(tr("Linear"));
    pitchType->addItem(tr("Cubic"));
    pitchType->setCurrentIndex(int(pitchTransition));

    QHBoxLayout *pitchRow = new QHBoxLayout;
    pitchRow->addWidget(new QLabel(utf8("f₀")));
    pitchRow->addWidget(pitch);
    pitchRow->addWidget(pitchType);

    QSpinBox *amplitude = new QSpinBox(this);
    amplitude->setRange(0, 100);
    amplitude->setValue(targetAmplitude * 100);
    amplitude->setSuffix(" %");

    QComboBox *amplitudeType = new QComboBox(this);
    amplitudeType->addItem(tr("Step"));
    amplitudeType->addItem(tr("Linear"));
    amplitudeType->addItem(tr("Cubic"));
    amplitudeType->setCurrentIndex(int(amplitudeTransition));

    QHBoxLayout *amplitudeRow = new QHBoxLayout;
    amplitudeRow->addWidget(new QLabel(utf8("Aᵥ")));
    amplitudeRow->addWidget(amplitude);
    amplitudeRow->addWidget(amplitudeType);

    QToolButton *removeButton = new QToolButton(this);
    removeButton->setIcon(QIcon(":/icons/minus.png"));

    QFormLayout *layout = new QFormLayout(this);
    layout->addRow("T", durationInput);
    layout->addRow(pitchRow);
    layout->addRow(amplitudeRow);
    layout->addRow(removeButton);
    layout->setAlignment(removeButton, Qt::AlignRight);
    setLayout(layout);

    QObject::connect(durationInput, qOverload<int>(&QSpinBox::valueChanged),
                     this, [this](int value) {
                         m_duration = value / 1000.0;
                         emit updated();
                     });

    QObject::connect(pitch, qOverload<int>(&QSpinBox::valueChanged), this,
                     [this](int value) {
                         m_targetPitch = value;
                         emit updated();
                     });

    QObject::connect(pitchType, qOverload<int>(&QComboBox::currentIndexChanged),
                     this, [this](int value) {
                         m_pitchTransition = variable_plan::transition(value);
                         emit updated();
                     });

    QObject::connect(amplitude, qOverload<int>(&QSpinBox::valueChanged), this,
                     [this](int value) {
                         m_targetAmplitude = value / 100.0;
                         emit updated();
                     });

    QObject::connect(
        amplitudeType, qOverload<int>(&QComboBox::currentIndexChanged), this,
        [this](int value) {
            m_amplitudeTransition = variable_plan::transition(value);
            emit updated();
        });

    QObject::connect(removeButton, &QToolButton::pressed, this,
                     [this]() { emit requestRemove(this); });
}

SourcePlanItem::SourcePlanItem(double initialPitch, double initialAmplitude,
                               QWidget *parent)
    : QFrame(parent),
      m_duration(0),
      m_targetPitch(initialPitch),
      m_targetAmplitude(initialAmplitude) {
    setObjectName("SourcePlanItem");
    setStyleSheet(
        "QFrame#SourcePlanItem { border-style: solid; border-width: 2px; "
        "border-radius: 7px; }");

    QSpinBox *pitch = new QSpinBox(this);
    pitch->setRange(20, 800);
    pitch->setSuffix(" Hz");
    pitch->setValue(initialPitch);

    QSpinBox *amplitude = new QSpinBox(this);
    amplitude->setRange(0, 100);
    amplitude->setValue(initialAmplitude * 100);
    amplitude->setSuffix(" %");

    QFormLayout *layout = new QFormLayout(this);
    layout->addRow(utf8("f₀"), pitch);
    layout->addRow(utf8("Aᵥ"), amplitude);
    setLayout(layout);

    QObject::connect(pitch, qOverload<int>(&QSpinBox::valueChanged), this,
                     [this](int value) {
                         m_targetPitch = value;
                         emit updated();
                     });

    QObject::connect(amplitude, qOverload<int>(&QSpinBox::valueChanged), this,
                     [this](int value) {
                         m_targetAmplitude = value / 100.0;
                         emit updated();
                     });
}

void SourcePlanItem::updatePlans() {
    const double targetTime = appState->pitchPlan()->duration() + m_duration;

    switch (m_pitchTransition) {
        case variable_plan::TransitionStep:
            appState->pitchPlan()->stepToValueAtTime(m_targetPitch, targetTime);
            break;
        case variable_plan::TransitionLinear:
            appState->pitchPlan()->linearToValueAtTime(m_targetPitch,
                                                       targetTime);
            break;
        case variable_plan::TransitionCubic:
            appState->pitchPlan()->cubicToValueAtTime(m_targetPitch,
                                                      targetTime);
            break;
    }

    switch (m_amplitudeTransition) {
        case variable_plan::TransitionStep:
            appState->amplitudePlan()->stepToValueAtTime(m_targetAmplitude,
                                                         targetTime);
            break;
        case variable_plan::TransitionLinear:
            appState->amplitudePlan()->linearToValueAtTime(m_targetAmplitude,
                                                           targetTime);
            break;
        case variable_plan::TransitionCubic:
            appState->amplitudePlan()->cubicToValueAtTime(m_targetAmplitude,
                                                          targetTime);
            break;
    }
}

double SourcePlanItem::duration() const { return m_duration; }

double SourcePlanItem::pitch() const { return m_targetPitch; }

double SourcePlanItem::amplitude() const { return m_targetAmplitude; }
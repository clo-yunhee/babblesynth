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

#include "animal_crossing.h"

#include <QBoxLayout>
#include <QDebug>
#include <QDirIterator>
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSlider>
#include <QStandardPaths>
#include <iostream>
#include <random>

#include "../app_state.h"
#include "frequency_scale.h"

using namespace babblesynth::gui::voicefx;
using namespace xercesc;

AnimalCrossing::AnimalCrossing()
    : m_phonemeDictionary(new phonemes::PhonemeDictionary) {
    QGroupBox *pitchBox = new QGroupBox(tr("Pitch"));
    {
        QSlider *pitchSlider = new QSlider(Qt::Horizontal);
        pitchSlider->setRange(hzToMel(50), hzToMel(500));

        m_pitchLabel = new QLabel("800 Hz");
        m_pitchLabel->setMinimumWidth(m_pitchLabel->sizeHint().width());
        m_pitchLabel->setAlignment(Qt::AlignRight);

        connect(pitchSlider, &QSlider::valueChanged, this,
                &AnimalCrossing::handlePitchChanged);

        pitchSlider->setValue(hzToMel(420));

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
                &AnimalCrossing::handleDurationChanged);

        durationSlider->setValue(70);

        QHBoxLayout *durationLayout = new QHBoxLayout;
        durationLayout->addWidget(durationSlider);
        durationLayout->addWidget(m_durationLabel);
        durationBox->setLayout(durationLayout);
    }

    QGroupBox *dictionaryBox = new QGroupBox(tr("Phoneme dictionary"));
    {
        QPushButton *dictFileButton =
            new QPushButton(tr("Load dictionary file"));

        m_dictionaryFileLabel = new QLabel;

        connect(dictFileButton, &QPushButton::pressed, this,
                &AnimalCrossing::handleOpenDictionaryFile);

        QPushButton *dictEditButton = new QPushButton(tr("Edit"));

        QHBoxLayout *dictionaryLayout = new QHBoxLayout;
        dictionaryLayout->addWidget(dictFileButton);
        dictionaryLayout->addWidget(m_dictionaryFileLabel);
        dictionaryLayout->addWidget(dictEditButton);
        dictionaryBox->setLayout(dictionaryLayout);
    }

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(durationBox);
    hLayout->addWidget(pitchBox);
    hLayout->addStretch();

    QHBoxLayout *hLayout2 = new QHBoxLayout;
    hLayout2->addStretch();
    hLayout2->addWidget(dictionaryBox);
    hLayout2->addStretch();

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addLayout(hLayout);
    vLayout->addLayout(hLayout2);
    vLayout->addStretch();

    setLayout(vLayout);

    extractDictionaryFiles();
    loadDictionaryFile("./dictionaries/english.xml");
}

AnimalCrossing::~AnimalCrossing() {}

void AnimalCrossing::updateDialogueTextChanged(const QString &text) {
    m_textBytes = text.toLocal8Bit();

    updatePhonemes();
    updatePlans();
}

void AnimalCrossing::handleOpenDictionaryFile() {
    QString filePath = QFileDialog::getOpenFileName(
        this, tr("Open dictionary file"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "Phoneme dictionary definition (*.xml)");

    if (filePath.isNull()) {
        return;
    }

    loadDictionaryFile(filePath);
}

void AnimalCrossing::handlePitchChanged(int value) {
    m_pitch = melToHz(value);
    m_pitchLabel->setText(QString("%1 Hz").arg(m_pitch, 0, 'f', 1));

    updatePlans();
}

void AnimalCrossing::handleDurationChanged(int value) {
    m_duration = double(value) / 1000.0;
    m_durationLabel->setText(QString("%1 ms").arg(value));

    updatePlans();
}

void AnimalCrossing::extractDictionaryFiles() {
    QDir().mkdir("./dictionaries");
    QDirIterator dirIt(":/dictionaries");

    while (dirIt.hasNext()) {
        QString filePath = dirIt.next();
        QString fileName = dirIt.fileName();

        QString dest = QString("./dictionaries/%1").arg(fileName);
        if (QFile::exists(dest)) {
            QFile::remove(dest);
        }

        QFile::copy(filePath, dest);
    }
}

void AnimalCrossing::loadDictionaryFile(const QString &filePath) {
    try {
        if (m_phonemeDictionary != nullptr) {
            delete m_phonemeDictionary;
        }

        m_phonemeDictionary = phonemes::PhonemeDictionary::loadFromXML(
            filePath.toStdString().c_str());

        QFileInfo fileInfo(filePath);

        m_dictionaryFileLabel->setText(fileInfo.fileName());

        updatePhonemes();
        updatePlans();
    } catch (const std::logic_error &e) {
        QMessageBox::warning(this, tr("Phoneme dictionary failed to load"),
                             tr("Phoneme dictionary at %1 failed to load:\n%2")
                                 .arg(filePath)
                                 .arg(e.what()));
    }
}

void AnimalCrossing::updatePhonemes() {
    m_phonemeMappings =
        m_phonemeDictionary->mappingsFor(m_textBytes.constData());

    std::cout << m_phonemeMappings << std::endl;
}

void AnimalCrossing::updatePlans() {
    appState->pitchPlan()->reset(m_pitch);
    appState->amplitudePlan()->reset(0);

    double time = 0.1;

    appState->amplitudePlan()->linearToValueAtTime(0, time);

    // Reset formant and antiformant plans.
    appState->formantFrequencyPlan(0)->reset(700);
    appState->formantFrequencyPlan(1)->reset(1200);
    appState->formantFrequencyPlan(2)->reset(2400);
    appState->formantFrequencyPlan(3)->reset(2900);
    appState->formantFrequencyPlan(4)->reset(4200);

    for (int i = 0; i < 5; ++i) {
        appState->formantBandwidthPlan(i)->reset(80 + i * 15);
    }

    appState->antiformantFrequencyPlan(0)->reset(400);
    appState->antiformantBandwidthPlan(0)->reset(80);

    appState->antiformantFrequencyPlan(1)->reset(1200);
    appState->antiformantBandwidthPlan(1)->reset(110);

    // For Animal Crossing voices, scale formant frequency with the pitch.
    // Assume the dictionary was measured with an average pitch of 130 Hz.
    // Morph the center frequencies by a constant factor to the set pitch.

    const double heliumFactor = std::max(1.0, m_pitch / 150);

    // Set up the RNG for phoneme duration and inter-phoneme pause duration
    // using the text as a seed.

    std::mt19937 rng(std::hash<std::string>{}(m_textBytes.toStdString()));

    std::array intervals{0.0, 3.0, 6.0, 12.0};
    std::array weights{1.0, 10.0, 80.0, 100.0};
    std::piecewise_linear_distribution<double> pauseDis(
        intervals.begin(), intervals.end(), weights.begin());

    std::normal_distribution<double> durationFactorDis(1.0, 0.02);

    for (const auto &mapping : m_phonemeMappings) {
        double timeOffset;

        const auto &fnPole = [&time, &timeOffset, heliumFactor](
                                 const int i, const double frequency,
                                 const double bandwidth) {
            appState->formantFrequencyPlan(i)->cubicToValueAtTime(
                frequency * heliumFactor, time + timeOffset);
            appState->formantBandwidthPlan(i)->cubicToValueAtTime(
                bandwidth * heliumFactor, time + timeOffset);
        };

        const auto &fnZero = [&time, &timeOffset, heliumFactor](
                                 const int i, const double frequency,
                                 const double bandwidth) {
            appState->antiformantFrequencyPlan(i)->cubicToValueAtTime(
                frequency * heliumFactor, time + timeOffset);
            appState->antiformantBandwidthPlan(i)->cubicToValueAtTime(
                bandwidth * heliumFactor, time + timeOffset);
        };

        const double durationFactor =
            std::min(std::max(durationFactorDis(rng), 0.75), 1.25);

        const double duration = mapping.duration * durationFactor;

        timeOffset = duration * 15.0 / 1000.0;
        appState->amplitudePlan()->linearToValueAtTime(mapping.intensity,
                                                       time + timeOffset);
        timeOffset += duration * 5.0 / 1000.0;
        mapping.phoneme->updatePlansWith(fnPole, fnZero);

        time += duration * m_duration;

        timeOffset = -duration * 5.0 / 1000.0;
        appState->amplitudePlan()->linearToValueAtTime(mapping.intensity,
                                                       time + timeOffset);
        timeOffset -= duration * 15.0 / 1000.0;
        mapping.phoneme->updatePlansWith(fnPole, fnZero);

        // Pause of random duration.
        const double pauseDuration = pauseDis(rng);

        time += 0.5 * pauseDuration / 1000.0;
        appState->amplitudePlan()->linearToValueAtTime(0, time);
        time += 0.5 * pauseDuration / 1000.0;
    }

    appState->pitchPlan()->linearToValueAtTime(m_pitch, time);

    appState->updatePlans();
}
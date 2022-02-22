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

#include "animal_crossing.h"

#include <QBoxLayout>
#include <QDirIterator>
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSlider>
#include <QStandardPaths>
#include <iostream>

#include "../app_state.h"
#include "frequency_scale.h"

using namespace babblesynth::gui::voicefx;
using namespace xercesc;

AnimalCrossing::AnimalCrossing()
    : m_phonemeDictionary(new phonemes::PhonemeDictionary) {
    QGroupBox *dictionaryBox = new QGroupBox(tr("Phoneme dictionary"));
    {
        QPushButton *dictFileButton =
            new QPushButton(tr("Open dictionary file"));

        m_dictionaryFileLabel = new QLabel;

        connect(dictFileButton, &QPushButton::pressed, this,
                &AnimalCrossing::handleOpenDictionaryFile);

        QHBoxLayout *dictionaryLayout = new QHBoxLayout;
        dictionaryLayout->addWidget(dictFileButton);
        dictionaryLayout->addWidget(m_dictionaryFileLabel);
        dictionaryBox->setLayout(dictionaryLayout);
    }

    QGroupBox *pitchBox = new QGroupBox(tr("Pitch"));
    {
        QSlider *pitchSlider = new QSlider(Qt::Horizontal);
        pitchSlider->setRange(hzToMel(50), hzToMel(500));

        m_pitchLabel = new QLabel("800 Hz");
        m_pitchLabel->setMinimumWidth(m_pitchLabel->sizeHint().width());
        m_pitchLabel->setAlignment(Qt::AlignRight);

        connect(pitchSlider, &QSlider::valueChanged, this,
                &AnimalCrossing::handlePitchChanged);

        pitchSlider->setValue(hzToMel(120));

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

        durationSlider->setValue(90);

        QHBoxLayout *durationLayout = new QHBoxLayout;
        durationLayout->addWidget(durationSlider);
        durationLayout->addWidget(m_durationLabel);
        durationBox->setLayout(durationLayout);
    }

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(dictionaryBox);
    hLayout->addWidget(pitchBox);
    hLayout->addStretch();

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addLayout(hLayout);
    vLayout->addStretch();

    setLayout(vLayout);

    extractDictionaryFiles();
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
        QFile(dirIt.next())
            .copy(QString("./dictionaries/%1").arg(dirIt.fileName()));
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

    for (const auto &mapping : m_phonemeMappings) {
        appState->amplitudePlan()->linearToValueAtTime(
            mapping.intensity, time + mapping.duration * 15.0 / 1000.0);
        time += mapping.duration * m_duration;
        appState->amplitudePlan()->linearToValueAtTime(
            mapping.intensity, time - mapping.duration * 5.0 / 1000.0);
    }

    appState->pitchPlan()->linearToValueAtTime(m_pitch, time);

    appState->updatePlans();
}
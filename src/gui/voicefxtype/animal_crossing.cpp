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
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSlider>
#include <QStandardPaths>

#include "../app_state.h"
#include "frequency_scale.h"

using namespace babblesynth::gui::voicefx;
using namespace xercesc;

AnimalCrossing::AnimalCrossing() {
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

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(dictionaryBox);
    hLayout->addWidget(pitchBox);
    hLayout->addStretch();

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addLayout(hLayout);
    vLayout->addStretch();

    setLayout(vLayout);
}

AnimalCrossing::~AnimalCrossing() {}

void AnimalCrossing::updateDialogueTextChanged(const QString &text) {
    const QByteArray textBytes = text.toLocal8Bit();

    m_phonemeMappings = m_phonemeDictionary->mappingsFor(textBytes.constData());

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

    try {
        m_phonemeDictionary = phonemes::PhonemeDictionary::loadFromXML(
            filePath.toStdString().c_str());

        QFileInfo fileInfo(filePath);

        m_dictionaryFileLabel->setText(fileInfo.fileName());
    } catch (const std::logic_error &e) {
        QMessageBox::warning(this, tr("Phoneme dictionary failed to load"),
                             tr("Phoneme dictionary at %1 failed to load:\n%2")
                                 .arg(filePath)
                                 .arg(e.what()));
    }
}

void AnimalCrossing::handlePitchChanged(int value) {
    m_pitch = melToHz(value);
    m_pitchLabel->setText(QString("%1 Hz").arg(m_pitch, 0, 'f', 1));

    updatePlans();
}

void AnimalCrossing::updatePlans() {
    appState->pitchPlan()->reset(m_pitch);
    appState->amplitudePlan()->reset(0);

    double time = 0.1;

    appState->amplitudePlan()->linearToValueAtTime(0, time);

    for (const auto &mapping : m_phonemeMappings) {
        // TODO: add plans for mapping
    }

    appState->updatePlans();
}
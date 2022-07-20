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

#include "phoneme_editor.h"

#include <QBoxLayout>
#include <QDebug>
#include <QGroupBox>
#include <QLabel>

using namespace xercesc;
using namespace babblesynth::gui;

static std::vector<double> blackmanNuttall(const int N) {
    constexpr double a0 = 0.3635819;
    constexpr double a1 = 0.4891775;
    constexpr double a2 = 0.1365995;
    constexpr double a3 = 0.0106411;

    std::vector<double> window(N);

    for (int n = 0; n < N; ++n) {
        window[n] = a0 - a1 * cos(2 * M_PI * n / (N - 1)) +
                    a2 * cos(4 * M_PI * n / (N - 1)) -
                    a3 * cos(6 * M_PI * n / (N - 1));
    }

    return window;
}

PhonemeEditor::PhonemeEditor(phonemes::PhonemeDictionary **pDictionary,
                             QWidget *parent)
    : QWidget(parent),
      m_ptrDictionary(pDictionary),
      m_recording(false),
      m_selectedRow(0),
      m_changed(false) {
    setObjectName("PhonemeEditor");
    setWindowTitle(tr("Phoneme dictionary editor"));
    setFixedSize(QSize(400, 800));

    m_audioRecorder = new AudioRecorder(this);
    m_sampleRate = m_audioRecorder->preferredSampleRate();

    connect(m_audioRecorder, &AudioRecorder::stopped, this,
            &PhonemeEditor::onRecordingStopped);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    {
        m_mappingList = new QListWidget(this);

        m_mappingList->blockSignals(true);
        connect(m_mappingList, &QListWidget::currentTextChanged, this,
                &PhonemeEditor::onMappingSelected);
        m_mappingList->blockSignals(false);

        leftLayout->addWidget(m_mappingList);
    }

    QVBoxLayout *rightLayout = new QVBoxLayout;
    {
        QHBoxLayout *buttonsLayout = new QHBoxLayout;
        {
            QPushButton *addButton = new QPushButton(tr("Add"));
            QPushButton *deleteButton = new QPushButton(tr("Delete"));
            m_saveButton = new QPushButton(tr("Save"));
            m_saveButton->setEnabled(false);

            connect(addButton, &QPushButton::pressed, this,
                    &PhonemeEditor::onMappingAdd);
            connect(deleteButton, &QPushButton::pressed, this,
                    &PhonemeEditor::onMappingDelete);
            connect(m_saveButton, &QPushButton::pressed, this,
                    &PhonemeEditor::onMappingSave);

            buttonsLayout->addWidget(addButton);
            buttonsLayout->addWidget(deleteButton);
            buttonsLayout->addWidget(m_saveButton);
        }

        QGroupBox *mappingBox = new QGroupBox(tr("Mapping"));
        QHBoxLayout *mappingLayout = new QHBoxLayout;
        mappingBox->setLayout(mappingLayout);
        {
            m_mappingEdit = new QLineEdit;
            m_mappingEdit->setEnabled(false);

            connect(m_mappingEdit, &QLineEdit::textEdited, this,
                    &PhonemeEditor::onMappingRenamed);

            mappingLayout->addStretch();
            mappingLayout->addWidget(m_mappingEdit);
            mappingLayout->addStretch();
        }

        QGroupBox *recordBox = new QGroupBox(tr("Recording"));
        QHBoxLayout *recordLayout = new QHBoxLayout;
        recordBox->setLayout(recordLayout);
        {
            m_recordButton = new QPushButton(tr("Record"));
            m_recordButton->setEnabled(false);
            connect(m_recordButton, &QPushButton::pressed, m_audioRecorder,
                    &AudioRecorder::start);

            connect(m_audioRecorder, &AudioRecorder::started, this, [=]() {
                m_recordButton->setText(tr("Stop"));
                m_recordButton->disconnect();
                connect(m_recordButton, &QPushButton::pressed, m_audioRecorder,
                        &AudioRecorder::stop);
                m_recordButton->setFocus(Qt::OtherFocusReason);
                m_recording = true;
            });

            connect(m_audioRecorder, &AudioRecorder::stopped, this, [=]() {
                m_recordButton->setText(tr("Record"));
                m_recordButton->disconnect();
                connect(m_recordButton, &QPushButton::pressed, m_audioRecorder,
                        &AudioRecorder::start);
                m_recording = false;
            });

            m_recordDuration = new QLabel("0.00 s");

            connect(m_audioRecorder, &AudioRecorder::recording, this,
                    [=](int ms) {
                        m_recordDuration->setText(
                            QString("%1 s").arg(ms / 1000.0, 0, 'f', 2));
                        m_recordDuration->setEnabled(true);
                    });

            recordLayout->addStretch();
            recordLayout->addWidget(m_recordButton);
            recordLayout->addSpacing(1);
            recordLayout->addWidget(m_recordDuration);
            recordLayout->addStretch();
        }

        rightLayout->addLayout(buttonsLayout);
        rightLayout->addWidget(mappingBox);
        rightLayout->addWidget(recordBox);
        rightLayout->addStretch();
    }

    QHBoxLayout *rootLayout = new QHBoxLayout;

    rootLayout->addLayout(leftLayout);
    rootLayout->addLayout(rightLayout, 1);

    setLayout(rootLayout);

    connect(this, &PhonemeEditor::mappingsChanged, this,
            &PhonemeEditor::onMappingsChanged);

    emit mappingsChanged();
}

void PhonemeEditor::onRecordingStopped(
    const std::vector<double> &originalAudio) {
    // Resample to 10500 kHz.
    const double downSampleRate = 10500;
    auto audio = resample(originalAudio, m_sampleRate, downSampleRate);

    // Process audio.

    const int length = audio.size();

    constexpr double frameDuration = 20.0 / 1000.0;
    const int frameSamples = (int)std::round(frameDuration * downSampleRate);

    constexpr double gapDuration = 50.0 / 1000.0;
    const int gapSamples = (int)std::round(gapDuration * downSampleRate);

    // Window function for the chunks.
    const auto window = blackmanNuttall(frameSamples);

    // Do optimal pre-emphasis first.
    std::vector<double> alpha;
    do {
        // Do 1st order LP.
        alpha = arma::fitArOnly(audio, 1);
        if (fabs(alpha[0]) < 0.001) break;
        // Do preemphasis.
        for (int i = length - 1; i > 0; --i) {
            audio[i] += alpha[0] * audio[i - 1];
        }
    } while (fabs(alpha[0]) >= 0.001);

    std::vector<double> frame(frameSamples);
    int start = 0;
    int phonemeNumber = 0;

    std::vector<phonemes::PhonemeMapping> mappingList;

    double maxFrameIntensity = 0;

    while (start + frameSamples < length) {
        std::copy(audio.begin() + start, audio.begin() + start + frameSamples,
                  frame.begin());

        // Find min, max, and mean for amplitude/intensity calculation.

        double min = std::numeric_limits<double>::max();
        double max = std::numeric_limits<double>::lowest();
        double mean = 0;

        for (int i = 0; i < frameSamples; ++i) {
            if (frame[i] < min) min = frame[i];
            if (frame[i] > max) max = frame[i];
            mean += frame[i];
        }
        mean /= frameSamples;

        const double frameIntensity =
            std::max(std::abs(min - mean), std::abs(max - mean));

        if (frameIntensity > maxFrameIntensity) {
            maxFrameIntensity = frameIntensity;
        }

        // Apply window.

        for (int i = 0; i < frameSamples; ++i) {
            frame[i] *= window[i];
        }

        const auto phoneme = generatePhoneme(frame, downSampleRate);

        mappingList.push_back({phoneme, 0, frameIntensity});

        start += gapSamples;
        phonemeNumber++;
    }

    /* TODO: process the last chunk, may not be needed */

    // Normalize the frame intensity by the intensity of the whole audio
    for (auto &mapping : mappingList) {
        mapping.intensity /= maxFrameIntensity;
        mapping.duration = 1.0 / mappingList.size();
    }

    const double duration = originalAudio.size() / double(m_sampleRate);
    m_recordDuration->setText(QString("%1 s").arg(duration, 0, 'f', 2));
    m_recordDuration->setEnabled(false);

    m_changed = true;
    m_newMappings = std::move(mappingList);
    m_saveButton->setEnabled(true);
}

void PhonemeEditor::onMappingRenamed(const QString &text) {
    m_newName = text;

    m_changed = true;
    m_saveButton->setEnabled(true);
}

void PhonemeEditor::onMappingSelected(const QString &name) {
    if (m_recording) {
        QMessageBox::warning(this, tr("Still recording"),
                             tr("You're still recording this mapping"));
        QTimer::singleShot(0, [this]() {
            m_mappingList->blockSignals(true);
            m_mappingList->setCurrentRow(m_selectedRow);
            m_mappingList->blockSignals(false);
        });
        return;
    }

    if (m_changed) {
        QMessageBox::StandardButton result = QMessageBox::question(
            this, tr("Discard changes"),
            tr("This mapping has unsaved changes. Discard changes?"));

        if (result == QMessageBox::Yes) {
            m_changed = false;
            m_newName = "";
            m_newMappings.clear();
        } else {
            QTimer::singleShot(0, [this]() {
                m_mappingList->blockSignals(true);
                m_mappingList->setCurrentRow(m_selectedRow);
                m_mappingList->blockSignals(false);
            });
            return;
        }
    }

    auto &dictionary = (*m_ptrDictionary);

    if (!dictionary->mappingExists(name)) return;

    m_selectedMapping = name;
    m_selectedRow = m_mappingList->currentRow();
    m_mappingEdit->setText(name);

    m_recordDuration->setText("0.00s");
    m_recordDuration->setEnabled(false);

    m_saveButton->setEnabled(false);
    m_mappingEdit->setEnabled(true);
    m_recordButton->setEnabled(true);
}

void PhonemeEditor::onMappingAdd() {
    auto &dictionary = (*m_ptrDictionary);

    bool ok;
    const QString name =
        QInputDialog::getText(this, tr("Enter the mapping characters"),
                              tr("Mapping:"), QLineEdit::Normal, "", &ok);

    if (!ok) return;

    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("Empty name"),
                             tr("Can't create an empty mapping"));
        return;
    }

    if (dictionary->mappingExists(name)) {
        QMessageBox::warning(this, tr("Already exists"),
                             tr("This mapping already exists"));
        return;
    }

    dictionary->addOrReplaceMapping(name, {});

    emit mappingsChanged();

    onMappingSelected(name);
}

void PhonemeEditor::onMappingDelete() {
    auto &dictionary = (*m_ptrDictionary);

    dictionary->deleteMapping(m_selectedMapping);

    emit mappingsChanged();
}

void PhonemeEditor::onMappingSave() {
    auto &dictionary = (*m_ptrDictionary);

    if (!m_newMappings.empty()) {
        if (dictionary->mappingExists(m_selectedMapping)) {
            dictionary->deleteMapping(m_selectedMapping);
        }

        dictionary->addOrReplaceMapping(m_selectedMapping, m_newMappings);

        m_newMappings.clear();
    }

    if (!m_newName.isEmpty()) {
        dictionary->renameMapping(m_selectedMapping, m_newName);
        m_selectedMapping = m_newName;

        m_newName = "";
    }

    m_changed = false;
    m_saveButton->setEnabled(false);

    emit mappingsChanged();
}

void PhonemeEditor::onMappingsChanged() {
    auto &mappings = (*m_ptrDictionary)->m_mappings;

    QStringList nameList;
    for (const auto &[name, mapping] : mappings) {
        nameList.append(name);
    }
    nameList.sort(Qt::CaseInsensitive);

    m_mappingList->clear();
    m_mappingList->addItems(nameList);

    int row = 0;
    for (; row < nameList.size(); ++row) {
        if (m_selectedMapping == nameList.at(row)) {
            break;
        }
    }

    m_mappingList->setCurrentRow(row);

    const bool isEmpty = m_selectedMapping.isEmpty() || nameList.empty();

    m_saveButton->setEnabled(!isEmpty && m_changed);
    m_mappingEdit->setEnabled(!isEmpty);
    m_recordButton->setEnabled(!isEmpty);

    if (isEmpty) {
        m_mappingEdit->clear();
        m_recordDuration->setText("0.00 s");
    }
}

void PhonemeEditor::closeEvent(QCloseEvent *event) {
    if (m_recording) {
        QMessageBox::warning(this, tr("Still recording"),
                             tr("You're still recording this mapping"));
        event->ignore();
        return;
    }

    if (m_changed) {
        QMessageBox::StandardButton result = QMessageBox::question(
            this, tr("Discard changes"),
            tr("This mapping has unsaved changes. Discard changes?"));

        if (result == QMessageBox::Yes) {
            m_changed = false;
            m_newName = "";
            m_newMappings.clear();
            event->accept();
        } else {
            event->ignore();
        }
    }
}

phonemes::Phoneme PhonemeEditor::generatePhoneme(
    const std::vector<double> &audio, const double sampleRate) {
    const int arTerms = 10;
    const int maTerms = 4;

    const auto model = arma::fit(audio, arTerms, maTerms, 1e-10);

    auto arRoots = filter::solveRoots(model.ar);
    auto maRoots = filter::solveRoots(model.ma);

    phonemes::Phoneme phoneme;

    // Analyze and add AR polynomial roots (poles)

    std::sort(arRoots.begin(), arRoots.end(), [](const auto &x, const auto &y) {
        return std::abs(std::arg(x)) < std::abs(std::arg(y));
    });

    std::sort(maRoots.begin(), maRoots.end(), [](const auto &x, const auto &y) {
        return std::abs(std::arg(x)) < std::abs(std::arg(y));
    });

    int n = 1;

    for (const auto &z : arRoots) {
        if (z.imag() < 0) {
            continue;
        }

        const double r = std::abs(z);
        if (r <= 0.6 || r >= 1.0) {
            continue;
        }

        const double phi = std::arg(z);

        const double frequency = std::abs(phi) * sampleRate / (2 * M_PI);
        if (frequency <= 50 || frequency >= sampleRate / 2 - 50) {
            continue;
        }

        const double bandwidth =
            std::clamp(-std::log(r) * sampleRate / M_PI, 40.0, 200.0);

        phoneme.addPole(frequency, bandwidth);

        qDebug() << "pole" << n << ": (" << frequency << "Hz," << bandwidth
                 << "Hz )";

        ++n;
    }

    // Analyze and add MA polynomial roots (zeros)

    std::sort(maRoots.begin(), maRoots.end(), [](const auto &x, const auto &y) {
        return std::abs(std::arg(x)) < std::abs(std::arg(y));
    });

    int m = 1;

    for (const auto &z : maRoots) {
        if (z.imag() < 0) {
            continue;
        }

        const double r = std::abs(z);
        if (r <= 0.6 || r >= 1.0) {
            continue;
        }

        const double phi = std::arg(z);

        const double frequency = std::abs(phi) * sampleRate / (2 * M_PI);
        if (frequency <= 50 || frequency >= sampleRate / 2 - 50) {
            continue;
        }

        const double bandwidth =
            std::clamp(-std::log(r) * sampleRate / M_PI, 40.0, 200.0);

        phoneme.addZero(frequency, bandwidth);

        qDebug() << "zero " << m << ": (" << frequency << "Hz, " << bandwidth
                 << "Hz)";

        ++m;
    }

    qDebug() << "---";

    return phoneme;
}
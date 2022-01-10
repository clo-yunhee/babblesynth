﻿/*
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

#include "app_window.h"

using namespace babblesynth::gui;

std::shared_ptr<AppState> babblesynth::gui::appState;

AppWindow::AppWindow() : QMainWindow() {
    setObjectName("SourceParameters");
    setWindowTitle("BabbleSynth");

    m_audioPlayer = new AudioPlayer(this);
    m_sampleRate = m_audioPlayer->preferredSampleRate();

    appState = std::make_shared<AppState>(m_sampleRate);
    appState->updatePlans();

    m_sourceParameters = new SourceParameters;

    QWidget *centralWidget = new QWidget;

    QPushButton *playButton = new QPushButton(tr("Play"), centralWidget);
    QObject::connect(playButton, &QPushButton::pressed, this,
                     &AppWindow::renderAndPlay);

    QPushButton *saveButton = new QPushButton(tr("Save"), centralWidget);
    QObject::connect(saveButton, &QPushButton::pressed, this,
                     &AppWindow::renderAndSave);

    QPushButton *sourceButton =
        new QPushButton(tr("Source parameters"), centralWidget);
    QObject::connect(sourceButton, &QPushButton::pressed, m_sourceParameters,
                     &QWidget::show);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addStretch();

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(playButton);
    rightLayout->addWidget(saveButton);
    rightLayout->addWidget(sourceButton);
    rightLayout->addStretch();

    QFrame *rootSeparator = new QFrame(centralWidget);
    rootSeparator->setFrameShape(QFrame::VLine);
    rootSeparator->setFrameShadow(QFrame::Sunken);

    QHBoxLayout *rootLayout = new QHBoxLayout;
    rootLayout->addLayout(leftLayout, 1);
    rootLayout->addSpacing(2);
    rootLayout->addWidget(rootSeparator);
    rootLayout->addSpacing(2);
    rootLayout->addLayout(rightLayout);

    centralWidget->setLayout(rootLayout);
    setCentralWidget(centralWidget);
}

AppWindow::~AppWindow() { delete m_sourceParameters; }

std::vector<double> AppWindow::render() const {
    std::vector<std::pair<int, int>> pitchPeriods;
    double Oq;

    std::vector<double> glottalSource =
        appState->source()->generate(pitchPeriods, &Oq);

    std::vector<double> output = appState->formantFilter()->generateFrom(
        glottalSource, pitchPeriods, Oq);

    return output;
}

void AppWindow::renderAndPlay() { m_audioPlayer->play(render()); }

void AppWindow::renderAndSave() {
    const auto &[filterIndices, filters] = m_audioWriter.supportedFileFormats();
    QString selectedFilter;

    QString filePath = QFileDialog::getSaveFileName(
        this, "Save audio file",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        filters.join(";;"), &selectedFilter);

    if (filePath.isNull()) {
        return;
    }

    int formatIndex = filterIndices.at(filters.indexOf(selectedFilter));

    m_audioWriter.write(filePath, formatIndex, render());
}

void AppWindow::closeEvent(QCloseEvent *event) { qApp->quit(); }
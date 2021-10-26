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


#include "app_window.h"
#include "qmainwindow.h"
#include "qnamespace.h"

using namespace babblesynth::gui;

std::shared_ptr<AppState> babblesynth::gui::appState;

AppWindow::AppWindow()
    : QMainWindow()
{
    setObjectName("SourceParameters");
    setWindowTitle("BabbleSynth");

    m_sampleRate = 48000;

    appState = std::make_shared<AppState>(m_sampleRate);
    appState->updatePlans();

    m_audioPlayer = new AudioPlayer(m_sampleRate, this);

    m_sourceParameters = new SourceParameters;
    m_sourcePlan = new SourcePlan;
    m_filterTracks = new FilterTracks(5);

    QObject::connect(m_sourcePlan, &SourcePlan::updated, m_filterTracks, &FilterTracks::redrawGraph);

    QWidget *centralWidget = new QWidget;

    QPushButton *sourceButton = new QPushButton("Source parameters", centralWidget);
    QObject::connect(sourceButton, &QPushButton::pressed, m_sourceParameters, &QWidget::show);

    QPushButton *filterButton = new QPushButton("Filter tracks", centralWidget);
    QObject::connect(filterButton, &QPushButton::pressed, m_filterTracks, &QWidget::show);

    QPushButton *pitchPlan = new QPushButton("Edit pitch track");
    QObject::connect(pitchPlan, &QPushButton::pressed, m_sourcePlan, &QWidget::show);
    
    QPushButton *playButton = new QPushButton("Play", centralWidget);
    QObject::connect(playButton, &QPushButton::pressed, this, &AppWindow::renderAndPlay);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(pitchPlan);
    leftLayout->addWidget(playButton);
    leftLayout->addStretch();

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(sourceButton);
    rightLayout->addWidget(filterButton);
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

AppWindow::~AppWindow()
{
    delete m_sourceParameters;
}

void AppWindow::renderAndPlay()
{
    std::vector<std::pair<int, int>> pitchPeriods;
    std::vector<double> glottalSource = appState->source()->generate(pitchPeriods);

    std::vector<double> output = appState->formantFilter()->generateFrom(glottalSource, pitchPeriods);

    m_audioPlayer->play(output);
}

void AppWindow::closeEvent(QCloseEvent *event)
{
    qApp->quit();
}
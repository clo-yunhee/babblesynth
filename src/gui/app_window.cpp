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

#include "voicefxtype/animal_crossing.h"
#include "voicefxtype/undertale.h"

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

    QButtonGroup *voiceFxBtnGroup = new QButtonGroup(this);

    QRadioButton *voiceFxBtnUndertale = new QRadioButton(tr("Undertale"));
    voiceFxBtnGroup->addButton(voiceFxBtnUndertale, VoiceFxUndertale);

    QRadioButton *voiceFxBtnAnimalCrossing =
        new QRadioButton(tr("Animal Crossing"));
    voiceFxBtnGroup->addButton(voiceFxBtnAnimalCrossing, VoiceFxAnimalCrossing);

    QHBoxLayout *voiceFxBtnLayout = new QHBoxLayout;
    voiceFxBtnLayout->addStretch();
    voiceFxBtnLayout->addWidget(voiceFxBtnUndertale);
    voiceFxBtnLayout->addWidget(voiceFxBtnAnimalCrossing);
    voiceFxBtnLayout->addStretch();

    m_voiceFxLayout = new QStackedLayout;
    m_voiceFxLayout->addWidget(new voicefx::Undertale);
    m_voiceFxLayout->addWidget(new voicefx::AnimalCrossing);

    QObject::connect(voiceFxBtnGroup, &QButtonGroup::idToggled, this,
                     &AppWindow::chooseVoiceFxType);

    voiceFxBtnUndertale->setChecked(true);

    m_dialogueText = new QPlainTextEdit(centralWidget);
    m_dialogueText->setPlaceholderText("Enter example dialogue text here.");
    // Fix to three rows.
    {
        QFontMetrics m(m_dialogueText->font());
        m_dialogueText->setMaximumHeight(3 * m.lineSpacing());
    }

    QObject::connect(m_dialogueText, &QPlainTextEdit::textChanged, this,
                     &AppWindow::handleDialogueTextChanged);

    m_dialogueText->setPlainText(
        tr("Hey! If I asked you about bugs that would be easy to "
           "imitate...which ones would you pick?"));

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(
        new QLabel(tr("Voice effect settings template:"), centralWidget), 0,
        Qt::AlignHCenter);
    leftLayout->addLayout(voiceFxBtnLayout);
    leftLayout->addSpacing(1);
    leftLayout->addLayout(m_voiceFxLayout, 4);
    leftLayout->addStretch(1);
    leftLayout->addWidget(
        new QLabel(tr("Example dialogue text:"), centralWidget), 0,
        Qt::AlignHCenter);
    leftLayout->addWidget(m_dialogueText);

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
        this, tr("Save audio file"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        filters.join(";;"), &selectedFilter);

    if (filePath.isNull()) {
        return;
    }

    int formatIndex = filterIndices.at(filters.indexOf(selectedFilter));

    m_audioWriter.write(filePath, formatIndex, render());
}

void AppWindow::chooseVoiceFxType(int id, bool checked) {
    if (!checked) return;

    m_voiceFxLayout->setCurrentIndex(id);
}

void AppWindow::handleDialogueTextChanged() {
    auto widget =
        static_cast<voicefx::VoiceFxType *>(m_voiceFxLayout->currentWidget());

    QString text = m_dialogueText->toPlainText();

    widget->updateDialogueTextChanged(text);
}

void AppWindow::closeEvent(QCloseEvent *event) { qApp->quit(); }
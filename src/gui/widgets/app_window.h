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

#ifndef BABBLESYNTH_APP_WINDOW_H
#define BABBLESYNTH_APP_WINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QStackedLayout>
#include <vector>

#include "../audio_player.h"
#include "../audio_writer.h"
#include "source_parameters.h"

namespace babblesynth {
namespace gui {

class AppWindow : public QMainWindow {
    Q_OBJECT

   public:
    enum VoiceFxType {
        VoiceFxUndertale = 0,
        VoiceFxAnimalCrossing,
    };

    AppWindow();
    virtual ~AppWindow();

   private slots:
    void renderAndPlay();
    void renderAndSave();
    void chooseVoiceFxType(int id, bool checked);
    void handleDialogueTextChanged();

   protected:
    void closeEvent(QCloseEvent *event) override;

   private:
    std::vector<double> render() const;

    int m_sampleRate;

    bool m_isPlaying;

    AudioPlayer *m_audioPlayer;
    AudioWriter m_audioWriter;
    SourceParameters *m_sourceParameters;

    QStackedLayout *m_voiceFxLayout;

    QPlainTextEdit *m_dialogueText;
};

}  // namespace gui
}  // namespace babblesynth

#endif  // BABBLESYNTH_APP_WINDOW_H
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

#ifndef BABBLESYNTH_VOICEFX_UNDERTALE_H
#define BABBLESYNTH_VOICEFX_UNDERTALE_H

#include <QLabel>

#include "voicefxtype.h"

namespace babblesynth {
namespace gui {
namespace voicefx {

class Undertale : public VoiceFxType {
    Q_OBJECT
   public:
    Undertale();
    ~Undertale();

   public slots:
    void updateDialogueTextChanged(const QString &text) override;

   private slots:
    void handlePitchChanged(int value);
    void handleDurationChanged(int value);
    void handlePauseChanged(int value);

   private:
    void updatePlans();

    QLabel *m_pitchLabel;
    QLabel *m_durationLabel;
    QLabel *m_pauseLabel;

    double m_pitch;
    double m_duration;
    double m_pauseRatio;

    enum CharType {
        CharTypeLetter,
        CharTypeSpace,
        CharTypePunctuation,
    };

    std::vector<CharType> m_chars;
};

}  // namespace voicefx
}  // namespace gui
}  // namespace babblesynth

#endif  // BABBLESYNTH_VOICEFX_UNDERTALE_H
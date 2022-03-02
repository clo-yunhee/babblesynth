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

#ifndef BABBLESYNTH_VOICEFX_ANIMAL_CROSSING_H
#define BABBLESYNTH_VOICEFX_ANIMAL_CROSSING_H

#include <QLabel>

#include "../phonemes/phoneme_dictionary.h"
#include "voicefxtype.h"

namespace babblesynth {
namespace gui {
namespace voicefx {

class AnimalCrossing : public VoiceFxType {
    Q_OBJECT
   public:
    AnimalCrossing();
    virtual ~AnimalCrossing();

   public slots:
    void updateDialogueTextChanged(const QString &text) override;

   private slots:
    void handleOpenDictionaryFile();
    void handlePitchChanged(int value);
    void handleDurationChanged(int value);

   private:
    void extractDictionaryFiles();
    void loadDictionaryFile(const QString &filePath);

    void updatePhonemes();
    void updatePlans();

    QLabel *m_pitchLabel;
    QLabel *m_durationLabel;
    QLabel *m_dictionaryFileLabel;
    QLabel *m_dictionaryDirtyLabel;

    double m_pitch;
    double m_duration;

    phonemes::PhonemeDictionary *m_phonemeDictionary;

    QByteArray m_textBytes;
    std::vector<phonemes::PhonemeMapping> m_phonemeMappings;
};

}  // namespace voicefx
}  // namespace gui
}  // namespace babblesynth

#endif  // BABBLESYNTH_VOICEFX_ANIMAL_CROSSING_H
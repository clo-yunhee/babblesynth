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

#ifndef BABBLESYNTH_PHONEME_EDITOR_H
#define BABBLESYNTH_PHONEME_EDITOR_H

#include <QCloseEvent>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QWidget>

#include "../audio_recorder.h"
#include "../phonemes/phoneme_dictionary.h"

namespace babblesynth {
namespace gui {

class PhonemeEditor : public QWidget {
    Q_OBJECT

   public:
    PhonemeEditor(phonemes::PhonemeDictionary **pDictionary,
                  QWidget *parent = nullptr);

   signals:
    void mappingsChanged();

   private slots:
    void onRecordingStopped(const std::vector<double> &audio);
    void onMappingRenamed(const QString &text);
    void onMappingSelected(const QString &name);
    void onMappingAdd();
    void onMappingDelete();
    void onMappingSave();
    void onMappingsChanged();

   protected:
    void closeEvent(QCloseEvent *event) override;

   private:
    phonemes::Phoneme generatePhoneme(const std::vector<double> &audio,
                                      double sampleRate);

    phonemes::PhonemeDictionary **m_ptrDictionary;

    AudioRecorder *m_audioRecorder;
    int m_sampleRate;

    QListWidget *m_mappingList;
    QPushButton *m_saveButton;
    QLineEdit *m_mappingEdit;

    bool m_recording;
    QPushButton *m_recordButton;
    QLabel *m_recordDuration;

    int m_selectedRow;
    QString m_selectedMapping;

    bool m_changed;
    QString m_newName;
    std::vector<phonemes::PhonemeMapping> m_newMappings;
};

}  // namespace gui
}  // namespace babblesynth

#endif  // BABBLESYNTH_PHONEME_EDITOR_H
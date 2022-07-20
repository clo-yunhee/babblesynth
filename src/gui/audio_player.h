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

#ifndef BABBLESYNTH_AUDIO_PLAYER_H
#define BABBLESYNTH_AUDIO_PLAYER_H

#include <QAudio>
#include <QAudioDevice>
#include <QAudioFormat>
#include <QAudioSink>
#include <QBuffer>
#include <QByteArray>
#include <QMediaDevices>
#include <QObject>
#include <vector>

#include "app_state.h"

namespace babblesynth {
namespace gui {

class AudioPlayer : public QObject {
    Q_OBJECT

   public:
    AudioPlayer(QObject *parent = nullptr);

    void play(const std::vector<double> &data);

    int preferredSampleRate() const;

   public slots:
    void stop();

   signals:
    void started();
    void stopped();

   private slots:
    void onStateChanged(QAudio::State state);

   private:
    void initAudio();

    QAudioDevice m_deviceInfo;

    int m_sampleRate;
    QAudioFormat m_audioFormat;

    QAudioSink *m_audio;

    QByteArray m_data;
    QBuffer m_buffer;

    bool m_playing;
};

}  // namespace gui
}  // namespace babblesynth

#endif  // BABBLESYNTH_AUDIO_PLAYER_H
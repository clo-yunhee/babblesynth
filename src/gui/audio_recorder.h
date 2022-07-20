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

#ifndef BABBLESYNTH_AUDIO_RECORDER_H
#define BABBLESYNTH_AUDIO_RECORDER_H

#include <QAudio>
#include <QAudioDevice>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioSource>
#include <QBuffer>
#include <QByteArray>
#include <QMediaDevices>
#include <QObject>
#include <QTimer>
#include <vector>

#include "app_state.h"

namespace babblesynth {
namespace gui {

class AudioRecorder : public QObject {
    Q_OBJECT

   public:
    AudioRecorder(QObject *parent = nullptr);

    int preferredSampleRate() const;

   public slots:
    void start();
    void stop();

   signals:
    void started();
    void stopped(std::vector<double> audio);
    void recording(int durationMillis);

   private slots:
    void onStateChanged(QAudio::State state);
    void onNotified();

   private:
    void onStopped();

    void initAudio();

    QAudioDevice m_deviceInfo;

    int m_sampleRate;
    QAudioFormat m_audioFormat;

    QTimer m_timer;

    QAudioSource *m_audio;

    QBuffer m_buffer;

    bool m_recording;
};

}  // namespace gui
}  // namespace babblesynth

#endif  // BABBLESYNTH_AUDIO_RECORDER_H
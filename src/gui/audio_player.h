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


#ifndef BABBLESYNTH_AUDIO_PLAYER_H
#define BABBLESYNTH_AUDIO_PLAYER_H

#include <QtWidgets>
#include <QtMultimedia>

#include "app_state.h"
#include "source_parameters.h"

namespace babblesynth {
namespace gui {

class AudioPlayer : public QObject {
    Q_OBJECT
    
public:
    AudioPlayer(int sampleRate = 48000, QObject *parent = nullptr);

    void setSampleRate(int sampleRate);
    void play(const std::vector<double>& data);

private slots:
    void onStateChanged(QAudio::State state);

private:
    QAudioDeviceInfo m_deviceInfo;

    int m_sampleRate;
    QAudioFormat m_audioFormat;

    QAudioOutput *m_audio;

    QByteArray m_data;
    QBuffer m_buffer;

    bool m_playing;
};

}
}

#endif // BABBLESYNTH_AUDIO_PLAYER_H
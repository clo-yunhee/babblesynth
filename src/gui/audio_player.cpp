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

#include "audio_player.h"

#include <QDebug>
#include <stdexcept>

#include "widgets/app_window.h"

using namespace babblesynth::gui;

static void f64_to_s16(QByteArray& dst, const std::vector<double>& src) {
    constexpr int channels = 2;
    const int sampleCount = src.size();
    dst.resize(sizeof(int16_t) * channels * sampleCount);

    int16_t* pDst = reinterpret_cast<int16_t*>(dst.data());

    int r;
    size_t i, ch;
    for (i = 0; i < sampleCount; ++i) {
        double x = src[i];
        double c;
        c = ((x < -1) ? -1 : ((x > 1) ? 1 : x));
        c = c + 1;
        r = (int)(c * 32767.5);
        r = r - 32768;
        for (ch = 0; ch < channels; ++ch) {
            pDst[i * channels + ch] = (int16_t)r;
        }
    }
}

AudioPlayer::AudioPlayer(QObject* parent)
    : QObject(parent),
      m_deviceInfo(QAudioDeviceInfo::defaultOutputDevice()),
      m_audio(nullptr) {
    initAudio();
    m_buffer.setBuffer(&m_data);
}

void AudioPlayer::play(const std::vector<double>& data) {
    if (m_playing) {
        m_audio->stop();
    }
    f64_to_s16(m_data, data);
    m_buffer.open(QIODevice::ReadOnly);
    m_audio->start(&m_buffer);
}

int AudioPlayer::preferredSampleRate() const { return m_sampleRate; }

void AudioPlayer::onStateChanged(QAudio::State state) {
    switch (state) {
        case QAudio::ActiveState:
            m_playing = true;
            break;
        case QAudio::IdleState:
            m_audio->stop();
            m_buffer.close();
            m_playing = false;
            break;
        case QAudio::StoppedState:
            if (m_audio->error() != QAudio::NoError) {
                qWarning() << "Audio device stopped unexpectedly";
            }
            break;
        default:
            break;
    }
}

void AudioPlayer::initAudio() {
    m_sampleRate = m_deviceInfo.preferredFormat().sampleRate();

    m_audioFormat.setSampleRate(m_sampleRate);
    m_audioFormat.setChannelCount(2);
    m_audioFormat.setSampleSize(16);
    m_audioFormat.setSampleType(QAudioFormat::SignedInt);
    m_audioFormat.setCodec("audio/pcm");

    if (!m_deviceInfo.isFormatSupported(m_audioFormat)) {
        throw std::invalid_argument("audio format not supported");
    }

    if (m_audio != nullptr) {
        m_audio->stop();
        m_audio->deleteLater();
    }

    m_audio = new QAudioOutput(m_deviceInfo, m_audioFormat, this);
    m_audio->setVolume(0.6);
    QObject::connect(m_audio, &QAudioOutput::stateChanged, this,
                     &AudioPlayer::onStateChanged);
}

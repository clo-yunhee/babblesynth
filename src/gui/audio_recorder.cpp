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

#include "audio_recorder.h"

#include <QDebug>
#include <stdexcept>

#include "widgets/app_window.h"

using namespace babblesynth::gui;

static void s16_to_f64(std::vector<double>& dst, const QByteArray& src) {
    constexpr int channels = 1;
    const int sampleCount = (src.size() / sizeof(int16_t)) / channels;
    dst.resize(sampleCount);

    auto pSrc = reinterpret_cast<const int16_t*>(src.constData());

    size_t i, ch;
    for (i = 0; i < sampleCount; ++i) {
        double x = 0.0;

        for (ch = 0; ch < channels; ++ch) {
            double c;
            c = pSrc[i * channels + ch];
            c = c + 32768;
            c = c * 0.0000305180437933928435187;
            c = c - 1;

            x += c;
        }

        dst[i] = x / channels;
    }
}

AudioRecorder::AudioRecorder(QObject* parent)
    : QObject(parent),
      m_deviceInfo(QMediaDevices::defaultAudioOutput()),
      m_audio(nullptr) {
    m_timer.setInterval(50);
    m_timer.setTimerType(Qt::PreciseTimer);
    connect(&m_timer, &QTimer::timeout, this, &AudioRecorder::onNotified);
    initAudio();
}

void AudioRecorder::start() {
    if (m_recording) {
        m_audio->stop();
    }
    m_buffer.open(QIODevice::WriteOnly);
    m_audio->start(&m_buffer);
}

void AudioRecorder::stop() {
    if (m_recording) m_audio->stop();
}

int AudioRecorder::preferredSampleRate() const { return m_sampleRate; }

void AudioRecorder::onStateChanged(QAudio::State state) {
    switch (state) {
        case QAudio::ActiveState:
            m_recording = true;
            m_timer.start();
            emit started();
            break;
        case QAudio::IdleState:
            // onNotified();
            m_audio->stop();
            m_buffer.close();
            m_timer.stop();
            m_recording = false;
            onStopped();
            break;
        case QAudio::StoppedState:
            if (m_audio->error() != QAudio::NoError) {
                qWarning() << "Audio device stopped unexpectedly";
            }
            m_buffer.close();
            m_timer.stop();
            onStopped();
            break;
        default:
            break;
    }
}

void AudioRecorder::onNotified() {
    emit recording(m_audio->processedUSecs() / 1000);
}

void AudioRecorder::onStopped() {
    std::vector<double> audio;
    s16_to_f64(audio, m_buffer.data());
    emit stopped(audio);
}

void AudioRecorder::initAudio() {
    m_sampleRate = m_deviceInfo.preferredFormat().sampleRate();

    m_audioFormat.setSampleRate(m_sampleRate);
    m_audioFormat.setChannelCount(1);
    m_audioFormat.setSampleFormat(QAudioFormat::Int16);

    if (!m_deviceInfo.isFormatSupported(m_audioFormat)) {
        throw std::invalid_argument("audio format not supported");
    }

    if (m_audio != nullptr) {
        m_audio->stop();
        m_audio->deleteLater();
    }

    m_audio = new QAudioSource(m_deviceInfo, m_audioFormat, this);

    connect(m_audio, &QAudioSource::stateChanged, this,
            &AudioRecorder::onStateChanged);
}

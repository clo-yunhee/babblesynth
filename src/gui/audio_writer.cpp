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

#include "audio_writer.h"

#include <sndfile.h>

using namespace babblesynth::gui;

void AudioWriter::write(const QString &filePath,
                        const std::vector<double> &data) {
    SNDFILE *sndfile;
    int mode = SFM_WRITE;

    SF_INFO sfinfo;
    sfinfo.samplerate = 48000;
    sfinfo.channels = 1;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_24;

#if defined(_WIN32) && defined(_UNICODE)
    std::wstring filePathString = filePath.toStdWString();
    sndfile = sf_wchar_open(filePathString.c_str(), mode, &sfinfo);
#else
    std::string filePathString = filePath.toStdString();
    sndfile = sf_open(filePathString.c_str(), mode, &sfinfo);
#endif

    if (sndfile == nullptr) {
        qDebug() << "error opening file:" << sf_strerror(nullptr);
        return;
    }

    sf_writef_double(sndfile, data.data(), data.size());

    int err = sf_close(sndfile);
    if (err != 0) {
        qDebug() << "error closing file:" << sf_error_number(err);
    }
}

QStringList AudioWriter::supportedFileFormats() const {
    // List supported file formats by this build of sndfile.
    SF_FORMAT_INFO format_info;
    int k, count;

    sf_command(nullptr, SFC_GET_SIMPLE_FORMAT_COUNT, &count, sizeof(int));

    std::set<QString> extSet;

    for (k = 0; k < count; k++) {
        format_info.format = k;
        sf_command(nullptr, SFC_GET_SIMPLE_FORMAT, &format_info,
                   sizeof(format_info));
        extSet.emplace(QString("*.%1").arg(format_info.extension));
    };

    return QStringList(extSet.cbegin(), extSet.cend());
}
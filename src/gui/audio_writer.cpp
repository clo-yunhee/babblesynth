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

#include <QDebug>

using namespace babblesynth::gui;

void AudioWriter::write(const QString &filePath, const int formatIndex,
                        const std::vector<double> &data) {
    SNDFILE *sndfile;
    int mode = SFM_WRITE;

    SF_INFO sfinfo;
    sfinfo.samplerate = 48000;
    sfinfo.channels = 1;
    sfinfo.format = (formatIndex & SF_FORMAT_TYPEMASK) | SF_FORMAT_PCM_24;

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

std::pair<std::vector<int>, QStringList> AudioWriter::supportedFileFormats()
    const {
    // List supported file formats by this build of sndfile.
    SF_FORMAT_INFO info;
    SF_INFO sfinfo;
    int majorCount;

    sf_command(nullptr, SFC_GET_FORMAT_MAJOR_COUNT, &majorCount, sizeof(int));

    std::vector<int> indexList;
    QStringList typeList;

    sfinfo.samplerate = 48000;
    sfinfo.channels = 1;

    for (int m = 0; m < majorCount; m++) {
        info.format = m;
        sf_command(NULL, SFC_GET_FORMAT_MAJOR, &info, sizeof(info));

        QString name = QString::fromLocal8Bit(info.name);
        QString extension = QString::fromLocal8Bit(info.extension);

        sfinfo.format = (info.format & SF_FORMAT_TYPEMASK) | SF_FORMAT_PCM_24;

        if (sf_format_check(&sfinfo)) {
            indexList.push_back(info.format & SF_FORMAT_TYPEMASK);
            typeList.append(QString("%1 (*.%2)").arg(name).arg(extension));
        }
    }

    return std::make_pair(indexList, typeList);
}
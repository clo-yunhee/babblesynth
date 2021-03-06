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

#ifndef BABBLESYNTH_AUDIO_WRITER_H
#define BABBLESYNTH_AUDIO_WRITER_H

#include <QString>
#include <QStringList>
#include <utility>
#include <vector>

#include "app_state.h"
#include "widgets/source_parameters.h"

namespace babblesynth {
namespace gui {

class AudioWriter {
   public:
    void write(const QString &filePath, int formatIndex,
               const std::vector<double> &data);

    std::pair<std::vector<int>, QStringList> supportedFileFormats() const;
};

}  // namespace gui
}  // namespace babblesynth

#endif  // BABBLESYNTH_AUDIO_WRITER_H
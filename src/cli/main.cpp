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


#include "filter/formant_filter.h"
#include "source/abstract_source.h"
#include "variable_plan.h"
#include <iostream>

#include <babblesynth.h>
#include <dr_wav.h>

int main(int argc, char *argv[])
{
    int sampleRate = 48'000;

    std::cout << "Generating at " << sampleRate << " Hz sample rate\n";

    babblesynth::generator::source_generator source(sampleRate);

    babblesynth::variable_plan pitch(true, 130.81);
    pitch.linearToValueAtTime(130.81, 0.6);
    pitch.cubicToValueAtTime(164.81, 0.7);
    pitch.linearToValueAtTime(164.81, 1.2);
    pitch.cubicToValueAtTime(196.00, 1.3);
    pitch.linearToValueAtTime(196.00, 1.8);
    pitch.cubicToValueAtTime(246.94, 1.9);
    pitch.linearToValueAtTime(246.94, 2.4);
    pitch.cubicToValueAtTime(261.63, 2.5);
    pitch.linearToValueAtTime(261.63, 3.6);

    babblesynth::variable_plan amplitude(true, 1e-10);
    amplitude.linearToValueAtTime(0.8, 0.1);
    amplitude.linearToValueAtTime(1.0, 1.8);
    amplitude.linearToValueAtTime(0.8, 3.5);
    amplitude.linearToValueAtTime(0.2, 3.6);

    source.getParameter("Source type").setValue(babblesynth::source::sources.valueOf("LF"));
    source.getParameter("Pitch plan").setValue(pitch);
    source.getParameter("Amplitude plan").setValue(amplitude);
    source.getParameter("Jitter amplitude").setValue(2);

    source.getSource()->getParameter("Oq").setValue(0.6);
    source.getSource()->getParameter("am").setValue(0.8);
    source.getSource()->getParameter("Qa").setValue(0.2);

    std::vector<std::pair<int, int>> pitchPeriods;
    std::vector<double> glottalSource = source.generate(pitchPeriods);

    babblesynth::filter::formant_filter vtf(sampleRate);

    babblesynth::variable_plan F1(false, 900);
    babblesynth::variable_plan F2(false, 1300);
    babblesynth::variable_plan F3(false, 2400);
    babblesynth::variable_plan F4(false, 2800);
    babblesynth::variable_plan F5(false, 3800);

    F1.linearToValueAtTime(1100, 3.0);
    F2.linearToValueAtTime(1400, 3.0);

    vtf.getParameter("F1 plan").setValue(F1);
    vtf.getParameter("F2 plan").setValue(F2);
    vtf.getParameter("F3 plan").setValue(F3);
    vtf.getParameter("F4 plan").setValue(F4);
    vtf.getParameter("F5 plan").setValue(F5);

    std::vector<double> output = vtf.generateFrom(glottalSource, pitchPeriods);

    int frameCount = output.size();

    std::cout << "Audio is " << frameCount << " samples long\n";

    std::vector<int16_t> output_s16(frameCount);
    drwav_f64_to_s16(output_s16.data(), output.data(), frameCount);

    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_PCM;
    format.channels = 1;
    format.sampleRate = sampleRate;
    format.bitsPerSample = 16;

    drwav wav;
    drwav_init_file_write(&wav, "output.wav", &format, nullptr);
    drwav_write_pcm_frames(&wav, frameCount, output_s16.data());
    drwav_uninit(&wav);

    std::cout << "Written to output.wav\n";
    std::cout << std::flush;

    return 0;
}
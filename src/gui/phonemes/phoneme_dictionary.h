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

#ifndef BABBLESYNTH_PHONEMES_PHONEME_DICTIONARY_H
#define BABBLESYNTH_PHONEMES_PHONEME_DICTIONARY_H

#include <memory>
#include <unordered_map>
#include <xercesc/dom/DOM.hpp>

#include "phoneme.h"

namespace babblesynth {
namespace gui {
namespace phonemes {

class PhonemeDictionary {
   public:
    struct MappingDef {
        Phoneme *phoneme;
        double duration;
        double intensity;
    };

    virtual ~PhonemeDictionary();

    std::vector<MappingDef> mappingsFor(const char *text);

    static PhonemeDictionary *loadFromXML(const char *xmlFilename);

   private:
    PhonemeDictionary(xercesc::DOMDocument *doc);

    std::unordered_map<XMLCh *, std::unique_ptr<Phoneme>> m_phonemes;
    std::unordered_map<XMLCh *, std::vector<MappingDef>> m_mappings;
};

using PhonemeMapping = PhonemeDictionary::MappingDef;

}  // namespace phonemes
}  // namespace gui
}  // namespace babblesynth

#endif  // BABBLESYNTH_PHONEMES_PHONEME_DICTIONARY_H
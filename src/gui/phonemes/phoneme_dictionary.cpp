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

#include "phoneme_dictionary.h"

#include <iostream>
#include <string>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

using namespace babblesynth::gui::phonemes;
using namespace xercesc;

inline bool stringEquals(const XMLCh *xmlStr, const char *cStr2) {
    XMLCh *xmlStr2 = XMLString::transcode(cStr2);
    bool test = XMLString::equals(xmlStr, xmlStr2);
    XMLString::release(&xmlStr2);
    return test;
}

inline bool stringEqualsI(const XMLCh *xmlStr, const char *cStr2) {
    XMLCh *xmlStr2 = XMLString::transcode(cStr2);
    bool test = (0 == XMLString::compareIString(xmlStr, xmlStr2));
    XMLString::release(&xmlStr2);
    return test;
}

PhonemeDictionary::~PhonemeDictionary() {
    for (auto &[name, phoneme] : m_phonemes) {
        XMLString::release(const_cast<XMLCh **>(&name));
    }

    for (auto &[chars, phoneme] : m_mappings) {
        XMLString::release(const_cast<XMLCh **>(&chars));
    }
}

std::vector<PhonemeMapping> PhonemeDictionary::mappingsFor(const char *text) {
    XMLCh *textXml = XMLString::transcode(text);
    int textLength = XMLString::stringLen(textXml);

    std::vector<MappingDef> mappings;

    int chIndex = 0;

    while (chIndex < textLength) {
        int prefixLengthFound = 0;
        std::vector<MappingDef> mappingFound;

        // Search for the longest mapping that starts at chIndex.
        for (const auto &[prefix, mapping] : m_mappings) {
            const int prefixLength = XMLString::stringLen(prefix);

            if (prefixLength > prefixLengthFound &&
                XMLString::startsWithI(textXml + chIndex, prefix)) {
                prefixLengthFound = prefixLength;
                mappingFound = mapping;
            }
        }

        // If no mapping was found, advance by one character and try again.
        if (prefixLengthFound > 0) {
            chIndex += prefixLengthFound;
            mappings.insert(mappings.end(), mappingFound.cbegin(),
                            mappingFound.cend());
        } else {
            chIndex++;
        }
    }

    XMLString::release(&textXml);

    return mappings;
}

PhonemeDictionary *PhonemeDictionary::loadFromXML(const char *xmlFilename) {
    XMLCh tempStr[100];
    XMLString::transcode("LS", tempStr, 99);
    DOMImplementation *impl =
        DOMImplementationRegistry::getDOMImplementation(tempStr);
    DOMLSParser *parser =
        ((DOMImplementationLS *)impl)
            ->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);

    DOMDocument *doc = 0;

    try {
        doc = parser->parseURI(xmlFilename);
    } catch (const XMLException &toCatch) {
        char *message = XMLString::transcode(toCatch.getMessage());
        std::cout << "Exception message is: \n" << message << "\n";
        XMLString::release(&message);
        return nullptr;
    } catch (const DOMException &toCatch) {
        char *message = XMLString::transcode(toCatch.msg);
        std::cout << "Exception message is: \n" << message << "\n";
        XMLString::release(&message);
        return nullptr;
    } catch (...) {
        std::cout << "Unexpected Exception \n";
        return nullptr;
    }

    parser->release();

    return new PhonemeDictionary(doc);
}

PhonemeDictionary::PhonemeDictionary(DOMDocument *doc) {
    constexpr XMLCh tagDictionary[] = {
        chLatin_d, chLatin_i, chLatin_c, chLatin_t, chLatin_i, chLatin_o,
        chLatin_n, chLatin_a, chLatin_r, chLatin_y, chNull};

    constexpr XMLCh tagPhonemes[] = {chLatin_p, chLatin_h, chLatin_o,
                                     chLatin_n, chLatin_e, chLatin_m,
                                     chLatin_e, chLatin_s, chNull};

    constexpr XMLCh tagPhoneme[] = {chLatin_p, chLatin_h, chLatin_o, chLatin_n,
                                    chLatin_e, chLatin_m, chLatin_e, chNull};

    constexpr XMLCh tagName[] = {chLatin_n, chLatin_a, chLatin_m, chLatin_e,
                                 chNull};

    constexpr XMLCh tagPole[] = {chLatin_p, chLatin_o, chLatin_l, chLatin_e,
                                 chNull};

    constexpr XMLCh tagZero[] = {chLatin_z, chLatin_e, chLatin_r, chLatin_o,
                                 chNull};

    constexpr XMLCh tagFrequency[] = {
        chLatin_f, chLatin_r, chLatin_e, chLatin_q, chLatin_u,
        chLatin_e, chLatin_n, chLatin_c, chLatin_y, chNull};

    constexpr XMLCh tagQuality[] = {chLatin_q, chLatin_u, chLatin_a, chLatin_l,
                                    chLatin_i, chLatin_t, chLatin_y, chNull};

    constexpr XMLCh tagMappings[] = {chLatin_m, chLatin_a, chLatin_p,
                                     chLatin_p, chLatin_i, chLatin_n,
                                     chLatin_g, chLatin_s, chNull};

    constexpr XMLCh tagMapping[] = {chLatin_m, chLatin_a, chLatin_p, chLatin_p,
                                    chLatin_i, chLatin_n, chLatin_g, chNull};

    constexpr XMLCh tagFor[] = {chLatin_f, chLatin_o, chLatin_r, chNull};

    constexpr XMLCh tagDuration[] = {chLatin_d, chLatin_u, chLatin_r,
                                     chLatin_a, chLatin_t, chLatin_i,
                                     chLatin_o, chLatin_n, chNull};

    constexpr XMLCh tagIntensity[] = {
        chLatin_i, chLatin_n, chLatin_t, chLatin_e, chLatin_n,
        chLatin_s, chLatin_i, chLatin_t, chLatin_y, chNull};

    DOMElement *root = doc->getDocumentElement();

    if (root == nullptr) {
        throw std::logic_error(
            "Phoneme dictionary document element does not exist");
    }

    if (!XMLString::equals(root->getTagName(), tagDictionary)) {
        throw std::logic_error(
            "Phoneme dictionary root tag must be <dictionary>");
    }

    DOMNodeList *phonemesList = doc->getElementsByTagName(tagPhonemes);

    if (phonemesList->getLength() == 0) {
        throw std::logic_error(
            "Phoneme dictionary must contain at least one <phonemes> tag");
    }

    for (int iPhs = 0; iPhs < phonemesList->getLength(); ++iPhs) {
        DOMElement *phonemesElement = (DOMElement *)phonemesList->item(iPhs);

        DOMNodeList *phonemeList =
            phonemesElement->getElementsByTagName(tagPhoneme);

        if (phonemeList->getLength() == 0) {
            throw std::logic_error(
                "<phonemes> tag must contain at least one <phoneme> tag");
        }

        for (int iPh = 0; iPh < phonemeList->getLength(); ++iPh) {
            DOMElement *phonemeElement = (DOMElement *)phonemeList->item(iPh);

            // Find name.
            const XMLCh *name = phonemeElement->getAttribute(tagName);

            if (name == nullptr) {
                throw std::logic_error(
                    "<phoneme> tag must have a name attribute");
            }

            auto phoneme = std::make_unique<Phoneme>(name);

            // Find pole definitions.
            DOMNodeList *poleList =
                phonemeElement->getElementsByTagName(tagPole);

            for (int iPole = 0; iPole < poleList->getLength(); ++iPole) {
                DOMElement *poleElement = (DOMElement *)poleList->item(iPole);

                const XMLCh *frequencyXmlStr =
                    poleElement->getAttribute(tagFrequency);

                if (frequencyXmlStr == nullptr) {
                    throw std::logic_error(
                        "<pole> tag must have a frequency attribute");
                }

                char *frequencyCStr = XMLString::transcode(frequencyXmlStr);

                double frequency =
                    std::stod(std::string(frequencyCStr), nullptr);

                const XMLCh *qualityXmlStr =
                    poleElement->getAttribute(tagQuality);

                if (qualityXmlStr == nullptr) {
                    throw std::logic_error(
                        "<pole> tag must have a quality attribute");
                }

                char *qualityCStr = XMLString::transcode(qualityXmlStr);

                double quality = std::stod(std::string(qualityCStr), nullptr);

                phoneme->addPole(frequency, quality);

                XMLString::release(&frequencyCStr);
                XMLString::release(&qualityCStr);
            }

            // Find zero definitions.
            DOMNodeList *zeroList =
                phonemeElement->getElementsByTagName(tagZero);

            for (int iZero = 0; iZero < zeroList->getLength(); ++iZero) {
                DOMElement *zeroElement = (DOMElement *)zeroList->item(iZero);

                const XMLCh *frequencyXmlStr =
                    zeroElement->getAttribute(tagFrequency);

                if (frequencyXmlStr == nullptr) {
                    throw std::logic_error(
                        "<zero> tag must have a frequency attribute");
                }

                char *frequencyCStr = XMLString::transcode(frequencyXmlStr);

                double frequency =
                    std::stod(std::string(frequencyCStr), nullptr);

                const XMLCh *qualityXmlStr =
                    zeroElement->getAttribute(tagQuality);

                if (qualityXmlStr == nullptr) {
                    throw std::logic_error(
                        "<zero> tag must have a quality attribute");
                }

                char *qualityCStr = XMLString::transcode(qualityXmlStr);

                double quality = std::stod(std::string(qualityCStr), nullptr);

                phoneme->addZero(frequency, quality);

                XMLString::release(&frequencyCStr);
                XMLString::release(&qualityCStr);
            }

            m_phonemes.emplace(XMLString::replicate(name), std::move(phoneme));
        }
    }

    DOMNodeList *mappingsList = doc->getElementsByTagName(tagMappings);

    if (mappingsList->getLength() == 0) {
        throw std::logic_error(
            "Phoneme dictionary must contain at least one <mappings> tag");
    }

    for (int iMaps = 0; iMaps < mappingsList->getLength(); ++iMaps) {
        DOMElement *mappingsElement = (DOMElement *)mappingsList->item(iMaps);

        DOMNodeList *mappingList =
            mappingsElement->getElementsByTagName(tagMapping);

        if (mappingList->getLength() == 0) {
            throw std::logic_error(
                "<mappings> tag must contain at least one <mapping> tag");
        }

        for (int iMap = 0; iMap < mappingList->getLength(); ++iMap) {
            DOMElement *mappingElement = (DOMElement *)mappingList->item(iMap);

            // Find for.
            const XMLCh *forString = mappingElement->getAttribute(tagFor);

            if (forString == nullptr) {
                throw std::logic_error(
                    "<mapping> tag must have a for attribute");
            }

            // Get phonemes.
            DOMNodeList *phonemeList =
                mappingElement->getElementsByTagName(tagPhoneme);

            if (phonemeList->getLength() < 1) {
                throw std::logic_error(
                    "<mapping> tag must contain at least one <phoneme> "
                    "tag");
            }

            std::vector<MappingDef> mappingDefs;

            for (int iPh = 0; iPh < phonemeList->getLength(); ++iPh) {
                DOMElement *phonemeElement =
                    (DOMElement *)phonemeList->item(iPh);

                // Find duration.
                const XMLCh *durationXmlStr =
                    phonemeElement->getAttribute(tagDuration);

                if (durationXmlStr == nullptr) {
                    throw std::logic_error(
                        "<phoneme> tag must have a duration attribute");
                }

                char *durationCStr = XMLString::transcode(durationXmlStr);

                double duration = std::stod(std::string(durationCStr), nullptr);

                // Find intensity.
                const XMLCh *intensityXmlStr =
                    phonemeElement->getAttribute(tagIntensity);

                if (intensityXmlStr == nullptr) {
                    throw std::logic_error(
                        "<phoneme> tag must have an intensity attribute");
                }

                char *intensityCStr = XMLString::transcode(intensityXmlStr);

                double intensity =
                    std::stod(std::string(intensityCStr), nullptr);

                // Check that <phoneme> only contains text.
                DOMNodeList *phonemeChildren = phonemeElement->getChildNodes();

                if (phonemeChildren->getLength() != 1) {
                    throw std::logic_error(
                        "<phoneme> tag must only contain text content");
                }

                DOMNode *phonemeChild = phonemeChildren->item(0);

                if (phonemeChild->getNodeType() != DOMNode::TEXT_NODE) {
                    throw std::logic_error(
                        "<phoneme> tag must only contain text content");
                }

                const XMLCh *phonemeString = phonemeChild->getNodeValue();

                // Check that there is a phoneme with that name.
                bool doesPhonemeMatch = false;
                Phoneme *matchingPhoneme;

                for (const auto &[name, phoneme] : m_phonemes) {
                    if (XMLString::equals(name, phonemeString)) {
                        doesPhonemeMatch = true;
                        matchingPhoneme = phoneme.get();
                        break;
                    }
                }

                if (!doesPhonemeMatch) {
                    throw std::logic_error(
                        "<phoneme> tag must match a phoneme defined in one "
                        "of "
                        "the <phonemes> groups");
                }

                mappingDefs.push_back({matchingPhoneme, duration, intensity});
            }

            m_mappings.emplace(XMLString::replicate(forString),
                               std::move(mappingDefs));
        }
    }
}
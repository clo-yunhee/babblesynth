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

#include "phoneme_dictionary.h"

#include <QDebug>
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

PhonemeDictionary::~PhonemeDictionary() {}

std::vector<PhonemeMapping> PhonemeDictionary::mappingsFor(
    const XMLWStr &text) {
    int textLength = text.length();

    std::vector<PhonemeMapping> mappings;

    int chIndex = 0;

    while (chIndex < textLength) {
        int prefixLengthFound = 0;
        std::vector<PhonemeMapping> mappingFound;

        // Search for the longest mapping that starts at chIndex.
        for (const auto &[prefix, mapping] : m_mappings) {
            const int prefixLength = prefix.length();

            if (prefixLength > prefixLengthFound &&
                text.startsWith(prefix, chIndex)) {
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

    return mappings;
}

void PhonemeDictionary::addOrReplaceMapping(
    const XMLWStr &name, const std::vector<PhonemeMapping> &mappings) {
    const XMLWStr prefix(name + "_");

    std::vector<PhonemeMapping> namedMappings(mappings);

    for (int num = 0; num < mappings.size(); ++num) {
        const XMLWStr phonemeName(prefix + std::to_string(num));

        namedMappings[num].phoneme.m_name = phonemeName;

        m_phonemes.emplace(phonemeName, namedMappings[num].phoneme);
    }

    auto it = m_mappings.find(name);
    // Replace if it exists.
    if (it != m_mappings.end()) {
        it->second = namedMappings;
    } else {
        m_mappings.emplace(name, namedMappings);
    }
}

void PhonemeDictionary::deleteMapping(const XMLWStr &name) {
    const XMLWStr prefix(name + "_");

    // Delete it from mappings
    auto mapIt = m_mappings.begin();
    while (mapIt != m_mappings.end()) {
        if (mapIt->first == name) {
            mapIt = m_mappings.erase(mapIt);
            break;
        } else {
            ++mapIt;
        }
    }

    // Delete the corresponding phoneme mappings
    auto phoneIt = m_phonemes.begin();
    while (phoneIt != m_phonemes.end()) {
        if (phoneIt->first.startsWith(prefix)) {
            phoneIt = m_phonemes.erase(phoneIt);
        } else {
            ++phoneIt;
        }
    }
}

void PhonemeDictionary::renameMapping(const XMLWStr &oldName,
                                      const XMLWStr &newName) {
    std::vector<PhonemeMapping> mappings;

    const XMLWStr oldPrefix(oldName + "_");
    const XMLWStr newPrefix(newName + "_");

    // Delete old mapping, save phoneme list
    auto mapIt = m_mappings.begin();
    while (mapIt != m_mappings.end()) {
        if (mapIt->first == oldName) {
            mappings = mapIt->second;
            mapIt = m_mappings.erase(mapIt);
            break;
        } else {
            ++mapIt;
        }
    }

    // Delete all the old phonemes
    auto phoneIt = m_phonemes.begin();
    while (phoneIt != m_phonemes.end()) {
        if (phoneIt->first.startsWith(oldPrefix)) {
            phoneIt = m_phonemes.erase(phoneIt);
        } else {
            ++phoneIt;
        }
    }

    // Remap all the phonemes
    for (int num = 0; num < mappings.size(); ++num) {
        const XMLWStr newPhonemeName(newPrefix + std::to_string(num));

        mappings[num].phoneme.m_name = newPhonemeName;

        m_phonemes.emplace(newPhonemeName, mappings[num].phoneme);
    }

    m_mappings.emplace(newName, std::move(mappings));
}

bool PhonemeDictionary::mappingExists(const XMLWStr &name) const {
    return m_mappings.find(name) != m_mappings.end();
}

const std::vector<PhonemeMapping> &PhonemeDictionary::phonemesForMapping(
    const XMLWStr &name) {
    return m_mappings.find(name)->second;
}

PhonemeDictionary *PhonemeDictionary::loadFromXML(const XMLWStr &filename) {
    XMLCh tempStr[100];
    XMLString::transcode("LS", tempStr, 99);
    DOMImplementation *impl =
        DOMImplementationRegistry::getDOMImplementation(tempStr);
    DOMLSParser *parser =
        ((DOMImplementationLS *)impl)
            ->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);

    // Make sure the parser gives ownership of the document before being
    // released.
    if (parser->getDomConfig()->canSetParameter(
            XMLUni::fgXercesUserAdoptsDOMDocument, true))
        parser->getDomConfig()->setParameter(
            XMLUni::fgXercesUserAdoptsDOMDocument, true);

    DOMDocument *doc = nullptr;

    try {
        doc = parser->parseURI(filename);
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
    const auto tagDictionary = "dictionary"_x;
    const auto tagPhonemes = "phonemes"_x;
    const auto tagPhoneme = "phoneme"_x;
    const auto tagName = "name"_x;
    const auto tagPole = "pole"_x;
    const auto tagZero = "zero"_x;
    const auto tagFrequency = "frequency"_x;
    const auto tagBandwidth = "bandwidth"_x;
    const auto tagMappings = "mappings"_x;
    const auto tagMapping = "mapping"_x;
    const auto tagFor = "for"_x;
    const auto tagDuration = "duration"_x;
    const auto tagIntensity = "intensity"_x;

    if (doc == nullptr) {
        throw std::logic_error("Phoneme dictionary document does not exist");
    }

    DOMElement *root = doc->getDocumentElement();

    if (root == nullptr) {
        throw std::logic_error(
            "Phoneme dictionary document element does not exist");
    }

    if (tagDictionary != root->getTagName()) {
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

            Phoneme phoneme(name);

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

                const XMLCh *bandwidthXmlStr =
                    poleElement->getAttribute(tagBandwidth);

                if (bandwidthXmlStr == nullptr) {
                    throw std::logic_error(
                        "<pole> tag must have a bandwidth attribute");
                }

                char *bandwidthCStr = XMLString::transcode(bandwidthXmlStr);

                double bandwidth =
                    std::stod(std::string(bandwidthCStr), nullptr);

                phoneme.addPole(frequency, bandwidth);

                XMLString::release(&frequencyCStr);
                XMLString::release(&bandwidthCStr);
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

                const XMLCh *bandwidthXmlStr =
                    zeroElement->getAttribute(tagBandwidth);

                if (bandwidthXmlStr == nullptr) {
                    throw std::logic_error(
                        "<zero> tag must have a bandwidth attribute");
                }

                char *bandwidthCStr = XMLString::transcode(bandwidthXmlStr);

                double bandwidth =
                    std::stod(std::string(bandwidthCStr), nullptr);

                phoneme.addZero(frequency, bandwidth);

                XMLString::release(&frequencyCStr);
                XMLString::release(&bandwidthCStr);
            }

            m_phonemes.emplace(name, std::move(phoneme));
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

            std::vector<PhonemeMapping> mappingDefs;

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
                const Phoneme *matchingPhoneme;

                for (const auto &[name, phoneme] : m_phonemes) {
                    if (name == phonemeString) {
                        doesPhonemeMatch = true;
                        matchingPhoneme = &phoneme;
                        break;
                    }
                }

                if (!doesPhonemeMatch) {
                    throw std::logic_error(
                        "<phoneme> tag must match a phoneme defined in one "
                        "of "
                        "the <phonemes> groups");
                }

                mappingDefs.push_back({*matchingPhoneme, duration, intensity});
            }

            m_mappings.emplace(forString, std::move(mappingDefs));
        }
    }
}

void PhonemeDictionary::saveToXml(const XMLWStr &filename) {
    const auto tagDictionary = "dictionary"_x;
    const auto tagPhonemes = "phonemes"_x;
    const auto tagPhoneme = "phoneme"_x;
    const auto tagName = "name"_x;
    const auto tagPole = "pole"_x;
    const auto tagZero = "zero"_x;
    const auto tagFrequency = "frequency"_x;
    const auto tagBandwidth = "bandwidth"_x;
    const auto tagMappings = "mappings"_x;
    const auto tagMapping = "mapping"_x;
    const auto tagFor = "for"_x;
    const auto tagDuration = "duration"_x;
    const auto tagIntensity = "intensity"_x;

    XMLCh tempStr[100];
    XMLString::transcode("LS", tempStr, 99);
    DOMImplementation *impl =
        DOMImplementationRegistry::getDOMImplementation(tempStr);

    char buf[128];

    try {
        DOMDocument *doc = impl->createDocument(0, tagDictionary, 0);

        DOMElement *root = doc->getDocumentElement();

        DOMElement *phonemesElement = doc->createElement(tagPhonemes);
        root->appendChild(phonemesElement);

        for (const auto &[name, phoneme] : m_phonemes) {
            DOMElement *phonemeElement = doc->createElement(tagPhoneme);
            phonemesElement->appendChild(phonemeElement);

            phonemeElement->setAttribute(tagName, name);

            for (const auto &pole : phoneme.m_poles) {
                DOMElement *poleElement = doc->createElement(tagPole);
                phonemeElement->appendChild(poleElement);

                std::sprintf(buf, "%g", pole.frequency);
                poleElement->setAttribute(tagFrequency, XMLWStr(buf));

                std::sprintf(buf, "%g", pole.bandwidth);
                poleElement->setAttribute(tagBandwidth, XMLWStr(buf));
            }

            for (const auto &zero : phoneme.m_zeros) {
                DOMElement *zeroElement = doc->createElement(tagZero);
                phonemeElement->appendChild(zeroElement);

                std::sprintf(buf, "%g", zero.frequency);
                zeroElement->setAttribute(tagFrequency, XMLWStr(buf));

                std::sprintf(buf, "%g", zero.bandwidth);
                zeroElement->setAttribute(tagBandwidth, XMLWStr(buf));
            }
        }

        DOMElement *mappingsElement = doc->createElement(tagMappings);
        root->appendChild(mappingsElement);

        for (const auto &[name, mapping] : m_mappings) {
            DOMElement *mappingElement = doc->createElement(tagMapping);
            mappingsElement->appendChild(mappingElement);

            mappingElement->setAttribute(tagFor, name);

            for (const auto &phoneme : mapping) {
                DOMElement *phonemeElement = doc->createElement(tagPhoneme);
                mappingElement->appendChild(phonemeElement);

                std::sprintf(buf, "%g", phoneme.duration);
                phonemeElement->setAttribute(tagDuration, XMLWStr(buf));

                std::sprintf(buf, "%g", phoneme.intensity);
                phonemeElement->setAttribute(tagIntensity, XMLWStr(buf));

                phonemeElement->setTextContent(phoneme.phoneme.m_name);
            }
        }

        DOMLSSerializer *serializer =
            ((DOMImplementationLS *)impl)->createLSSerializer();

        if (serializer->getDomConfig()->canSetParameter(
                XMLUni::fgDOMWRTFormatPrettyPrint, true))
            serializer->getDomConfig()->setParameter(
                XMLUni::fgDOMWRTFormatPrettyPrint, true);

        serializer->writeToURI(doc, filename);

        serializer->release();
        doc->release();
    } catch (const XMLException &toCatch) {
        char *message = XMLString::transcode(toCatch.getMessage());
        std::cout << "Exception message is: \n" << message << "\n";
        XMLString::release(&message);
        return;
    } catch (const DOMException &toCatch) {
        char *message = XMLString::transcode(toCatch.msg);
        std::cout << "Exception message is: \n" << message << "\n";
        XMLString::release(&message);
        return;
    } catch (...) {
        std::cout << "Unexpected Exception \n";
        return;
    }
}

std::ostream &operator<<(std::ostream &os,
                         const PhonemeDictionary &dictionary) {
    os << "PhonemeDictionary[\n";
    os << "  phonemes: [\n";

    for (const auto &[name, phoneme] : dictionary.m_phonemes) {
        os << "    (" << name << "),\n";
    }

    os << "  ],\n";
    os << "  mappings: [\n";

    for (const auto &[name, mappings] : dictionary.m_mappings) {
        os << "    (" << name << " => " << mappings << "),\n";
    }

    os << "  ]\n";
    os << "]" << std::endl;

    return os;
}

std::ostream &operator<<(std::ostream &os,
                         const std::vector<PhonemeMapping> &mappings) {
    os << "[";
    for (const auto &def : mappings) {
        os << "<" << def.phoneme.name() << ">, ";
    }
    os << "]";
    return os;
}
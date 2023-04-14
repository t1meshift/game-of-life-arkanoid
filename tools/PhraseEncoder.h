#pragma once

#include <array>
#include <vector>
#include <string>

namespace maslo {
    constexpr std::array<const char *, 16> DEFAULT_PHRASES {
        "amogus",
        "sussy",
        "baka",
        "poggers",
        "cap",
        "chill",
        "flex",
        "fr",
        "cringe",
        "based",
        "ayo",
        "drip",
        "rizz",
        "vibe",
        "no",
        "real"
    };

    template<uint8_t PassphraseBitEntropy = 4>
    class PhraseEncoder {
    public:
        explicit PhraseEncoder(std::array<const char*, 1 << PassphraseBitEntropy> phrases = DEFAULT_PHRASES)
            : m_phrases{std::move(phrases)} {
            // TODO: loose up the conditions
            static_assert(CHAR_BIT == 8 && PassphraseBitEntropy <= 8 && 8 % PassphraseBitEntropy == 0);
        }

        static constexpr uint8_t phrasesPerByte{8 / PassphraseBitEntropy};

        std::vector<uint8_t> decode(std::string_view encodedString) {
            std::vector<uint8_t> result;
            int phraseCnt = 0;

            std::string currentPhrase;
            size_t currentOffset = 0;
            do {
                size_t spacePos = encodedString.find_first_of(' ', currentOffset);
                size_t size = spacePos;
                if (size != std::string_view::npos) {
                    size = size - currentOffset;
                }
                currentPhrase = encodedString.substr(currentOffset, size);

                // TODO: amortize this
                bool found = false;
                for (int i = 0; i < m_phrases.size(); ++i) {
                    if (currentPhrase == m_phrases.at(i)) {
                        found = true;
                        auto byteIdx = (phraseCnt++) / phrasesPerByte;
                        if (byteIdx >= result.size()) {
                            result.push_back(0);
                        }
                        result[byteIdx] <<= PassphraseBitEntropy;
                        result[byteIdx] |= i & ((1 << PassphraseBitEntropy) - 1);
                        break;
                    }
                }
                if (!found) {
                    result.clear();
                    return result;
                }

                currentOffset = spacePos + (spacePos != std::string_view::npos);
            }
            while (currentOffset != std::string_view::npos || currentOffset < encodedString.size());

            return result;
        }

        std::string encode(const std::vector<uint8_t>& bytes) {
            std::string result;
            bool first = true;
            for (auto byte : bytes) {
                for (int8_t i = phrasesPerByte - 1; i >= 0; --i) {
                    uint8_t val = (byte >> (PassphraseBitEntropy * i)) & ((1 << PassphraseBitEntropy) - 1);
                    if (first) {
                        result = m_phrases[val];
                        first = false;
                    }
                    else {
                        result += fmt::format(" {}", m_phrases[val]);
                    }
                }
            }
            return result;
        }
    private:
        std::array<const char*, 1 << PassphraseBitEntropy> m_phrases;
    };
}
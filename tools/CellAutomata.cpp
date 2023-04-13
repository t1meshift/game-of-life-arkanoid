#include "CellAutomata.h"

#include <utility>
#include <stdexcept>
#include <array>
#include <numeric>
#include <fmt/format.h>

namespace maslo {
    CellAutomataRules::CellAutomataRules(const std::string& ruleset) : m_isCorrect(true) {
        enum class RuleParserState {
            START,
            BIRTH,
            SURVIVAL,
            ERROR
        };

        RuleParserState state = RuleParserState::START;

        for (const char& c : ruleset) {
            switch (c) {
                case 'B':
                    state = RuleParserState::BIRTH;
                    continue;
                case 'S':
                    state = RuleParserState::SURVIVAL;
                    continue;
                case '/':
                    continue;
                default:
                    break;
            }
            if (c >= '0' && c <= '9') {
                if (state == RuleParserState::BIRTH) {
                    auto i = m_birth.insert(c - '0');
                    if (!i.second) {
                        state = RuleParserState::ERROR;
                        break;
                    }
                }
                else if (state == RuleParserState::SURVIVAL) {
                    auto i = m_survival.insert(c - '0');
                    if (!i.second) {
                        state = RuleParserState::ERROR;
                        break;
                    }
                }
                else {
                    state = RuleParserState::ERROR;
                    break;
                }
            }
        }

        if (state == RuleParserState::ERROR) {
            m_isCorrect = false;
            return;
        }
    }

    CellAutomataRules CellAutomataRules::makeClassicLife() {
        return CellAutomataRules("B3/S23");
    }

    CellAutomataRules CellAutomataRules::make34Life() {
        return CellAutomataRules("B34/S34");
    }

    const std::set<uint8_t> &CellAutomataRules::getBirthCondition() const {
        return m_birth;
    }

    const std::set<uint8_t> &CellAutomataRules::getSurvivalCondition() const {
        return m_survival;
    }

    bool CellAutomataRules::isCorrect() const {
        return m_isCorrect;
    }

    CellAutomata::CellAutomata(size_t width, size_t height, CellAutomataRules rules)
        : m_width(width), m_height(height), m_rules(std::move(rules)), m_generation(0) {
        m_field.reserve(m_width * m_height);
        m_field.assign(m_width * m_height, 0);
    }

    void CellAutomata::initMap(const std::vector<std::vector<uint8_t>> &map) {
        auto gotHeight = map.size();
        auto gotWidth = map.empty() ? 0 : map.at(0).size();

        m_generation = 0;

        if (m_width != gotWidth || m_height != gotHeight) {
            throw std::runtime_error(
                    fmt::format(
                            "Dimension mismatch; expected {}x{}, got {}x{}",
                            m_width, m_height, gotWidth, gotHeight
                    )
            );
        }

        // assuming m_field.reserve() was called previously
        for (size_t i = 0; i < gotHeight; ++i) {
            if (map.at(i).size() != gotWidth) {
                throw std::runtime_error("Incorrect map format");
            }
            for (size_t j = 0; j < gotWidth; ++j) {
                m_field[i * gotWidth + j] = map.at(i).at(j);
            }
        }
    }

    uint8_t CellAutomata::getCell(int x, int y) const {
        checkBorder(x, 0, static_cast<int>(m_width - 1));
        checkBorder(y, 0, static_cast<int>(m_height - 1));
        return m_field[y * m_width + x];
    }

    void CellAutomata::checkBorder(int &coordinate, int min, int max) {
        if (coordinate < min) {
            coordinate = max;
        }
        if (coordinate > max) {
            coordinate = min;
        }
    }

    void CellAutomata::setCell(int x, int y, uint8_t value) {
        checkBorder(x, 0, static_cast<int>(m_width - 1));
        checkBorder(y, 0, static_cast<int>(m_height - 1));
        m_field[y * m_width + x] = value;
    }

    void CellAutomata::update() {
        auto newGen = m_field;
        std::array<uint8_t, 8> neighbors{};

        // Inefficient (O(W*H) memory) and P R O U D

        for (auto i = 0; i < m_height; ++i) {
            for (auto j = 0; j < m_width; ++j) {
                neighbors = {
                        getCell(j - 1, i - 1), getCell(j, i - 1), getCell(j + 1, i - 1),
                        getCell(j - 1, i), /* neighbors only, */ getCell(j + 1, i),
                        getCell(j - 1, i + 1), getCell(j, i + 1), getCell(j + 1, i + 1)
                        };
                auto s = std::accumulate(neighbors.cbegin(), neighbors.cend(), 0, [](int a, int b) {
                    return a + (b != 0);
                });
                auto isAlive = getCell(j, i) != 0;
                if (!m_rules.getSurvivalCondition().contains(s) && isAlive) {
                    newGen[i * m_width + j] = 0;
                }
                if (m_rules.getBirthCondition().contains(s) && !isAlive) {
                    newGen[i * m_width + j] = 1;
                }
            }
        }

        m_field = newGen;
        ++m_generation;
    }

    size_t CellAutomata::getGeneration() const {
        return m_generation;
    }
}

#pragma once

#include <string>
#include <vector>
#include <set>

namespace maslo {
    // See https://en.wikipedia.org/wiki/Life-like_cellular_automaton
    class CellAutomataRules {
    public:
        explicit CellAutomataRules(const std::string& ruleset);
        static CellAutomataRules makeClassicLife();
        static CellAutomataRules make34Life();


        [[nodiscard]] const std::set<uint8_t>& getBirthCondition() const;
        [[nodiscard]] const std::set<uint8_t>& getSurvivalCondition() const;
        [[nodiscard]] bool isCorrect() const;
    private:
        std::set<uint8_t> m_birth;
        std::set<uint8_t> m_survival;
        bool m_isCorrect;
    };

    class CellAutomata {
    public:
        CellAutomata(size_t width, size_t height, CellAutomataRules rules = CellAutomataRules::makeClassicLife());

        void initMap(const std::vector<std::vector<uint8_t>>& map);
        [[nodiscard]] uint8_t getCell(int x, int y) const;
        void setCell(int x, int y, uint8_t value);

        void update();

        [[nodiscard]] size_t getGeneration() const;
    private:
        static void checkBorder(int& coordinate, int min, int max);
    private:
        CellAutomataRules m_rules;
        std::vector<uint8_t> m_field;
        size_t m_width;
        size_t m_height;
        size_t m_generation;
    };
}

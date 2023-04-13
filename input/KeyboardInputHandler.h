#pragma once

#include <vector>
#include <memory>

namespace maslo {
    /**
     * Assuming all raw keys have `int` type
     */
    class KeyboardInputHandler {
    public:
        KeyboardInputHandler() = default;

        void addKey(int key) {
            m_keys.push_back(key);
        }

        void setKeys(const std::vector<int>& keys) {
            m_keys = keys;
        }

        void clearKeys() {
            m_keys.clear();
        }

        [[nodiscard]] const std::vector<int>& getRawKeys() const {
            return m_keys;
        }
    protected:
        std::vector<int> m_keys;
    };
}

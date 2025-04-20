module;

#include <iostream>

export module demo.Window;

export class Window {
public:
    auto open() -> void
    {
        std::puts("Window is open");
        m_is_open = true;
    }

    [[nodiscard]]
    auto is_open() const -> bool
    {
        return m_is_open;
    }

    auto close() -> void
    {
        std::puts("Window is closed");
        m_is_open = false;
    }

private:
    bool m_is_open{};
};

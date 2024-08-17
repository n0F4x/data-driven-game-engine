#include "Store.hpp"

Store::~Store() noexcept
{
    while (!m_map.empty()) {
        m_map.pop_back();
    }
}

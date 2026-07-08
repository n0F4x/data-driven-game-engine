module;

#include <cstddef>

export module ddge.util.TypeList;

namespace ddge::util {

export template <typename... Ts>
struct TypeList {
    [[nodiscard]]
    consteval static auto size() noexcept -> std::size_t
    {
        return sizeof...(Ts);
    }
};

}   // namespace ddge::util

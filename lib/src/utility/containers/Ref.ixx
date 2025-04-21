module;

#include <functional>

export module utility.containers.Ref;

import utility.meta.concepts.decayed;

namespace util {

export template <typename T>
    requires(!std::is_reference_v<T>)
class Ref {
public:
    constexpr explicit Ref(T& ref);

    [[nodiscard]]
    auto get() const -> T&;

    [[nodiscard]]
    constexpr auto operator->() const -> T*;

private:
    std::reference_wrapper<T> m_ref;
};

}   // namespace util

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr util::Ref<T>::Ref(T& ref) : m_ref{ ref }
{}

template <typename T>
    requires(!std::is_reference_v<T>)
auto util::Ref<T>::get() const -> T&
{
    return m_ref.get();
}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr auto util::Ref<T>::operator->() const -> T*
{
    return std::addressof(m_ref.get());
}

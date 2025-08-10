module;

#include <functional>

export module ddge.utility.containers.Ref;

namespace ddge::util {

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

}   // namespace ddge::util

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr ddge::util::Ref<T>::Ref(T& ref) : m_ref{ ref }
{}

template <typename T>
    requires(!std::is_reference_v<T>)
auto ddge::util::Ref<T>::get() const -> T&
{
    return m_ref.get();
}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr auto ddge::util::Ref<T>::operator->() const -> T*
{
    return std::addressof(m_ref.get());
}

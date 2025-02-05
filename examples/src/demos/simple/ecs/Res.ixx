module;

#include <functional>
#include <memory>
#include <type_traits>

export module ecs:Res;

namespace ecs {

export template <typename T>
    requires std::is_object_v<T>
class Res {
public:
    using Underlying = T;

    explicit Res(T& ref);

    [[nodiscard]]
    auto operator->() const -> T*;

private:
    std::reference_wrapper<T> m_ref;
};

}   // namespace ecs

template <typename T>
    requires std::is_object_v<T>
ecs::Res<T>::Res(T& ref) : m_ref{ ref }
{}

template <typename T>
    requires std::is_object_v<T>
auto ecs::Res<T>::operator->() const -> T*
{
    return std::addressof(m_ref.get());
}

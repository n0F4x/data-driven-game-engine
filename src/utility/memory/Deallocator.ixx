export module utility.memory.Deallocator;

namespace util {

export template <typename Allocator_T>
class Deallocator {
public:
    template <typename T>
    constexpr static auto operator()(T* pointer) -> void;
};

}   // namespace util

template <typename Allocator_T>
template <typename T>
constexpr auto util::Deallocator<Allocator_T>::operator()(T* pointer) -> void
{
    Allocator_T{}.deallocate(pointer);
}

export module utility.Overloaded;

namespace util {

export template <typename... Ts>
struct Overloaded : Ts... {
    using Ts::operator()...;

    consteval static auto operator()(auto...) -> void = delete("unsupported type");
};

}   // namespace util

export module ddge.utility.Overloaded;

namespace ddge::util {

export template <typename... Ts>
struct Overloaded : Ts... {
    using Ts::operator()...;

    consteval static auto operator()(auto...) -> void = delete("unsupported type");
};

}   // namespace ddge::util

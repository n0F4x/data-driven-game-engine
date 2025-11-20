export module ddge.utility.meta.type_traits.always_true;

namespace ddge::util::meta {

export template <typename T>
struct always_true {
    constexpr static bool value = true;
};

}   // namespace ddge::util::meta

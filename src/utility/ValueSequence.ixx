export module utility.ValueSequence;

import utility.meta.type_traits.all_same;

namespace util {

export template <auto... values_T>
    requires(::util::meta::all_same_v<decltype(values_T)...>)
struct ValueSequence {};

}   // namespace util

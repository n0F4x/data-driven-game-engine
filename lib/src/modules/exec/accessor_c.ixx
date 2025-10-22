export module ddge.modules.exec.accessor_c;

import ddge.utility.meta.concepts.naked;

namespace ddge::exec {

export template <typename T>
concept accessor_c = util::meta::naked_c<T>;

}   // namespace ddge::exec

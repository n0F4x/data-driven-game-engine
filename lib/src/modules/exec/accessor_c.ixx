export module ddge.modules.exec.accessor_c;

import ddge.modules.exec.locks.has_lock_group_c;

import ddge.utility.meta.concepts.naked;
import ddge.utility.meta.concepts.storable;

namespace ddge::exec {

export template <typename T>
concept accessor_c = util::meta::naked_c<T> && util::meta::storable_c<T>
                  && has_lock_group_c<T>;

}   // namespace ddge::exec

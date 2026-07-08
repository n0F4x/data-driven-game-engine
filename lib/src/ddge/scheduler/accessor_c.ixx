export module ddge.scheduler.accessor_c;

import ddge.scheduler.locks.has_lock_group_c;

import ddge.util.meta.concepts.naked;
import ddge.util.meta.concepts.storable;

namespace ddge::scheduler {

export template <typename T>
concept accessor_c = util::meta::naked_c<T> && util::meta::storable_c<T>
                  && has_lock_group_c<T>;

}   // namespace ddge::scheduler

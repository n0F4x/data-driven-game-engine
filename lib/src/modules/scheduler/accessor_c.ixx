export module ddge.modules.scheduler.accessor_c;

import ddge.utility.meta.concepts.naked;

namespace ddge::scheduler {

export template <typename T>
concept accessor_c = util::meta::naked_c<T>;

}   // namespace ddge::scheduler

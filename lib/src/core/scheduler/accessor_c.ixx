export module core.scheduler.accessor_c;

import utility.meta.concepts.naked;

namespace core::scheduler {

export template <typename T>
concept accessor_c = util::meta::naked_c<T>;

}   // namespace core::scheduler

export module modules.scheduler.accessor_c;

import utility.meta.concepts.naked;

namespace modules::scheduler {

export template <typename T>
concept accessor_c = util::meta::naked_c<T>;

}   // namespace modules::scheduler

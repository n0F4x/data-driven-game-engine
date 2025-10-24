export module ddge.modules.exec.locks.Lockable;

import ddge.modules.exec.locks.Lock;
import ddge.modules.exec.locks.ReaderLock;
import ddge.modules.exec.locks.WriterLock;

import ddge.utility.meta.type_traits.type_list.type_list_concat;
import ddge.utility.meta.type_traits.type_list.type_list_unique;
import ddge.utility.TypeList;

namespace ddge::exec {

export template <lock_c... Locks_T>
struct Lockable {
    using Locks = util::meta::type_list_unique_t<
        util::meta::type_list_concat_t<typename Locks_T::LockedTypes...>>;
};

}   // namespace ddge::exec

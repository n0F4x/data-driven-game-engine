module;

#include <function2/function2.hpp>

export module core.scheduler.Task;

namespace core::scheduler {

// TODO: use `std::move_only_function`
export template <typename Result_T>
using Task = fu2::unique_function<Result_T()>;

}   // namespace core::scheduler

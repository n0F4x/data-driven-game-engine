module;

#include <functional>

export module core.scheduler.Task;

namespace core::scheduler {

export template <typename Result_T>
using Task = std::function<Result_T()>;

}   // namespace core::scheduler

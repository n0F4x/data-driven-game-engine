module;

#include <function2/function2.hpp>

export module ddge.modules.exec.v2.Task;

namespace ddge::exec::v2 {

export using Task = fu2::unique_function<void()>;

}   // namespace ddge::exec::v2

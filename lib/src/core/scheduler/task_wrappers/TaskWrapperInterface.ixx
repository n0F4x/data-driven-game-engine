export module core.scheduler.task_wrappers.TaskWrapperInterface;

import utility.meta.concepts.type_list.type_list;

import core.scheduler.task_wrappers.TaskWrapperBase;

namespace core::scheduler {

export template <util::meta::type_list_c Dependencies_T>
class TaskWrapperInterface : public TaskWrapperBase {
public:
    using Dependencies = Dependencies_T;
};

}   // namespace core::scheduler

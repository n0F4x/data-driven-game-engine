module;

#include <concepts>
#include <memory>
#include <optional>
#include <utility>

// TODO: remove once Clang can mangle
#include <function2/function2.hpp>

export module ddge.modules.execution.v2.primitives.repeat;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.v2.as_task_blueprint;
import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.convertible_to_TaskBlueprint_c;
import ddge.modules.execution.v2.gatherers.WaitAll;
import ddge.modules.execution.v2.TaskBlueprint;
import ddge.modules.execution.v2.TaskBuilder;
import ddge.modules.execution.v2.TaskBuilderBundle;
import ddge.modules.execution.v2.TaskBundle;
import ddge.modules.execution.v2.TaskFinishedCallback;
import ddge.modules.execution.v2.TaskHubBuilder;
import ddge.modules.execution.v2.TaskHubProxy;

namespace ddge::exec::v2 {

export template <
    convertible_to_TaskBlueprint_c<void> MainTaskBlueprint_T,
    std::integral                        Repetition_T>
auto repeat(
    MainTaskBlueprint_T&&                               main_blueprint,
    TaskBlueprint<Repetition_T, Cardinality::eSingle>&& repetition_specifier_blueprint
) -> TaskBlueprint<void, Cardinality::eSingle>;

}   // namespace ddge::exec::v2

[[nodiscard]]
auto sync(ddge::exec::v2::TaskBuilder<void>&& task_builder)
    -> ddge::exec::v2::TaskBuilder<void>
{
    return std::move(task_builder);
}

[[nodiscard]]
auto sync(ddge::exec::v2::TaskBuilderBundle<void>&& task_builder_bundle)
    -> ddge::exec::v2::TaskBuilder<void>
{
    return std::move(task_builder_bundle).sync(ddge::exec::v2::WaitAllBuilder{});
}

template <
    ddge::exec::v2::convertible_to_TaskBlueprint_c<void> MainTaskBlueprint_T,
    std::integral                                        Repetition_T>
auto ddge::exec::v2::repeat(
    MainTaskBlueprint_T&&                               main_blueprint,
    TaskBlueprint<Repetition_T, Cardinality::eSingle>&& repetition_specifier_blueprint
) -> TaskBlueprint<void, Cardinality::eSingle>
{
    return TaskBlueprint<void, Cardinality::eSingle>{
        [x_main_blueprint = as_task_blueprint<void>(std::move(main_blueprint)),
         x_repetition_specifier_blueprint = std::move(repetition_specifier_blueprint)]   //
        () mutable -> TaskBuilder<void> {
            return TaskBuilder<void>{
                [y_main_blueprint                 = std::move(x_main_blueprint),
                 y_repetition_specifier_blueprint = std::move(
                     x_repetition_specifier_blueprint
                 )](   //
                    Nexus&                       nexus,
                    TaskHubBuilder&              task_hub_builder,
                    TaskFinishedCallback<void>&& callback
                ) mutable -> TaskBundle   //
                {
                    class Looper {
                    public:
                        explicit Looper(TaskFinishedCallback<void>&& callback)
                            : m_callback{ std::move(callback) }
                        {}

                        auto set_repetition(Repetition_T repetition) -> void
                        {
                            m_repetition = repetition;
                        }
                        auto set_looped_task(TaskBundle&& looped_task) -> void
                        {
                            m_looped_task = std::move(looped_task);
                        }
                        auto schedule_next_iteration(const TaskHubProxy& task_hub_proxy)
                            -> void
                        {
                            if (m_repetition > 0) {
                                --*m_repetition;
                                m_looped_task->operator()(task_hub_proxy);
                            }
                            else {
                                m_callback(task_hub_proxy);
                            }
                        }
                        auto call_callback(const TaskHubProxy& task_hub_proxy) -> void
                        {
                            m_callback(task_hub_proxy);
                        }

                    private:
                        std::optional<Repetition_T> m_repetition;
                        std::optional<TaskBundle>   m_looped_task;
                        TaskFinishedCallback<void>  m_callback;
                    };

                    std::shared_ptr<Looper> looper{
                        std::make_shared<Looper>(std::move(callback))   //
                    };

                    TaskBundle main_task =   //
                        ::sync(std::move(y_main_blueprint).materialize())
                            .build(
                                nexus,   //
                                task_hub_builder,
                                [looper](const TaskHubProxy& task_hub_proxy) -> void {
                                    looper->schedule_next_iteration(task_hub_proxy);
                                }
                            );
                    looper->set_looped_task(std::move(main_task));

                    TaskBundle repetition_specifier_task =   //
                        std::move(y_repetition_specifier_blueprint)
                            .materialize()
                            .build(
                                nexus,   //
                                task_hub_builder,
                                [looper](
                                    const TaskHubProxy& task_hub_proxy,
                                    const Repetition_T  repetition
                                ) -> void {
                                    looper->set_repetition(repetition);
                                    looper->schedule_next_iteration(task_hub_proxy);
                                }
                            );

                    return repetition_specifier_task;
                }
            };
        }   //
    };
}

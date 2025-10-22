module;

#include <memory>
#include <utility>

// TODO: remove once Clang can mangle
#include <function2/function2.hpp>

export module ddge.modules.execution.v2.primitives.run_if;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.gatherers.WaitAll;
import ddge.modules.execution.v2.TaskBlueprint;
import ddge.modules.execution.v2.TaskBuilder;
import ddge.modules.execution.v2.TaskBuilderBundle;
import ddge.modules.execution.v2.TaskBundle;
import ddge.modules.execution.v2.TaskFinishedCallback;
import ddge.modules.execution.v2.TaskHubBuilder;
import ddge.modules.execution.v2.TaskHubProxy;

namespace ddge::exec::v2 {

export template <Cardinality cardinality_T>
auto run_if(
    TaskBlueprint<void, cardinality_T>&&        main_blueprint,
    TaskBlueprint<bool, Cardinality::eSingle>&& predicate_blueprint
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

template <ddge::exec::v2::Cardinality cardinality_T>
auto ddge::exec::v2::run_if(
    TaskBlueprint<void, cardinality_T>&&        main_blueprint,
    TaskBlueprint<bool, Cardinality::eSingle>&& predicate_blueprint
) -> TaskBlueprint<void, Cardinality::eSingle>
{
    return TaskBlueprint<void, Cardinality::eSingle>{
        [x_main_blueprint      = std::move(main_blueprint),
         x_predicate_blueprint = std::move(predicate_blueprint)]   //
        () mutable -> TaskBuilder<void>                            //
        {
            return TaskBuilder<void>{
                [y_main_blueprint      = std::move(x_main_blueprint),
                 y_predicate_blueprint = std::move(x_predicate_blueprint)](
                    Nexus&                       nexus,
                    TaskHubBuilder&              task_hub_builder,
                    TaskFinishedCallback<void>&& callback
                ) mutable -> TaskBundle   //
                {
                    const std::shared_ptr<TaskFinishedCallback<void>> shared_callback{
                        std::make_shared<TaskFinishedCallback<void>>(std::move(callback))
                    };

                    return std::move(y_predicate_blueprint)
                        .materialize()
                        .build(
                            nexus,   //
                            task_hub_builder,
                            [main_task =
                                 ::sync(std::move(y_main_blueprint).materialize())
                                     .build(
                                         nexus,
                                         task_hub_builder,
                                         [shared_callback](
                                             const TaskHubProxy& task_hub_proxy
                                         ) { shared_callback->operator()(task_hub_proxy); }
                                     ),
                             shared_callback]                             //
                            (const TaskHubProxy& task_hub_proxy,
                             const bool          should_execute) mutable -> void   //
                            {
                                if (should_execute) {
                                    main_task(task_hub_proxy);
                                }
                                else {
                                    shared_callback->operator()(task_hub_proxy);
                                }
                            }
                        );
                }   //
            };
        }   //
    };
}

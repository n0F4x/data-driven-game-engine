#pragma once

#include <vector>

#include "engine/Scene.hpp"
#include "engine/SceneGraph.hpp"

class Controller;
class Stage;


class Scheduler {
public:
  ///------------------------------///
 ///  Constructors / Destructors  ///
///------------------------------///
    [[nodiscard]] Scheduler(SceneGraph& sceneGraph);
    [[nodiscard]] Scheduler(const Scheduler&) = delete;
    [[nodiscard]] Scheduler(Scheduler&&) noexcept = default;

  ///--------------------///
 ///  Member functions  ///
///--------------------///
    void iterate(Controller& controller);
    void add_stage(Stage&& stage);

  ///------------------///
 ///  Static helpers  ///
///------------------///
    static [[nodiscard]] auto empty(Scheduler& scheduler) -> bool;

private:
  ///--------------------///
 ///  Member variables  ///
///--------------------///
    std::vector<Stage> stages;
    SceneGraph& sceneGraph;
    Scene prevScene;
    Scene scene;
};

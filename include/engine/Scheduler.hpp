#pragma once

#include <vector>
#include <functional>

#include "engine/Scene.hpp"

class Controller;
class SceneGraph;
class Stage;


class Scheduler {
public:
  ///------------------------------///
 ///  Constructors / Destructors  ///
///------------------------------///
    [[nodiscard]] explicit Scheduler(std::function<Scene()>&& sceneMaker);
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
    [[nodiscard]] static auto empty(Scheduler& scheduler) -> bool;

private:
  ///--------------------///
 ///  Member variables  ///
///--------------------///
    std::vector<Stage> stages;
    std::function<Scene()> sceneMaker;
    Scene prevScene;
    Scene scene;
};

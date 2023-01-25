#pragma once

#include <functional>
#include <vector>

#include "framework/Scene.hpp"

namespace engine {

class Controller;
class Stage;

class Scheduler final {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit Scheduler(std::function<fw::Scene()>&& t_sceneMaker);
    [[nodiscard]] Scheduler(const Scheduler&) = delete;
    [[nodiscard]] Scheduler(Scheduler&&) noexcept = default;

    ///--------------------///
    ///  Member functions  ///
    ///--------------------///
    [[nodiscard]] auto empty() const noexcept -> bool;
    void iterate(Controller& t_controller);
    void add_stage(Stage&& t_stage);

private:
    ///--------------------///
    ///  Member variables  ///
    ///--------------------///
    std::vector<Stage> m_stages;
    std::function<fw::Scene()> m_sceneMaker;
    fw::Scene m_previousScene;
    fw::Scene m_scene;
};

}   // namespace engine

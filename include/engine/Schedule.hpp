#pragma once

#include <functional>
#include <vector>

#include "framework/Scene.hpp"

namespace engine {

class App;
class Controller;
class Stage;

class Schedule final {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit Schedule(App& t_app);

    ///-----------///
    ///  Methods  ///
    ///-----------///
    void run();
    void add_stage(Stage&& t_stage);

private:
    void iterate(Controller& t_controller);

    ///-------------///
    ///  Variables  ///
    ///-------------///
    App& m_app;
    std::vector<Stage> m_stages;
    fw::Scene m_previousScene;
    fw::Scene m_scene;
};

}   // namespace engine

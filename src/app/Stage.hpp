#pragma once

#include <vector>

#include "Controller.hpp"

namespace app {

template <class ControllerType>
class BasicStage final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Controller = ControllerType;
    using System = std::function<void(Controller)>;
    using SystemContainer = std::vector<System>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit BasicStage(SystemContainer&& t_systems) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    void run(Controller t_controller) const;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    SystemContainer m_systems;
};

template <class ControllerType>
class BasicStage<ControllerType>::Builder {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Product = BasicStage<ControllerType>;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] explicit(false) operator Product() noexcept;
    [[nodiscard]] auto build() noexcept -> Product;

    [[nodiscard]] auto add_system(BasicStage::System&& t_system) -> Builder&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    SystemContainer m_systems;
};

using Stage = BasicStage<Controller&>;

}   // namespace app

#include "app/Stage.inl"

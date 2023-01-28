#pragma once

#include <algorithm>
#include <functional>
#include <future>
#include <ranges>
#include <vector>

#include "engine/Controller.hpp"

namespace engine {

template <class Controller>
class BasicStage final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using ControllerType = Controller;
    using SystemType = std::function<void(ControllerType&)>;
    using SystemContainer = std::vector<SystemType>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit BasicStage(SystemContainer&& t_systems) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto empty() const noexcept -> bool;
    void run(ControllerType& t_controller) const;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    SystemContainer m_systems;
};

template <class Controller>
class BasicStage<Controller>::Builder {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] explicit(false) operator BasicStage<Controller>() noexcept;
    [[nodiscard]] auto build() noexcept -> BasicStage<Controller>;

    [[nodiscard]] auto add_system(BasicStage::SystemType&& t_system)
        -> Builder&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    SystemContainer m_systems;
};

template <class Controller>
BasicStage<Controller>::BasicStage(SystemContainer&& t_systems) noexcept
    : m_systems{ std::move(t_systems) } {}

template <class Controller>
auto BasicStage<Controller>::empty() const noexcept -> bool {
    return std::ranges::empty(m_systems);
}

template <class Controller>
void BasicStage<Controller>::run(Controller& t_controller) const {
    std::vector<std::future<void>> futures;

    std::ranges::for_each(m_systems, [&futures, &t_controller](auto t_system) {
        futures.push_back(
            std::async(std::launch::async, t_system, std::ref(t_controller)));
    });

    // throw potential exception from threads
    std::ranges::for_each(futures, &std::future<void>::get);
}

template <class Controller>
BasicStage<Controller>::Builder::operator BasicStage<Controller>() noexcept {
    return build();
}

template <class Controller>
auto BasicStage<Controller>::Builder::build() noexcept
    -> BasicStage<Controller> {
    return BasicStage{ std::move(m_systems) };
}

template <class Controller>
auto BasicStage<Controller>::Builder::add_system(SystemType&& t_system)
    -> Builder& {
    m_systems.push_back(std::move(t_system));
    return *this;
}

using Stage = BasicStage<Controller>;

}   // namespace engine

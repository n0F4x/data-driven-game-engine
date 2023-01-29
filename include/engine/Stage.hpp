#pragma once

#include <algorithm>
#include <functional>
#include <future>
#include <ranges>
#include <vector>

#include "engine/Controller.hpp"

namespace engine {

template <class TController>
class BasicStage final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Controller = TController;
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

template <class TController>
class BasicStage<TController>::Builder {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Product = BasicStage<TController>;

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

template <class TController>
BasicStage<TController>::BasicStage(SystemContainer&& t_systems) noexcept
    : m_systems{ std::move(t_systems) } {}

template <class TController>
void BasicStage<TController>::run(Controller t_controller) const {
    std::vector<std::future<void>> futures;

    std::ranges::for_each(m_systems, [&futures, &t_controller](auto t_system) {
        futures.push_back(
            std::async(std::launch::async, t_system, std::ref(t_controller)));
    });

    // throw potential exception from threads
    std::ranges::for_each(futures, &std::future<void>::get);
}

template <class TController>
BasicStage<TController>::Builder::operator Product() noexcept {
    return build();
}

template <class TController>
auto BasicStage<TController>::Builder::build() noexcept -> Product {
    return BasicStage{ std::move(m_systems) };
}

template <class TController>
auto BasicStage<TController>::Builder::add_system(System&& t_system)
    -> Builder& {
    m_systems.push_back(std::move(t_system));
    return *this;
}

using Stage = BasicStage<Controller&>;

}   // namespace engine

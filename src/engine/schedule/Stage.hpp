#pragma once

#include <vector>
#include <functional>

namespace engine {

class Controller;

class Stage final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using System = std::function<void(Controller)>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit Stage(std::vector<System>&& t_systems) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    void run(Controller t_controller) const;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    std::vector<System> m_systems;
};

class Stage::Builder {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Product = Stage;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] explicit(false) operator Product() noexcept;
    [[nodiscard]] auto build() noexcept -> Product;

    [[nodiscard]] auto add_system(System&& t_system) -> Builder&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    std::vector<System> m_systems;
};

}   // namespace engine

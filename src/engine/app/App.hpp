#pragma once

#include <functional>
#include <optional>
#include <type_traits>

#include <entt/entity/registry.hpp>

namespace engine {

template <typename Func, class App>
concept Runner = std::is_nothrow_invocable_v<Func, App&>;

class App {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Context = entt::registry::context;

    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;
    class Instance;

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create() noexcept -> Builder;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto run(const Runner<App> auto& t_runner) noexcept -> void;

    [[nodiscard]] auto context() noexcept -> Context&;

private:
    ///******************///
    ///  Friend Classes  ///
    ///******************///
    friend Builder;

    ///*************///
    ///  Variables  ///
    ///*************///
    Context m_context;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit App(Context&& t_context) noexcept;
};

}   // namespace engine

#include "App.inl"

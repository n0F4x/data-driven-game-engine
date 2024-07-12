#pragma once

#include <type_traits>

#include "store/Store.hpp"

class App {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]]
    static auto create() -> Builder;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]]
    auto plugins() noexcept -> Store&;
    [[nodiscard]]
    auto plugins() const noexcept -> const Store&;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    Store m_plugins;
};

#include "Builder.hpp"

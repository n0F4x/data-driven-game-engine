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

    explicit App(Builder&& t_builder);

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]]
    auto store() noexcept -> Store&;
    [[nodiscard]]
    auto store() const noexcept -> const Store&;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    Store m_store;
};

#include "Builder.hpp"

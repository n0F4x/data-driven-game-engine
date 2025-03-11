module;

#include <concepts>

#ifdef ENGINE_ENABLE_STATIC_TESTS
  #include <tuple>
#endif

export module core.app.Builder;

import :extension_c;
import :details;

import core.app.App;

namespace core::app {

export using Builder = ::BasicBuilder<>;

export template <typename Builder_T>
concept builder_c =
    std::derived_from<std::remove_cvref_t<Builder_T>, BasicBuilderBase<RootExtension>>;

export template <typename Extension_T>
concept extension_c = ::extension_c<Extension_T>;

export template <typename Builder_T, typename... Extensions_T>
concept extended_with_c = builder_c<Builder_T>
                       && (std::derived_from<std::remove_cvref_t<Builder_T>, Extensions_T>
                           && ...);

}   // namespace core::app

module :private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

static_assert(
    [] {
        std::ignore = core::app::Builder{};

        return true;
    }(),
    "create test failed"
);

static_assert(
    [] {
        struct Dummy1 {};
        struct Dummy2 {};

        std::ignore = core::app::Builder{}.extend_with<Dummy1>().extend_with<Dummy2>();

        return true;
    }(),
    "extend_with test failed"
);

static_assert(
    [] {
        struct Dummy1 {};
        struct Dummy2 {};

        std::ignore = core::app::Builder{}.extend_with<Dummy1>().swap_extension<Dummy1>(
            [](Dummy1) { return Dummy2{}; }
        );

        return true;
    }(),
    "swap test failed"
);

#endif

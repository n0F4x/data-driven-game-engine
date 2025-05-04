#include <memory>
#include <vector>

import utility.containers.StackedTuple;

namespace {

struct First {
    int value{ 42 };
};

struct Second {
    std::reference_wrapper<const int> ref;
};

[[nodiscard]]
constexpr auto make_first() -> First
{
    return First{};
}

[[nodiscard]]
constexpr auto make_second_fn(First& first) -> Second
{
    return Second{ .ref = first.value };
}

auto make_second_lambda{ [] [[nodiscard]]
                         (const First& first) -> Second {
                             return Second{ .ref = first.value };
                         } };

struct MakeSecondFunctor {
    [[nodiscard]]
    constexpr auto operator()(First& first) const -> Second
    {
        return Second{ .ref = first.value };
    }
};

}   // namespace

static_assert(
    [] {
        util::StackedTuple<>{};

        return true;
    }(),
    "empty creation test failed"
);

static_assert(
    [] {
        util::StackedTuple<First, Second> stacked_tuple{ make_first, make_second_fn };

        return stacked_tuple.get<First>().value == stacked_tuple.get<Second>().ref.get();
    }(),
    "inject with function pointer test failed"
);

static_assert(
    [] {
        util::StackedTuple<First, Second> stacked_tuple{ make_first, make_second_lambda };

        return stacked_tuple.get<First>().value == stacked_tuple.get<Second>().ref.get();
    }(),
    "inject with lambda test failed"
);

static_assert(
    [] {
        util::StackedTuple<First, Second> stacked_tuple{ make_first, MakeSecondFunctor{} };

        return stacked_tuple.get<First>().value == stacked_tuple.get<Second>().ref.get();
    }(),
    "inject with functor test failed"
);

static_assert(
    [] {
        std::vector<int> order;
        order.reserve(3);

        struct OrderWriter {
            std::reference_wrapper<std::vector<int>> order;
            int                                      number;

            constexpr ~OrderWriter()
            {
                order.get().push_back(number);
            }
        };

        auto stacked_tuple =
            std::make_unique<util::StackedTuple<OrderWriter, OrderWriter, OrderWriter>>(
                [&order] { return OrderWriter{ .order = order, .number = 0 }; },
                [&order] { return OrderWriter{ .order = order, .number = 1 }; },
                [&order] { return OrderWriter{ .order = order, .number = 2 }; }
            );

        stacked_tuple.reset();

        const std::vector expected{ 2, 1, 0 };

        return order == expected;
    }(),
    "destruction order test failed"
);

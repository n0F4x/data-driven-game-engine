export module ddge.utility.no_op;

namespace ddge::util {

export struct NoOp {
    constexpr static auto operator()(auto&&...) -> void {}
};

export inline constexpr NoOp no_op;

}   // namespace ddge::util

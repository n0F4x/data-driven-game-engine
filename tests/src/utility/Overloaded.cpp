import ddge.utility.Overloaded;

static_assert(
    [] {
        ddge::util::Overloaded{ [] {} };

        return true;
    }(),
    "construct from lambda"
);

auto function() -> void {}

static_assert(
    [] {
        ddge::util::Overloaded{ function };

        return true;
    }(),
    "construct from function"
);

static_assert(
    [] {
        ddge::util::Overloaded{ [] static {} };

        return true;
    }(),
    "construct from static lambda"
);

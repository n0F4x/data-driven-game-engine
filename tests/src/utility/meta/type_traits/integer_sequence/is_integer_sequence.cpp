import ddge.utility.meta.type_traits.integer_sequence.is_integer_sequence;

namespace {

template <typename T, T...>
struct Sequence {};

}   // namespace

static_assert(ddge::util::meta::is_integer_sequence_v<Sequence<int>>);
static_assert(ddge::util::meta::is_integer_sequence_v<Sequence<int, 1, 0>>);

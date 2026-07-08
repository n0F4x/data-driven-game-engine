import ddge.util.meta.concepts.functional.function_reference;

namespace ddge::util::meta {

static_assert(!function_reference_c<void()>);
static_assert(function_reference_c<void (&)()>);
static_assert(!function_reference_c<void (*)()>);

}   // namespace ddge::util::meta

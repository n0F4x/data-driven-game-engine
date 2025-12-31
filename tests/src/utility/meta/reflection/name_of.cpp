#include <string_view>

import ddge.utility.meta.reflection.name_of;

template <typename T>
struct Dummy {
    int member;
};

static_assert(
    ddge::util::meta::name_of<&Dummy<int(void) const && noexcept>::member>() == "member"
);

module;

#include <cstdint>

export module ddge.utility.meta.reflection.hash;

import ddge.utility.meta.reflection.name_of;

namespace ddge::util::meta {

// TODO: use reflection instead

export using TypeHash = uint64_t;

// TODO: make it consteval once meta::name_of<T>() gives the same output for inlined
//       namespaces
export template <typename T>
constexpr auto hash() noexcept -> TypeHash;

}   // namespace ddge::util::meta

namespace ddge::util::meta {

template <typename T>
constexpr auto hash() noexcept -> TypeHash
{
    // "Fowler–Noll–Vo - 1a" hash function
    constexpr TypeHash offset{ 14'695'981'039'346'656'037ull };
    constexpr TypeHash prime{ 1'099'511'628'211ull };

    TypeHash result{ offset };

    for (const auto character : name_of<T>()) {
        result = (result ^ static_cast<TypeHash>(character)) * prime;
    }

    return result;
}

}   // namespace ddge::util::meta

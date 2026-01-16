module;

#include <cstdint>

export module ddge.utility.meta.reflection.hash;

import ddge.utility.meta.reflection.name_of;

namespace ddge::util::meta {

// TODO: use reflection instead

export using TypeHash = uint64_t;

export template <typename T>
consteval auto hash() noexcept -> TypeHash;

}   // namespace ddge::util::meta

namespace ddge::util::meta {

template <typename T>
consteval auto hash() noexcept -> TypeHash
{
    // "Fowler–Noll–Vo - 1a" hash function
    constexpr TypeHash offset = 14'695'981'039'346'656'037;
    constexpr TypeHash prime  = 1'099'511'628'211;

    TypeHash result{ offset };

    for (const auto character : name_of<T>()) {
        result = (result ^ static_cast<TypeHash>(character)) * prime;
    }

    return result;
}

}   // namespace ddge::util::meta

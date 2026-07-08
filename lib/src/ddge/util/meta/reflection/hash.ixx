module;

#include <cstdint>
#include <string_view>

export module ddge.util.meta.reflection.hash;

import ddge.util.meta.reflection.name_of;

namespace ddge::util::meta {

export template <typename T>
[[nodiscard]]
consteval auto hash_u32() noexcept -> uint32_t;

export template <typename T>
[[nodiscard]]
consteval auto hash_u64() noexcept -> uint64_t;

}   // namespace ddge::util::meta

namespace ddge::util::meta {

constexpr auto hash_u32(const std::string_view string) noexcept -> uint32_t
{
    /*
     * "Fowler–Noll–Vo - 1a" hash function
     */

    constexpr uint32_t offset{ 2'166'136'261ull };
    // ReSharper disable once CppTooWideScope
    constexpr uint32_t prime{ 16'777'619ull };

    uint32_t result{ offset };

    for (const auto character : string) {
        result = (result ^ static_cast<uint32_t>(character)) * prime;
    }

    return result;
}

constexpr auto hash_u64(const std::string_view string) noexcept -> uint64_t
{
    /*
     * "Fowler–Noll–Vo - 1a" hash function
     */

    constexpr uint64_t offset{ 14'695'981'039'346'656'037ull };
    // ReSharper disable once CppTooWideScope
    constexpr uint64_t prime{ 1'099'511'628'211ull };

    uint64_t result{ offset };

    for (const auto character : string) {
        result = (result ^ static_cast<uint64_t>(character)) * prime;
    }

    return result;
}

// TODO: use reflection

template <typename T>
consteval auto hash_u32() noexcept -> uint32_t
{
    return hash_u32(name_of<T>());
}

template <typename T>
consteval auto hash_u64() noexcept -> uint64_t
{
    return hash_u64(name_of<T>());
}

}   // namespace ddge::util::meta

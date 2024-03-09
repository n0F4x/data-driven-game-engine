#pragma once

#include <filesystem>
#include <span>

#include <tl/optional.hpp>

#include "engine/common/Cache.hpp"
#include "engine/common/Handle.hpp"
#include "engine/common/ID.hpp"

#include "Image.hpp"

namespace engine::renderer {

class ImageLoader {
public:
    [[nodiscard]] static auto hash(const std::filesystem::path& t_filepath) -> ID;

    ImageLoader() noexcept = default;
    explicit ImageLoader(Cache& t_cache) noexcept;

    [[nodiscard]] auto load_from_file(const std::filesystem::path& t_filepath)
        -> tl::optional<Handle<Image>>;

    [[nodiscard]] auto load_from_memory(std::span<const std::uint8_t> t_data)
        -> tl::optional<Handle<Image>>;

private:
    tl::optional<Cache&> m_cache;
};

}   // namespace engine::renderer

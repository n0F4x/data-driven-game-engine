#pragma once

#include <filesystem>
#include <span>

#include <tl/optional.hpp>

#include "core/common/Cache.hpp"
#include "core/common/Handle.hpp"
#include "core/common/ID.hpp"

#include "Image.hpp"

namespace core::renderer {

class ImageLoader {
public:
    ImageLoader() noexcept = default;
    explicit ImageLoader(Cache& t_cache) noexcept;

    [[nodiscard]] auto load_from_file(const std::filesystem::path& t_filepath)
        -> tl::optional<Handle<Image>>;

    [[nodiscard]] auto load_from_memory(std::span<const std::uint8_t> t_data)
        -> tl::optional<Handle<Image>>;

private:
    tl::optional<Cache&> m_cache;
};

}   // namespace core::renderer

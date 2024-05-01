#pragma once

#include <filesystem>
#include <span>

#include <tl/optional.hpp>

#include "core/cache/Cache.hpp"
#include "core/cache/Handle.hpp"
#include "core/graphics/model/Image.hpp"

namespace core::graphics {

class ImageLoader {
public:
    ImageLoader() noexcept = default;
    explicit ImageLoader(cache::Cache& t_cache) noexcept;

    [[nodiscard]]
    auto load_from_file(const std::filesystem::path& t_filepath
    ) -> tl::optional<cache::Handle<Image>>;

    [[nodiscard]]
    auto load_from_memory(std::span<const std::uint8_t> t_data
    ) -> tl::optional<cache::Handle<Image>>;

private:
    tl::optional<cache::Cache&> m_cache;
};

}   // namespace core::graphics

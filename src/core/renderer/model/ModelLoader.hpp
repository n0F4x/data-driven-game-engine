#pragma once

#include <filesystem>

#include <tl/optional.hpp>

#include "core/renderer/base/allocator/Allocator.hpp"

#include "Model.hpp"
#include "StagingModel.hpp"

namespace core::renderer {

class ModelLoader {
public:
    [[nodiscard]] static auto load_from_file(
        const std::filesystem::path& t_filepath,
        const Allocator&             t_allocator
    ) noexcept -> tl::optional<StagingModel>;
};

}   // namespace core::renderer

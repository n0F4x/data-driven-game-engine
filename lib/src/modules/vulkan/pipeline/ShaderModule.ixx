module;

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <span>
#include <vector>

export module ddge.modules.vulkan.pipeline.ShaderModule;

namespace ddge::vulkan {

export class ShaderModule {
public:
    [[nodiscard]]
    static auto load_from_file(const std::filesystem::path& filepath)
        -> std::optional<ShaderModule>;

    [[nodiscard]]
    auto code() const noexcept -> std::span<const uint32_t>;

private:
    std::vector<std::byte> m_code;

    ShaderModule() = default;
};

}   // namespace ddge::vulkan

module :private;

namespace ddge::vulkan {

auto ShaderModule::load_from_file(const std::filesystem::path& filepath)
    -> std::optional<ShaderModule>
{
    std::ifstream file{ filepath, std::ios::binary | std::ios::in | std::ios::ate };

    const std::streamsize file_size{ file.tellg() };
    if (file_size == -1) {
        return std::nullopt;
    }

    static_assert(sizeof(std::byte) == sizeof(std::ifstream::char_type));

    std::optional<ShaderModule> result{ ShaderModule{} };
    result->m_code.resize(static_cast<std::size_t>(file_size));

    file.seekg(0, std::ios::beg);
    file.read(
        reinterpret_cast<std::istream::char_type*>(result->m_code.data()), file_size
    );

    return result;
}

auto ShaderModule::code() const noexcept -> std::span<const uint32_t>
{
    return std::span<const uint32_t>{
        reinterpret_cast<const uint32_t*>(m_code.data()),
        m_code.size() / (sizeof(uint32_t) / sizeof(std::byte))   //
    };
}

}   // namespace ddge::vulkan

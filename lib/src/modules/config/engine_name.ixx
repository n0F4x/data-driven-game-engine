export module ddge.modules.config.engine_name;

namespace ddge::config {

export [[nodiscard]]
constexpr auto engine_name() noexcept -> const char*;

}   // namespace ddge::config

module :private;

constexpr auto ddge::config::engine_name() noexcept -> const char*
{
    constexpr static const char* result{ "ENGINE" };
    return result;
}

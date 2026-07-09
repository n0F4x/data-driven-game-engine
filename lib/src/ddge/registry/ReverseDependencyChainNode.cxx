module;

#include <cassert>
#include <cstring>
#include <format>
#include <string>
#include <string_view>

module ddge.registry.ReverseDependencyChainNode;

namespace ddge::registry {

auto ReverseDependencyChainNode::contains(const uint64_t hash) const noexcept -> bool
{
    return this->hash == hash || (previous != nullptr && previous->contains(hash));
}

auto ReverseDependencyChainNode::format(
    const std::pmr::string::allocator_type& allocator
) const -> std::pmr::string
{
    std::pmr::string result{ allocator };
    format(result, 0);
    return result;
}

auto ReverseDependencyChainNode::format(
    std::pmr::string& out,
    const std::size_t capacity
) const -> void
{
    if (previous != nullptr)
    {
        previous->format(out, capacity + std::strlen(" <- ") + name.length());
        out.append(" <- ");
    }
    else
    {
        assert(
            out.empty()
            && "this invocation should be the one to start building the string"
        );
        out.reserve(capacity + name.size());
    }

    out.append(std::format("{}", name));
}

}   // namespace ddge::registry

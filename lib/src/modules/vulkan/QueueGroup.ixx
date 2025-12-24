module;

#include <cstdint>
#include <optional>
#include <utility>
#include <vector>

#include "utility/contract_macros.hpp"

export module ddge.modules.vulkan.QueueGroup;

import vulkan_hpp;

import ddge.modules.vulkan.QueuePack;
import ddge.utility.containers.OptionalRef;
import ddge.utility.contracts;
import ddge.utility.meta.type_traits.const_like;

namespace ddge::vulkan {

struct QueueGroupCreateInfo {
    std::vector<QueuePack>  queue_packs;
    std::optional<uint32_t> graphics_queue_pack_index;
    std::optional<uint32_t> compute_queue_pack_index;
    std::optional<uint32_t> host_to_device_transfer_queue_pack_index;
    std::optional<uint32_t> device_to_host_transfer_queue_pack_index;
    std::optional<uint32_t> dedicated_sparse_binding_queue_pack_index;
};

struct QueueGroupPrecondition {
    explicit QueueGroupPrecondition(const QueueGroupCreateInfo& create_info)
    {
        [[maybe_unused]]
        const auto is_valid =
            [&create_info](const std::optional<uint32_t>& queue_index) -> bool {
            return !queue_index.has_value()
                || (*queue_index < create_info.queue_packs.size()
                    && *create_info.queue_packs[*queue_index].queue != nullptr);
        };

        PRECOND(is_valid(create_info.graphics_queue_pack_index));
        PRECOND(is_valid(create_info.compute_queue_pack_index));
        PRECOND(is_valid(create_info.host_to_device_transfer_queue_pack_index));
        PRECOND(is_valid(create_info.device_to_host_transfer_queue_pack_index));
        PRECOND(is_valid(create_info.dedicated_sparse_binding_queue_pack_index));
    }
};

export class QueueGroup : QueueGroupPrecondition {
public:
    using CreateInfo = QueueGroupCreateInfo;

    explicit QueueGroup(CreateInfo&& create_info)
        : QueueGroupPrecondition{ create_info },
          m_queue_packs{ std::move(create_info.queue_packs) },
          m_graphics_queue_pack_index{ create_info.graphics_queue_pack_index },
          m_compute_queue_pack_index{ create_info.compute_queue_pack_index },
          m_host_to_device_transfer_queue_pack_index{
              create_info.host_to_device_transfer_queue_pack_index
          },
          m_device_to_host_transfer_queue_pack_index{
              create_info.device_to_host_transfer_queue_pack_index
          },
          m_dedicated_sparse_binding_queue_pack_index{
              create_info.dedicated_sparse_binding_queue_pack_index
          }
    {}

    [[nodiscard]]
    auto graphics_queue_family() const noexcept -> std::optional<uint32_t>
    {
        return family_at(m_graphics_queue_pack_index);
    }

    template <typename Self_T>
    [[nodiscard]]
    auto graphics_queue(this Self_T& self) noexcept
        -> util::OptionalRef<util::meta::const_like_t<vk::raii::Queue, Self_T>>
    {
        return self.queue_at(self.m_graphics_queue_pack_index);
    }

    [[nodiscard]]
    auto compute_queue_family() const noexcept -> std::optional<uint32_t>
    {
        return family_at(m_compute_queue_pack_index);
    }

    template <typename Self_T>
    [[nodiscard]]
    auto compute_queue(this Self_T& self) noexcept
        -> util::OptionalRef<util::meta::const_like_t<vk::raii::Queue, Self_T>>
    {
        return self.queue_at[self.m_compute_queue_pack_index];
    }

    [[nodiscard]]
    auto host_to_device_tranfer_queue_family() const noexcept -> std::optional<uint32_t>
    {
        return family_at(m_host_to_device_transfer_queue_pack_index);
    }

    template <typename Self_T>
    [[nodiscard]]
    auto host_to_device_tranfer_queue(this Self_T& self) noexcept
        -> util::OptionalRef<util::meta::const_like_t<vk::raii::Queue, Self_T>>
    {
        return self.queue_at[self.m_host_to_device_transfer_queue_pack_index];
    }

    [[nodiscard]]
    auto device_to_host_transfer_queue_family() const noexcept -> std::optional<uint32_t>
    {
        return family_at(m_device_to_host_transfer_queue_pack_index);
    }

    template <typename Self_T>
    [[nodiscard]]
    auto device_to_host_transfer_queue(this Self_T& self) noexcept
        -> util::OptionalRef<util::meta::const_like_t<vk::raii::Queue, Self_T>>
    {
        return self.queue_at[self.m_device_to_host_transfer_queue_pack_index];
    }

    [[nodiscard]]
    auto dedicated_sparse_binding_queue_family() const noexcept -> std::optional<uint32_t>
    {
        return family_at(m_dedicated_sparse_binding_queue_pack_index);
    }

    template <typename Self_T>
    [[nodiscard]]
    auto dedicated_sparse_binding_queue(this Self_T& self) noexcept
        -> util::OptionalRef<util::meta::const_like_t<vk::raii::Queue, Self_T>>
    {
        return self.queue_at[self.m_dedicated_sparse_binding_queue_pack_index];
    }

private:
    std::vector<QueuePack>  m_queue_packs;
    std::optional<uint32_t> m_graphics_queue_pack_index;
    std::optional<uint32_t> m_compute_queue_pack_index;
    std::optional<uint32_t> m_host_to_device_transfer_queue_pack_index;
    std::optional<uint32_t> m_device_to_host_transfer_queue_pack_index;
    std::optional<uint32_t> m_dedicated_sparse_binding_queue_pack_index;

    [[nodiscard]]
    auto family_at(const std::optional<uint32_t>& opt_index) const noexcept
        -> std::optional<uint32_t>
    {
        return opt_index.transform([this](const uint32_t index) noexcept -> uint32_t {
            return m_queue_packs[index].family_index;
        });
    }

    template <typename Self_T>
    [[nodiscard]]
    auto queue_at(this Self_T& self, const std::optional<uint32_t>& opt_index) noexcept
        -> util::OptionalRef<util::meta::const_like_t<vk::raii::Queue, Self_T>>
    {
        if (opt_index.has_value()) {
            return self.m_queue_packs[*opt_index].queue;
        }
        return std::nullopt;
    }
};

}   // namespace ddge::vulkan

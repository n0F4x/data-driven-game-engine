module;

#include <filesystem>
#include <source_location>

#include <vulkan/vulkan.hpp>

#include <VkBootstrap.h>

#include <glm/ext/vector_float3.hpp>

module demos.virtual_texture.VirtualTexture;

import core.gfx.resources.VirtualImage;

import core.image.jpeg.Image;

import core.renderer.base.device.Device;
import core.renderer.base.allocator.Allocator;
import core.renderer.resources.Buffer;
import core.renderer.resources.SeqWriteBuffer;

import demos.virtual_texture.init;
import demos.virtual_texture.Vertex;

template <typename T>
[[nodiscard]]
static auto create_staging_buffer(
    const core::renderer::base::Allocator& allocator,
    const std::span<const T>               data
) -> core::renderer::resources::SeqWriteBuffer<std::remove_const_t<T>>
{
    const vk::BufferCreateInfo staging_buffer_create_info{
        .size  = data.size_bytes(),
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
    };

    return core::renderer::resources::SeqWriteBuffer<std::remove_const_t<T>>{
        allocator, staging_buffer_create_info, data.data()
    };
}

[[nodiscard]]
static auto create_gpu_only_buffer(
    const core::renderer::base::Allocator& allocator,
    const vk::BufferUsageFlags             usage_flags,
    const uint32_t                         size
) -> core::renderer::resources::Buffer
{
    const vk::BufferCreateInfo buffer_create_info = {
        .size = size, .usage = usage_flags | vk::BufferUsageFlagBits::eTransferDst
    };

    return core::renderer::resources::Buffer{ allocator, buffer_create_info };
}

template <std::invocable<vk::CommandBuffer> Executor>
static auto
    execute_command(const core::renderer::base::Device& device, Executor&& executor)
        -> void
{
    const vk::UniqueCommandPool command_pool{ examples::base::init::create_command_pool(
        device.get(), device.info().get_queue_index(vkb::QueueType::graphics).value()
    ) };
    const vk::CommandBufferAllocateInfo command_buffer_allocate_info{
        .commandPool        = command_pool.get(),
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
    };
    const vk::CommandBuffer command_buffer{
        device->allocateCommandBuffers(command_buffer_allocate_info).front()
    };
    constexpr static vk::CommandBufferBeginInfo begin_info{};
    command_buffer.begin(begin_info);


    std::invoke(std::forward<Executor>(executor), command_buffer);


    command_buffer.end();
    const vk::SubmitInfo submit_info{
        .commandBufferCount = 1,
        .pCommandBuffers    = &command_buffer,
    };
    vk::UniqueFence fence{ device->createFenceUnique({}) };
    static_cast<vk::Queue>(device.info().get_queue(vkb::QueueType::graphics).value())
        .submit(submit_info, fence.get());
    std::ignore =
        device->waitForFences(std::array{ fence.get() }, vk::True, 100'000'000'000);
    device->resetCommandPool(command_pool.get());
}

[[nodiscard]]
static auto create_vertex_buffer(
    const core::renderer::base::Device&    device,
    const core::renderer::base::Allocator& allocator,
    const glm::vec3&                       center,
    const float                            scale = 1.f
) -> core::renderer::resources::Buffer
{
    const float scale_divided_by_2{ scale / 2 };

    const std::array vertices{
        demo::Vertex{ .position = { center.x - scale_divided_by_2,
 center.y + scale_divided_by_2,
 center.z },
                     .uv       = { 1.0f, 0.0f } },
        demo::Vertex{ .position = { center.x + scale_divided_by_2,
 center.y + scale_divided_by_2,
 center.z },
                     .uv       = { 0.0f, 0.0f } },
        demo::Vertex{ .position = { center.x + scale_divided_by_2,
 center.y - scale_divided_by_2,
 center.z },
                     .uv       = { 0.0f, 1.0f } },
        demo::Vertex{ .position = { center.x - scale_divided_by_2,
 center.y - scale_divided_by_2,
 center.z },
                     .uv       = { 1.0f, 1.0f } }
    };

    const core::renderer::resources::SeqWriteBuffer staging_buffer{
        ::create_staging_buffer<demo::Vertex>(allocator, vertices)
    };
    staging_buffer.set(std::span{ vertices });

    core::renderer::resources::Buffer vertex_buffer{ ::create_gpu_only_buffer(
        allocator,
        vk::BufferUsageFlagBits::eVertexBuffer,
        static_cast<uint32_t>(staging_buffer.size_bytes())
    ) };

    ::execute_command(device, [&](const vk::CommandBuffer command_buffer) {
        const vk::BufferCopy copy_region{
            .size = static_cast<uint32_t>(staging_buffer.size_bytes()),
        };
        command_buffer.copyBuffer(
            staging_buffer.get(), vertex_buffer.buffer().get(), copy_region
        );
    });

    return vertex_buffer;
}

[[nodiscard]]
static auto create_index_buffer(
    const core::renderer::base::Device&    device,
    const core::renderer::base::Allocator& allocator
) -> core::renderer::resources::Buffer
{
    constexpr static std::array indices{ uint32_t{ 0 }, uint32_t{ 1 }, uint32_t{ 2 },
                                         uint32_t{ 2 }, uint32_t{ 3 }, uint32_t{ 0 } };

    const core::renderer::resources::SeqWriteBuffer staging_buffer{
        ::create_staging_buffer<uint32_t>(allocator, indices)
    };
    staging_buffer.set(std::span{ indices });

    core::renderer::resources::Buffer index_buffer{ ::create_gpu_only_buffer(
        allocator,
        vk::BufferUsageFlagBits::eIndexBuffer,
        static_cast<uint32_t>(staging_buffer.size_bytes())
    ) };

    ::execute_command(device, [&](const vk::CommandBuffer command_buffer) {
        const vk::BufferCopy copy_region{
            .size = static_cast<uint32_t>(staging_buffer.size_bytes()),
        };
        command_buffer.copyBuffer(
            staging_buffer.get(), index_buffer.buffer().get(), copy_region
        );
    });

    return index_buffer;
}

demo::VirtualTexture::VirtualTexture(
    const core::renderer::base::Device&    device,
    const core::renderer::base::Allocator& allocator
)
    : m_device_ref{ device },
      m_allocator_ref{ allocator },
      m_position{ -0.5, 0.f, -1.f },
      m_debug_position{ 0.5, 0.f, -1.f },
      m_vertex_buffer{ ::create_vertex_buffer(device, allocator, m_position, 1.f) },
      m_debug_vertex_buffer{ ::create_vertex_buffer(device, allocator, m_debug_position) },
      m_index_buffer{ ::create_index_buffer(device, allocator) },
      m_virtual_image{ init::create_virtual_image(
          device,
          allocator,
          std::make_unique<core::image::jpeg::Image>(core::image::jpeg::Image::load_from(
              std::filesystem::path{ std::source_location::current().file_name() }
                  .parent_path()
                  .parent_path()
                  .parent_path()
                  .parent_path()
              / "assets/textures/wheat_field.jpg"
          ))
      ) },
      m_virtual_image_sampler{ init::create_virtual_image_sampler(device) },
      m_virtual_image_debug_sampler{ init::create_virtual_image_sampler(device) }
{}

auto demo::VirtualTexture::position() const noexcept -> const glm::vec3&
{
    return m_position;
}

auto demo::VirtualTexture::debug_position() const noexcept -> const glm::vec3&
{
    return m_debug_position;
}

auto demo::VirtualTexture::get() noexcept -> core::gfx::resources::VirtualImage&
{
    return m_virtual_image;
}

auto demo::VirtualTexture::get() const noexcept
    -> const core::gfx::resources::VirtualImage&
{
    return m_virtual_image;
}

auto demo::VirtualTexture::view() const noexcept -> vk::ImageView
{
    return m_virtual_image.view();
}

auto demo::VirtualTexture::debug_view() const noexcept -> vk::ImageView
{
    return m_virtual_image.debug_image().view();
}

auto demo::VirtualTexture::sampler() const noexcept -> vk::Sampler
{
    return m_virtual_image_sampler.get();
}

auto demo::VirtualTexture::debug_sampler() const noexcept -> vk::Sampler
{
    return m_virtual_image_debug_sampler.get();
}

auto demo::VirtualTexture::draw(const vk::CommandBuffer command_buffer) -> void
{
    ::execute_command(
        m_device_ref.get(),
        [this](const vk::CommandBuffer transfer_command_buffer) {
            m_virtual_image.update(
                m_allocator_ref.get(),
                m_device_ref.get().info().get_queue(vkb::QueueType::graphics).value(),
                transfer_command_buffer
            );
        }
    );
    m_virtual_image.clean_up_after_update();

    command_buffer.bindVertexBuffers(0, m_vertex_buffer.buffer().get(), { 0 });
    command_buffer.bindIndexBuffer(
        m_index_buffer.buffer().get(), 0, vk::IndexType::eUint32
    );

    command_buffer.drawIndexed(6, 1, 0, 0, 0);
}

auto demo::VirtualTexture::draw_debug(const vk::CommandBuffer command_buffer) const
    -> void
{
    command_buffer.bindVertexBuffers(0, m_debug_vertex_buffer.buffer().get(), { 0 });
    command_buffer.drawIndexed(6, 1, 0, 0, 0);
}

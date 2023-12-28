#include "demo.hpp"

#include <atomic>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

#include <tl/optional.hpp>

#include <SFML/Window.hpp>

#include <entt/core/hashed_string.hpp>

#include <engine/asset_manager/AssetRegistry.hpp>
#include <engine/renderer/Allocator.hpp>
#include <engine/renderer/Device.hpp>
#include <engine/renderer/scene/RenderObject.hpp>
#include <engine/renderer/Swapchain.hpp>
#include <engine/utility/converters.hpp>
#include <engine/utility/vma/Image.hpp>
#include <engine/window/Window.hpp>

#include "Camera.hpp"
#include "Controller.hpp"
#include "demo_init.hpp"

using namespace entt::literals;
using namespace engine;

constexpr uint32_t g_frame_count{ 2 };

struct DemoApp {
    renderer::Device&                  device;
    renderer::Allocator&               allocator;
    renderer::Swapchain&               swapchain;
    vk::UniqueRenderPass               render_pass;
    vma::Image                         depth_image;
    vk::UniqueImageView                depth_image_view;
    std::vector<vk::UniqueFramebuffer> framebuffers;
    vk::UniqueDescriptorSetLayout      descriptor_set_layout;
    vk::UniquePipelineLayout           pipeline_layout;
    vk::UniquePipeline                 pipeline;
    vk::UniqueCommandPool              command_pool;
    std::vector<vk::CommandBuffer>     command_buffers;
    vk::UniqueDescriptorPool           descriptor_pool;
    std::vector<vk::UniqueSemaphore>   image_acquired_semaphores;
    std::vector<vk::UniqueSemaphore>   render_finished_semaphores;
    std::vector<vk::UniqueFence>       in_flight_fences;
    uint32_t                           frame_index{};

    std::unique_ptr<renderer::MeshBuffer> mesh_buffer;
    renderer::RenderObject                render_object;

    [[nodiscard]] static auto create(
        Store&             t_store,
        const std::string& t_model_filepath
    ) -> tl::optional<DemoApp>
    {
        auto& device{ t_store.at<renderer::Device>() };
        auto& allocator{ t_store.at<renderer::Allocator>() };

        auto& swapchain{ t_store.at<renderer::Swapchain>() };
        swapchain.set_framebuffer_size(
            to_extent2D(t_store.at<window::Window>().framebuffer_size())
        );
        if (!swapchain.get()) {
            return tl::nullopt;
        }

        auto render_pass{
            init::create_render_pass(swapchain.get()->surface_format(), device)
        };
        if (!*render_pass) {
            return tl::nullopt;
        }

        auto depth_image{ init::create_depth_image(
            device.physical_device(), *allocator, swapchain.get()->extent()
        ) };
        if (!*depth_image) {
            return tl::nullopt;
        }

        auto depth_image_view{
            init::create_depth_image_view(device, *depth_image)
        };
        if (!*depth_image_view) {
            return tl::nullopt;
        }

        auto framebuffers{ init::create_framebuffers(
            *device,
            swapchain.get()->extent(),
            swapchain.get()->image_views(),
            *render_pass,
            *depth_image_view
        ) };
        if (framebuffers.empty()) {
            return tl::nullopt;
        }

        auto descriptor_set_layout{ init::create_descriptor_set_layout(*device
        ) };
        if (!*descriptor_set_layout) {
            return tl::nullopt;
        }

        auto pipeline_layout{ init::create_pipeline_layout(
            *device, *descriptor_set_layout, sizeof(Camera)
        ) };
        if (!*pipeline_layout) {
            return tl::nullopt;
        }

        auto pipeline{
            init::create_pipeline(*device, *pipeline_layout, *render_pass)
        };
        if (!*pipeline) {
            return tl::nullopt;
        }

        auto command_pool{ init::create_command_pool(
            *device, device.graphics_queue_family_index()
        ) };
        if (!*command_pool) {
            return tl::nullopt;
        }

        auto command_buffers{
            init::create_command_buffers(*device, *command_pool, g_frame_count)
        };
        if (command_buffers.empty()) {
            return tl::nullopt;
        }

        auto image_acquired_semaphores{
            init::create_semaphores(*device, g_frame_count)
        };
        if (image_acquired_semaphores.empty()) {
            return tl::nullopt;
        }

        auto render_finished_semaphores{
            init::create_semaphores(*device, g_frame_count)
        };
        if (render_finished_semaphores.empty()) {
            return tl::nullopt;
        }

        auto in_flight_fences{ init::create_fences(*device, g_frame_count) };
        if (in_flight_fences.empty()) {
            return tl::nullopt;
        }

        auto opt_model{ init::create_model(t_model_filepath) };
        if (!opt_model) {
            std::cout << "Model could not be created properly\n";
            return tl::nullopt;
        }
        auto model{ t_store.at<asset_manager::AssetRegistry>().emplace(
            std::move(*opt_model), "Model 0"_hs
        ) };
        auto descriptor_pool{
            init::create_descriptor_pool(*device, init::count_meshes(model))
        };
        if (!*descriptor_pool) {
            return tl::nullopt;
        }
        auto opt_mesh_buffer{ init::create_mesh_buffer(device, allocator, model) };
        if (!opt_mesh_buffer) {
            return tl::nullopt;
        }
        auto unique_mesh_buffer{
            std::make_unique<renderer::MeshBuffer>(std::move(*opt_mesh_buffer))
        };
        auto render_object{ renderer::RenderObject::create(
            *device,
            allocator,
            *descriptor_set_layout,
            *descriptor_pool,
            model,
            *unique_mesh_buffer
        ) };
        if (!render_object) {
            std::cout << "Model could not be loaded correctly\n";
            return tl::nullopt;
        }


        return DemoApp{
            .device                     = device,
            .allocator                  = allocator,
            .swapchain                  = swapchain,
            .render_pass                = std::move(render_pass),
            .depth_image                = std::move(depth_image),
            .depth_image_view           = std::move(depth_image_view),
            .framebuffers               = std::move(framebuffers),
            .descriptor_set_layout      = std::move(descriptor_set_layout),
            .pipeline_layout            = std::move(pipeline_layout),
            .pipeline                   = std::move(pipeline),
            .command_pool               = std::move(command_pool),
            .command_buffers            = std::move(command_buffers),
            .descriptor_pool            = std::move(descriptor_pool),
            .image_acquired_semaphores  = std::move(image_acquired_semaphores),
            .render_finished_semaphores = std::move(render_finished_semaphores),
            .in_flight_fences           = std::move(in_flight_fences),
            .mesh_buffer                = std::move(unique_mesh_buffer),
            .render_object              = std::move(*render_object),
        };
    }

    auto render(
        vk::Extent2D  t_framebuffer_size,
        const Camera& t_camera
    ) noexcept -> void
    {
        swapchain.set_framebuffer_size(t_framebuffer_size);

        while (device->waitForFences(
                   { *in_flight_fences[frame_index] }, true, UINT64_MAX
               )
               == vk::Result::eTimeout)
            ;

        swapchain
            .acquire_next_image(*image_acquired_semaphores[frame_index], {})
            .transform([&](uint32_t image_index) {
                device->resetFences({ *in_flight_fences[frame_index] });
                command_buffers[frame_index].reset();

                record_command_buffer(image_index, t_camera);

                std::array wait_semaphores{
                    *image_acquired_semaphores[frame_index]
                };
                std::array<vk::PipelineStageFlags, wait_semaphores.size()>
                    wait_stages{
                        vk::PipelineStageFlagBits::eColorAttachmentOutput
                    };
                std::array signal_semaphores{
                    *render_finished_semaphores[frame_index]
                };
                vk::SubmitInfo submit_info{
                    .waitSemaphoreCount =
                        static_cast<uint32_t>(wait_semaphores.size()),
                    .pWaitSemaphores    = wait_semaphores.data(),
                    .pWaitDstStageMask  = wait_stages.data(),
                    .commandBufferCount = 1,
                    .pCommandBuffers    = &command_buffers[frame_index],
                    .signalSemaphoreCount =
                        static_cast<uint32_t>(signal_semaphores.size()),
                    .pSignalSemaphores = signal_semaphores.data()
                };
                static_cast<void>(device.graphics_queue().submit(
                    submit_info, *in_flight_fences[frame_index]
                ));

                swapchain.present(signal_semaphores);
            });

        frame_index = (frame_index + 1) % g_frame_count;
    }

    auto record_command_buffer(uint32_t t_image_index, Camera t_camera) noexcept
        -> void
    {
        auto command_buffer = command_buffers[frame_index];
        vk::CommandBufferBeginInfo command_buffer_begin_info{};

        static_cast<void>(command_buffer.begin(command_buffer_begin_info));


        std::array clearValues{
            vk::ClearValue{
                .color = { std::array{ 0.01f, 0.01f, 0.01f, 0.01f } } },
            vk::ClearValue{ .depthStencil = { 1.f, 0 } }
        };

        vk::RenderPassBeginInfo render_pass_begin_info{
            .renderPass      = *render_pass,
            .framebuffer     = *framebuffers[t_image_index],
            .renderArea      = { .extent = swapchain.get()->extent() },
            .clearValueCount = static_cast<uint32_t>(clearValues.size()),
            .pClearValues    = clearValues.data()
        };

        command_buffer.beginRenderPass(
            render_pass_begin_info, vk::SubpassContents::eInline
        );

        auto extent{ swapchain.get()->extent() };
        command_buffer.setViewport(
            0,
            vk::Viewport{ .width    = static_cast<float>(extent.width),
                          .height   = static_cast<float>(extent.height),
                          .maxDepth = 1.f }
        );
        command_buffer.setScissor(0, vk::Rect2D{ {}, extent });

        command_buffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics, *pipeline
        );

        t_camera.set_perspective_projection(
            50.f,
            static_cast<float>(extent.width)
                / static_cast<float>(extent.height),
            0.1f,
            10000.f
        );
        command_buffer.pushConstants(
            *pipeline_layout,
            vk::ShaderStageFlagBits::eVertex,
            0,
            sizeof(Camera),
            &t_camera
        );

        render_object.draw(command_buffer, *pipeline_layout);


        command_buffer.endRenderPass();
        static_cast<void>(command_buffer.end());
    }
};

auto demo::run(engine::App& t_app, const std::string& t_model_filepath) noexcept
    -> int
{
    return DemoApp::create(t_app.store(), t_model_filepath)
        .transform([&](DemoApp t_demo) {
            t_demo.swapchain.on_swapchain_recreated(
                [&t_demo](const vulkan::Swapchain& t_swapchain) {
                    t_demo.depth_image.reset();
                    t_demo.depth_image = init::create_depth_image(
                        t_demo.device.physical_device(),
                        *t_demo.allocator,
                        t_swapchain.extent()
                    );
                }
            );
            t_demo.swapchain.on_swapchain_recreated(
                [&t_demo](const vulkan::Swapchain& t_swapchain) {
                    t_demo.depth_image_view.reset();
                    t_demo.depth_image_view = init::create_depth_image_view(
                        t_demo.device, *t_demo.depth_image
                    );
                }
            );
            t_demo.swapchain.on_swapchain_recreated(
                [&t_demo](const vulkan::Swapchain& t_swapchain) {
                    t_demo.framebuffers.clear();
                    t_demo.framebuffers = init::create_framebuffers(
                        *t_demo.device,
                        t_swapchain.extent(),
                        t_swapchain.image_views(),
                        *t_demo.render_pass,
                        *t_demo.depth_image_view
                    );
                }
            );

            bool        running{ true };
            const auto& window{ t_app.store().at<window::Window>() };
            sf::Event   event{};

            Controller controller;

            std::atomic<vk::Extent2D> framebuffer_size{};
            Camera                    camera;
            std::mutex                camera_lock{};

            auto rendering = std::async(std::launch::async, [&] {
                Camera render_camera;
                while (running) {
                    camera_lock.lock();
                    render_camera = camera;
                    camera_lock.unlock();
                    t_demo.render(framebuffer_size, render_camera);
                }
            });

            auto last_time = std::chrono::high_resolution_clock::now();
            while (running) {
                std::this_thread::sleep_for(
                    std::chrono::duration<float, std::chrono::seconds::period>{
                        1.f / 60.f }
                );
                auto now = std::chrono::high_resolution_clock::now();
                std::chrono::duration<float, std::chrono::seconds::period>
                    delta_time{ now - last_time };
                last_time = now;

                while (window->pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        running = false;
                    }
                    if (event.type == sf::Event::KeyPressed
                        && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                    {
                        running = false;
                    }
                    if (event.type == sf::Event::Resized) {
                        framebuffer_size.store(
                            to_extent2D(window.framebuffer_size())
                        );
                    }
                }

                controller.update(delta_time.count());
                camera_lock.lock();
                camera = controller.update_camera(camera);
                camera_lock.unlock();
            }

            rendering.get();
            static_cast<void>(t_demo.device->waitIdle());

            return 0;
        })
        .value_or(-1);
}

#include "demo.hpp"

#include <atomic>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>

#include <tl/optional.hpp>

#include <SFML/Window.hpp>

#include <engine/renderer/Device.hpp>
#include <engine/renderer/scene/RenderScene.hpp>
#include <engine/renderer/Swapchain.hpp>
#include <engine/utils/converters.hpp>
#include <engine/utils/vulkan/VmaImage.hpp>
#include <engine/window/Window.hpp>

#include "Camera.hpp"
#include "Controller.hpp"
#include "demo_init.hpp"

using namespace engine;

constexpr uint32_t g_frame_count{ 2 };

struct DemoApp {
    renderer::Device&                device;
    renderer::Swapchain&             swapchain;
    vulkan::RenderPass               render_pass;
    vulkan::VmaImage                 depth_image;
    vulkan::ImageView                depth_image_view;
    std::vector<vulkan::Framebuffer> framebuffers;
    vulkan::DescriptorSetLayout      descriptor_set_layout;
    vulkan::PipelineLayout           pipeline_layout;
    vulkan::Pipeline                 pipeline;
    vulkan::CommandPool              command_pool;
    std::vector<vk::CommandBuffer>   command_buffers;
    vulkan::DescriptorPool           descriptor_pool;
    std::vector<vulkan::Semaphore>   image_acquired_semaphores;
    std::vector<vulkan::Semaphore>   render_finished_semaphores;
    std::vector<vulkan::Fence>       in_flight_fences;
    uint32_t                         frame_index{};

    renderer::RenderScene  render_scene;
    renderer::RenderObject render_object;
    bool                   swapchain_recreated{ false };

    static auto flush_model(
        const renderer::Device& t_device,
        renderer::RenderScene&  t_render_scene
    ) noexcept -> void
    {
        auto transfer_command_pool{ init::create_command_pool(
            *t_device, t_device.transfer_queue_family_index()
        ) };
        vk::CommandBufferAllocateInfo command_buffer_allocate_info{
            .commandPool        = *transfer_command_pool,
            .level              = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = 1
        };
        auto command_buffer{
            t_device->allocateCommandBuffers(command_buffer_allocate_info)
                .value.front()
        };

        vk::CommandBufferBeginInfo begin_info{};
        static_cast<void>(command_buffer.begin(begin_info));
        t_render_scene.flush(command_buffer);
        static_cast<void>(command_buffer.end());

        vk::SubmitInfo submit_info{
            .commandBufferCount = 1,
            .pCommandBuffers    = &command_buffer,
        };
        vulkan::Fence fence{ *t_device, t_device->createFence({}).value };

        static_cast<void>(
            t_device.transfer_queue().submit(1, &submit_info, *fence)
        );

        auto raw_fence{ *fence };
        static_cast<void>(
            t_device->waitForFences(1, &raw_fence, true, 100'000'000'000)
        );
        t_device->resetCommandPool(*transfer_command_pool);
    }

    [[nodiscard]] static auto create(
        Store&             t_store,
        const std::string& t_model_filepath
    ) noexcept -> tl::optional<DemoApp>
    {
        auto opt_device{ t_store.find<renderer::Device>() };
        if (!opt_device) {
            return tl::nullopt;
        }
        auto& device{ *opt_device };

        auto opt_swapchain{ t_store.find<renderer::Swapchain>() };
        if (!opt_swapchain) {
            return tl::nullopt;
        }
        opt_swapchain->set_framebuffer_size(utils::to_extent2D(
            t_store.find<window::Window>().value().framebuffer_size()
        ));
        if (!opt_swapchain->get()) {
            return tl::nullopt;
        }
        auto& swapchain{ opt_swapchain.value() };

        auto render_pass{
            init::create_render_pass(swapchain.get()->surface_format(), device)
        };
        if (!*render_pass) {
            return tl::nullopt;
        }

        auto depth_image{
            init::create_depth_image(device, swapchain.get()->extent())
        };
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

        renderer::RenderScene render_scene;
        auto                  opt_model{ init::create_model(t_model_filepath) };
        if (!opt_model) {
            std::cout << "Model could not be created properly\n";
            return tl::nullopt;
        }
        auto descriptor_pool{ init::create_descriptor_pool(
            *device, init::count_meshes(*opt_model)
        ) };
        if (!*descriptor_pool) {
            return tl::nullopt;
        }
        auto render_object{
            std::move(opt_model)
                .and_then([&](auto&& t_model) {
                    return render_scene.load(
                        device, std::forward<decltype(t_model)>(t_model)
                    );
                })
                .and_then([&](auto t_model_handle) {
                    return t_model_handle.spawn(
                        device, *descriptor_set_layout, *descriptor_pool
                    );
                })
        };
        if (!render_object) {
            std::cout << "Model could not be loaded correctly\n";
            return tl::nullopt;
        }
        flush_model(device, render_scene);


        return DemoApp{
            .device                     = device,
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
            .render_scene               = std::move(render_scene),
            .render_object              = std::move(*render_object),
        };
    }

    auto render(vk::Extent2D t_framebuffer_size, Camera t_camera) noexcept
        -> void
    {
        swapchain.set_framebuffer_size(t_framebuffer_size);

        while (device->waitForFences(
                   { *in_flight_fences[frame_index] }, true, UINT64_MAX
               )
               == vk::Result::eTimeout)
            ;

        auto opt_image_index{ swapchain.acquire_next_image(
            *image_acquired_semaphores[frame_index], {}
        ) };

        if (opt_image_index) {
            device->resetFences({ *in_flight_fences[frame_index] });
            command_buffers[frame_index].reset();

            record_command_buffer(*opt_image_index, t_camera);

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
        }

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
    DemoApp::create(t_app.store(), t_model_filepath)
        .transform([&](DemoApp t_demo) {
            t_demo.swapchain.on_swapchain_recreated(
                [&t_demo](const vulkan::Swapchain& t_swapchain) {
                    t_demo.depth_image.destroy();
                    t_demo.depth_image = init::create_depth_image(
                        t_demo.device, t_swapchain.extent()
                    );
                }
            );
            t_demo.swapchain.on_swapchain_recreated(
                [&t_demo](const vulkan::Swapchain& t_swapchain) {
                    t_demo.depth_image_view.destroy();
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

            bool      running{ true };
            auto&     window{ t_app.store().find<window::Window>().value() };
            sf::Event event{};

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
                            utils::to_extent2D(window.framebuffer_size())
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
        });

    return 0;
}

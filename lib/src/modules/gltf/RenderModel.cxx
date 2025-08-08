module;

#include <filesystem>
#include <format>
#include <functional>
#include <future>
#include <ranges>
#include <source_location>

#include <VkBootstrap.h>

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>

module modules.gltf.RenderModel;

import modules.gltf.Image;
import modules.gltf.Material;
import modules.gltf.Mesh;
import modules.gltf.Texture;

import modules.renderer.base.device.Device;
import modules.renderer.base.resources.Buffer;
import modules.renderer.base.resources.Image;
import modules.renderer.material_system.GraphicsPipelineBuilder;
import modules.renderer.material_system.Program;
import modules.renderer.material_system.Shader;
import modules.renderer.material_system.ShaderModule;
import modules.renderer.model.ModelLayout;
import modules.renderer.resources.Buffer;
import modules.renderer.resources.RandomAccessBuffer;
import modules.renderer.resources.SeqWriteBuffer;

struct ShaderVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 UV0;
    glm::vec2 UV1;
    glm::vec4 color;
};

struct ShaderTexture {
    uint32_t image_index;
    uint32_t sampler_index;
};

struct ShaderTextureInfo {
    uint32_t index;
    uint32_t texCoord;
};

struct ShaderPbrMetallicRoughness {
    glm::vec4         baseColorFactor;
    ShaderTextureInfo baseColorTexture;
    float             metallicFactor;
    float             roughnessFactor;
    ShaderTextureInfo metallicRoughnessTexture;
};

struct ShaderNormalTextureInfo {
    uint32_t index;
    uint32_t texCoord;
    float    scale;
};

struct ShaderOcclusionTextureInfo {
    uint32_t index;
    uint32_t texCoord;
    float    strength;
};

struct ShaderMaterial {
    ShaderPbrMetallicRoughness pbrMetallicRoughness;
    ShaderNormalTextureInfo    normalTexture;
    ShaderOcclusionTextureInfo occlusionTexture;
    ShaderTextureInfo          emissiveTexture;
    glm::vec2                  _padding0;
    glm::vec3                  emissiveFactor;
    float                      alphaCutoff;
    glm::vec4                  _padding1;
    glm::vec4                  _padding2;
};

template <typename T>
[[nodiscard]]
static auto create_staging_buffer(
    const modules::renderer::base::Allocator& allocator,
    const std::span<T>                     data
) -> std::optional<modules::renderer::resources::SeqWriteBuffer<std::remove_const_t<T>>>
{
    if (data.empty()) {
        return std::nullopt;
    }

    const vk::BufferCreateInfo staging_buffer_create_info{
        .size  = data.size_bytes(),
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
    };

    return modules::renderer::resources::SeqWriteBuffer<std::remove_const_t<T>>{
        allocator, staging_buffer_create_info, data.data()
    };
}

[[nodiscard]]
static auto create_gpu_only_buffer(
    const modules::renderer::base::Allocator& allocator,
    const vk::BufferUsageFlags             usage_flags,
    const uint32_t                         size
) -> std::optional<modules::renderer::resources::Buffer>
{
    if (size == 0) {
        return std::nullopt;
    }

    const vk::BufferCreateInfo buffer_create_info = {
        .size = size, .usage = usage_flags | vk::BufferUsageFlagBits::eTransferDst
    };

    return modules::renderer::resources::Buffer{ allocator, buffer_create_info };
}

template <typename UniformBlock>
[[nodiscard]]
static auto create_buffer(const modules::renderer::base::Allocator& allocator)
    -> modules::renderer::resources::RandomAccessBuffer<UniformBlock>
{
    constexpr static vk::BufferCreateInfo buffer_create_info = {
        .size = sizeof(UniformBlock), .usage = vk::BufferUsageFlagBits::eUniformBuffer
    };

    return modules::renderer::resources::RandomAccessBuffer<UniformBlock>{
        allocator, buffer_create_info
    };
}

[[nodiscard]]
static auto convert_material(const modules::gltf::Material& material) noexcept
    -> ShaderMaterial
{
    return ShaderMaterial{
        .pbrMetallicRoughness =
            ShaderPbrMetallicRoughness{
                                       .baseColorFactor = material.pbr_metallic_roughness.base_color_factor,
                                       .baseColorTexture =
                    ShaderTextureInfo{
                        .index =
                            material.pbr_metallic_roughness.base_color_texture_info
                                .transform([](const modules::gltf::TextureInfo& texture_info
                                           ) { return texture_info.texture_index; })
                                .value_or(std::numeric_limits<uint32_t>::max()),
                        .texCoord =
                            material.pbr_metallic_roughness.base_color_texture_info
                                .transform([](const modules::gltf::TextureInfo& texture_info
                                           ) { return texture_info.tex_coord_index; })
                                .value_or(std::numeric_limits<uint32_t>::max()),
                    }, .metallicFactor  = material.pbr_metallic_roughness.metallic_factor,
                                       .roughnessFactor = material.pbr_metallic_roughness.roughness_factor,
                                       .metallicRoughnessTexture =
                    ShaderTextureInfo{
                        .index =
                            material.pbr_metallic_roughness
                                .metallic_roughness_texture_info
                                .transform([](const modules::gltf::TextureInfo& texture_info
                                           ) { return texture_info.texture_index; })
                                .value_or(std::numeric_limits<uint32_t>::max()),
                        .texCoord =
                            material.pbr_metallic_roughness
                                .metallic_roughness_texture_info
                                .transform([](const modules::gltf::TextureInfo& texture_info
                                           ) { return texture_info.tex_coord_index; })
                                .value_or(std::numeric_limits<uint32_t>::max()),
                    }, },
        .normalTexture =
            ShaderNormalTextureInfo{
                                       .index = material.normal_texture_info
                             .transform([](const modules::gltf::Material::NormalTextureInfo&
                                               texture_info
                                        ) { return texture_info.texture_index; })
                             .value_or(std::numeric_limits<uint32_t>::max()),
                                       .texCoord = material.normal_texture_info
                                .transform([](const modules::gltf::Material::
                                                  NormalTextureInfo& texture_info
                                           ) { return texture_info.tex_coord_index; })
                                .value_or(std::numeric_limits<uint32_t>::max()),
                                       .scale = material.normal_texture_info
                             .transform([](const modules::gltf::Material::NormalTextureInfo&
                                               texture_info
                                        ) { return texture_info.scale; })
                             .value_or(1.f),
                                       },
        .occlusionTexture =
            ShaderOcclusionTextureInfo{
                                       .index = material.occlusion_texture_info
                             .transform([](const modules::gltf::Material::
                                               OcclusionTextureInfo& texture_info
                                        ) { return texture_info.texture_index; })
                             .value_or(std::numeric_limits<uint32_t>::max()),
                                       .texCoord = material.occlusion_texture_info
                                .transform([](const modules::gltf::Material::
                                                  OcclusionTextureInfo& texture_info
                                           ) { return texture_info.tex_coord_index; })
                                .value_or(std::numeric_limits<uint32_t>::max()),
                                       .strength = material.occlusion_texture_info
                                .transform([](const modules::gltf::Material::
                                                  OcclusionTextureInfo& texture_info
                                           ) { return texture_info.strength; })
                                .value_or(1.f),
                                       },
        .emissiveTexture =
            ShaderTextureInfo{
                                       .index = material.emissive_texture_info
                             .transform([](const modules::gltf::TextureInfo& texture_info) {
                                 return texture_info.texture_index;
                             })
                             .value_or(std::numeric_limits<uint32_t>::max()),
                                       .texCoord = material.emissive_texture_info
                                .transform([](const modules::gltf::TextureInfo& texture_info
                                           ) { return texture_info.tex_coord_index; })
                                .value_or(std::numeric_limits<uint32_t>::max()),
                                       },
        .emissiveFactor = material.emissive_factor,
        .alphaCutoff    = material.alpha_mode == modules::gltf::Material::AlphaMode::eMask
                            ? material.alpha_cutoff
                            : -1.f,
    };
}

[[nodiscard]]
static auto create_base_descriptor_set(
    const vk::Device              device,
    const vk::DescriptorSetLayout descriptor_set_layout,
    const vk::DescriptorPool      descriptor_pool,
    const modules::renderer::resources::RandomAccessBuffer<vk::DeviceAddress>& vertex_uniform,
    const modules::renderer::resources::RandomAccessBuffer<vk::DeviceAddress>&
                      transform_uniform,
    const vk::Sampler default_sampler,
    const modules::renderer::resources::RandomAccessBuffer<vk::DeviceAddress>& texture_uniform,
    const modules::renderer::resources::RandomAccessBuffer<ShaderMaterial>&
        default_material_uniform,
    const modules::renderer::resources::RandomAccessBuffer<vk::DeviceAddress>& material_uniform
) -> vk::UniqueDescriptorSet
{
    const vk::DescriptorSetAllocateInfo descriptor_set_allocate_info{
        .descriptorPool     = descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts        = &descriptor_set_layout,
    };
    vk::UniqueDescriptorSet result{ std::move(
        device.allocateDescriptorSetsUnique(descriptor_set_allocate_info).front()
    ) };

    const vk::DescriptorBufferInfo vertex_buffer_info{
        .buffer = vertex_uniform.buffer(),
        .range  = vertex_uniform.size_bytes(),
    };
    const vk::DescriptorBufferInfo transform_buffer_info{
        .buffer = transform_uniform.buffer(),
        .range  = transform_uniform.size_bytes(),
    };
    const vk::DescriptorImageInfo default_sampler_image_info{
        .sampler = default_sampler,
    };
    const vk::DescriptorBufferInfo texture_buffer_info{
        .buffer = texture_uniform.buffer(),
        .range  = texture_uniform.size_bytes(),
    };
    const vk::DescriptorBufferInfo default_material_buffer_info{
        .buffer = default_material_uniform.buffer(),
        .range  = default_material_uniform.size_bytes(),
    };
    const vk::DescriptorBufferInfo material_buffer_info{
        .buffer = material_uniform.buffer(),
        .range  = material_uniform.size_bytes(),
    };

    const std::array write_descriptor_sets{
        vk::WriteDescriptorSet{
                               .dstSet          = result.get(),
                               .dstBinding      = 0,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eUniformBuffer,
                               .pBufferInfo     = &vertex_buffer_info,
                               },
        vk::WriteDescriptorSet{
                               .dstSet          = result.get(),
                               .dstBinding      = 1,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eUniformBuffer,
                               .pBufferInfo     = &transform_buffer_info,
                               },
        vk::WriteDescriptorSet{
                               .dstSet          = result.get(),
                               .dstBinding      = 2,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eSampler,
                               .pImageInfo      = &default_sampler_image_info,
                               },
        vk::WriteDescriptorSet{
                               .dstSet          = result.get(),
                               .dstBinding      = 3,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eUniformBuffer,
                               .pBufferInfo     = &texture_buffer_info,
                               },
        vk::WriteDescriptorSet{
                               .dstSet          = result.get(),
                               .dstBinding      = 4,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eUniformBuffer,
                               .pBufferInfo     = &default_material_buffer_info,
                               },
        vk::WriteDescriptorSet{
                               .dstSet          = result.get(),
                               .dstBinding      = 5,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eUniformBuffer,
                               .pBufferInfo     = &material_buffer_info,
                               },
    };

    device.updateDescriptorSets(
        write_descriptor_sets.size(), write_descriptor_sets.data(), 0, nullptr
    );

    return result;
}

[[nodiscard]]
static auto create_image_descriptor_set(
    const vk::Device               device,
    const vk::DescriptorSetLayout  descriptor_set_layout,
    const vk::DescriptorPool       descriptor_pool,
    const std::ranges::range auto& image_views
) -> vk::UniqueDescriptorSet
{
    const uint32_t descriptor_count{ static_cast<uint32_t>(image_views.size()) };
    const vk::DescriptorSetVariableDescriptorCountAllocateInfo variable_info{
        .descriptorSetCount = 1,
        .pDescriptorCounts  = &descriptor_count,
    };
    const vk::DescriptorSetAllocateInfo descriptor_set_allocate_info{
        .pNext              = &variable_info,
        .descriptorPool     = descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts        = &descriptor_set_layout,
    };
    vk::UniqueDescriptorSet result{ std::move(
        device.allocateDescriptorSetsUnique(descriptor_set_allocate_info).front()
    ) };

    const std::vector image_infos{
        image_views | std::views::transform([](const vk::ImageView image_view) {
            return vk::DescriptorImageInfo{
                .imageView   = image_view,
                .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
            };
        })
        | std::ranges::to<std::vector>()
    };

    const std::vector write_descriptor_sets{
        std::views::iota(0u, image_infos.size())
        | std::views::transform([&](const uint32_t index) {
              return vk::WriteDescriptorSet{
                  .dstSet          = result.get(),
                  .dstBinding      = 0,
                  .dstArrayElement = index,
                  .descriptorCount = 1,
                  .descriptorType  = vk::DescriptorType::eSampledImage,
                  .pImageInfo      = &image_infos.at(index),
              };
          })
        | std::ranges::to<std::vector>()
    };

    device.updateDescriptorSets(
        static_cast<uint32_t>(write_descriptor_sets.size()),
        write_descriptor_sets.data(),
        0,
        nullptr
    );

    return result;
}

[[nodiscard]]
static auto to_mag_filter(const modules::gltf::Sampler::MagFilter mag_filter) noexcept
    -> vk::Filter
{
    using enum modules::gltf::Sampler::MagFilter;
    switch (mag_filter) {
        case eNearest: return vk::Filter::eNearest;
        case eLinear:  return vk::Filter::eLinear;
    }
    std::unreachable();
}

[[nodiscard]]
static auto to_min_filter(const modules::gltf::Sampler::MinFilter min_filter) noexcept
    -> vk::Filter
{
    using enum modules::gltf::Sampler::MinFilter;
    switch (min_filter) {
        case eNearest:              return vk::Filter::eNearest;
        case eLinear:               return vk::Filter::eLinear;
        case eNearestMipmapNearest: return vk::Filter::eNearest;
        case eLinearMipmapNearest:  return vk::Filter::eLinear;
        case eNearestMipmapLinear:  return vk::Filter::eNearest;
        case eLinearMipmapLinear:   return vk::Filter::eLinear;
    }
    std::unreachable();
}

[[nodiscard]]
static auto to_mipmap_mode(const modules::gltf::Sampler::MinFilter min_filter) noexcept
    -> vk::SamplerMipmapMode
{
    using enum modules::gltf::Sampler::MinFilter;
    switch (min_filter) {
        case eNearest:              [[fallthrough]];
        case eLinear:               return vk::SamplerMipmapMode::eLinear;
        case eNearestMipmapNearest: [[fallthrough]];
        case eLinearMipmapNearest:  return vk::SamplerMipmapMode::eNearest;
        case eNearestMipmapLinear:  [[fallthrough]];
        case eLinearMipmapLinear:   return vk::SamplerMipmapMode::eLinear;
    }
    std::unreachable();
}

[[nodiscard]]
static auto to_address_mode(const modules::gltf::Sampler::WrapMode wrap_mode) noexcept
    -> vk::SamplerAddressMode
{
    using enum modules::gltf::Sampler::WrapMode;
    switch (wrap_mode) {
        case eClampToEdge:    return vk::SamplerAddressMode::eClampToEdge;
        case eMirroredRepeat: return vk::SamplerAddressMode::eMirroredRepeat;
        case eRepeat:         return vk::SamplerAddressMode::eRepeat;
    }
    std::unreachable();
}

[[nodiscard]]
static auto create_sampler(
    const modules::renderer::base::Device& device,
    const modules::gltf::Sampler&          sampler_info
) -> vk::UniqueSampler
{
    const vk::PhysicalDeviceLimits limits{
        device.physical_device().getProperties().limits
    };

    const vk::SamplerCreateInfo sampler_create_info{
        .magFilter        = ::to_mag_filter(sampler_info.mag_filter),
        .minFilter        = ::to_min_filter(sampler_info.min_filter),
        .mipmapMode       = ::to_mipmap_mode(sampler_info.min_filter),
        .addressModeU     = ::to_address_mode(sampler_info.wrap_s),
        .addressModeV     = ::to_address_mode(sampler_info.wrap_t),
        .addressModeW     = vk::SamplerAddressMode::eRepeat,
        .anisotropyEnable = vk::True,
        .maxAnisotropy    = limits.maxSamplerAnisotropy,
        .maxLod           = vk::LodClampNone,
    };

    return device->createSamplerUnique(sampler_create_info);
}

[[nodiscard]]
static auto create_sampler_descriptor_set(
    const vk::Device                      device,
    const vk::DescriptorSetLayout         descriptor_set_layout,
    const vk::DescriptorPool              descriptor_pool,
    const std::vector<vk::UniqueSampler>& samplers
) -> vk::UniqueDescriptorSet
{
    const uint32_t descriptor_count{ static_cast<uint32_t>(samplers.size()) };
    const vk::DescriptorSetVariableDescriptorCountAllocateInfo variable_info{
        .descriptorSetCount = 1,
        .pDescriptorCounts  = &descriptor_count,
    };
    const vk::DescriptorSetAllocateInfo descriptor_set_allocate_info{
        .pNext              = &variable_info,
        .descriptorPool     = descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts        = &descriptor_set_layout,
    };
    vk::UniqueDescriptorSet result{ std::move(
        device.allocateDescriptorSetsUnique(descriptor_set_allocate_info).front()
    ) };

    const std::vector image_infos{
        samplers | std::views::transform([](const vk::UniqueSampler& sampler) {
            return vk::DescriptorImageInfo{
                .sampler = sampler.get(),
            };
        })
        | std::ranges::to<std::vector>()
    };

    const std::vector write_descriptor_sets{
        std::views::iota(0u, image_infos.size())
        | std::views::transform([&](const uint32_t index) {
              return vk::WriteDescriptorSet{
                  .dstSet          = result.get(),
                  .dstBinding      = 0,
                  .dstArrayElement = index,
                  .descriptorCount = 1,
                  .descriptorType  = vk::DescriptorType::eSampler,
                  .pImageInfo      = &image_infos.at(index),
              };
          })
        | std::ranges::to<std::vector>()
    };

    device.updateDescriptorSets(
        static_cast<uint32_t>(write_descriptor_sets.size()),
        write_descriptor_sets.data(),
        0,
        nullptr
    );

    return result;
}

[[nodiscard]]
static auto convert(const modules::gltf::Mesh::Primitive::Topology topology)
    -> vk::PrimitiveTopology
{
    using enum modules::gltf::Mesh::Primitive::Topology;
    switch (topology) {
        case ePoints:     return vk::PrimitiveTopology::ePointList;
        case eLineStrips: return vk::PrimitiveTopology::eLineStrip;
        case eLineLoops:
            throw std::runtime_error{ std::format(
                "Unsupported primitive topology: {}", std::to_underlying(eLineLoops)
            ) };
        case eLines:          return vk::PrimitiveTopology::eLineList;
        case eTriangles:      return vk::PrimitiveTopology::eTriangleList;
        case eTriangleStrips: return vk::PrimitiveTopology::eTriangleStrip;
        case eTriangleFans:   return vk::PrimitiveTopology::eTriangleFan;
    }
    std::unreachable();
}

[[nodiscard]]
static auto create_program(const vk::Device device, modules::cache::Cache& cache)
    -> modules::renderer::Program
{
    static const std::filesystem::path shader_path{
        std::filesystem::path{ std::source_location::current().file_name() }.parent_path()
        / "shaders"
    };
    static const std::filesystem::path vertex_shader_path{ shader_path
                                                           / "model.vert.spv" };
    static const std::filesystem::path fragment_shader_path{ shader_path
                                                             / "pbr.frag.spv" };

    return modules::renderer::Program{
        modules::renderer::Shader{ cache.lazy_emplace<const modules::renderer::ShaderModule>(
            modules::renderer::ShaderModule::hash(vertex_shader_path),
            [device]() {
                return modules::renderer::ShaderModule::load(device, vertex_shader_path);
            }
        ) },
        modules::renderer::Shader{ cache.lazy_emplace<const modules::renderer::ShaderModule>(
            modules::renderer::ShaderModule::hash(fragment_shader_path),
            [device]() {
                return modules::renderer::ShaderModule::load(device, fragment_shader_path);
            }
        ) }
    };
}

[[nodiscard]]
static auto create_pipeline(
    const vk::Device                                   device,
    const modules::gltf::RenderModel::PipelineCreateInfo& create_info,
    const modules::gltf::Mesh::Primitive                  primitive,
    const modules::gltf::Material                         material,
    modules::cache::Cache&                                cache
) -> modules::cache::Handle<vk::UniquePipeline>
{
    modules::renderer::GraphicsPipelineBuilder builder{ ::create_program(device, cache) };

    builder.set_layout(create_info.layout);
    builder.set_render_pass(create_info.render_pass);
    builder.set_primitive_topology(::convert(primitive.mode));
    builder.set_cull_mode(
        material.double_sided ? vk::CullModeFlagBits::eNone : vk::CullModeFlagBits::eBack
    );
    if (material.alpha_mode == modules::gltf::Material::AlphaMode::eBlend) {
        builder.enable_blending();
    }

    const std::size_t hash{ hash_value(builder) };

    return cache.lazy_emplace<vk::UniquePipeline>(
        hash,
        std::bind_front(
            &modules::renderer::GraphicsPipelineBuilder::build, builder, device, nullptr
        )
    );
}

auto modules::gltf::RenderModel::create_loader(
    const renderer::base::Device&                     device,
    const renderer::base::Allocator&                  allocator,
    const std::span<const vk::DescriptorSetLayout, 3> descriptor_set_layouts,
    const PipelineCreateInfo&                         pipeline_create_info,
    const vk::DescriptorPool                          descriptor_pool,
    const cache::Handle<const Model>&                 model,
    cache::Cache&                                     cache,
    const bool                                        use_virtual_images
) -> std::packaged_task<RenderModel(vk::CommandBuffer)>
{
    // TODO: handle model buffers with no elements

    std::optional<renderer::resources::SeqWriteBuffer<uint32_t>> index_staging_buffer{
        ::create_staging_buffer(allocator, std::span{ model->indices() })
    };
    std::optional<renderer::resources::Buffer> index_buffer{ ::create_gpu_only_buffer(
        allocator,
        vk::BufferUsageFlagBits::eIndexBuffer,
        static_cast<uint32_t>(std::span{ model->indices() }.size_bytes())
    ) };

    std::vector<ShaderVertex> vertices{
        model->vertices() | std::views::transform([](const Model::Vertex& vertex) {
            return ShaderVertex{
                .position = vertex.position,
                .normal   = vertex.normal,
                .UV0      = vertex.uv_0,
                .UV1      = vertex.uv_1,
                .color    = vertex.color,
            };
        })
        | std::ranges::to<std::vector>()
    };
    std::optional<renderer::resources::SeqWriteBuffer<ShaderVertex>> vertex_staging_buffer{
        ::create_staging_buffer(allocator, std::span{ vertices })
    };
    std::optional<renderer::resources::Buffer> vertex_buffer{ ::create_gpu_only_buffer(
        allocator,
        vk::BufferUsageFlagBits::eStorageBuffer
            | vk::BufferUsageFlagBits::eShaderDeviceAddress,
        static_cast<uint32_t>(std::span{ vertices }.size_bytes())
    ) };
    renderer::resources::RandomAccessBuffer<vk::DeviceAddress> vertex_uniform{
        ::create_buffer<vk::DeviceAddress>(allocator)
    };

    std::vector nodes_with_mesh{
        model->nodes() | std::views::filter([](const Node& node) {
            return node.mesh_index().has_value();
        })
        | std::views::transform([](const Node& node) { return std::cref(node); })
        | std::ranges::to<std::vector>()
    };
    std::vector<glm::mat4> transforms(nodes_with_mesh.size());
    std::ranges::for_each(nodes_with_mesh, [&transforms, model](const Node& node) {
        transforms.at(node.mesh_index().value()) = node.matrix(*model);
    });
    std::optional<renderer::resources::SeqWriteBuffer<glm::mat4>> transform_staging_buffer{
        ::create_staging_buffer(allocator, std::span{ transforms })
    };
    std::optional<renderer::resources::Buffer> transform_buffer{ ::create_gpu_only_buffer(
        allocator,
        vk::BufferUsageFlagBits::eStorageBuffer
            | vk::BufferUsageFlagBits::eShaderDeviceAddress,
        static_cast<uint32_t>(std::span{ transforms }.size_bytes())
    ) };
    renderer::resources::RandomAccessBuffer<vk::DeviceAddress> transform_uniform{
        ::create_buffer<vk::DeviceAddress>(allocator)
    };

    vk::UniqueSampler default_sampler{
        ::create_sampler(device, Model::default_sampler())
    };

    std::vector<ShaderTexture> textures{
        model->textures() | std::views::transform([](const Texture& texture) {
            return ShaderTexture{
                .image_index = texture.image_index,
                .sampler_index =
                    texture.sampler_index.value_or(std::numeric_limits<uint32_t>::max()),
            };
        })
        | std::ranges::to<std::vector>()
    };
    std::optional<renderer::resources::SeqWriteBuffer<ShaderTexture>>
        texture_staging_buffer{
            ::create_staging_buffer(allocator, std::span{ textures })
        };
    std::optional<renderer::resources::Buffer> texture_buffer{ ::create_gpu_only_buffer(
        allocator,
        vk::BufferUsageFlagBits::eStorageBuffer
            | vk::BufferUsageFlagBits::eShaderDeviceAddress,
        static_cast<uint32_t>(std::span{ textures }.size_bytes())
    ) };
    renderer::resources::RandomAccessBuffer<vk::DeviceAddress> texture_uniform{
        ::create_buffer<vk::DeviceAddress>(allocator)
    };

    const ShaderMaterial default_material{ ::convert_material(Model::default_material()) };
    renderer::resources::RandomAccessBuffer<ShaderMaterial> default_material_uniform{
        allocator,
        vk::BufferCreateInfo{ .size  = static_cast<uint32_t>(sizeof(ShaderMaterial)),
                             .usage = vk::BufferUsageFlagBits::eUniformBuffer },
        &default_material
    };

    std::vector<ShaderMaterial> materials{ model->materials()
                                           | std::views::transform(::convert_material)
                                           | std::ranges::to<std::vector>() };
    std::optional<renderer::resources::SeqWriteBuffer<ShaderMaterial>>
        material_staging_buffer{
            ::create_staging_buffer(allocator, std::span{ materials })
        };
    std::optional<renderer::resources::Buffer> material_buffer{ ::create_gpu_only_buffer(
        allocator,
        vk::BufferUsageFlagBits::eStorageBuffer
            | vk::BufferUsageFlagBits::eShaderDeviceAddress,
        static_cast<uint32_t>(std::span{ materials }.size_bytes())
    ) };
    renderer::resources::RandomAccessBuffer<vk::DeviceAddress> material_uniform{
        ::create_buffer<vk::DeviceAddress>(allocator)
    };

    vk::UniqueDescriptorSet base_descriptor_set{ ::create_base_descriptor_set(
        device.get(),
        descriptor_set_layouts[0],
        descriptor_pool,
        vertex_uniform,
        transform_uniform,
        default_sampler.get(),
        texture_uniform,
        default_material_uniform,
        material_uniform
    ) };

    std::vector<ImageVariantLoader> image_loaders{
        model->images()
        | std::views::transform(
            [&device,
             &allocator,
             use_virtual_images](const Image& source) -> ImageVariantLoader {
                if (use_virtual_images) {
                    return gfx::resources::VirtualImage::Loader{
                        device.physical_device(), device.get(), allocator, *source
                    };
                }
                return gfx::resources::Image::Loader{ device.get(), allocator, *source };
            }
        )
        | std::ranges::to<std::vector>()
    };

    vk::UniqueDescriptorSet image_descriptor_set{ ::create_image_descriptor_set(
        device.get(),
        descriptor_set_layouts[1],
        descriptor_pool,
        image_loaders | std::views::transform([](const ImageVariantLoader& variant_loader) {
            return variant_loader.visit([](const auto& loader) { return loader.view(); });
        })
    ) };

    std::vector<vk::UniqueSampler> samplers{
        model->samplers()
        | std::views::transform(std::bind_front(::create_sampler, std::cref(device)))
        | std::ranges::to<std::vector>()
    };
    vk::UniqueDescriptorSet sampler_descriptor_set{ ::create_sampler_descriptor_set(
        device.get(), descriptor_set_layouts[2], descriptor_pool, samplers
    ) };

    std::vector<Mesh> meshes{
        model->meshes() | std::views::transform([&](const gltf::Mesh& mesh) {
            return Mesh{
                .primitives =
                    mesh.primitives
                    | std::views::transform([&](const gltf::Mesh::Primitive& primitive) {
                          return Mesh::Primitive{
                              .pipeline = ::create_pipeline(
                                  device.get(),
                                  pipeline_create_info,
                                  primitive,
                                  primitive.material_index
                                      .transform([&model](const std::size_t material_index) {
                                          return model->materials().at(material_index);
                                      })
                                      .value_or(Model::default_material()),
                                  cache
                              ),
                              .material_index    = primitive.material_index,
                              .first_index_index = primitive.first_index_index,
                              .index_count       = primitive.index_count,
                          };
                      })
                    | std::ranges::to<std::vector>()
            };
        })
        | std::ranges::to<std::vector>()
    };

    auto update_virtual_images_callback{ [transforms,
                                          textures,
                                          materials,
                                          default_material](
                                             const std::span<ImageVariant> images,
                                             const gfx::Camera&            camera,
                                             const uint32_t transform_index,
                                             std::optional<uint32_t> optional_material_index
                                         ) mutable {
        const glm::mat4& transform{ transforms.at(transform_index) };
        const glm::vec3 position{ glm::vec4{ 1.f } * transform };

        std::ranges::for_each(
            std::views::single(optional_material_index
                                   .transform([&materials](const uint32_t material_index) {
                                       return std::cref(materials.at(material_index));
                                   })
                                   .value_or(std::cref(default_material))),
            [&textures, &images, &camera, &position](const ShaderMaterial& material) {
                if (material.pbrMetallicRoughness.baseColorTexture.index
                    != std::numeric_limits<uint32_t>::max())
                {
                    if (ImageVariant
                            & image_variant{ images.at(
                                textures
                                    .at(material.pbrMetallicRoughness.baseColorTexture.index
                                    )
                                    .image_index
                            ) };
                        std::holds_alternative<gfx::resources::VirtualImage>(image_variant
                        ))
                    {
                        std::get<gfx::resources::VirtualImage>(image_variant)
                            .request_blocks_by_distance_from_camera(
                                glm::distance(camera.position(), position), 10
                            );
                    }
                }

                if (material.pbrMetallicRoughness.metallicRoughnessTexture.index
                    != std::numeric_limits<uint32_t>::max())
                {
                    if (ImageVariant
                            & image_variant{ images.at(
                                textures
                                    .at(material.pbrMetallicRoughness
                                            .metallicRoughnessTexture.index)
                                    .image_index
                            ) };
                        std::holds_alternative<gfx::resources::VirtualImage>(image_variant
                        ))
                    {
                        std::get<gfx::resources::VirtualImage>(image_variant)
                            .request_blocks_by_distance_from_camera(
                                glm::distance(camera.position(), position), 10
                            );
                    }
                }

                if (material.normalTexture.index != std::numeric_limits<uint32_t>::max())
                {
                    if (ImageVariant
                            & image_variant{ images.at(
                                textures.at(material.normalTexture.index).image_index
                            ) };
                        std::holds_alternative<gfx::resources::VirtualImage>(image_variant
                        ))
                    {
                        std::get<gfx::resources::VirtualImage>(image_variant)
                            .request_blocks_by_distance_from_camera(
                                glm::distance(camera.position(), position), 10
                            );
                    }
                }

                if (material.occlusionTexture.index
                    != std::numeric_limits<uint32_t>::max()) {
                    if (ImageVariant
                            & image_variant{ images.at(
                                textures.at(material.occlusionTexture.index).image_index
                            ) };
                        std::holds_alternative<gfx::resources::VirtualImage>(image_variant
                        ))
                    {
                        std::get<gfx::resources::VirtualImage>(image_variant)
                            .request_blocks_by_distance_from_camera(
                                glm::distance(camera.position(), position), 10
                            );
                    }
                }

                if (material.emissiveTexture.index
                    != std::numeric_limits<uint32_t>::max()) {
                    if (ImageVariant
                            & image_variant{ images.at(
                                textures.at(material.emissiveTexture.index).image_index
                            ) };
                        std::holds_alternative<gfx::resources::VirtualImage>(image_variant
                        ))
                    {
                        std::get<gfx::resources::VirtualImage>(image_variant)
                            .request_blocks_by_distance_from_camera(
                                glm::distance(camera.position(), position), 10
                            );
                    }
                }
            }
        );
    } };

    return std::packaged_task{
        [physical_device = device.physical_device(),
         device          = device.get(),
         sparse_queue    = device.info().get_queue(vkb::QueueType::graphics).value(),
         index_buffer_size =
             static_cast<uint32_t>(std::span{ model->indices() }.size_bytes()),
         index_staging_buffer = std::move(index_staging_buffer),
         index_buffer         = std::move(index_buffer),
         vertex_buffer_size   = static_cast<uint32_t>(std::span{ vertices }.size_bytes()),
         vertex_staging_buffer = std::move(vertex_staging_buffer),
         vertex_buffer         = std::move(vertex_buffer),
         vertex_uniform        = std::move(vertex_uniform),
         transform_buffer_size =
             static_cast<uint32_t>(std::span{ transforms }.size_bytes()),
         transform_staging_buffer = std::move(transform_staging_buffer),
         transform_buffer         = std::move(transform_buffer),
         transform_uniform        = std::move(transform_uniform),
         default_sampler          = std::move(default_sampler),
         texture_buffer_size = static_cast<uint32_t>(std::span{ textures }.size_bytes()),
         texture_staging_buffer   = std::move(texture_staging_buffer),
         texture_buffer           = std::move(texture_buffer),
         texture_uniform          = std::move(texture_uniform),
         default_material_uniform = std::move(default_material_uniform),
         material_buffer_size = static_cast<uint32_t>(std::span{ materials }.size_bytes()),
         material_staging_buffer        = std::move(material_staging_buffer),
         material_buffer                = std::move(material_buffer),
         material_uniform               = std::move(material_uniform),
         base_descriptor_set            = std::move(base_descriptor_set),
         image_loaders                  = std::move(image_loaders),
         image_descriptor_set           = std::move(image_descriptor_set),
         samplers                       = std::move(samplers),
         sampler_descriptor_set         = std::move(sampler_descriptor_set),
         meshes                         = std::move(meshes),
         update_virtual_images_callback = std::move(update_virtual_images_callback)](
            const vk::CommandBuffer transfer_command_buffer
        ) mutable -> RenderModel {
            if (index_buffer_size > 0) {
                transfer_command_buffer.copyBuffer(
                    index_staging_buffer->get(),
                    index_buffer->buffer().get(),
                    std::array{ vk::BufferCopy{ .size = index_buffer_size } }
                );
            }

            if (vertex_buffer_size > 0) {
                transfer_command_buffer.copyBuffer(
                    vertex_staging_buffer->get(),
                    vertex_buffer->buffer().get(),
                    std::array{ vk::BufferCopy{ .size = vertex_buffer_size } }
                );
            }

            if (transform_buffer_size > 0) {
                transfer_command_buffer.copyBuffer(
                    transform_staging_buffer->get(),
                    transform_buffer->buffer().get(),
                    std::array{ vk::BufferCopy{ .size = transform_buffer_size } }
                );
            }

            if (texture_buffer_size > 0) {
                transfer_command_buffer.copyBuffer(
                    texture_staging_buffer->get(),
                    texture_buffer->buffer().get(),
                    std::array{ vk::BufferCopy{ .size = texture_buffer_size } }
                );
            }

            if (material_buffer_size > 0) {
                transfer_command_buffer.copyBuffer(
                    material_staging_buffer->get(),
                    material_buffer->buffer().get(),
                    std::array{ vk::BufferCopy{ .size = material_buffer_size } }
                );
            }

            std::vector<ImageVariant> images{
                std::views::transform(
                    std::views::as_rvalue(image_loaders),
                    [physical_device, sparse_queue, transfer_command_buffer](
                        ImageVariantLoader&& image_variant_loader
                    ) -> ImageVariant {
                        if (std::holds_alternative<gfx::resources::VirtualImage::Loader>(
                                image_variant_loader
                            ))
                        {
                            return std::get<gfx::resources::VirtualImage::
                                                Loader>(std::move(image_variant_loader))(
                                physical_device,
                                sparse_queue,
                                transfer_command_buffer,
                                renderer::base::Image::State{
                                    vk::PipelineStageFlagBits::eFragmentShader,
                                    vk::AccessFlagBits::eShaderRead,
                                    vk::ImageLayout::eShaderReadOnlyOptimal,
                                }
                            );
                        }
                        return std::get<gfx::resources::Image::
                                            Loader>(std::move(image_variant_loader))(
                            physical_device,
                            transfer_command_buffer,
                            renderer::base::Image::State{
                                vk::PipelineStageFlagBits::eFragmentShader,
                                vk::AccessFlagBits::eShaderRead,
                                vk::ImageLayout::eShaderReadOnlyOptimal,
                            }
                        );
                    }
                )
                | std::ranges::to<std::vector>()
            };

            return RenderModel{ device,
                                std::move(index_buffer),
                                std::move(vertex_buffer),
                                std::move(vertex_uniform),
                                std::move(transform_buffer),
                                std::move(transform_uniform),
                                std::move(default_sampler),
                                std::move(texture_buffer),
                                std::move(texture_uniform),
                                std::move(default_material_uniform),
                                std::move(material_buffer),
                                std::move(material_uniform),
                                std::move(base_descriptor_set),
                                std::move(images),
                                std::move(image_descriptor_set),
                                std::move(samplers),
                                std::move(sampler_descriptor_set),
                                std::move(meshes),
                                std::move(update_virtual_images_callback) };
        }
    };
}

auto modules::gltf::RenderModel::update(
    const gfx::Camera&               camera,
    const renderer::base::Allocator& allocator,
    const vk::Queue                  sparse_queue,
    const vk::CommandBuffer          transfer_command_buffer
) -> void
{
    if (m_update_virtual_images) {
        for (const auto& [mesh, mesh_index] :
             std::views::zip(m_meshes, std::views::iota(0u, m_meshes.size())))
        {
            for (const auto& [pipeline, material_index, first_index_index, index_count] :
                 mesh.primitives)
            {
                m_update_virtual_images(m_images, camera, mesh_index, material_index);
            }
        }
    }

    std::ranges::for_each(
        m_images,
        [&allocator, sparse_queue, transfer_command_buffer](ImageVariant& image_variant) {
            if (std::holds_alternative<gfx::resources::VirtualImage>(image_variant)) {
                std::get<gfx::resources::VirtualImage>(image_variant)
                    .update(allocator, sparse_queue, transfer_command_buffer);
            }
        }
    );
}

auto modules::gltf::RenderModel::draw(
    const vk::CommandBuffer  graphics_command_buffer,
    const vk::PipelineLayout pipeline_layout
) const noexcept -> void
{
    if (m_index_buffer.has_value()) {
        graphics_command_buffer.bindIndexBuffer(
            m_index_buffer->buffer().get(), 0, vk::IndexType::eUint32
        );
    }

    graphics_command_buffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        pipeline_layout,
        1,
        std::array{ m_base_descriptor_set.get(),
                    m_image_descriptor_set.get(),
                    m_sampler_descriptor_set.get() },
        nullptr
    );

    for (const auto& [mesh, mesh_index] :
         std::views::zip(m_meshes, std::views::iota(0u, m_meshes.size())))
    {
        for (const auto& [pipeline, material_index, first_index_index, index_count] :
             mesh.primitives)
        {
            graphics_command_buffer.bindPipeline(
                vk::PipelineBindPoint::eGraphics, pipeline.get()->get()
            );

            renderer::ModelLayout::PushConstants push_constants{
                .transform_index = mesh_index,
                .material_index =
                    material_index.value_or(std::numeric_limits<uint32_t>::max()),
            };
            graphics_command_buffer.pushConstants(
                pipeline_layout,
                vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                0,
                sizeof(renderer::ModelLayout::PushConstants),
                &push_constants
            );

            if (m_index_buffer.has_value()) {
                graphics_command_buffer.drawIndexed(
                    index_count, 1, first_index_index, 0, 0
                );
            }
            else {
                // TODO: graphics_command_buffer.draw(...);
            }
        }
    }
}

modules::gltf::RenderModel::RenderModel(
    const vk::Device                                             device,
    std::optional<renderer::resources::Buffer>&&                 index_buffer,
    std::optional<renderer::resources::Buffer>&&                 vertex_buffer,
    renderer::resources::RandomAccessBuffer<vk::DeviceAddress>&& vertex_uniform,
    std::optional<renderer::resources::Buffer>&&                 transform_buffer,
    renderer::resources::RandomAccessBuffer<vk::DeviceAddress>&& transform_uniform,
    vk::UniqueSampler&&                                          default_sampler,
    std::optional<renderer::resources::Buffer>&&                 texture_buffer,
    renderer::resources::RandomAccessBuffer<vk::DeviceAddress>&& texture_uniform,
    renderer::resources::RandomAccessBuffer<ShaderMaterial>&&    default_material_uniform,
    std::optional<renderer::resources::Buffer>&&                 material_buffer,
    renderer::resources::RandomAccessBuffer<vk::DeviceAddress>&& material_uniform,
    vk::UniqueDescriptorSet&&                                    base_descriptor_set,
    std::vector<ImageVariant>&&                                  images,
    vk::UniqueDescriptorSet&&                                    image_descriptor_set,
    std::vector<vk::UniqueSampler>&&                             samplers,
    vk::UniqueDescriptorSet&&                                    sampler_descriptor_set,
    std::vector<Mesh>&&                                          meshes,
    UpdateVirtualImagesT&&                                       update_virtual_images
)
    : m_index_buffer{ std::move(index_buffer) },
      m_vertex_buffer{ std::move(vertex_buffer) },
      m_vertex_uniform{ std::move(vertex_uniform) },
      m_transform_buffer{ std::move(transform_buffer) },
      m_transform_uniform{ std::move(transform_uniform) },
      m_default_sampler{ std::move(default_sampler) },
      m_texture_buffer{ std::move(texture_buffer) },
      m_texture_uniform{ std::move(texture_uniform) },
      m_default_material_uniform{ std::move(default_material_uniform) },
      m_material_buffer{ std::move(material_buffer) },
      m_material_uniform{ std::move(material_uniform) },
      m_base_descriptor_set{ std::move(base_descriptor_set) },
      m_images{ std::move(images) },
      m_image_descriptor_set{ std::move(image_descriptor_set) },
      m_samplers{ std::move(samplers) },
      m_sampler_descriptor_set{ std::move(sampler_descriptor_set) },
      m_meshes{ std::move(meshes) },
      m_update_virtual_images{ std::move(update_virtual_images) }
{
    m_vertex_uniform.set(m_vertex_buffer
                             .transform([](const renderer::resources::Buffer& buffer) {
                                 return std::cref(buffer.buffer());
                             })
                             .transform(&renderer::base::Buffer::get)
                             .transform([device](const vk::Buffer buffer) {
                                 return device.getBufferAddress(
                                     vk::BufferDeviceAddressInfo{
                                         .buffer = buffer,
                                     }
                                 );
                             })
                             .value_or(vk::DeviceAddress{}));

    m_transform_uniform.set(m_transform_buffer
                                .transform([](const renderer::resources::Buffer& buffer) {
                                    return std::cref(buffer.buffer());
                                })
                                .transform(&renderer::base::Buffer::get)
                                .transform([device](const vk::Buffer buffer) {
                                    return device.getBufferAddress(
                                        vk::BufferDeviceAddressInfo{
                                            .buffer = buffer,
                                        }
                                    );
                                })
                                .value_or(vk::DeviceAddress{}));

    m_texture_uniform.set(m_texture_buffer
                              .transform([](const renderer::resources::Buffer& buffer) {
                                  return std::cref(buffer.buffer());
                              })
                              .transform(&renderer::base::Buffer::get)
                              .transform([device](const vk::Buffer buffer) {
                                  return device.getBufferAddress(
                                      vk::BufferDeviceAddressInfo{
                                          .buffer = buffer,
                                      }
                                  );
                              })
                              .value_or(vk::DeviceAddress{}));

    m_material_uniform.set(m_material_buffer
                               .transform([](const renderer::resources::Buffer& buffer) {
                                   return std::cref(buffer.buffer());
                               })
                               .transform(&renderer::base::Buffer::get)
                               .transform([device](const vk::Buffer buffer) {
                                   return device.getBufferAddress(
                                       vk::BufferDeviceAddressInfo{
                                           .buffer = buffer,
                                       }
                                   );
                               })
                               .value_or(vk::DeviceAddress{}));
}

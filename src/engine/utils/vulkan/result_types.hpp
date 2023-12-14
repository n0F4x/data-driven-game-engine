#pragma once

#include <stdexcept>
#include <type_traits>
#include <variant>

#include <vulkan/vulkan.hpp>

namespace engine::vulkan {

namespace err {

class Success {};

class NotReady {};

class Timeout {};

class EventSet {};

class EventReset {};

class Incomplete {};

class ErrorOutOfHostMemory {};

class ErrorOutOfDeviceMemory {};

class ErrorInitializationFailed {};

class ErrorDeviceLost {};

class ErrorMemoryMapFailed {};

class ErrorLayerNotPresent {};

class ErrorExtensionNotPresent {};

class ErrorFeatureNotPresent {};

class ErrorIncompatibleDriver {};

class ErrorTooManyObjects {};

class ErrorFormatNotSupported {};

class ErrorFragmentedPool {};

class ErrorUnknown {};

// Provided by VK_VERSION_1_1
class ErrorOutOfPoolMemory {};

// Provided by VK_VERSION_1_1
class ErrorInvalidExternalHandle {};

// Provided by VK_VERSION_1_2
class ErrorFragmentation {};

// Provided by VK_VERSION_1_2
class ErrorInvalidOpaqueCaptureAddress {};

// Provided by VK_VERSION_1_3
class PipelineCompileRequired {};

// Provided by VK_KHR_surface
class ErrorSurfaceLostKHR {};

// Provided by VK_KHR_surface
class ErrorNativeWindowInUseKHR {};

// Provided by VK_KHR_swapchain
class SuboptimalKHR {};

// Provided by VK_KHR_swapchain
class ErrorOutOfDateKHR {};

// Provided by VK_KHR_display_swapchain
class ErrorIncompatibleDisplayKHR {};

// Provided by VK_EXT_debug_report
class ErrorValidationFailedEXT {};

// Provided by VK_NV_glsl_shader
class ErrorInvalidShaderNV {};

// Provided by VK_KHR_video_queue
class ErrorImageUsageNotSupportedKHR {};

// Provided by VK_KHR_video_queue
class ErrorVideoPictureLayoutNotSupportedKHR {};

// Provided by VK_KHR_video_queue
class ErrorVideoProfileOperationNotSupportedKHR {};

// Provided by VK_KHR_video_queue
class ErrorVideoProfileFormatNotSupportedKHR {};

// Provided by VK_KHR_video_queue
class ErrorVideoProfileCodecNotSupportedKHR {};

// Provided by VK_KHR_video_queue
class ErrorVideoStdVersionNotSupportedKHR {};

// Provided by VK_EXT_image_drm_format_modifier
class ErrorInvalidDrmFormatModifierPlaneLayoutEXT {};

// Provided by VK_KHR_global_priority
class ErrorNotPermittedKHR {};

// Provided by VK_EXT_full_screen_exclusive
class ErrorFullScreenExclusiveModeLostEXT {};

// Provided by VK_KHR_deferred_host_operations
class ThreadIdleKHR {};

// Provided by VK_KHR_deferred_host_operations
class ThreadDoneKHR {};

// Provided by VK_KHR_deferred_host_operations
class OperationDeferredKHR {};

// Provided by VK_KHR_deferred_host_operations
class OperationNotDeferredKHR {};

// Provided by VK_EXT_image_compression_control
class ErrorCompressionExhaustedEXT {};

// Provided by VK_EXT_shader_object
class ErrorIncompatibleShaderBinaryEXT {};

// Provided by VK_KHR_maintenance1
using ErrorOutOfPoolMemoryKHR = ErrorOutOfPoolMemory;

// Provided by VK_KHR_external_memory
using ErrorInvalidExternalHandleKHR = ErrorInvalidExternalHandle;

// Provided by VK_EXT_descriptor_indexing
using ErrorFragmentationEXT = ErrorFragmentation;

// Provided by VK_EXT_global_priority
using ErrorNotPermittedEXT = ErrorNotPermittedKHR;

// Provided by VK_EXT_buffer_device_address
using ErrorInvalidDeviceAddressEXT = ErrorInvalidOpaqueCaptureAddress;

// Provided by VK_KHR_buffer_device_address
using ErrorInvalidOpaqueCaptureAddressKHR = ErrorInvalidOpaqueCaptureAddress;

// Provided by VK_EXT_pipeline_creation_cache_control
using PipelineCompileRequiredEXT = PipelineCompileRequired;

// Provided by VK_EXT_pipeline_creation_cache_control
using ErrorPipelineCompileRequiredEXT = PipelineCompileRequired;

}   // namespace err

template <typename... Args>
[[nodiscard]] auto get(vk::Result t_result) -> std::variant<Args...>;

}   // namespace engine::vulkan

#include "result_types.inl"

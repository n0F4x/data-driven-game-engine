#pragma once

#include <stdexcept>
#include <type_traits>
#include <variant>

#include <vulkan/vulkan_enums.hpp>

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

namespace {

using Variant = std::variant<
    err::Success,
    err::NotReady,
    err::Timeout,
    err::EventSet,
    err::EventReset,
    err::Incomplete,
    err::ErrorOutOfHostMemory,
    err::ErrorOutOfDeviceMemory,
    err::ErrorInitializationFailed,
    err::ErrorDeviceLost,
    err::ErrorMemoryMapFailed,
    err::ErrorLayerNotPresent,
    err::ErrorExtensionNotPresent,
    err::ErrorFeatureNotPresent,
    err::ErrorIncompatibleDriver,
    err::ErrorTooManyObjects,
    err::ErrorFormatNotSupported,
    err::ErrorFragmentedPool,
    err::ErrorUnknown,
    err::ErrorOutOfPoolMemory,
    err::ErrorInvalidExternalHandle,
    err::ErrorFragmentation,
    err::ErrorInvalidOpaqueCaptureAddress,
    err::PipelineCompileRequired,
    err::ErrorSurfaceLostKHR,
    err::ErrorNativeWindowInUseKHR,
    err::SuboptimalKHR,
    err::ErrorOutOfDateKHR,
    err::ErrorIncompatibleDisplayKHR,
    err::ErrorValidationFailedEXT,
    err::ErrorInvalidShaderNV,
    err::ErrorImageUsageNotSupportedKHR,
    err::ErrorVideoPictureLayoutNotSupportedKHR,
    err::ErrorVideoProfileOperationNotSupportedKHR,
    err::ErrorVideoProfileFormatNotSupportedKHR,
    err::ErrorVideoProfileCodecNotSupportedKHR,
    err::ErrorVideoStdVersionNotSupportedKHR,
    err::ErrorInvalidDrmFormatModifierPlaneLayoutEXT,
    err::ErrorNotPermittedKHR,
    err::ErrorFullScreenExclusiveModeLostEXT,
    err::ThreadIdleKHR,
    err::ThreadDoneKHR,
    err::OperationDeferredKHR,
    err::OperationNotDeferredKHR,
    err::ErrorCompressionExhaustedEXT,
    err::ErrorIncompatibleShaderBinaryEXT>;

[[maybe_unused]] auto to_variant(vk::Result t_result) -> Variant
{
    switch (t_result) {
        case vk::Result::eSuccess: return err::Success{};
        case vk::Result::eNotReady: return err::NotReady{};
        case vk::Result::eTimeout: return err::Timeout{};
        case vk::Result::eEventSet: return err::EventSet{};
        case vk::Result::eEventReset: return err::EventReset{};
        case vk::Result::eIncomplete: return err::Incomplete{};
        case vk::Result::eErrorOutOfHostMemory:
            return err::ErrorOutOfHostMemory{};
        case vk::Result::eErrorOutOfDeviceMemory:
            return err::ErrorOutOfDeviceMemory{};
        case vk::Result::eErrorInitializationFailed:
            return err::ErrorInitializationFailed{};
        case vk::Result::eErrorDeviceLost: return err::ErrorDeviceLost{};
        case vk::Result::eErrorMemoryMapFailed:
            return err::ErrorMemoryMapFailed{};
        case vk::Result::eErrorLayerNotPresent:
            return err::ErrorLayerNotPresent{};
        case vk::Result::eErrorExtensionNotPresent:
            return err::ErrorExtensionNotPresent{};
        case vk::Result::eErrorFeatureNotPresent:
            return err::ErrorFeatureNotPresent{};
        case vk::Result::eErrorIncompatibleDriver:
            return err::ErrorIncompatibleDriver{};
        case vk::Result::eErrorTooManyObjects:
            return err::ErrorTooManyObjects{};
        case vk::Result::eErrorFormatNotSupported:
            return err::ErrorFormatNotSupported{};
        case vk::Result::eErrorFragmentedPool:
            return err::ErrorFragmentedPool{};
        case vk::Result::eErrorUnknown: return err::ErrorUnknown{};
        case vk::Result::eErrorOutOfPoolMemory:
            return err::ErrorOutOfPoolMemory{};
        case vk::Result::eErrorInvalidExternalHandle:
            return err::ErrorInvalidExternalHandle{};
        case vk::Result::eErrorFragmentation: return err::ErrorFragmentation{};
        case vk::Result::eErrorInvalidOpaqueCaptureAddress:
            return err::ErrorInvalidOpaqueCaptureAddress{};
        case vk::Result::ePipelineCompileRequired:
            return err::PipelineCompileRequired{};
        case vk::Result::eErrorSurfaceLostKHR:
            return err::ErrorSurfaceLostKHR{};
        case vk::Result::eErrorNativeWindowInUseKHR:
            return err::ErrorNativeWindowInUseKHR{};
        case vk::Result::eSuboptimalKHR: return err::SuboptimalKHR{};
        case vk::Result::eErrorOutOfDateKHR: return err::ErrorOutOfDateKHR{};
        case vk::Result::eErrorIncompatibleDisplayKHR:
            return err::ErrorIncompatibleDisplayKHR{};
        case vk::Result::eErrorValidationFailedEXT:
            return err::ErrorValidationFailedEXT{};
        case vk::Result::eErrorInvalidShaderNV:
            return err::ErrorInvalidShaderNV{};
        case vk::Result::eErrorImageUsageNotSupportedKHR:
            return err::ErrorImageUsageNotSupportedKHR{};
        case vk::Result::eErrorVideoPictureLayoutNotSupportedKHR:
            return err::ErrorVideoPictureLayoutNotSupportedKHR{};
        case vk::Result::eErrorVideoProfileOperationNotSupportedKHR:
            return err::ErrorVideoProfileOperationNotSupportedKHR{};
        case vk::Result::eErrorVideoProfileFormatNotSupportedKHR:
            return err::ErrorVideoProfileFormatNotSupportedKHR{};
        case vk::Result::eErrorVideoProfileCodecNotSupportedKHR:
            return err::ErrorVideoProfileCodecNotSupportedKHR{};
        case vk::Result::eErrorVideoStdVersionNotSupportedKHR:
            return err::ErrorVideoStdVersionNotSupportedKHR{};
        case vk::Result::eErrorInvalidDrmFormatModifierPlaneLayoutEXT:
            return err::ErrorInvalidDrmFormatModifierPlaneLayoutEXT{};
        case vk::Result::eErrorNotPermittedKHR:
            return err::ErrorNotPermittedKHR{};
#if defined(VK_USE_PLATFORM_WIN32_KHR)
        case vk::Result::eErrorFullScreenExclusiveModeLostEXT:
            return err::ErrorFullScreenExclusiveModeLostEXT{};
#endif /*VK_USE_PLATFORM_WIN32_KHR*/
        case vk::Result::eThreadIdleKHR: return err::ThreadIdleKHR{};
        case vk::Result::eThreadDoneKHR: return err::ThreadDoneKHR{};
        case vk::Result::eOperationDeferredKHR:
            return err::OperationDeferredKHR{};
        case vk::Result::eOperationNotDeferredKHR:
            return err::OperationNotDeferredKHR{};
        case vk::Result::eErrorCompressionExhaustedEXT:
            return err::ErrorCompressionExhaustedEXT{};
        default: throw std::logic_error{ "Unhandled case" };
    }
}

template <typename... Args>
auto variant_cast(Variant&& t_original) -> std::variant<Args...>
{
    return std::visit(
        [](auto&& result) -> std::variant<Args...> {
            if constexpr ((std::is_same_v<decltype(result), Args> || ...)) {
                return std::forward(result);
            }
            throw std::logic_error{ "Unexpected case" };
        },
        std::forward<Variant>(t_original)
    );
}

}   // namespace

template <typename... Args>
auto get(vk::Result t_result) -> std::variant<Args...>
{
    return variant_cast<Args...>(to_variant(t_result));
}

}   // namespace engine::vulkan

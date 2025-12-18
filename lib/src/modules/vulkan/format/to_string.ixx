module;

#include <string>
#include <unordered_map>

export module ddge.modules.vulkan.format.to_string;

import vulkan_hpp;

import ddge.utility.containers.Lazy;

namespace ddge::vulkan {

export [[nodiscard]]
constexpr auto to_string(vk::Result result) -> const char*;

}   // namespace ddge::vulkan

namespace ddge::vulkan {

constexpr auto to_string(const vk::Result result) -> const char*
{
    switch (result) {
        case vk::Result::eSuccess: {
            return "VK_SUCCESS";
        }
        case vk::Result::eNotReady: {
            return "VK_NOT_READY";
        }
        case vk::Result::eTimeout: {
            return "VK_TIMEOUT";
        }
        case vk::Result::eEventSet: {
            return "VK_EVENT_SET";
        }
        case vk::Result::eEventReset: {
            return "VK_EVENT_RESET";
        }
        case vk::Result::eIncomplete: {
            return "VK_INCOMPLETE";
        }
        case vk::Result::eErrorOutOfHostMemory: {
            return "VK_ERROR_OUT_OF_HOST_MEMORY";
        }
        case vk::Result::eErrorOutOfDeviceMemory: {
            return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        }
        case vk::Result::eErrorInitializationFailed: {
            return "VK_ERROR_INITIALIZATION_FAILED";
        }
        case vk::Result::eErrorDeviceLost: {
            return "VK_ERROR_DEVICE_LOST";
        }
        case vk::Result::eErrorMemoryMapFailed: {
            return "VK_ERROR_MEMORY_MAP_FAILED";
        }
        case vk::Result::eErrorLayerNotPresent: {
            return "VK_ERROR_LAYER_NOT_PRESENT";
        }
        case vk::Result::eErrorExtensionNotPresent: {
            return "VK_ERROR_EXTENSION_NOT_PRESENT";
        }
        case vk::Result::eErrorFeatureNotPresent: {
            return "VK_ERROR_FEATURE_NOT_PRESENT";
        }
        case vk::Result::eErrorIncompatibleDriver: {
            return "VK_ERROR_INCOMPATIBLE_DRIVER";
        }
        case vk::Result::eErrorTooManyObjects: {
            return "VK_ERROR_TOO_MANY_OBJECTS";
        }
        case vk::Result::eErrorFormatNotSupported: {
            return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        }
        case vk::Result::eErrorFragmentedPool: {
            return "VK_ERROR_FRAGMENTED_POOL";
        }
        case vk::Result::eErrorUnknown: {
            return "VK_ERROR_UNKNOWN";
        }
        case vk::Result::eErrorValidationFailedEXT: {
            return "VK_ERROR_VALIDATION_FAILED_EXT";
        }
        case vk::Result::eErrorOutOfPoolMemory: {
            return "VK_ERROR_OUT_OF_POOL_MEMORY";
        }
        case vk::Result::eErrorInvalidExternalHandle: {
            return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        }
        case vk::Result::eErrorInvalidOpaqueCaptureAddress: {
            return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        }
        case vk::Result::eErrorFragmentation: {
            return "VK_ERROR_FRAGMENTATION";
        }
        case vk::Result::ePipelineCompileRequired: {
            return "VK_PIPELINE_COMPILE_REQUIRED";
        }
        case vk::Result::eErrorNotPermitted: {
            return "VK_ERROR_NOT_PERMITTED";
        }
        case vk::Result::eErrorSurfaceLostKHR: {
            return "VK_ERROR_SURFACE_LOST_KHR";
        }
        case vk::Result::eErrorNativeWindowInUseKHR: {
            return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        }
        case vk::Result::eSuboptimalKHR: {
            return "VK_SUBOPTIMAL_KHR";
        }
        case vk::Result::eErrorOutOfDateKHR: {
            return "VK_ERROR_OUT_OF_DATE_KHR";
        }
        case vk::Result::eErrorIncompatibleDisplayKHR: {
            return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        }
        case vk::Result::eErrorInvalidShaderNV: {
            return "VK_ERROR_INVALID_SHADER_NV";
        }
        case vk::Result::eErrorImageUsageNotSupportedKHR: {
            return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
        }
        case vk::Result::eErrorVideoPictureLayoutNotSupportedKHR: {
            return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
        }
        case vk::Result::eErrorVideoProfileOperationNotSupportedKHR: {
            return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
        }
        case vk::Result::eErrorVideoProfileFormatNotSupportedKHR: {
            return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
        }
        case vk::Result::eErrorVideoProfileCodecNotSupportedKHR: {
            return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
        }
        case vk::Result::eErrorVideoStdVersionNotSupportedKHR: {
            return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
        }
        case vk::Result::eErrorInvalidDrmFormatModifierPlaneLayoutEXT: {
            return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        }
        // case vk::Result::eErrorPresentTimingQueueFullExt: {
        // return "VK_ERROR_PRESENT_TIMING_QUEUE_FULL_EXT";
        // }
        // case vk::Result::eErrorFullScreenExclusiveModeLostExt: {
        // return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
        // }
        case vk::Result::eThreadIdleKHR: {
            return "VK_THREAD_IDLE_KHR";
        }
        case vk::Result::eThreadDoneKHR: {
            return "VK_THREAD_DONE_KHR";
        }
        case vk::Result::eOperationDeferredKHR: {
            return "VK_OPERATION_DEFERRED_KHR";
        }
        case vk::Result::eOperationNotDeferredKHR: {
            return "VK_OPERATION_NOT_DEFERRED_KHR";
        }
        case vk::Result::eErrorInvalidVideoStdParametersKHR: {
            return "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
        }
        case vk::Result::eErrorCompressionExhaustedEXT: {
            return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
        }
        case vk::Result::eIncompatibleShaderBinaryEXT: {
            return "VK_INCOMPATIBLE_SHADER_BINARY_EXT";
        }
        case vk::Result::ePipelineBinaryMissingKHR: {
            return "VK_PIPELINE_BINARY_MISSING_KHR";
        }
        case vk::Result::eErrorNotEnoughSpaceKHR: {
            return "VK_ERROR_NOT_ENOUGH_SPACE_KHR";
        }
        default: {
            static std::unordered_map<vk::Result, std::string> result_string_map;

            return result_string_map
                .try_emplace(
                    result,   //
                    util::Lazy{ [result] { return vk::to_string(result); } }
                )
                .first->second.c_str();
        }
    }
}

}   // namespace ddge::vulkan

#include "result_types.hpp"

namespace engine::vulkan::internal {

[[nodiscard]] auto to_variant(vk::Result t_result) -> Variant
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

}   // namespace engine::vulkan::internal

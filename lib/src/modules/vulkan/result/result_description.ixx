export module ddge.modules.vulkan.result.result_description;

import vulkan_hpp;

import ddge.utility.contracts;

namespace ddge::vulkan {

export [[nodiscard]]
constexpr auto result_description(vk::Result result) noexcept -> const char*;

}   // namespace ddge::vulkan

namespace ddge::vulkan {

constexpr auto result_description(const vk::Result result) noexcept -> const char*
{
    // source:
    // https://docs.vulkan.org/refpages/latest/refpages/source/VkResult.html#_description

    switch (result) {
        case vk::Result::eSuccess: {
            return "Command successfully completed";
        }
        case vk::Result::eNotReady: {
            return "A fence or query has not yet completed";
        }
        case vk::Result::eTimeout: {
            return "A wait operation has not completed in the specified time";
        }
        case vk::Result::eEventSet: {
            return "An event is signaled";
        }
        case vk::Result::eEventReset: {
            return "An event is unsignaled";
        }
        case vk::Result::eIncomplete: {
            return "A return array was too small for the result";
        }
        case vk::Result::eSuboptimalKHR: {
            return "A swapchain no longer matches the surface properties exactly, but "
                       "can still be used to present to the surface successfully.";
        }
        case vk::Result::eThreadIdleKHR: {
            return "A deferred operation is not complete but there is currently no work "
                       "for this thread to do at the time of this call.";
        }
        case vk::Result::eThreadDoneKHR: {
            return "A deferred operation is not complete but there is no work remaining "
                       "to assign to additional threads.";
        }
        case vk::Result::eOperationDeferredKHR: {
            return "A deferred operation was requested and at least some of the work was "
                       "deferred.";
        }
        case vk::Result::eOperationNotDeferredKHR: {
            return "A deferred operation was requested and no operations were deferred.";
        }
        case vk::Result::ePipelineCompileRequired: {
            return "A requested pipeline creation would have required compilation, but "
                       "the application requested compilation to not be performed.";
        }
        case vk::Result::ePipelineBinaryMissingKHR: {
            return "The application attempted to create a pipeline binary by querying an "
                       "internal cache, but the internal cache entry did not exist.";
        }
        case vk::Result::eIncompatibleShaderBinaryEXT: {
            return "The provided binary shader code is not compatible with this device.";
        }
        case vk::Result::eErrorOutOfHostMemory: {
            return "A host memory allocation has failed.";
        }
        case vk::Result::eErrorOutOfDeviceMemory: {
            return "A device memory allocation has failed.";
        }
        case vk::Result::eErrorInitializationFailed: {
            return "Initialization of an object could not be completed for "
                       "implementation-specific reasons.";
        }
        case vk::Result::eErrorDeviceLost: {
            return "The logical or physical device has been lost.";
        }
        case vk::Result::eErrorMemoryMapFailed: {
            return "Mapping of a memory object has failed.";
        }
        case vk::Result::eErrorLayerNotPresent: {
            return "A requested layer is not present or could not be loaded.";
        }
        case vk::Result::eErrorExtensionNotPresent: {
            return "A requested extension is not supported.";
        }
        case vk::Result::eErrorFeatureNotPresent: {
            return "A requested feature is not supported.";
        }
        case vk::Result::eErrorIncompatibleDriver: {
            return "The requested version of Vulkan is not supported by the driver or is "
                       "otherwise incompatible for implementation-specific reasons.";
        }
        case vk::Result::eErrorTooManyObjects: {
            return "Too many objects of the type have already been created.";
        }
        case vk::Result::eErrorFormatNotSupported: {
            return "A requested format is not supported on this device.";
        }
        case vk::Result::eErrorFragmentedPool: {
            return "A pool allocation has failed due to fragmentation of the pool’s "
                       "memory. This must only be returned if no attempt to allocate host or "
                       "device memory was made to accommodate the new allocation. This "
                       "should be returned in preference to VK_ERROR_OUT_OF_POOL_MEMORY, but "
                       "only if the implementation is certain that the pool allocation "
                       "failure was due to fragmentation.";
        }
        case vk::Result::eErrorSurfaceLostKHR: {
            return "A surface is no longer available.";
        }
        case vk::Result::eErrorNativeWindowInUseKHR: {
            return
                    "The requested window is already in use by Vulkan or another API in a "
                    "manner which prevents it from being used again.";
        }
        case vk::Result::eErrorOutOfDateKHR: {
            return "A surface has changed in such a way that it is no longer compatible "
                       "with the swapchain, and further presentation requests using the "
                       "swapchain will fail. Applications must query the new surface "
                       "properties and recreate their swapchain if they wish to continue "
                       "presenting to the surface.";
        }
        case vk::Result::eErrorIncompatibleDisplayKHR: {
            return "The display used by a swapchain does not use the same presentable "
                       "image layout, or is incompatible in a way that prevents sharing an "
                       "image.";
        }
        case vk::Result::eErrorInvalidShaderNV: {
            return "One or more shaders failed to compile or link. More details are "
                       "reported back to the application via VK_EXT_debug_report if enabled.";
        }
        case vk::Result::eErrorOutOfPoolMemory: {
            return "A pool memory allocation has failed. This must only be returned if "
                       "no attempt to allocate host or device memory was made to accommodate "
                       "the new allocation. If the failure was definitely due to "
                       "fragmentation of the pool, VK_ERROR_FRAGMENTED_POOL should be "
                       "returned instead.";
        }
        case vk::Result::eErrorInvalidExternalHandle: {
            return "An external handle is not a valid handle of the specified type.";
        }
        case vk::Result::eErrorFragmentation: {
            return "A descriptor pool creation has failed due to fragmentation.";
        }
        case vk::Result::eErrorInvalidOpaqueCaptureAddress: {
            return "A buffer creation or memory allocation failed because the requested "
                       "address is not available. A shader group handle assignment failed "
                       "because the requested shader group handle information is no longer "
                       "valid.";
        }
        case vk::Result::eErrorValidationFailedEXT: {
            return "A command failed because invalid usage was detected by the "
                       "implementation or a validation layer. This may result in the command "
                       "not being dispatched to the ICD.";
        }
        case vk::Result::eErrorCompressionExhaustedEXT: {
            return "An image creation failed because internal resources required for "
                       "compression are exhausted. This must only be returned when "
                       "fixed-rate compression is requested.";
        }
        case vk::Result::eErrorImageUsageNotSupportedKHR: {
            return "The requested VkImageUsageFlags are not supported.";
        }
        case vk::Result::eErrorVideoPictureLayoutNotSupportedKHR: {
            return "The requested video picture layout is not supported.";
        }
        case vk::Result::eErrorVideoProfileOperationNotSupportedKHR: {
            return "A video profile operation specified via "
                       "VkVideoProfileInfoKHR::videoCodecOperation is not supported.";
        }
        case vk::Result::eErrorVideoProfileFormatNotSupportedKHR: {
            return "Format parameters in a requested VkVideoProfileInfoKHR chain are not "
                       "supported.";
        }
        case vk::Result::eErrorVideoProfileCodecNotSupportedKHR: {
            return "Codec-specific parameters in a requested VkVideoProfileInfoKHR chain "
                       "are not supported.";
        }
        case vk::Result::eErrorVideoStdVersionNotSupportedKHR: {
            return "The specified video Std header version is not supported.";
        }
        case vk::Result::eErrorInvalidVideoStdParametersKHR: {
            return "The specified Video Std parameters do not adhere to the syntactic or "
                       "semantic requirements of the used video compression standard, or "
                       "values derived from parameters according to the rules defined by the "
                       "used video compression standard do not adhere to the capabilities of "
                       "the video compression standard or the implementation.";
        }
        case vk::Result::eErrorNotPermittedKHR: {
            return "The driver implementation has denied a request to acquire a priority "
                       "above the default priority (VK_QUEUE_GLOBAL_PRIORITY_MEDIUM_EXT) "
                       "because the application does not have sufficient privileges.";
        }
        case vk::Result::eErrorNotEnoughSpaceKHR: {
            return "The application did not provide enough space to return all the "
                       "required data.";
        }
        case vk::Result::eErrorUnknown: {
            return "An unknown error has occurred; either the application has provided "
                       "invalid input, or an implementation failure has occurred.";
        }
        default: {
            return "unknown";
        }
    }
}

}   // namespace ddge::vulkan

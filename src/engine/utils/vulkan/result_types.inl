namespace engine::vulkan {

namespace internal {

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

[[nodiscard]] auto to_variant(vk::Result t_result) -> Variant;

template <typename... Args>
[[nodiscard]] auto variant_cast(Variant&& t_original) -> std::variant<Args...>
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

}   // namespace internal

template <typename... Args>
auto get(vk::Result t_result) -> std::variant<Args...>
{
    return variant_cast<Args...>(internal::to_variant(t_result));
}

}   // namespace engine::vulkan

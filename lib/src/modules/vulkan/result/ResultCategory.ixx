export module ddge.modules.vulkan.result.ResultCategory;

namespace ddge::vulkan {

export enum struct ResultCategory {
    eSuccess,
    eRuntimeError,
    ePreconditionViolationError,
    eInternalContractViolationError,
};

}   // namespace ddge::vulkan

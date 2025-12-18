module;

#include <format>
#include <stdexcept>

#include <fmt/compile.h>

#include "utility/contract_macros.hpp"

module ddge.modules.vulkan.result.VulkanError;

import vulkan_hpp;

import ddge.modules.vulkan.format.to_string;
import ddge.modules.vulkan.result.result_category;
import ddge.modules.vulkan.result.result_description;
import ddge.modules.vulkan.result.ResultCategory;
import ddge.utility.contracts;

namespace ddge::vulkan {

VulkanErrorPrecondition::VulkanErrorPrecondition(const vk::Result runtime_error_code)
{
    using namespace fmt::literals;

    PRECOND(
        result_category(runtime_error_code) == ResultCategory::eRuntimeError,
        fmt::format(
            "Error code (`{}`) does not represent a runtime error"_cf,
            vulkan::to_string(runtime_error_code)
        )
    );
}

VulkanError::VulkanError(const vk::Result runtime_error_code)
    : VulkanErrorPrecondition{ runtime_error_code },
      std::runtime_error{
          std::format(
              "`{}` - {}",
              vulkan::to_string(runtime_error_code),
              result_description(runtime_error_code).get()
          )   //
      }
{}

}   // namespace ddge::vulkan

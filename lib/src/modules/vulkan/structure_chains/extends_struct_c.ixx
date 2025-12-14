export module ddge.modules.vulkan.structure_chains.extends_struct_c;

import vulkan_hpp;

namespace ddge::vulkan {

export template <typename T, typename ExtendedStruct_T>
concept extends_struct_c =
    static_cast<bool>(vk::StructExtends<T, ExtendedStruct_T>::value);

}   // namespace ddge::vulkan

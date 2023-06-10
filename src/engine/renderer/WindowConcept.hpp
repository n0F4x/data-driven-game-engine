#pragma once

#include <concepts>


typedef struct VkInstance_T* VkInstance;

#if defined(__LP64__) || defined(_WIN64)                               \
    || (defined(__x86_64__) && !defined(__ILP32__)) || defined(_M_X64) \
    || defined(__ia64) || defined(_M_IA64) || defined(__aarch64__)     \
    || defined(__powerpc64__)

typedef struct VkSurfaceKHR_T* VkSurfaceKHR;

#else

typedef uint64_t VkSurfaceKHR;

#endif


struct VkAllocationCallbacks;

namespace engine::renderer {

template <class WindowType>
concept WindowConcept =
    requires(WindowType t_window,
             const VkInstance& t_instance,
             VkSurfaceKHR& t_surface,
             const VkAllocationCallbacks* t_allocator) {
        {
            t_window.createVulkanSurface(t_instance, t_surface, t_allocator)
        } -> std::convertible_to<bool>;
    };

}   // namespace engine::renderer

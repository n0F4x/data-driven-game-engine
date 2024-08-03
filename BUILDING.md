# Building

## Toolchain

The project is required to compile using Clang version 18 with libc++ AND GCC version 14 with libstc++.

Support for other setups are not a priority until a planned release.

## CMake

External libraries used to be fetched through CMake's `FetchContent`.
This got too slow as the project grew, hence the move to vcpkg.

`FetchContent` is only used for library versions that are not available via vcpkg.

## vcpkg

vcpkg is used as the project's primary dependency manager.

## Vulkan

The setup is in the process of moving away from using the Vulkan SDK.
Statically linking to Vulkan is already not required, as CMake only depends on the VulkanHeaders package, that helps loading the Vulkan functions dynamically.

The shader files need to be compiled into .spv format in order to be used by the example(test) program.
CMake already does that if a glslc executable is found on the machine (it comes with the Vulkan SDK).

To use the Vulkan Validation Layers (when `ENGINE_VULKAN_DEBUG` is defined - see `engine_debug` in CMake and cmake-presets) a local installation is still required.

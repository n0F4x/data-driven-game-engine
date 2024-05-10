# Building

## Compiler

The project currently requires Clang version 17 at least.

## CMake

External libraries used to be fetched through CMakeFetchContent.
This got too slow as the project grew, hence the move to vcpkg.

## vcpkg

Using [vcpkg](https://github.com/microsoft/vcpkg?tab=readme-ov-file#vcpkg-overview) is highly recommended as the vcpkg manifest file describes versions of the project's dependencies.

## Vulkan

The setup is currently moving away from using the Vulkan SDK.
Statically linking to Vulkan is already not required, as CMake only depends on the VulkanHeaders package, that helps loading the Vulkan functions dynamically.

To use the Vulkan Validation Layers (in `Debug` mode) local installation is still required.
The repository has not yet been tested in `Release` mode.

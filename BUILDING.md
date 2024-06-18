# Building

## Compiler

The project currently requires Clang version 17 at least.

## CMake

External libraries used to be fetched through CMakeFetchContent.
This got too slow as the project grew, hence the move to vcpkg.

## vcpkg

The project now uses vcpkg as its primary dependency manager.

## Vulkan

The setup is currently moving away from using the Vulkan SDK.
Statically linking to Vulkan is already not required, as CMake only depends on the VulkanHeaders package, that helps loading the Vulkan functions dynamically.

The shader files need to be compiled into .spv format in order to be used by the example(test) program.
CMake already does that if a glslc executable is found on the machine (it comes with the Vulkan SDK).

To use the Vulkan Validation Layers (in `Debug` mode) a local installation is still required.
The repository has not yet been tested in `Release` mode.

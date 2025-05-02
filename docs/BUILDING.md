# Building

## Toolchain

The project is required to work with **Clang 19** and **libc++**.

Support for other setups is not a priority until a planned release.

## CMake

External libraries used to be fetched through CMake's `FetchContent`.
This got too slow as the project grew, hence the move to a package manager.

`FetchContent` is still used for libraries that don't have the proper versions in a remote.

## Conan

Conan 2 is used as the project's primary dependency manager.

You can install conan using pip ([tutorial](https://docs.conan.io/2/installation.html)).
After this, you would probably want to set up a conan [profile](https://docs.conan.io/2/reference/config_files/profiles.html#profiles) to use with this project.

Run `conan install .` to set up all the third-party dependencies (or `conan install . -pr=your_profile`).
This automatically creates a _CMakeUserPresets.json_ file that can be used to build the project (with your IDE).
`conan build .` not only installs the dependencies, but also builds the project.

Conan profile example:

```
[settings]
arch=x86_64
build_type=Debug
compiler=clang
compiler.version=19
compiler.libcxx=libc++
compiler.cppstd=26
os=Windows

[conf]
tools.build:compiler_executables={"c": "C:/dev/MSYS2/clang64/bin/clang.exe", "cpp": "C:/dev/MSYS2/clang64/bin/clang++.exe"}
tools.cmake.cmaketoolchain:generator=Ninja
tools.cmake.cmaketoolchain:extra_variables={'engine_debug': 'ON', 'engine_test': 'ON', 'engine_examples': 'ON'}

[buildenv]
PATH+=(path)C:/path/to/cmake/bin
PATH+=(path)C:/path/to/ninja/bin
```

## Vulkan

Generally, Vulkan would be heavy to set up without the [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/).
Useful tools included in the SDK that are used by the project are the glslc shader compiler and the Vulkan Validation Layers.
You can install glslc separately if you prefer.
The Vulkan Validation Layers are optional.
(Use the CMake _engine_debug_ option to make use of the validation layers.)

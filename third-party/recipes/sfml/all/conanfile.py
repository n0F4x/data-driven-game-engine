import os

from conan import ConanFile, Version
from conan.errors import ConanInvalidConfiguration
from conan.tools.apple import is_apple_os
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import modules.apply_conandata_patches, export_conandata_patches, get, copy, rmdir
from conan.tools.microsoft import is_msvc, is_msvc_static_runtime


class SFMLRecipe(ConanFile):
    name = "sfml"
    package_type = "library"

    # Optional metadata
    license = "Zlib"
    homepage = "https://www.sfml-dev.org"
    url = "<local>"
    description = "Simple and Fast Multimedia Library."
    topics = ("multimedia", "games", "graphics", "audio")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "window": [True, False],
        "graphics": [True, False],
        "network": [True, False],
        "audio": [True, False],
        # "use_drm": [True, False],  # Linux only - missing required libgbm
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "window": True,
        "graphics": True,
        "network": True,
        "audio": True,
    }
    implements = ["auto_shared_fpic"]

    def validate(self):
        check_min_cppstd(self, 17)

        if self.options.graphics and not self.options.window:
            raise ConanInvalidConfiguration(f"-o={self.ref}:graphics=True requires -o={self.ref}:window=True")

        if self.options.get_safe("shared") and is_msvc_static_runtime(self):
            raise ConanInvalidConfiguration(f"{self.ref} does not support shared libraries with static runtime")

        if self.settings.os not in ["Windows", "Linux", "FreeBSD", "Macos"]:
            raise ConanInvalidConfiguration(f"{self.ref} not supported on {self.settings.os}")

    def validate_build(self):
        if self.settings.os == "Macos" and self.settings.compiler != "apple-clang":
            raise ConanInvalidConfiguration(
                f"{self.ref} is not supported on {self.settings.os} with {self.settings.compiler}")

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

        if not self.options.window:
            self.options.rm_safe("opengl")

    def export_sources(self):
        export_conandata_patches(self)

    def source(self):
        get(self, **self.conan_data["sources"][self.version], destination=self.source_folder,
            strip_root=True)
        apply_conandata_patches(self)

    def requirements(self):
        if self.options.window:
            if self.settings.os in ["Windows", "Linux", "FreeBSD", "Macos"]:
                self.requires("opengl/system")
            if self.settings.os == "Linux":
                self.requires("libudev/system")
            if self.settings.os in ["Linux", "FreeBSD"]:
                self.requires("xorg/system")
        if self.options.graphics:
            self.requires("freetype/2.13.2")
        if self.options.audio:
            self.requires("flac/1.4.3")
            self.requires("vorbis/1.3.7")

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.24 <4]")

    def layout(self):
        cmake_layout(self, src_folder="src")

    def generate(self):
        tc = CMakeToolchain(self)

        tc.variables["SFML_BUILD_WINDOW"] = self.options.window
        tc.variables["SFML_BUILD_GRAPHICS"] = self.options.graphics
        tc.variables["SFML_BUILD_NETWORK"] = self.options.network
        tc.variables["SFML_BUILD_AUDIO"] = self.options.audio
        tc.variables["SFML_INSTALL_PKGCONFIG_FILES"] = False
        tc.variables["SFML_GENERATE_PDB"] = False
        tc.variables["SFML_USE_SYSTEM_DEPS"] = True
        tc.variables["SFML_WARNINGS_AS_ERRORS"] = False

        tc.generate()

        deps = CMakeDeps(self)
        deps.check_components_exist = True
        deps.set_property("freetype", "cmake_file_name", "Freetype")
        deps.set_property("freetype", "cmake_target_name", "Freetype::Freetype")
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, pattern="license.md", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))
        cmake = CMake(self)
        cmake.install()
        rmdir(self, os.path.join(self.package_folder, "lib", "cmake"))
        rmdir(self, os.path.join(self.package_folder, "share"))

    @property
    def _sfml_components(self):
        def gdi32():
            return ["gdi32"] if self.settings.os == "Windows" else []

        def winmm():
            return ["winmm"] if self.settings.os == "Windows" else []

        def ws2_32():
            return ["ws2_32"] if self.settings.os == "Windows" else []

        def dl():
            return ["dl"] if self.settings.os in ["Linux", "FreeBSD"] else []

        def libudev():
            return ["libudev::libudev"] if self.settings.os == "Linux" else []

        def xorg():
            return ["xorg::xorg"] if self.settings.os == "Linux" else []

        def pthread():
            return ["pthread"] if self.settings.os in ["Linux", "FreeBSD"] else []

        def rt():
            return ["rt"] if self.settings.os in ["Linux", "FreeBSD"] else []

        def usbhid():
            return ["usbhid"] if self.settings.os == "FreeBSD" else []

        def foundation():
            return ["Foundation"] if is_apple_os(self) else []

        def appkit():
            return ["AppKit"] if self.settings.os == "Macos" else []

        def carbon():
            return ["Carbon"] if self.settings.os == "Macos" else []

        def iokit():
            return ["IOKit"] if self.settings.os == "Macos" else []

        def coreservices():
            return ["CoreServices"] if self.settings.os == "Macos" else []

        suffix = ""
        if not self.options.get_safe("shared"):
            suffix += "-s"
        if self.settings.build_type == "Debug":
            suffix += "-d"

        sfml_components = {
            "system": {
                "cmake_target_name": "SFML::System",
                "libs": [f"sfml-system{suffix}"],
                "system_libs": winmm() + pthread() + rt(),
            },
        }

        if self.settings.os == "Windows":
            main_suffix = ""
            if self.settings.build_type == "Debug":
                main_suffix += "-d"
            sfml_components.update({
                "main": {
                    "cmake_target_name": "SFML::Main",
                    "libs": [f"sfml-main{main_suffix}"],
                },
            })
        if self.options.window:
            sfml_components.update({
                "window": {
                    "cmake_target_name": "SFML::Window",
                    "libs": [f"sfml-window{suffix}"],
                    "requires": ["system", "opengl::opengl"] + xorg() + libudev(),
                    "system_libs": dl() + gdi32() + winmm() + usbhid(),
                    "frameworks": foundation() + appkit() + iokit() + carbon() + coreservices(),
                },
            })
        if self.options.graphics:
            sfml_components.update({
                "graphics": {
                    "cmake_target_name": "SFML::Graphics",
                    "libs": [f"sfml-graphics{suffix}"],
                    "requires": ["window", "freetype::freetype"],
                },
            })
        if self.options.network:
            sfml_components.update({
                "network": {
                    "cmake_target_name": "SFML::Network",
                    "libs": [f"sfml-network{suffix}"],
                    "requires": ["system"],
                    "system_libs": ws2_32(),
                },
            })
        if self.options.audio:
            sfml_components.update({
                "audio": {
                    "cmake_target_name": "SFML::Audio",
                    "libs": [f"sfml-audio{suffix}"],
                    "requires": [
                        "system",
                        "flac::flac",
                        "vorbis::vorbis",
                    ],
                },
            })

        return sfml_components

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "SFML")

        def _register_components(components):
            # SFML_STATIC is buggy, so it should be defined explicitly
            defines = [] if self.options.get_safe("shared") else ["SFML_STATIC"]
            for component, values in components.items():
                cmake_target_name = values.get("cmake_target_name", [])
                libs = values.get("libs", [])
                requires = values.get("requires", [])
                system_libs = values.get("system_libs", [])
                frameworks = values.get("frameworks", [])

                self.cpp_info.components[component].set_property("cmake_target_name", cmake_target_name)
                self.cpp_info.components[component].libs = libs
                self.cpp_info.components[component].defines = defines
                self.cpp_info.components[component].requires = requires
                self.cpp_info.components[component].system_libs = system_libs
                self.cpp_info.components[component].frameworks = frameworks

        _register_components(self._sfml_components)

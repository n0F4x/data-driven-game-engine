from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake


class ConanApplication(ConanFile):
    package_type = "application"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("gsl-lite/0.42.0")
        self.requires("plf_colony/7.39")
        self.requires("tsl-ordered-map/1.1.0")
        self.requires("spdlog/1.15.0")
        self.requires("glfw/3.4")
        self.requires("vulkan-headers/1.3.296.0")
        self.requires("glm/1.0.1")
        self.requires("ktx/4.3.2")
        self.requires("stb/cci.20240531")
        self.requires("entt/3.14.0")

        self.requires("catch2/3.8.0", test=True)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

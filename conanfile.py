from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class DataDrivenGameEngineRecipe(ConanFile):
    name = "data-driven_game_engine"
    package_type = "library"

    # Optional metadata
    url = "https://github.com/n0F4x/data-driven-game-engine"

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        # TODO: "visibility": ["default", "hidden", "internal", "protected"],  # controls -fvisibility
        "log_level": ["off", "critical", "error", "warning", "info", "debug", "trace"],
        "enable_tests": [True, False],
        "enable_examples": [True, False],
        "use_asan": [True, False], # Address Sanitizer
        "use_ubsan": [True, False], # UB Sanitizer
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        # TODO: "visibility": "hidden",
        "log_level": "info",
        "enable_tests": False,
        "enable_examples": False,
        "use_asan": False,
        "use_ubsan": False,
    }
    implements = ["auto_shared_fpic"]

    def validate(self):
        check_min_cppstd(self, "26")

        if (self.settings.compiler != "clang"
                or self.settings.compiler.version != 20
                or self.settings.compiler.libcxx != "libc++"):
            raise ConanInvalidConfiguration("Only Clang 20 with libc++ is supported")

        if self.conf.get("tools.cmake.cmaketoolchain:generator") != "Ninja":
            raise ConanInvalidConfiguration("Ninja is required for CXX modules")

    def requirements(self):
        self.requires("gsl-lite/0.42.0")
        self.requires("tsl-ordered-map/1.1.0")
        self.requires("fmt/11.1.3")
        self.requires("spdlog/1.15.1")
        self.requires("glfw/3.4")
        self.requires("vulkan-headers/1.3.296.0")
        self.requires("vk-bootstrap/1.3.296")
        self.requires("glm/1.0.1")
        self.requires("ktx/4.4.0")
        self.requires("stb/cci.20240531")
        self.requires("entt/3.14.0")

        if self.options.enable_tests:
            self.requires("catch2/3.8.0", test=True)

        if self.options.enable_examples:
            self.requires("sfml/3.0.1")

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.30 <4]")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        # CMakeDeps
        deps = CMakeDeps(self)
        deps.generate()

        # CMakeToolChain
        tc = CMakeToolchain(self)

        project_prefix = "ENGINE_"
        tc.cache_variables[project_prefix + "LOG_LEVEL"] \
            = project_prefix + "LOG_LEVEL_" + self.options.log_level.value.upper()
        tc.cache_variables[project_prefix + "ENABLE_TESTS"] = self.options.enable_tests
        tc.cache_variables[project_prefix + "ENABLE_EXAMPLES"] = self.options.enable_examples
        tc.cache_variables[project_prefix + "USE_ASAN"] = self.options.use_asan
        tc.cache_variables[project_prefix + "USE_UBSAN"] = self.options.use_ubsan

        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if self.options.get_safe("enable_tests") and not self.conf.get("tools.build:skip_test", default=False):
            self.run("tests", cwd="tests")

    def package_info(self):
        self.cpp_info.libs = ["engine_lib"]

from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class DataDrivenGameEngineRecipe(ConanFile):
    name = "data-driven_game_engine"
    version = "0.0.1"
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
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        # TODO: "visibility": "hidden",
        "log_level": "info",
    }
    implements = ["auto_shared_fpic"]

    @property
    def _dev(self):
        return bool(self.conf.get(f"user.{self.name}:dev", default=False))

    @property
    def _enable_tests(self):
        return self._dev and bool(self.conf.get(f"user.{self.name}:enable_tests", default=False))

    @property
    def _enable_examples(self):
        return self._dev and bool(self.conf.get(f"user.{self.name}:enable_examples", default=False))

    def validate(self):
        check_min_cppstd(self, "26")

        if (self.settings.compiler != "clang"
                or self.settings.compiler.version != 21
                or self.settings.compiler.libcxx != "libc++"):
            raise ConanInvalidConfiguration("Only Clang 21 with libc++ is supported")

        if self.conf.get("tools.cmake.cmaketoolchain:generator") != "Ninja":
            raise ConanInvalidConfiguration("Ninja is required for CXX modules")

        if bool(self.conf.get(f"user.{self.name}:enable_tests", default=False)) and not self._dev:
            raise ConanInvalidConfiguration(
                f"'user.{self.name}:enable_tests' requires 'user.{self.name}:dev'"
            )

        if bool(self.conf.get(f"user.{self.name}:enable_examples", default=False)) and not self._dev:
            raise ConanInvalidConfiguration(
                f"'user.{self.name}:enable_examples' requires 'user.{self.name}:dev'"
            )

    def build_requirements(self):
        if self._dev:
            self.tool_requires("cmake/[~3.30]")
        else:
            self.tool_requires("cmake/[>=3.30]")

    def requirements(self):
        self.requires("gsl-lite/1.0.1", transitive_headers=True)
        self.requires("tl-function-ref/1.0.0", transitive_headers=True)
        self.requires("function2/4.2.5", transitive_headers=True)
        self.requires("tsl-ordered-map/1.1.0", transitive_headers=True)
        self.requires("fmt/12.0.0", transitive_headers=True, force=True)
        self.requires("spdlog/1.15.3")
        self.requires("glfw/3.4", transitive_headers=True)
        self.requires("vulkan-headers/1.3.296.0")
        self.requires("vulkan-memory-allocator/3.2.1", transitive_headers=True)
        self.requires("vk-bootstrap/1.3.296", transitive_headers=True)
        self.requires("glm/1.0.1", transitive_headers=True)
        self.requires("ktx/4.3.2", transitive_headers=True, options={"tools": False})
        self.requires("stb/cci.20240531", transitive_headers=True)
        self.requires("fastgltf/0.9.0", transitive_headers=True)
        self.requires("entt/3.15.0", transitive_headers=True)

        if self._enable_tests:
            self.test_requires("catch2/3.10.0")
        if self._enable_examples:
            self.requires("sfml/3.0.1")

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
        tc.cache_variables[project_prefix + "ENABLE_TESTS"] = self._enable_tests
        tc.cache_variables[project_prefix + "ENABLE_EXAMPLES"] = self._enable_examples

        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure(build_script_folder=None if self._dev else "lib")
        cmake.build()
        if self._enable_tests and not self.conf.get("tools.build:skip_test", default=False):
            self.run("tests", cwd="tests")

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "engine")
        self.cpp_info.libs = ["engine"]
        self.cpp_info.set_property("cmake_target_name", "engine::engine")

        # TODO: remove these once Conan learns cxx modules
        self.cpp_info.set_property("cmake_find_mode", "none")
        self.cpp_info.builddirs = ["."]

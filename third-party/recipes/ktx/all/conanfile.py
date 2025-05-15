from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import apply_conandata_patches, copy, export_conandata_patches, get, rmdir, replace_in_file, save
import os


class KtxConan(ConanFile):
    name = "ktx"
    description = "Khronos Texture library and tool."
    license = "Apache-2.0"
    topics = ("texture", "khronos")
    homepage = "https://github.com/KhronosGroup/KTX-Software"
    url = "<local>"
    package_type = "library"
    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "sse": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "sse": True,
    }

    @property
    def _has_sse_support(self):
        return self.settings.arch in ["x86", "x86_64"]

    def export_sources(self):
        export_conandata_patches(self)

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
        if not self._has_sse_support:
            del self.options.sse

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self, src_folder="src")

    def requirements(self):
        self.requires("zstd/1.5.5")

    def validate(self):
        check_min_cppstd(self, 17)

    def _patch_sources(self):
        apply_conandata_patches(self)
        # Unvendor several libs (we rely on patch files to link those libs)
        # It's worth noting that vendored jpeg-compressor can't be replaced by CCI equivalent
        ## zstd
        rmdir(self, os.path.join(self.source_folder, "external", "basisu", "zstd"))
        ## fmt
        rmdir(self, os.path.join(self.source_folder, "external", "fmt"))
        # Avoid copying test assets
        rmdir(self, os.path.join(self.source_folder, "tests"))
        save(self, os.path.join(self.source_folder, "tests", "CMakeLists.txt"), "")

    def source(self):
        get(self, **self.conan_data["sources"][self.version], strip_root=True)
        self._patch_sources()

    def generate(self):
        tc = CMakeToolchain(self)
        tc.cache_variables["KTX_GIT_VERSION_FULL"] = "v4.4.0"
        tc.cache_variables["KTX_FEATURE_DOC"] = False
        tc.cache_variables["KTX_FEATURE_JNI"] = False
        tc.cache_variables["KTX_FEATURE_PY"] = False
        tc.cache_variables["KTX_FEATURE_TESTS"] = False
        tc.cache_variables["KTX_FEATURE_ETC_UNPACK"] = True
        tc.cache_variables["KTX_FEATURE_KTX1"] = True
        tc.cache_variables["KTX_FEATURE_KTX2"] = True
        tc.cache_variables["KTX_FEATURE_VK_UPLOAD"] = True
        tc.cache_variables["KTX_FEATURE_GL_UPLOAD"] = True
        tc.cache_variables["KTX_FEATURE_LOADTEST_APPS"] = False
        tc.cache_variables["BASISU_SUPPORT_SSE"] = self.options.get_safe("sse", False)
        tc.cache_variables["BASISU_SUPPORT_OPENCL"] = False
        tc.cache_variables["KTX_FEATURE_TOOLS"] = False
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, "LICENSE.md", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))
        copy(self, "*", src=os.path.join(self.source_folder, "LICENSES"), dst=os.path.join(self.package_folder, "licenses"))
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["ktx"]
        self.cpp_info.set_property("cmake_file_name", "Ktx")
        self.cpp_info.set_property("cmake_target_name", "KTX::ktx")
        self.cpp_info.defines+=["KTX_FEATURE_WRITE"]
        if not self.options.get_safe("shared"):
            self.cpp_info.defines+=["KHRONOS_STATIC"]

        # if self.settings.os == "Linux":
        #     self.cpp_info.system_libs.extend(["m", "dl", "pthread"])

        self.cpp_info.requires = ["zstd::zstd"]

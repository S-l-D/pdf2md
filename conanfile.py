from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout


class Pdf2MdEngineRecipe(ConanFile):
    name = "pdf2md_engine"
    version = "0.1.0"
    settings = "os", "compiler", "build_type", "arch"

    options = {
        "enable_tests": [True, False],
    }
    default_options = {
        "enable_tests": True,
        # Форсируем легковесную CPU-сборку для ONNX Runtime
        "onnxruntime/*:shared": True,
        "libwebp/*:shared": False,
    }

    def requirements(self):
        self.requires("onnxruntime/1.18.1")
        self.requires("libwebp/1.4.0")
        self.requires("fmt/11.0.2")
        self.requires("cli11/2.4.2")
        self.requires("stb/cci.20230920")

    def build_requirements(self):
        if self.options.enable_tests:
            self.test_requires("gtest/1.15.0")

    def layout(self):
        # Создает build/Debug или build/Release и CMakeUserPresets.json
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        tc = CMakeToolchain(self, generator="Ninja")
        tc.variables["TESTS_ENABLED"] = self.options.enable_tests
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
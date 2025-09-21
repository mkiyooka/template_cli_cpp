from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain


class TemplateCLICppConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        self.requires("cli11/2.5.0")  # CLI parsing library
        self.requires("tomlplusplus/3.4.0")  # TOML configuration library
        self.requires("fmt/11.2.0")  # Formatting library
        self.requires("doctest/2.4.11")  # Testing framework

    def layout(self):
        # ルートディレクトリのbuildフォルダを使用
        self.folders.build = "."
        self.folders.generators = "generators"

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        # CMakeUserPresets.jsonの生成を無効化
        tc.user_presets_path = False
        tc.generate()

    def configure(self):
        pass

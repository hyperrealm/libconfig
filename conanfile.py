from conans import ConanFile, CMake, tools


class HelloConan(ConanFile):
    name = "LibConfig"
    version = "1.7.2"
    license = "GNU LESSER GENERAL PUBLIC LICENSE"
    author = """Mark Lindner - Lead developer & maintainer.
                Daniel Marjamäki - Enhancements & bugfixes.
                Andrew Tytula - Windows port.
                Glenn Herteg - Enhancements, bugfixes, documentation corrections.s
                Matt Renaud - Enhancements & bugfixes.
                JoseLuis Tallon - Enhancements & bugfixes"""
    url = "hyperrealm.github.io/libconfig/"
    description = "Libconfig is a simple library for processing structured configuration files, like this one. This file format is more compact and more readable than XML. And unlike XML, it is type-aware, so it is not necessary to do string parsing in application code."
    topics = ("config")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    generators = "cmake"
    exports_sources="*"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("*.h", dst="include", src="lib")
        self.copy("*.hpp", dst="include", src="lib")
        self.copy("*.h++", dst="include", src="lib")
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["config", "config++"]


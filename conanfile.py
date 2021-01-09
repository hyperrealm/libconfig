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
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": False}
    generators = "cmake"
    exports_sources="*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            del self.options.fPIC

    def build(self):
        self.cmake = CMake(self)
        self.cmake.configure(source_folder=self.build_folder)
        self.cmake.build()

    def package(self):
        self.cmake.install()

    def package_info(self):
        # FIXME: `libconfig` is `libconfig::libconfig` in `libconfigConfig.cmake`
        # FIXME: `libconfig++` is `libconfig::libconfig++` in `libconfig++Config.cmake`
        self.cpp_info.components["libconfig_c"].libs = ["libconfig"]
        if not self.options.shared:
            self.cpp_info.components["libconfig_c"].defines = ["LIBCONFIG_STATIC"]
        self.cpp_info.components["libconfig_c"].names["cmake_find_package"] = ["libconfig"]
        self.cpp_info.components["libconfig_c"].names["cmake_find_package_multi"] = ["libconfig"]
        self.cpp_info.components["libconfig_c"].names["pkg_config"] = "libconfig"
        self.cpp_info.components["libconfig_cpp"].libs = ["libconfig++"]
        if not self.options.shared:
            self.cpp_info.components["libconfig_cpp"].defines = ["LIBCONFIGXX_STATIC"]
        self.cpp_info.components["libconfig_cpp"].names["cmake_find_package"] = ["libconfig++"]
        self.cpp_info.components["libconfig_cpp"].names["cmake_find_package_multi"] = ["libconfig++"]
        self.cpp_info.components["libconfig_cpp"].names["pg_config"] = "libconfig++"
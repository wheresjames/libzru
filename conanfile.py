
#
#   Install dependencies
# $ conan install .
#
#   Conan build dependencies
# $ conan install . --build
#
#   Conan build
# $ conan build .
#
#   Build from cmake
# $ cmake --build .out -j 8
#

import datetime
from conans import ConanFile, tools, CMake

class Library_libzru(ConanFile):
    name = "libzru"
    company = ""
    version = "0.1"
    license = ""
    url = ""
    buildno = datetime.datetime.now().strftime("%y.%-m.%-d.%-H%M")
    # requires =
    settings = "os", "compiler", "arch", "build_type"
    options = {}
    default_options = ""
    exports = "*"
    generators = "cmake"
    build_policy = "missing"
    build_output = "./out"

    def conan_info(self):
        self.info.settings.clear()

    def configure(self):
        pass

    def build(self):
        cmake = CMake(self)
        # print("cmake.command_line : %s" % cmake.command_line)
        # print("cmake.build_config : %s" % cmake.build_config)
        appopts = "-DBuildApps=\"ON\""
        appopts += " -DAPPNAME=\"%s\" -DAPPVER=\"%s\" -DAPPBUILD=\"%s\"" % (self.name, self.version, self.buildno)
        self.run('cmake . -B %s %s %s' % (self.build_output, cmake.command_line, appopts))
        self.run("cmake --build %s -j 8 %s" % (self.build_output, cmake.build_config))

    def package(self):
        self.copy("*.so")

    def package_info(self):
        print(self.package_folder)


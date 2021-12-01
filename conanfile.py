from conans import python_requires, tools

base = python_requires("conan_template/[~=5]@robotkernel/stable")

class MainProject(base.RobotkernelConanFile):
    name = "service_provider_canopen_protocol"
    description = "robotkernel service provider for canopen protocol devices."
    exports_sources = ["*", "!.gitignore"] + ["!%s" % x for x in tools.Git().excluded_files()]
    requires = "robotkernel/[~=5]@robotkernel/stable"

    def package_info(self):
        super(base.RobotkernelConanFile, self).package_info()
        self.env_info.PYTHONPATH.append(os.path.join(self.package_folder, "bindings/python/rk_gui_plugin"))


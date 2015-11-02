#include "interface.hpp"
#include "libConfig.hpp"

bool addPackages() {
}

bool initLib(std::string root, std::string config, std::string arch, std::string DB) {
    libConfig::rootPath = root;
    libConfig::configFile = config;
    libConfig::arch= arch;
    libConfig::dbpath = DB;
}

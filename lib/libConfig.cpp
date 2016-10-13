#include "libConfig.hpp"
#include "interface.hpp"
#include "database.hpp"

std::string libConfig::rootPath = "/";
std::string libConfig::arch = "";
std::string libConfig::dbpath = "/var/lib/pacman-g2";
std::string libConfig::configFile = "../pong/etc/pong-lib.conf";

bool initLib(std::string root, std::string config, std::string arch, std::string DB) {
    if(root != "") {
        libConfig::rootPath = root;
    }
    if(config != "") {
        libConfig::configFile = config;
    }
    if(arch == "") {
#ifdef __i686__
        arch = "i686";
#elif __x86_64__
        arch = "x86_64";
#elif __arm__
        arch = "arm";
#endif
    }
    else {
        libConfig::arch= arch;
    }
    if(DB != "") {
        libConfig::dbpath = DB;
    }
    initDB(libConfig::dbpath);
    return true;
}

bool dbClean() {
    closeDB();
    return true;
}

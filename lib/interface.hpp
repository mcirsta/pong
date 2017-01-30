#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <string>
#include <vector>

enum class DBOpts {
    OP_REBUILD_DEPS,
    OP_ALL_DEPS,
    OP_REV_DEPS,
    OP_DIRECT_DEPS,
};

bool addPackages();

bool dbQuery(DBOpts opType, const std::vector<std::string> &opArg, std::string &retStr);

bool initLib(std::string rootPath, std::string config, std::string arch, std::string DB);

bool pongLibClose();

#endif

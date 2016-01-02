#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <string>

enum class DBOpts {
    OP_REBUILD_DEPS,
    OP_ALL_DEPS,
    OP_REV_DEPS,
    OP_DIRECT_DEPS,
};

bool addPackages();

const char* dbQuery(DBOpts opType, const char* opArg);

bool initLib(std::string rootPath, std::string config, std::string arch, std::string DB);

bool dbClean();

#endif

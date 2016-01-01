#ifndef DATABASE_H_
#define DATABASE_H_

#include <string>
#include <iostream>
#include <sqlite3.h>
#include <vector>

bool initDB(const std::string &dbpath);
bool closeDB();
bool openLegacyDB(const std::string &dbpath);
bool extractOldDB(FILE *dbFile);
bool createNewDB();
bool importData(const std::string &dbpath);

struct pkgData {
    std::string str, name, desc, version, sha1sum, arch, group;
    unsigned long long csize=0,usize=0;
};

enum class pRel { NONE,
       LESS,
       LESS_EQ,
       EQ,
       MORE,
       MORE_EQ,
     };

struct pkgDep {
    std::string depName, depVer;
    pRel depRel;
};

typedef std::vector<pkgDep> pkgDeps;
typedef std::vector<std::string> pkgReplaces;
typedef std::vector<std::string> pkgConflicts;
typedef std::vector<std::string> pkGProvides;

class globalDB {
public:
    static sqlite3 *getDBHandle();
    static void setDBHandle(sqlite3 * dbH);
    static sqlite3 *DBHandle;
};

#endif

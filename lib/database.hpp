#ifndef DATABASE_H_
#define DATABASE_H_

#include <string>
#include <iostream>
#include <sqlite3.h>
#include <vector>
#include <map>

bool initDB(const std::string &dbpath);
bool closeDB();
bool openLegacyDB(const std::string &dbpath);
bool extractOldDB(FILE *dbFile);
bool createNewDB();
bool importData(const std::string &dbpath);

struct pkgData {
    std::string desc, version, sha1sum, arch;
    sqlite3_int64 csize=0,usize=0, dbPkgId=0;
    std::vector<sqlite_int64> groupIds;
    bool installed, localPkg;
    std::string installedVer;
};

enum class pRel { NONE = 1,
                  LESS,
                  LESS_EQ,
                  EQ,
                  MORE,
                  MORE_EQ,
                  MAX,
                };

enum class instReason { EXPLICIT  = 0,  /* explicitly requested by the user */
                        DEPEND    = 1,  /* installed as a dependency for another package */
                        UNKNOWN   = 2,  /* not mentioned in local install file */
                      };

struct pkgLocalData {
    std::string desc, version, url, arch, buildDate, buildType, installDate, packager;
    instReason reason;
    sqlite3_int64 size=0, dbPkgId=0;
    std::vector<sqlite_int64> groupIds;
};


struct pkgDep {
    std::string depName, depVer;
    pRel depRel;
};

typedef std::vector<pkgDep> pkgDeps;
typedef std::vector<std::string> pkgReplaces;
typedef std::vector<std::string> pkgConflicts;
typedef std::vector<std::string> pkgProvides;
typedef sqlite3_stmt* p_sqlite3_stmt;

typedef std::map<std::string, pkgData> allPkgsMap;
typedef std::map<std::string, pkgLocalData> localPkgsMap;

struct packageRelData {
    std::string pkgName;
    pkgDeps pDeps;
    pkgReplaces pReplaces;
    pkgConflicts pConflicts;
    pkgProvides pProvides;
};

class globalDB {
public:
    static sqlite3 *getDBHandle();
    static void setDBHandle(sqlite3 * dbH);
    static sqlite3 *DBHandle;
    static sqlite3_int64 getGroupId(std::string grName);
    static sqlite3_int64 getArchId(std::string archName);
    static std::map <std::string,sqlite3_int64> grIds;
    static std::map <std::string,sqlite3_int64> archIds;
};

bool sqlite3PongQuery(sqlite3_stmt **sqlStmt, const char* queryText);
bool sqlite3PongBindText(p_sqlite3_stmt &sqlStmt, const std::string &bindText, const int &bindPos = 1);
bool sqlite3PongBindInt(p_sqlite3_stmt &sqlStmt,const int &bindInt, const int &bindPos = 1);
bool sqlite3PongStep(p_sqlite3_stmt &sqlStmt);
bool sqlite3PongFinalize(p_sqlite3_stmt &sqlStmt);


enum class PackageLines {
    NAME,
    VERSION,
    DESC,
    SHA1SUM,
    CSIZE,
    USIZE,
    ARCH,
    GROUPS,
    //local DB
    URL,
    SIZE,
    BUILDDATE,
    BUILDTYPE,
    INSTALLDATE,
    PACKAGER,
    REASON,
};

#endif

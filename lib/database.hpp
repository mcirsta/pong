#ifndef DATABASE_H_
#define DATABASE_H_

#include <string>
#include <iostream>
#include <sqlite3.h>

bool initDB(const std::string &dbpath);
bool openLegacyDB(const std::string &dbpath);
bool extractOldDB(FILE *dbFile);
bool createNewDB(const std::string &dbpath);
bool importData(const std::string &dbpath);

struct pkgData {
    std::string str, name, desc, version, sha1sum;
    unsigned long long csize=0,usize=0;
};

class globalDB {
public:
    static sqlite3 *getDBHandle();
    static void setDBHandle(sqlite3 * dbH);
    static sqlite3 *DBHandle;
};

#endif

#include "database.hpp"
#include "utils.hpp"

#include <cstdio>
#include <sqlite3.h>

bool extractOldDB(FILE *dbFile) {
    extractXZ(dbFile, "/tmp/pongTemp/");
}

bool openLegacyDB(std::string dbpath) {
    std::string dbName = dbpath + "/" + "frugalware-current.fdb";
    FILE *dbFile = fopen( dbName.c_str(), "r");
    if(!checkXZ(dbFile)) {
        std::cout<<"invalid original database file: "<<dbName<<std::endl;
        return false;
    }
    std::cout<<"looks good, opening: "<<dbName<<std::endl;
    extractOldDB(dbFile);
    fclose(dbFile);
    return true;
}

bool createNewDB(std::string dbpath) {
    std::string dbName = dbpath + "/" + "frugalware-current.pdb";
    sqlite3 *dbHandle;
    int r = sqlite3_open_v2(dbName.c_str(), &dbHandle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX
                            | SQLITE_OPEN_PRIVATECACHE, nullptr);
    if(r != SQLITE_OK) {
        std::cout<<"error while opening Sqlite database: "<<r;
        return false;
    }
    sqlite3_stmt *sqlStmt;
    const char *createStr = "CREATE TABLE Packages ( \
                            pkgId int,"
                                      ")";
    r = sqlite3_prepare_v2(dbHandle, createStr, -1, &sqlStmt, nullptr);
}

bool importData(std::string dbpath) {

}

bool initDB(std::string dbpath) {
    std::cout<<"database path: "<<dbpath<<std::endl;
    openLegacyDB(dbpath);
    createNewDB("/tmp/pongTemp");
    importData("");
}

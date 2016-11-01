#include "interface.hpp"
#include "database.hpp"

#include <iostream>

int getPkgID(const std::string &pkgName) {
    sqlite3_stmt *sqlStmt;
    int pkgID = -1;
    const char *getDirectDepsStr = "SELECT rowid FROM packages WHERE name=?;";
    if(sqlite3PongQuery(&sqlStmt, getDirectDepsStr) &&
            sqlite3PongBindText(sqlStmt, pkgName, 1) &&
            sqlite3_step(sqlStmt) == SQLITE_ROW)
    {
        pkgID = sqlite3_column_int(sqlStmt, 0);
    }
    else {
        std::cout<<sqlite3_errmsg(globalDB::getDBHandle())<< " : "<<pkgName<<std::endl;
    }
    sqlite3_finalize(sqlStmt);
    return pkgID;
}

bool allDepsQuery(int pkgID, std::string &retStr) {
    sqlite3_stmt *sqlStmt = nullptr;
    bool success = false;
    const char *getAllDepsStr= "WITH RECURSIVE " \
                               "alldeps(n) AS ( " \
                               "SELECT deps.dependID FROM deps WHERE deps.packageID=? " \
                               "UNION " \
                               "SELECT deps.dependID FROM deps,alldeps WHERE deps.packageID=alldeps.n ) " \
                               "SELECT name from alldeps JOIN packages ON packages.rowid=n ORDER BY name; ";
    if(sqlite3PongQuery(&sqlStmt, getAllDepsStr) && sqlite3PongBindInt(sqlStmt, pkgID, 1)) {
        while(sqlite3_step(sqlStmt) == SQLITE_ROW) {
            const unsigned char* dep = sqlite3_column_text(sqlStmt, 0);
            retStr += (const char *)dep;
            retStr += " ";
        }
        success = true;
    }
    sqlite3PongFinalize(sqlStmt);
    return success;
}

bool directDepsQuery(int pkgID, std::string &retStr) {
    sqlite3_stmt *sqlStmt = nullptr;
    bool success = false;
    const char *getDirectDepsStr= "SELECT name FROM deps JOIN packages ON deps.dependID=packages.rowid WHERE deps.packageID=?";
    if(sqlite3PongQuery(&sqlStmt, getDirectDepsStr) &&  sqlite3PongBindInt(sqlStmt, pkgID, 1)) {
        while(sqlite3_step(sqlStmt) == SQLITE_ROW) {
            const unsigned char* dep = sqlite3_column_text(sqlStmt, 0);
            retStr += (const char *)dep;
            retStr += " ";
        }
        success = true;
    }
    sqlite3PongFinalize(sqlStmt);
    return success;
}

bool rebuildDepsQuery(int pkgID, std::string &retStr) {
    sqlite3_stmt *sqlStmt = nullptr;
    bool success = false;
    const char *getRebuildDepsStr= "SELECT name FROM deps JOIN packages ON deps.packageID=packages.rowid "
                                   "WHERE deps.dependID=? AND NOT deps.dependRel=?;";
    if(sqlite3PongQuery(&sqlStmt, getRebuildDepsStr) &&
            sqlite3PongBindInt(sqlStmt, pkgID, 1) &&
            sqlite3PongBindInt(sqlStmt, static_cast<int>(pRel::NONE), 2)) {

        while(sqlite3_step(sqlStmt) == SQLITE_ROW) {
            const unsigned char* dep = sqlite3_column_text(sqlStmt, 0);
            retStr += (const char *)dep;
            retStr += " ";
        }
        success = true;
    }
    sqlite3PongFinalize(sqlStmt);
    return success;
}

bool revdepsQuery(int pkgID, std::string &retStr) {
    sqlite3_stmt *sqlStmt = nullptr;
    bool succes = false;
    const char *getRebuildDepsStr= "SELECT name FROM deps JOIN packages ON deps.packageID=packages.rowid "
                                   "WHERE deps.dependID=?;";
    if(sqlite3PongQuery(&sqlStmt, getRebuildDepsStr) && sqlite3PongBindInt(sqlStmt, pkgID, 1)) {
        std::string depends = "";
        while(sqlite3_step(sqlStmt) == SQLITE_ROW) {
            const unsigned char* dep = sqlite3_column_text(sqlStmt, 0);
            retStr += (const char *)dep;
            retStr += " ";
        }
        succes = true;
    }
    sqlite3PongFinalize(sqlStmt);
    return succes;
}


bool dbQuery(DBOpts opType, const char* opArg, std::string &retStr) {
    int pkgId = getPkgID(opArg);
    if(pkgId == 0) {
        std::cout<<"package "<<opArg<<" not found in the database"<<std::endl;
        return "";
    }
    switch (opType) {
    case DBOpts::OP_ALL_DEPS:       return allDepsQuery(pkgId, retStr);
    case DBOpts::OP_DIRECT_DEPS:    return directDepsQuery(pkgId, retStr);
    case DBOpts::OP_REBUILD_DEPS:   return rebuildDepsQuery(pkgId, retStr);
    case DBOpts::OP_REV_DEPS:       return revdepsQuery(pkgId, retStr);
    default: std::cout<<"unknown op"<<std::endl;    return "";
    }
}

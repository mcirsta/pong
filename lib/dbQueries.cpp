#include "interface.hpp"
#include "database.hpp"

#include <iostream>

int getPkgID(const char *pkgName) {
    sqlite3_stmt *sqlStmt;
    const char *getDirectDepsStr = "SELECT rowid FROM packages WHERE name=?;";
    int r = sqlite3_prepare_v2(globalDB::getDBHandle(), getDirectDepsStr, -1, &sqlStmt, nullptr);
    r = sqlite3_bind_text(sqlStmt, 1, pkgName, -1, SQLITE_STATIC);
    r = sqlite3_step(sqlStmt);
    int pkgID = 0;
    if(r == SQLITE_ROW) {
        pkgID = sqlite3_column_int(sqlStmt, 0);
    }
    else {
        std::cout<<sqlite3_errmsg(globalDB::getDBHandle())<< " : "<<pkgName<<std::endl;
    }
    r = sqlite3_finalize(sqlStmt);
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
    if(sqlite3PongQuery(&sqlStmt, getAllDepsStr) && sqlite3_bind_int(sqlStmt, 1, pkgID) == SQLITE_OK) {
        while(sqlite3_step(sqlStmt) == SQLITE_ROW) {
            const unsigned char* dep = sqlite3_column_text(sqlStmt, 0);
            retStr += (const char *)dep;
            retStr += " ";
        }
        success = true;
    }
    sqlite3_finalize(sqlStmt);
    return success;
}

bool directDepsQuery(int pkgID, std::string &retStr) {
    sqlite3_stmt *sqlStmt = nullptr;
    bool success = false;
    const char *getDirectDepsStr= "SELECT name FROM deps JOIN packages ON deps.dependID=packages.rowid WHERE deps.packageID=?";
    if(sqlite3PongQuery(&sqlStmt, getDirectDepsStr) &&  sqlite3_bind_int(sqlStmt, 1, pkgID) == SQLITE_OK) {
        sqlite3_bind_int(sqlStmt, 1, pkgID);
        while(sqlite3_step(sqlStmt) == SQLITE_ROW) {
            const unsigned char* dep = sqlite3_column_text(sqlStmt, 0);
            retStr += (const char *)dep;
            retStr += " ";
        }
        success = true;
    }
    sqlite3_finalize(sqlStmt);
    return success;
}

bool rebuildDepsQuery(int pkgID, std::string &retStr) {
    sqlite3_stmt *sqlStmt = nullptr;
    bool success = false;
    const char *getRebuildDepsStr= "SELECT name FROM deps JOIN packages ON deps.packageID=packages.rowid "
                                   "WHERE deps.dependID=? AND NOT deps.dependRel=?;";
    int r = sqlite3_prepare_v2(globalDB::getDBHandle(), getRebuildDepsStr, -1, &sqlStmt, nullptr);
    if(r != SQLITE_OK) {
        std::cout<<"error";
    }
    if(sqlite3PongQuery(&sqlStmt, getRebuildDepsStr) &&
            sqlite3_bind_int(sqlStmt, 1, pkgID) == SQLITE_OK &&
            sqlite3_bind_int(sqlStmt, 2, static_cast<int>(pRel::NONE)) == SQLITE_OK) {

        while(sqlite3_step(sqlStmt) == SQLITE_ROW) {
            const unsigned char* dep = sqlite3_column_text(sqlStmt, 0);
            retStr += (const char *)dep;
            retStr += " ";
        }
        success = true;
    }
    r = sqlite3_finalize(sqlStmt);
    return success;
}

bool revdepsQuery(int pkgID, std::string &retStr) {
    sqlite3_stmt *sqlStmt = nullptr;
    bool succes = false;
    const char *getRebuildDepsStr= "SELECT name FROM deps JOIN packages ON deps.packageID=packages.rowid "
                                   "WHERE deps.dependID=?;";
    if(sqlite3PongQuery(&sqlStmt, getRebuildDepsStr) && sqlite3_bind_int(sqlStmt, 1, pkgID) == SQLITE_OK) {
        std::string depends = "";
        while(sqlite3_step(sqlStmt) == SQLITE_ROW) {
            const unsigned char* dep = sqlite3_column_text(sqlStmt, 0);
            retStr += (const char *)dep;
            retStr += " ";
        }
        succes = true;
    }
    sqlite3_finalize(sqlStmt);
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

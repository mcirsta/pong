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

const char* allDepsQuery(int pkgID) {
    return "";
}

const char* directDepsQuery(int pkgID) {
    sqlite3_stmt *sqlStmt;
    const char *getDirectDepsStr= "SELECT name FROM deps JOIN packages ON deps.dependID=packages.rowid WHERE deps.packageID=?";
    int r = sqlite3_prepare_v2(globalDB::getDBHandle(), getDirectDepsStr, -1, &sqlStmt, nullptr);
    sqlite3_bind_int(sqlStmt, 1, pkgID);
    std::string depends = "";
    while(sqlite3_step(sqlStmt) == SQLITE_ROW) {
        const unsigned char* dep = sqlite3_column_text(sqlStmt, 0);
        depends += (const char *)dep;
        depends += " ";
    }
    r = sqlite3_finalize(sqlStmt);
    return depends.c_str();
}

const char* rebuildDepsQuery(int pkgID) {
    sqlite3_stmt *sqlStmt;
    const char *getRebuildDepsStr= "SELECT name FROM deps JOIN packages ON deps.packageID=packages.rowid "
                                   "WHERE deps.dependID=? AND NOT deps.dependRel=?;";
    int r = sqlite3_prepare_v2(globalDB::getDBHandle(), getRebuildDepsStr, -1, &sqlStmt, nullptr);
    sqlite3_bind_int(sqlStmt, 1, pkgID);
    sqlite3_bind_int(sqlStmt, 2, static_cast<int>(pRel::NONE));
    std::string depends = "";
    while(sqlite3_step(sqlStmt) == SQLITE_ROW) {
        const unsigned char* dep = sqlite3_column_text(sqlStmt, 0);
        depends += (const char *)dep;
        depends += " ";
    }
    r = sqlite3_finalize(sqlStmt);
    return depends.c_str();
}

const char* revdepsQuery(int pkgID) {
    sqlite3_stmt *sqlStmt;
    const char *getRebuildDepsStr= "SELECT name FROM deps JOIN packages ON deps.packageID=packages.rowid "
                                   "WHERE deps.dependID=?;";
    int r = sqlite3_prepare_v2(globalDB::getDBHandle(), getRebuildDepsStr, -1, &sqlStmt, nullptr);
    sqlite3_bind_int(sqlStmt, 1, pkgID);
    std::string depends = "";
    while(sqlite3_step(sqlStmt) == SQLITE_ROW) {
        const unsigned char* dep = sqlite3_column_text(sqlStmt, 0);
        depends += (const char *)dep;
        depends += " ";
    }
    r = sqlite3_finalize(sqlStmt);
    return depends.c_str();
}


const char* dbQuery(DBOpts opType, const char* opArg) {
    int pkgId = getPkgID(opArg);
    if(pkgId == 0) {
        std::cout<<"package "<<opArg<<" not found in the database"<<std::endl;
        return "";
    }
    switch (opType) {
    case DBOpts::OP_ALL_DEPS:       return allDepsQuery(pkgId);
    case DBOpts::OP_DIRECT_DEPS:    return directDepsQuery(pkgId);
    case DBOpts::OP_REBUILD_DEPS:   return rebuildDepsQuery(pkgId);
    case DBOpts::OP_REV_DEPS:       return revdepsQuery(pkgId);
    default: std::cout<<"unknown op"<<std::endl;    return "";
    }
}

#include "interface.hpp"
#include "database.hpp"

#include <iostream>
#include <cstring>

typedef std::map<std::string, int> pkgNameID;

bool getPkgID(pkgNameID &pkgNames) {
    sqlite3_stmt *sqlStmt;
    std::vector<int> pkgIDs;
    std::string getDirectDepsStr;
    if(pkgNames.size() < 1)  {
        std::cout<<"at least one package needed"<<std::endl;
        return false;
    }
    else  {
        if(pkgNames.size() == 1) {
            getDirectDepsStr = "SELECT rowid, name FROM packages WHERE name=?;";
        }
        else  {
            getDirectDepsStr = "SELECT rowid, name FROM packages WHERE name=? ";
            for(unsigned int i=0; i<pkgNames.size()-1; i++) {
                getDirectDepsStr += " OR name=?";
            }
            getDirectDepsStr += " ;";
        }
    }
    getDirectDepsStr ="SELECT rowid, name FROM packages WHERE name=?  OR name=? ;";
    if(sqlite3PongQuery(&sqlStmt, getDirectDepsStr.c_str()))   {
        unsigned int pNr = 1;
        for(const auto &pkg:pkgNames) {
            if(!sqlite3PongBindText(sqlStmt, pkg.first, pNr)) {
                break;
            }
            pNr++;
        }
        int sqlRes = sqlite3_step(sqlStmt);
        while(sqlRes == SQLITE_ROW)  {
            int pkgID = sqlite3_column_int(sqlStmt, 0);
            std::string pName = (const char*)sqlite3_column_text(sqlStmt, 1);
            pkgNames[pName] = pkgID;
            sqlRes = sqlite3_step(sqlStmt);
        }
        if(sqlRes != SQLITE_DONE) {
            std::cout<<sqlite3_errmsg(globalDB::getDBHandle())<<std::endl;
        }
    }
    else {
        std::cout<<sqlite3_errmsg(globalDB::getDBHandle())<<std::endl;
    }
    sqlite3_finalize(sqlStmt);
    return true;
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


bool dbQuery(DBOpts opType,const std::vector<std::string> &opArg, std::string &retStr) {
    pkgNameID reqPkgs;
    for(const auto &pkg:opArg) {
        reqPkgs[pkg] = -1;
    }
    getPkgID(reqPkgs);
    for(const auto &pkg:reqPkgs) {
        if(pkg.second == -1) {
             std::cout<<"package "<<pkg.first<<" not found in the database"<<std::endl;
        }
    }
    //    switch (opType) {
    //    case DBOpts::OP_ALL_DEPS:       return allDepsQuery(pkgId, retStr);
    //    case DBOpts::OP_DIRECT_DEPS:    return directDepsQuery(pkgId, retStr);
    //    case DBOpts::OP_REBUILD_DEPS:   return rebuildDepsQuery(pkgId, retStr);
    //    case DBOpts::OP_REV_DEPS:       return revdepsQuery(pkgId, retStr);
    //    default: std::cout<<"unknown op"<<std::endl;    return "";
    //    }
    return true;
}

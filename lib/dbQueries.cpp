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
        getDirectDepsStr = "SELECT rowid, name FROM packages WHERE name=? ";
        for(unsigned int i=0; i<pkgNames.size()-1; i++) {
            getDirectDepsStr += " OR name=?";
        }
        getDirectDepsStr += " ;";
    }
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


bool depQueryExec(const char* queryStr, const pkgNameID &reqPkgs, pkgNameID &retPkgs)
{
    sqlite3_stmt *sqlStmt = nullptr;
    bool success = false;
    if(sqlite3PongQuery(&sqlStmt, queryStr))   {
        unsigned int pNr = 1;
        for(const auto &pkg:reqPkgs) {
            if(!sqlite3PongBindInt(sqlStmt, pkg.second, pNr)) {
                break;
            }
            pNr++;
        }
        while(sqlite3_step(sqlStmt) == SQLITE_ROW) {
            const unsigned char* dep = sqlite3_column_text(sqlStmt, 0);
            retPkgs[(const char *)dep] = sqlite3_column_int64(sqlStmt, 1);
        }
        success = true;
    }
    else {
        std::cout<<sqlite3_errmsg(globalDB::getDBHandle())<<std::endl;
    }
    sqlite3PongFinalize(sqlStmt);
    return success;
}

bool allDepsQuery(const pkgNameID &reqPkgs, pkgNameID &retPkgs) {
    std::string getAllDepsStr= "WITH RECURSIVE " \
                               "alldeps(n) AS ( " \
                               "SELECT deps.dependID FROM deps WHERE ( deps.packageID=? ";
    for(unsigned int i=0; i<reqPkgs.size()-1; i++) {
        getAllDepsStr += " OR deps.packageID=?";
    }
    getAllDepsStr += " ) UNION " \
                     "SELECT deps.dependID FROM deps,alldeps WHERE deps.packageID=alldeps.n ) " \
                     "SELECT packages.name,packages.rowid from alldeps JOIN packages ON packages.rowid=n ORDER BY name; ";
    return depQueryExec(getAllDepsStr.c_str(), reqPkgs, retPkgs);
}

bool directDepsQuery(const pkgNameID &reqPkgs, pkgNameID &retPkgs) {
    std::string getDirectDepsStr= "SELECT packages.name,packages.rowid FROM deps JOIN packages ON deps.dependID=packages.rowid WHERE ( deps.packageID=?";
    for(unsigned int i=0; i<reqPkgs.size()-1; i++) {
        getDirectDepsStr += " OR deps.packageID=?";
    }
    getDirectDepsStr += " );";
    return depQueryExec(getDirectDepsStr.c_str(), reqPkgs, retPkgs);
}

bool rebuildDepsQuery(const pkgNameID &reqPkgs, pkgNameID &retPkgs) {
    std::string getRebuildDepsStr= "SELECT packages.name,packages.rowid FROM deps JOIN packages ON deps.packageID=packages.rowid "
                                   "WHERE ( deps.dependID=?";
    for(unsigned int i=0; i<reqPkgs.size()-1; i++) {
        getRebuildDepsStr += " OR deps.dependID=?";
    }
    getRebuildDepsStr += " ) AND NOT deps.dependRel="+ std::to_string(static_cast<unsigned int>(pRel::NONE)) +" ;";
    return depQueryExec(getRebuildDepsStr.c_str(), reqPkgs, retPkgs);
}

bool revdepsQuery(const pkgNameID &reqPkgs, pkgNameID &retPkgs) {
     std::string getRebuildDepsStr= "SELECT packages.name,packages.rowid FROM deps JOIN packages ON deps.packageID=packages.rowid "
                                   "WHERE ( deps.dependID=?";
     for(unsigned int i=0; i<reqPkgs.size()-1; i++) {
         getRebuildDepsStr += " OR deps.dependID=?";
     }
     getRebuildDepsStr += " );";
     return depQueryExec(getRebuildDepsStr.c_str(), reqPkgs, retPkgs);
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
            return false;
        }
    }
    pkgNameID foundPkgs;
    switch (opType) {
    case DBOpts::OP_ALL_DEPS:       allDepsQuery(reqPkgs, foundPkgs);
        break;
    case DBOpts::OP_DIRECT_DEPS:    directDepsQuery(reqPkgs, foundPkgs);
        break;
    case DBOpts::OP_REBUILD_DEPS:   rebuildDepsQuery(reqPkgs, foundPkgs);
        break;
    case DBOpts::OP_REV_DEPS:       revdepsQuery(reqPkgs, foundPkgs);
        break;
    default: std::cout<<"unknown op"<<std::endl;
        return false;
    }
    for(const auto &pkg:foundPkgs) {
        retStr += pkg.first + " ";
    }
    return true;
}

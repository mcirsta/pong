#include "database.hpp"
#include "utils.hpp"

#include <cstdio>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include <unistd.h>

sqlite3 *globalDB::DBHandle = nullptr;

//muti-char because search will be in the order of the init here
std::vector<std::pair<pRel,std::string>> dSigns = { {pRel::MORE_EQ, ">="},
                                                    {pRel::LESS_EQ, "<="},
                                                    {pRel::EQ, "="},
                                                    {pRel::MORE, ">"},
                                                    {pRel::LESS, "<"},
                                                  };

std::vector<packageRelData> pkgRelData;


void globalDB::setDBHandle(sqlite3 * dbH) {
    DBHandle = dbH;
}

sqlite3 *globalDB::getDBHandle() {
    return DBHandle;
}

bool extractOldDB(FILE *dbFile) {
   return extractXZ(dbFile, "/tmp/pongTemp/");
}

bool dbUpdateNeeded(const std::string &dbpath) {
//    //we always update the DB for now
//    std::string dbName = dbpath + "/" + "frugalware-current.pdb";
//    std::ifstream newDB(dbName);
//    if (newDB.good())
//    {
//        std::remove(dbName.c_str());
//    }
//    return true;
   return false;
}

bool openLegacyDB(const std::string &dbpath) {
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

bool openNewDB(const std::string &dbpath) {
    std::string dbName = dbpath + "/" + "frugalware-current.pdb";
    sqlite3 *dbHandle;
    int r = sqlite3_open_v2(dbName.c_str(), &dbHandle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX
                            | SQLITE_OPEN_PRIVATECACHE, nullptr);
    if(r != SQLITE_OK) {
        std::cout<<"error while opening Sqlite database: "<<r;
        return false;
    }
    globalDB::setDBHandle(dbHandle);
    return true;
}

bool createGroups(const std::string &grFile) {
    std::ifstream gfile(grFile);
    std::string grs;
    sqlite3_stmt *sqlStmt;
    const char *createGroupsTableStr = "CREATE TABLE pgroups ( " \
                                       " gname TEXT UNIQUE NOT NULL ); ";
    int r = sqlite3_prepare_v2(globalDB::getDBHandle(), createGroupsTableStr, -1, &sqlStmt, nullptr);
    if(r != SQLITE_OK) {
        std::cout<<"error";
    }
    r = sqlite3_step(sqlStmt);
    r = sqlite3_finalize(sqlStmt);
    const char *addGroupSql = "INSERT INTO pgroups (gname) VALUES (?);";
    if(gfile.good()) {
        while(std::getline(gfile, grs)) {
            r = sqlite3_prepare_v2(globalDB::getDBHandle(), addGroupSql, -1, &sqlStmt, nullptr);
            r = sqlite3_bind_text(sqlStmt, 1, grs.c_str(), grs.size(), SQLITE_STATIC);
            r = sqlite3_step(sqlStmt);
            r = sqlite3_finalize(sqlStmt);
        }
    }
    else {
        std::cout<<"could not find groups file, "<<grFile<<std::endl;
    }
    return true;
}

bool createArchs(const std::string &archFile) {
    std::ifstream afile(archFile);
    std::string ars;
    sqlite3_stmt *sqlStmt;
    const char *createArchTableStr = "CREATE TABLE archs ( " \
                                     " aname TEXT UNIQUE NOT NULL ); ";
    int r = sqlite3_prepare_v2(globalDB::getDBHandle(), createArchTableStr, -1, &sqlStmt, nullptr);
    if(r != SQLITE_OK) {
        std::cout<<"error";
    }
    r = sqlite3_step(sqlStmt);
    r = sqlite3_finalize(sqlStmt);
    const char *addGroupSql = "INSERT INTO archs (aname) VALUES (?);";
    if(afile.good()) {
        while(std::getline(afile, ars)) {
            r = sqlite3_prepare_v2(globalDB::getDBHandle(), addGroupSql, -1, &sqlStmt, nullptr);
            r = sqlite3_bind_text(sqlStmt, 1, ars.c_str(), ars.size(), SQLITE_STATIC);
            r = sqlite3_step(sqlStmt);
            r = sqlite3_finalize(sqlStmt);
        }
    }
    else {
        std::cout<<"could not find archs file, "<<archFile<<std::endl;
    }
    return true;
}

bool createRelationsTable() {
    const char *createRelTableStr = "CREATE TABLE depRels ( " \
                                    "relID INT PRIMARY KEY, " \
                                    "relSign TEXT UNIQUE NOT NULL); ";
    sqlite3_stmt *sqlStmt;
    int r = sqlite3_prepare_v2(globalDB::getDBHandle(), createRelTableStr, -1, &sqlStmt, nullptr);
    if(r != SQLITE_OK) {
        std::cout<<"error";
    }
    r = sqlite3_step(sqlStmt);
    r = sqlite3_finalize(sqlStmt);
    const char *addGroupSql = "INSERT INTO depRels (relID, relSign) VALUES (?, ?);";
    for(const auto &x : dSigns) {
        {
            r = sqlite3_prepare_v2(globalDB::getDBHandle(), addGroupSql, -1, &sqlStmt, nullptr);
            r = sqlite3_bind_int(sqlStmt, 1, static_cast<int>(x.first));
            r = sqlite3_bind_text(sqlStmt, 2, x.second.c_str(), x.second.size(), SQLITE_STATIC);
            r = sqlite3_step(sqlStmt);
            r = sqlite3_finalize(sqlStmt);
        }
    }
    return true;
}

bool createProvidesTable() {
    const char *createProvidesTableStr = "CREATE TABLE provides ( " \
                                    "newPkgID INT, " \
                                    "providedPkg TEXT NOT NULL); ";
    sqlite3_stmt *sqlStmt;
    int r = sqlite3_prepare_v2(globalDB::getDBHandle(), createProvidesTableStr, -1, &sqlStmt, nullptr);
    if(r != SQLITE_OK) {
        std::cout<<"error";
    }
    r = sqlite3_step(sqlStmt);
    r = sqlite3_finalize(sqlStmt);
    return true;
}

bool setProvidesData(const packageRelData &pRelData) {
    const char *addGroupSql = "INSERT INTO provides (newPkgID, providedPkg) " \
                              "VALUES ( (SELECT rowid FROM packages WHERE name=? )," \
                                         "?);";
    int r = 0;
    sqlite3_stmt *sqlStmt;
    for(const auto &x : pRelData.pProvides) {
        {
            r = sqlite3_prepare_v2(globalDB::getDBHandle(), addGroupSql, -1, &sqlStmt, nullptr);
            r = sqlite3_bind_text(sqlStmt, 1, pRelData.pkgName.c_str(), pRelData.pkgName.size(), SQLITE_STATIC);;
            r = sqlite3_bind_text(sqlStmt, 2, x.c_str(), x.size(), SQLITE_STATIC);
            r = sqlite3_step(sqlStmt);
            if(r != SQLITE_DONE) {
                std::cout<<sqlite3_errmsg(globalDB::getDBHandle())<< " : "<< pRelData.pkgName<<" - "<<x<<std::endl;
            }
            r = sqlite3_finalize(sqlStmt);
        }
    }
    return true;
}

bool createDepsTable() {
    const char *createDepTableStr = "CREATE TABLE deps ( " \
                                    "packageID INT NOT NULL, " \
                                    "dependID INT NOT NULL,"
                                    "dependRel INT NOT NULL,"
                                    "dependVersion TEXT ); ";
    sqlite3_stmt *sqlStmt;
    int r = sqlite3_prepare_v2(globalDB::getDBHandle(), createDepTableStr, -1, &sqlStmt, nullptr);
    if(r != SQLITE_OK) {
        std::cout<<"error";
    }
    r = sqlite3_step(sqlStmt);
    r = sqlite3_finalize(sqlStmt);
    return true;
}

bool setDepData(const packageRelData &pRelData) {
    const char *addDepSql = "INSERT INTO deps (packageID, dependID, dependRel, dependVersion) " \
                              "VALUES ( (SELECT rowid FROM packages WHERE name=?1 ), " \
                              "(CASE ifnull((SELECT rowid FROM packages WHERE name=?2), 0) " \
                              "WHEN  0 " \
                              "THEN (SELECT newPkgID FROM provides WHERE providedPkg=?2) " \
                              "ELSE (SELECT rowid FROM packages WHERE name=?2) " \
                              "END ), "\
                              " ?3, ?4);";
    int r = 0;
    sqlite3_stmt *sqlStmt;
    for(const auto &x : pRelData.pDeps) {
        {
            r = sqlite3_prepare_v2(globalDB::getDBHandle(), addDepSql, -1, &sqlStmt, nullptr);
            r = sqlite3_bind_text(sqlStmt, 1, pRelData.pkgName.c_str(), pRelData.pkgName.size(), SQLITE_STATIC);
            r = sqlite3_bind_text(sqlStmt, 2, x.depName.c_str(), x.depName.size(), SQLITE_STATIC);;
            r = sqlite3_bind_int(sqlStmt, 3, static_cast<int>(x.depRel));
            r = sqlite3_bind_text(sqlStmt, 4, x.depVer.c_str(), x.depVer.size(), SQLITE_STATIC);
            r = sqlite3_step(sqlStmt);
            if(r != SQLITE_DONE) {
                //OK so couldn't find the needed dep
                std::cout<<sqlite3_errmsg(globalDB::getDBHandle())<< " : "<< pRelData.pkgName<<" - "<<x.depName<<std::endl;
            }
            r = sqlite3_finalize(sqlStmt);
        }
    }
    return true;
}

bool createNewDB() {

    sqlite3_stmt *sqlStmt;
    const char *createMainDBStr = "CREATE TABLE packages ( " \
                                  "name           TEXT  UNIQUE  NOT NULL," \
                                  "description    TEXT    NOT NULL," \
                                  "version        TEXT    NOT NULL," \
                                  "csize          INT     NOT NULL," \
                                  "usize          INT     NOT NULL," \
                                  "arch           INT     NOT NULL REFERENCES archs," \
                                  "pgroup         INT    NOT NULL REFERENCES pgroups," \
                                  "sha1sum        TEXT," \
                                  "URL            TEXT );";
    int r = sqlite3_prepare_v2(globalDB::getDBHandle(), createMainDBStr, -1, &sqlStmt, nullptr);
    if(r != SQLITE_OK && sqlStmt != nullptr) {
        std::cout<<"error while preparing Sqlite statement: "<<r;
        sqlite3_finalize(sqlStmt);
        return false;
    }
    r = sqlite3_step(sqlStmt);
    if(r != SQLITE_DONE) {
        std::cout<<"error creating database: "<<sqlite3_errmsg(globalDB::getDBHandle())<<std::endl;
        sqlite3_finalize(sqlStmt);
        return false;
    }
    r = sqlite3_finalize(sqlStmt);
    createGroups("groups.txt");
    createArchs("archs.txt");
    createRelationsTable();
    createProvidesTable();
    createDepsTable();
    return true;
}

void getPkgData(const std::string &dbFile, pkgData &p) {
    std::ifstream file(dbFile);
    std::string str;
    while (std::getline(file, str))
    {
        if(str.size()<1) {
            continue;
        }

        if(str=="%NAME%")
            std::getline(file, p.name);
        else
            if(str=="%VERSION%")
                std::getline(file, p.version);
            else
                if(str=="%DESC%")
                    std::getline(file, p.desc);
                else
                    if(str=="%SHA1SUM%")
                        std::getline(file, p.sha1sum);
                    else
                        if(str=="%CSIZE%") {
                            std::getline(file, str);
                            p.csize = std::stoull(str);
                        }
                        else
                            if(str=="%USIZE%") {
                                std::getline(file, str);
                                p.usize = std::stoull(str);
                            }
                            else
                                if(str=="%ARCH%")
                                    std::getline(file, p.arch);
                                else
                                    if(str=="%GROUPS%")
                                        std::getline(file, p.group);
    }
    //hack to deal with gnome-docs stuff
    if(p.group=="gnome-docs")
        p.group="gnome-extra";
}

bool setPkgData(const pkgData &pData) {
    sqlite3_stmt *sqlStmt;
    const char *updatePkgDB = "INSERT INTO packages(name,description,version," \
                              "csize,usize,sha1sum,arch,pgroup) VALUES(?,?,?,?,?,?, " \
                              "(SELECT rowid FROM archs WHERE aname=?), " \
                              "(SELECT rowid FROM pgroups WHERE gname=?) );";
    int r = sqlite3_prepare_v2(globalDB::getDBHandle(), updatePkgDB, -1, &sqlStmt, nullptr);
    r = sqlite3_bind_text(sqlStmt,1,pData.name.c_str(),pData.name.size(),SQLITE_STATIC);
    r = sqlite3_bind_text(sqlStmt,2,pData.desc.c_str(),pData.desc.size(),SQLITE_STATIC);
    r = sqlite3_bind_text(sqlStmt,3,pData.version.c_str(),pData.version.size(),SQLITE_STATIC);
    r = sqlite3_bind_int(sqlStmt,4,pData.csize);
    r = sqlite3_bind_int(sqlStmt,5,pData.usize);
    r = sqlite3_bind_text(sqlStmt,6,pData.sha1sum.c_str(),pData.sha1sum.size(),SQLITE_STATIC);
    r = sqlite3_bind_text(sqlStmt,7,pData.arch.c_str(),pData.arch.size(),SQLITE_STATIC);
    r = sqlite3_bind_text(sqlStmt,8,pData.group.c_str(),pData.group.size(),SQLITE_STATIC);
    r = sqlite3_step(sqlStmt);
    if(r != SQLITE_DONE) {
        std::cout<<sqlite3_errmsg(globalDB::getDBHandle())<< " package: "<<pData.name<<std::endl;
    }
    r = sqlite3_finalize(sqlStmt);
    return true;
}

bool importGeneral(const std::string &dbFile, std::string &addPname) {
    pkgData pData;
    getPkgData(dbFile,pData);
    setPkgData(pData);
    addPname = pData.name;
    return true;
}

pkgDep buildDep(const std::string &str) {
    pkgDep pDep;
    bool depRelFound = false;
    for(const auto &x : dSigns) {
        size_t pos = str.find(x.second);
        if(pos != std::string::npos) {
            pDep.depVer = str.substr(pos + x.second.size());
            pDep.depName = str.substr(0, pos);
            pDep.depRel = x.first;
            depRelFound = true;
            break;
        }
    }
    if( !depRelFound ) {
        pDep.depVer = "";
        pDep.depName = str;
        pDep.depRel = pRel::NONE;
    }
    return pDep;
}

bool getDepData(const std::string &dbFile, packageRelData &pRelData) {
    std::ifstream file(dbFile);
    std::string str;
    while (std::getline(file, str))
    {
        if(str.size()<1) {
            continue;
        }
        if(str=="%DEPENDS%") {
            while(std::getline(file, str))
            {
                if(str.size()<1) {
                    break;
                }
                pRelData.pDeps.push_back(buildDep(str));
            }
        }
        else
            if(str=="%REPLACES%") {
                while(std::getline(file, str))
                {
                    if(str.size()<1) {
                        break;
                    }
                    pRelData.pReplaces.push_back(str);
                }
            }
            else
                if(str=="%CONFLICTS%") {
                    while(std::getline(file, str))
                    {
                        if(str.size()<1) {
                            break;
                        }
                        pRelData.pConflicts.push_back(str);
                    }
                }
                else
                    if(str=="%PROVIDES%") {
                        while(std::getline(file, str))
                        {
                            if(str.size()<1) {
                                break;
                            }
                            pRelData.pProvides.push_back(str);
                        }
                    }
    }
    return true;
}

bool importDeps(const std::string &dbFile, const std::string &pName) {
    pkgDeps pDeps;
    packageRelData pRelData;
    getDepData(dbFile, pRelData);
    pRelData.pkgName = pName;
    pkgRelData.push_back(pRelData);
    return true;
}


bool importData(const std::string &dbpath) {
    DIR           *d;
    struct dirent *dir;
    d = opendir(dbpath.c_str());
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if(dir->d_type == DT_DIR && strcmp(dir->d_name,".") && strcmp(dir->d_name,".."))
            {
                std::string fileDb = dbpath + "/" +dir->d_name + "/" + "desc";
                std::string depsDb = dbpath + "/" +dir->d_name + "/" + "depends";
                std::string addedPkgName;
                importGeneral(fileDb, addedPkgName);
                std::remove(fileDb.c_str());
                importDeps(depsDb, addedPkgName);
                std::remove(depsDb.c_str());
                int r = rmdir((dbpath + "/" +dir->d_name).c_str());
                if(r == -1)
                {
                    std::cout<<"error deleting folder"<<std::endl;
                }
            }
        }
        closedir(d);
    }
    for(const auto &x : pkgRelData) {
        setProvidesData(x);
    }
    for(const auto &x : pkgRelData) {
        setDepData(x);
    }
    return true;
}

bool initDB(const std::string &dbpath) {
    std::cout<<"database path: "<<dbpath<<std::endl;

    if(dbUpdateNeeded("/tmp/pongTemp")) {
        openLegacyDB(dbpath);
        openNewDB("/tmp/pongTemp");
        createNewDB();
        importData("/tmp/pongTemp");
    }
    else {
        openNewDB("/tmp/pongTemp");
    }
    return true;
}

bool closeDB() {
    if(sqlite3_close_v2(globalDB::getDBHandle()) == SQLITE_OK) {
        return true;
    }
    else {
        return false;
    }
}

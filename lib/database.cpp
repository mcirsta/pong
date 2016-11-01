#include "database.hpp"
#include "utils.hpp"

#include <cstdio>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

sqlite3 *globalDB::DBHandle = nullptr;
std::map <std::string,sqlite3_int64> globalDB::grIds;
std::map <std::string,sqlite3_int64> globalDB::archIds;


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

inline bool sqlite3PongQuery(sqlite3_stmt **sqlStmt, const char* queryText) {
    int r = sqlite3_prepare_v2(globalDB::getDBHandle(), queryText, -1, sqlStmt, nullptr);
    if(r != SQLITE_OK) {
        std::cout<<"error preparing query "<<queryText<<std::endl;
        return false;
    }
    return true;
}

inline bool sqlite3PongBindText(p_sqlite3_stmt &sqlStmt,const std::string &bindText, const int &bindPos) {
    int r = sqlite3_bind_text(sqlStmt, bindPos, bindText.c_str(), bindText.size(), SQLITE_STATIC);
    if(r != SQLITE_OK) {
        std::cout<<sqlite3_errmsg(globalDB::getDBHandle())<<std::endl;
        return false;
    }
    return true;
}

inline bool sqlite3PongBindInt(p_sqlite3_stmt &sqlStmt,const int &bindInt, const int &bindPos) {
    int r = sqlite3_bind_int(sqlStmt, bindPos,bindInt);
    if(r != SQLITE_OK) {
        std::cout<<sqlite3_errmsg(globalDB::getDBHandle())<<std::endl;
        return false;
    }
    return true;
}

inline bool sqlite3PongBindInt64(p_sqlite3_stmt &sqlStmt,const int64_t &bindInt, const int &bindPos) {
    int r = sqlite3_bind_int64(sqlStmt, bindPos,bindInt);
    if(r != SQLITE_OK) {
        std::cout<<sqlite3_errmsg(globalDB::getDBHandle())<<std::endl;
        return false;
    }
    return true;
}

inline bool sqlite3PongStep(p_sqlite3_stmt &sqlStmt) {
    int r = sqlite3_step(sqlStmt);
    if(r != SQLITE_DONE) {
        std::cout<<sqlite3_errmsg(globalDB::getDBHandle())<<std::endl;
        return false;
    }
    return true;
}

inline bool sqlite3PongFinalize(p_sqlite3_stmt &sqlStmt) {
    int r = sqlite3_finalize(sqlStmt);
    if(r != SQLITE_OK) {
        std::cout<<sqlite3_errmsg(globalDB::getDBHandle())<<std::endl;
        return false;
    }
    return true;
}

bool createDBQuery(const char* createSQLQuery) {
    sqlite3_stmt *sqlStmt;
    bool success = false;
    if(sqlite3PongQuery(&sqlStmt, createSQLQuery) &&
            sqlite3PongStep(sqlStmt))  {
        success = true;
    }
    else {
        std::cout<<"error running query : "<<createSQLQuery<<std::endl;
    }
    sqlite3PongFinalize(sqlStmt);
    return success;
}


sqlite3_int64 globalDB::getGrId(std::string grName) {
    sqlite3_int64 retGrId;
    if(grIds.find(grName) == grIds.end()) {
        sqlite3_stmt *sqlStmt = nullptr;
        bool success = false;
        const char *addGroupSql = "INSERT INTO pgroups (gname) VALUES (?);";
        if(sqlite3PongQuery(&sqlStmt, addGroupSql) &&
                sqlite3PongBindText(sqlStmt,grName) &&
                sqlite3PongStep(sqlStmt))
        {
            success = true;
        }
        else {
            std::cout<<"Error inserting group "<<grName<< " with " <<sqlite3_errmsg(globalDB::getDBHandle()) <<std::endl;

        }
        if(success) {
            retGrId = sqlite3_last_insert_rowid(globalDB::getDBHandle());
            grIds[grName] = retGrId;
        }
        else {
            retGrId = -1;
        }
        sqlite3_finalize(sqlStmt);
    }
    else {
        retGrId = grIds[grName];
    }
    return retGrId;
}

sqlite3_int64 globalDB::getArchId(std::string archName) {
    sqlite3_int64 retArchId;
    if(archIds.find(archName) == archIds.end()) {
        sqlite3_stmt *sqlStmt = nullptr;
        bool success = false;
        const char *addArchSql = "INSERT INTO archs (aname) VALUES (?);";
        if(sqlite3PongQuery(&sqlStmt, addArchSql) &&
                sqlite3PongBindText(sqlStmt, archName) &&
                sqlite3PongStep(sqlStmt))
        {
            success = true;
        }
        else {
            std::cout<<"Error inserting arch "<<archName<<" with " <<sqlite3_errmsg(globalDB::getDBHandle()) <<std::endl;
        }
        if(success) {
            retArchId = sqlite3_last_insert_rowid(globalDB::getDBHandle());
            archIds[archName] = retArchId;
        }
        else {
            retArchId = -1;
        }
        sqlite3_finalize(sqlStmt);
    }
    else {
        retArchId = archIds[archName];
    }
    return retArchId;
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
    return true;
}

bool openLegacyDB(const std::string& dbpath) {
    const std::string FILENAME = dbpath + "/" + "frugalware-current.fdb";
    bool isDbOpened = false;

    FILE* dbFile = fopen(FILENAME.c_str(), "r");
    if(!checkXZ(dbFile)) {
        std::cout << "invalid original database file: " << FILENAME << std::endl;
        isDbOpened = false;
    }
    else {
        std::cout << "looks good, opening: " << FILENAME << std::endl;
        extractOldDB(dbFile);
        fclose(dbFile);
        isDbOpened = true;
    }

    return isDbOpened;
}

bool deleteNewDB(const std::string &dbPath) {
    std::string remDBFile = dbPath + "/frugalware-current.pdb";
    std::remove(remDBFile.c_str());
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

bool createGroups() {
    const char *createGroupsTableStr = "CREATE TABLE pgroups ( " \
                                       " gname TEXT UNIQUE NOT NULL ); ";
    return createDBQuery(createGroupsTableStr);
}

bool createArchs() {
    const char *createArchTableStr = "CREATE TABLE archs ( " \
                                     " aname TEXT UNIQUE NOT NULL ); ";
    return createDBQuery(createArchTableStr);
}

bool createProvidesTable() {
    const char *createProvidesTableStr = "CREATE TABLE provides ( " \
                                         "newPkgID INT, " \
                                         "providedPkg TEXT NOT NULL); ";
    return createDBQuery(createProvidesTableStr);
}

bool createDepsTable() {
    const char *createDepTableStr = "CREATE TABLE deps ( " \
                                    "packageID INT NOT NULL, " \
                                    "dependID INT NOT NULL,"
                                    "dependRel INT NOT NULL,"
                                    "dependVersion TEXT ); ";
    return createDBQuery(createDepTableStr);
}

bool createPackages() {
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
    return createDBQuery(createMainDBStr);
}

bool createRelationsTable() {
    const char *createRelTableStr = "CREATE TABLE depRels ( " \
                                    "relID INT PRIMARY KEY, " \
                                    "relSign TEXT UNIQUE NOT NULL); ";
    bool success = false;
    success = createDBQuery(createRelTableStr);
    if(success) {
        const char *addGroupSql = "INSERT INTO depRels (relID, relSign) VALUES (?, ?);";
        sqlite3_stmt *sqlStmt = nullptr;
        sqlite3PongQuery(&sqlStmt, addGroupSql);
        for(const auto &x : dSigns) {
            if(sqlite3PongBindInt(sqlStmt, static_cast<int>(x.first), 1) &&
                    sqlite3PongBindText(sqlStmt, x.second, 2) &&
                    sqlite3PongStep(sqlStmt)) {
                 sqlite3_reset(sqlStmt);
            }
            else {
                std::cout<<"error running query : "<<addGroupSql<<std::endl;
            }
        }
        sqlite3PongFinalize(sqlStmt);
    }
    return true;
}

bool setProvidesData(const packageRelData &pRelData) {
    const char *addProvidesSql = "INSERT INTO provides (newPkgID, providedPkg) " \
                                 "VALUES ( (SELECT rowid FROM packages WHERE name=? )," \
                                 "?);";
    bool success = false;
    sqlite3_stmt *sqlStmt;
    for(const auto &x : pRelData.pProvides) {
        {
            if(sqlite3PongQuery(&sqlStmt, addProvidesSql) &&
                    sqlite3PongBindText(sqlStmt, pRelData.pkgName, 1) &&
                    sqlite3PongBindText(sqlStmt, x, 2) &&
                    sqlite3PongStep(sqlStmt) &&
                    sqlite3PongFinalize(sqlStmt)) {
                success = true;
            }
            else {
                success = false;
                std::cout<<sqlite3_errmsg(globalDB::getDBHandle())<< " : "<< pRelData.pkgName<<" - "<<x<<std::endl;
                break;
            }
        }
    }
    return success;
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
    bool success = false;
    sqlite3_stmt *sqlStmt;
    for(const auto &x : pRelData.pDeps) {
        {
            if(sqlite3PongQuery(&sqlStmt, addDepSql) &&
                    sqlite3PongBindText(sqlStmt, pRelData.pkgName, 1) &&
                    sqlite3PongBindText(sqlStmt, x.depName, 2) &&
                    sqlite3PongBindInt(sqlStmt,static_cast<int>(x.depRel), 3) &&
                    sqlite3PongBindText(sqlStmt, x.depName, 4) &&
                    sqlite3PongStep(sqlStmt) &&
                    sqlite3PongFinalize(sqlStmt))
            {
                success = true;
            }
            else {
                //We couldn't find the needed dep
                success = false;
                std::cout<<sqlite3_errmsg(globalDB::getDBHandle())<< " : "<< pRelData.pkgName<<" - "<<x.depName<<std::endl;
                break;
            }

        }
    }
    return success;
}

bool createNewDB() {
    createPackages();
    createGroups();
    createArchs();
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
}

bool setPkgData(const pkgData &pData) {
    sqlite3_stmt *sqlStmt;
    bool success = false;
    sqlite3_int64 archId = globalDB::getArchId(pData.arch);
    sqlite3_int64 grId = globalDB::getGrId(pData.group);
    const char *updatePkgDB = "INSERT INTO packages(name,description,version," \
                              "csize,usize,sha1sum,arch,pgroup) VALUES(?,?,?,?,?,?,?,?) ;";
    if ( sqlite3PongQuery(&sqlStmt, updatePkgDB) &&
         sqlite3PongBindText(sqlStmt, pData.name, 1) &&
         sqlite3PongBindText(sqlStmt, pData.desc, 2) &&
         sqlite3PongBindText(sqlStmt, pData.version, 3) &&
         sqlite3PongBindInt64(sqlStmt, pData.csize, 4) &&
         sqlite3PongBindInt64(sqlStmt, pData.usize, 5) &&
         sqlite3PongBindText(sqlStmt, pData.sha1sum, 6) &&
         sqlite3PongBindInt(sqlStmt, archId, 7) &&
         sqlite3PongBindInt(sqlStmt, grId, 8) &&
         sqlite3PongStep(sqlStmt) &&
         sqlite3PongFinalize(sqlStmt) )
    {
        success = true;
    }
    else {
        success = false;
        std::cout<<sqlite3_errmsg(globalDB::getDBHandle())<< " package: "<<pData.name<<std::endl;
    }

    return success;
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
    std::string dbVerFile =  dbpath + "/.version";
    std::remove(dbVerFile.c_str());
    for(const auto &x : pkgRelData) {
        setProvidesData(x);
    }
    for(const auto &x : pkgRelData) {
        setDepData(x);
    }
    return true;
}

bool initDB(const std::string& oldPath) {
    std::cout << "database path: " << oldPath << std::endl;

    const std::string NEW_PATH = "/tmp/pongTemp";
    if(dbUpdateNeeded(NEW_PATH)) {
        openLegacyDB(oldPath);
        deleteNewDB(NEW_PATH);
        openNewDB(NEW_PATH);
        createNewDB();
        importData(NEW_PATH);
    }
    else {
        openNewDB(NEW_PATH);
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

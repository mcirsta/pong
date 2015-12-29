#include "database.hpp"
#include "utils.hpp"

#include <cstdio>
#include <dirent.h>
#include <cstring>
#include <fstream>

sqlite3 *globalDB::DBHandle = nullptr;

void globalDB::setDBHandle(sqlite3 * dbH) {
    DBHandle = dbH;
}

sqlite3 *globalDB::getDBHandle() {
    return DBHandle;
}

bool extractOldDB(FILE *dbFile) {
    extractXZ(dbFile, "/tmp/pongTemp/");
}

bool dbUpdateNeeded(const std::string &dbpath) {
    //we always update the DB for now
    std::string dbName = dbpath + "/" + "frugalware-current.pdb";
    std::ifstream newDB(dbName);
    if (newDB.good())
    {
       std::remove(dbName.c_str());
    }
    return true;
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

bool createNewDB(const std::string &dbpath) {

    sqlite3_stmt *sqlStmt;
    const char *createMainDBStr = "CREATE TABLE packages ( " \
                                  "name           TEXT  UNIQUE  NOT NULL," \
                                  "description    TEXT    NOT NULL," \
                                  "version        TEXT    NOT NULL," \
                                  "csize          INT     NOT NULL," \
                                  "usize          INT     NOT NULL," \
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
        std::cout<<"error creating database: "<<r;
        sqlite3_finalize(sqlStmt);
        return false;
    }
    r = sqlite3_finalize(sqlStmt);
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
    }
}

bool setPkgData(const pkgData &pData) {
    sqlite3_stmt *sqlStmt;
    const char *updatePkgDB = "INSERT INTO packages(name,description,version," \
                              "csize,usize,sha1sum) VALUES(?,?,?,?,?,?);";
    int r = sqlite3_prepare_v2(globalDB::getDBHandle(), updatePkgDB, -1, &sqlStmt, nullptr);
    r = sqlite3_bind_text(sqlStmt,1,pData.name.c_str(),pData.name.size(),SQLITE_STATIC);
    r = sqlite3_bind_text(sqlStmt,2,pData.desc.c_str(),pData.desc.size(),SQLITE_STATIC);
    r = sqlite3_bind_text(sqlStmt,3,pData.version.c_str(),pData.version.size(),SQLITE_STATIC);
    r = sqlite3_bind_int(sqlStmt,4,pData.csize);
    r = sqlite3_bind_int(sqlStmt,5,pData.usize);
    r = sqlite3_bind_text(sqlStmt,6,pData.sha1sum.c_str(),pData.desc.size(),SQLITE_STATIC);
    r = sqlite3_step(sqlStmt);
    if(r != SQLITE_DONE) {
        std::cout<<sqlite3_errmsg(globalDB::getDBHandle())<<std::endl;
    }
    r = sqlite3_finalize(sqlStmt);
}

int importGeneral(const std::string &dbFile) {
     pkgData pData;
     getPkgData(dbFile,pData);
     setPkgData(pData);
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
                importGeneral(fileDb);
            }
        }
        closedir(d);
    }
}

bool initDB(const std::string &dbpath) {
    std::cout<<"database path: "<<dbpath<<std::endl;

    if(dbUpdateNeeded("/tmp/pongTemp")) {
        openLegacyDB(dbpath);
        openNewDB("/tmp/pongTemp");
        createNewDB("/tmp/pongTemp");
        importData("/tmp/pongTemp");
    }
    else {
        openNewDB("/tmp/pongTemp");
    }
}

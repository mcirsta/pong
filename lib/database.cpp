#include "database.hpp"
#include "utils.hpp"

#include <cstdio>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include <unistd.h>

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

bool createGroups(const std::string &grFile) {
    std::ifstream gfile(grFile);
    std::string grs;
    sqlite3_stmt *sqlStmt;
    const char *createGroupsTableStr = "CREATE TABLE pgroups ( " \
                                       " gname TEXT UNIQUE NOT NULL ); ";
    int r = sqlite3_prepare_v2(globalDB::getDBHandle(), createGroupsTableStr, -1, &sqlStmt, nullptr);
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
    return true;
}

bool createArchs(const std::string &archFile) {
    std::ifstream afile(archFile);
    std::string ars;
    sqlite3_stmt *sqlStmt;
    const char *createArchTableStr = "CREATE TABLE archs ( " \
                                     " aname TEXT UNIQUE NOT NULL ); ";
    int r = sqlite3_prepare_v2(globalDB::getDBHandle(), createArchTableStr, -1, &sqlStmt, nullptr);
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
    createGroups("/home/marius/groups.txt");
    createArchs("/home/marius/archs.txt");
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
    r = sqlite3_bind_text(sqlStmt,6,pData.sha1sum.c_str(),pData.desc.size(),SQLITE_STATIC);
    r = sqlite3_bind_text(sqlStmt,7,pData.arch.c_str(),pData.arch.size(),SQLITE_STATIC);
    r = sqlite3_bind_text(sqlStmt,8,pData.group.c_str(),pData.group.size(),SQLITE_STATIC);
    r = sqlite3_step(sqlStmt);
    if(r != SQLITE_DONE) {
        std::cout<<sqlite3_errmsg(globalDB::getDBHandle())<< " : "<<pData.name<<" a= "<<pData.arch<<std::endl;
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
                std::string depsDb = dbpath + "/" +dir->d_name + "/" + "depends";
                importGeneral(fileDb);
                std::remove(fileDb.c_str());
                std::remove(depsDb.c_str());
                int r = rmdir((dbpath + "/" +dir->d_name).c_str());
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
        createNewDB();
        importData("/tmp/pongTemp");
    }
    else {
        openNewDB("/tmp/pongTemp");
    }
}

bool closeDB() {
    if(sqlite3_close_v2(globalDB::getDBHandle()) == SQLITE_OK) {
        return true;
    }
    else {
        return false;
    }
}

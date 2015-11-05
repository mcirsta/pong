#include "database.hpp"
#include "utils.hpp"

#include "stdio.h"

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

}

bool importData(std::string dbpath) {

}

bool initDB(std::string dbpath) {
    std::cout<<"database path: "<<dbpath<<std::endl;
    openLegacyDB(dbpath);
    createNewDB("");
    importData("");
}

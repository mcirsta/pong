#ifndef DATABASE_H_
#define DATABASE_H_

#include <string>
#include <iostream>

bool initDB(std::string dbpath);
bool openLegacyDB(std::string dbpath);
bool createNewDB(std::string dbpath);
bool importData(std::string dbpath);

#endif

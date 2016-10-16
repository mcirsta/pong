#include "dispatcher.hpp"


bool databaseDispatcher(const  boost::program_options::variables_map &secondaryOpts, std::string &retStr) {
    bool isQuerySuccessful = false;
    if(secondaryOpts.find("alldeps") != secondaryOpts.end()) {
       return dbQuery(DBOpts::OP_ALL_DEPS, secondaryOpts["alldeps"].as<std::string>().c_str(), retStr);
    }
    else
        if(secondaryOpts.find("rebuilddeps") != secondaryOpts.end()) {
            return dbQuery(DBOpts::OP_REBUILD_DEPS, secondaryOpts["rebuilddeps"].as<std::string>().c_str(), retStr);
        }
        else
            if(secondaryOpts.find("revdeps") != secondaryOpts.end()) {
                return dbQuery(DBOpts::OP_REV_DEPS, secondaryOpts["revdeps"].as<std::string>().c_str(), retStr);
            }
            else
                if(secondaryOpts.find("directdeps") != secondaryOpts.end()) {
                    return dbQuery(DBOpts::OP_DIRECT_DEPS, secondaryOpts["directdeps"].as<std::string>().c_str(), retStr);
                }
    return isQuerySuccessful;
}

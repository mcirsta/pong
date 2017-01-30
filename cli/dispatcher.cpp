#include "dispatcher.hpp"


bool databaseDispatcher(const boost::program_options::variables_map &secondaryOpts, std::string &retStr) {
    bool isQuerySuccessful = false;

    if(secondaryOpts.find("alldeps") != secondaryOpts.end()) {
       isQuerySuccessful = dbQuery(DBOpts::OP_ALL_DEPS, secondaryOpts["input"].as<std::vector<std::string>>(), retStr);
    }
    else if(secondaryOpts.find("rebuilddeps") != secondaryOpts.end()) {
       isQuerySuccessful = dbQuery(DBOpts::OP_REBUILD_DEPS, secondaryOpts["input"].as<std::vector<std::string>>(), retStr);
    }
    else if(secondaryOpts.find("revdeps") != secondaryOpts.end()) {
       isQuerySuccessful = dbQuery(DBOpts::OP_REV_DEPS, secondaryOpts["input"].as<std::vector<std::string>>(), retStr);
    }
    else if(secondaryOpts.find("directdeps") != secondaryOpts.end()) {
       isQuerySuccessful = dbQuery(DBOpts::OP_DIRECT_DEPS, secondaryOpts["input"].as<std::vector<std::string>>(), retStr);
    }

    return isQuerySuccessful;
}

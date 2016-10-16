
#include "cliConfig.hpp"
#include "lib/interface.hpp"
#include "dispatcher.hpp"
#include "cliVersion.hpp"
#include <iostream>


int main (int argc, char *argv[]) 
{
    CliConfig cliConf(argc, argv);
    boost::program_options::variables_map secondaryOpts;
    P_OP mainOption = cliConf.getConfig(secondaryOpts);
    bool opSucceded = true;

    int8_t programStatus = 0;
    if(mainOption == P_OP::OP_VERSION) {
        //print version and exit
        printCliVersion();
        return programStatus;
    }
    if(mainOption == P_OP::OP_HELP) {
        //help message already printed, just exit
        return programStatus;
    }
    //we need to init the database for these options
    initLib(secondaryOpts["root"].as<std::string>(), secondaryOpts["config"].as<std::string>(), secondaryOpts["arch"].as<std::string>(),
            secondaryOpts["dbpath"].as<std::string>());
    std::string libRet = "";
    switch(mainOption) {
    case P_OP::OP_ADD :
        opSucceded = addPackages();
        break;
    case P_OP::OP_DATABASE :
        opSucceded = databaseDispatcher(secondaryOpts, libRet);
        std::cout<<"pong says: "<<libRet<<std::endl;
        break;
    case P_OP::OP_UPGRADE:
    case P_OP::OP_FRESHEN:
    case P_OP::OP_HELP:
    case P_OP::OP_UNKNOWN:
        break;
    case P_OP::OP_VERSION:
    case P_OP::OP_REMOVE:
    case P_OP::OP_QUERY:
    case P_OP::OP_SYNC:
    case P_OP::OP_PS:
    case P_OP::OP_VERCMP:
        std::cout<<"option not implemented"<<std::endl;
    }
    dbClean();

    programStatus = opSucceded ? 0 : 1;
    return programStatus;
}

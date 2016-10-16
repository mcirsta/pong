
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

    if(isLibInitRequired(mainOption)) {
        //we need to init the database for these options
        initLib(secondaryOpts["root"].as<std::string>(), secondaryOpts["config"].as<std::string>(), secondaryOpts["arch"].as<std::string>(),
                secondaryOpts["dbpath"].as<std::string>());
    }

    std::string libRet = "";
    bool opSucceded = true;
    switch(mainOption) {
    case P_OP::OP_UNKNOWN:
        //do nothing, exit successfully
        opSucceded = true;
        break;
    case P_OP::OP_VERSION:
        //print version and exit successfully
        printCliVersion();
        opSucceded = true;
        break;
    case P_OP::OP_HELP:
        //help message already printed, exit successfully
        opSucceded = true;
        break;
    case P_OP::OP_ADD :
        opSucceded = addPackages();
        break;
    case P_OP::OP_DATABASE :
        opSucceded = databaseDispatcher(secondaryOpts, libRet);
        dbClean();
        std::cout<<"pong says: "<<libRet<<std::endl;
        break;
    case P_OP::OP_UPGRADE:
    case P_OP::OP_FRESHEN:
    case P_OP::OP_REMOVE:
    case P_OP::OP_QUERY:
    case P_OP::OP_SYNC:
    case P_OP::OP_PS:
    case P_OP::OP_VERCMP:
        std::cout<<"option not implemented"<<std::endl;
        opSucceded = true;
        break;
    }

    int8_t programStatus = opSucceded ? 0 : 1;
    return programStatus;
}


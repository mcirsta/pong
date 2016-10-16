
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
    bool libInitDone = false;
    bool opSucceded = true;

    if(isLibInitRequired(mainOption)) {
        //we need to init the database for these options
        libInitDone = initLib(secondaryOpts["root"].as<std::string>(), secondaryOpts["config"].as<std::string>(), secondaryOpts["arch"].as<std::string>(),
                secondaryOpts["dbpath"].as<std::string>());
        if(!libInitDone) {
            std::cout<<"could not initialize pong library"<<std::endl;
            opSucceded = false;
        }
        else {
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
            case P_OP::OP_REMOVE:
            case P_OP::OP_QUERY:
            case P_OP::OP_SYNC:
            case P_OP::OP_PS:
            case P_OP::OP_VERCMP:
                std::cout<<"option not implemented"<<std::endl;
            default:
                opSucceded = true;
                break;
            }

            if(!pongLibClose()) {
                std::cout<<"could not properly close pong library"<<std::endl;
                opSucceded = false;
            }
        }
    }
    else {
        if (mainOption == P_OP::OP_VERSION) {
            printCliVersion();
        }
        opSucceded = true;
    }

    int8_t programStatus = opSucceded ? 0 : 1;
    return programStatus;
}


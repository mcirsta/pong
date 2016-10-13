
#include <iostream>

#include "cliConfig.hpp"
#include "lib/interface.hpp"
#include "dispatcher.hpp"

void printCliVersion()
{
    const float PACKAGE_VERSION = 0.1;
    const float PM_VERSION = 0.1;
    std::cout<<std::endl;
    std::cout<<"              |    Pong v"<<PACKAGE_VERSION<<" - libpong v"<< PM_VERSION<<std::endl;
    std::cout<<"                   Copyright (C) 2015 Marius Cirsta\n";
    std::cout<<"     o             See /usr/share/doc/pong-"<<PACKAGE_VERSION<<"/AUTHORS for more info."<<std::endl;
    std::cout<<"|                  "<<std::endl;
    std::cout<<"                   This program may be freely redistributed under"<<std::endl;
    std::cout<<"                   the terms of the GNU General Public License, version 3"<<std::endl;
    std::cout<<std::endl;
}

int main (int argc, char *argv[]) 
{
    CliConfig cliConf(argc, argv);
    boost::program_options::variables_map secondaryOpts;
    P_OP mainOption = cliConf.getConfig(secondaryOpts);
    bool opSucceded = true;
    if(mainOption == P_OP::OP_VERSION) {
        //print version and exit
        printCliVersion();
        return 0;
    }
    if(mainOption == P_OP::OP_HELP) {
        //help message already printed, just exit
        return 0;
    }
    //we need to init the database for these options
    initLib(secondaryOpts["root"].as<std::string>(), secondaryOpts["config"].as<std::string>(), secondaryOpts["arch"].as<std::string>(),
            secondaryOpts["dbpath"].as<std::string>());
    std::string libRet = "";
    switch(mainOption) {
    case P_OP::OP_ADD :             opSucceded = addPackages();                        break;
    case P_OP::OP_DATABASE :        opSucceded = databaseDispatcher(secondaryOpts, libRet);
        std::cout<<"pong says: "<<libRet<<std::endl;
        break;
    case P_OP::OP_UPGRADE:
    case P_OP::OP_FRESHEN:
    case P_OP::OP_HELP:
    case P_OP::OP_UNKNOWN:
    case P_OP::OP_VERSION:
    case P_OP::OP_REMOVE:
    case P_OP::OP_QUERY:
    case P_OP::OP_SYNC:
    case P_OP::OP_PS:
    case P_OP::OP_VERCMP:
        std::cout<<"option not implemented"<<std::endl;
    }
    dbClean();
    return opSucceded;
}

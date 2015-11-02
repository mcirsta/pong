#define PACKAGE_VERSION 0.1
#define PM_VERSION      0.1

#include <iostream>

#include "cliConfig.hpp"
#include "lib/interface.hpp"

void printCliVersion()
{
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
    initLib(secondaryOpts["root"].as<std::string>(), secondaryOpts["config"].as<std::string>(), secondaryOpts["arch"].as<std::string>(),
            secondaryOpts["dbpath"].as<std::string>());
    switch(mainOption) {
    case P_OP::OP_VERSION :         printCliVersion();  break;
    case P_OP::OP_ADD :             opSucceded = addPackages();  break;
    }
}


#include "cliVersion.hpp"
#include <iostream>


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


#define PACKAGE_VERSION 0.1
#define PM_VERSION      0.1

#include <iostream>

#include <boost/program_options.hpp>
namespace boost_po = boost::program_options;

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
    boost_po::options_description mainopts("Main options");
    mainopts.add_options()
            ("help,h",    "produce a help message")
            ("version,V", "output the version number")
            ("add,A",     "add fpm file")
            ("remove,R",  "remove package")
            ("upgrade,U", "upgrade fpm file")
            ("freshen,F", "feshen fpm file")
            ("query,Q",   "query package")
            ("sync,S",    "sync")
            ("ps,P",      "ps")
            ("vercmp,Y",  "compare versions")
            ;

    boost_po::options_description add("Add options");
    add.add_options()
            ("nodeps,d",   "skip dependency checks")
            ("force,f",    "force install, overwrite conflicting files")
            ("noarch",     "install the package, even if it is for an other arch")
            ;
    boost_po::options_description remove("Remove options");
    boost_po::options_description upgrade("Upgrade options");
    boost_po::options_description freshen("Freshen options");
    boost_po::options_description query("Query options");
    boost_po::options_description sync("Sync options");
    boost_po::options_description ps("ps options");
    boost_po::options_description vercmp("Vercmp options");

    boost_po::variables_map mainvm;
    boost_po::store(boost_po::parse_command_line(argc, argv, mainopts), mainvm);

    if(mainvm.count("help") || argc<=1) {
        std::cout<<mainopts;
        EXIT_SUCCESS;
    }

    if(mainvm.count("version")) {
        printCliVersion();
        EXIT_SUCCESS;
    }

    if(mainvm.count("add") || argc<=1) {
        std::cout<<mainopts;
    }

    if(mainvm.count("remove") || argc<=1) {
        std::cout<<mainopts;
    }

    if(mainvm.count("upgrade") || argc<=1) {
        std::cout<<mainopts;
    }

    if(mainvm.count("freshen") || argc<=1) {
        std::cout<<mainopts;
    }

    if(mainvm.count("query") || argc<=1) {
        std::cout<<mainopts;
    }
    if(mainvm.count("sync") || argc<=1) {
        std::cout<<mainopts;
    }

    if(mainvm.count("ps") || argc<=1) {
        std::cout<<mainopts;
    }

    if(mainvm.count("vercmp") || argc<=1) {
        std::cout<<mainopts;
    }

}

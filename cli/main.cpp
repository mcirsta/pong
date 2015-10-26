#define PACKAGE_VERSION 0.1
#define PM_VERSION      0.1

#include <iostream>

#include "cliConfig.hpp"

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
    boost_po::options_description commonOpts("Version compare options");
    commonOpts.add_options()
            ("config", boost_po::value<std::string>(), pOpts["config"])
            ("noconfirm",     pOpts["noconfirm"])
            ("ask",  boost_po::value<int>(), pOpts["ask"])
            ("regex",         pOpts["regex"])
            ("verbose,v",     pOpts["verbose"])
            ("root,r",   boost_po::value<std::string>(), pOpts["root"])
            ("dbpath,b", boost_po::value<std::string>(), pOpts["dbpath"])
            ("input", boost_po::value< std::vector<std::string>>(), pOpts["input"])
            ("help,h",          pOpts["help"])
            ;
    boost_po::options_description addRemoveOpts("Install only options");
    addRemoveOpts.add_options()
            ("noprogressbar", "do not show a progress bar when downloading files")
            ("noscriptlet",   "do not execute the install scriptlet if there is any")
            ;
    addRemoveOpts.add(commonOpts);



    boost_po::variables_map mainvm;

    //    try {

    //    boost_po::store(boost_po::parse_command_line(argc, argv, mainopts), mainvm);

    //    }
    //    catch(std::exception& e) {
    //        std::cout << e.what() << "\n";
    //    }

//    boost_po::positional_options_description p;
//    p.add("input", -1);
//    boost_po::command_line_parser(argc, argv).options(mainOpts).positional(p).run();

    boost_po::parsed_options parsedMain = boost_po::command_line_parser(argc, argv).options(mainOpts).allow_unregistered().run();

    std::vector<std::string> secondaryOpts = collect_unrecognized(parsedMain.options, boost_po::include_positional);

    boost_po::store(parsedMain, mainvm);

    if(mainvm.count("help") || argc<=1) {
        std::cout<<mainOpts;
        EXIT_SUCCESS;
    }

    if(mainvm.count("version")) {
        printCliVersion();
        EXIT_SUCCESS;
    }

    if(mainvm.count("add")) {
        mainvm.erase("add");
        std::cout<<"add chosen"<<std::endl;
        for(const auto &i:mainvm) {
            if(i.first == "nodeps") {
                std::cout<<"nodeps chosen"<<std::endl;
            }
            else if(i.first == "force") {
                std::cout<<"force chosen"<<std::endl;
            }
            else if(i.first == "noarch") {
                std::cout<<"dbonly chosen"<<std::endl;
            }
            else if(i.first == "input") {
                std::vector<std::string> inputNames = i.second.as< std::vector<std::string> >();
                std::cout<<"input: ";
                for(const auto& iname:inputNames) {
                     std::cout<<iname<<" ";
                }
                std::cout<<std::endl;
            }
            else {
                std::cout<<"invalid add option "<<i.first<<std::endl;
                std::cout<<addOpts<<std::endl;
            }
        }
    }

    if(mainvm.count("remove")) {
        mainvm.erase("remove");
        std::cout<<"remove chosen"<<std::endl;
        for(const auto &i:mainvm) {
            if(i.first == "cascade") {
                std::cout<<"cascade chosen"<<std::endl;
            }
            else if(i.first == "nodeps") {
                std::cout<<"nodeps chosen"<<std::endl;
            }
            else if(i.first == "dbonly") {
                std::cout<<"dbonly chosen"<<std::endl;
            }
            else if(i.first == "nosave") {
                std::cout<<"dbonly chosen"<<std::endl;
            }
            else if(i.first == "recursive") {
                std::cout<<"dbonly chosen"<<std::endl;
            }
            else if(i.first == "input") {
                std::vector<std::string> inputNames = i.second.as< std::vector<std::string> >();
                std::cout<<"input: ";
                for(const auto& iname:inputNames) {
                     std::cout<<iname<<" ";
                }
                std::cout<<std::endl;
            }
            else {
                std::cout<<"invalid remove option "<<i.first<<std::endl;
                std::cout<<removeOpts<<std::endl;
            }
        }
    }

    if(mainvm.count("upgrade") && mainvm.count("freshen")) {
        //freshen and upgrade are almost the same, freshen is just a special upgrade case
        std::string upname = "";
        if(mainvm.count("freshen")) {
            upname = "freshen";
            mainvm.erase("freshen");
            std::cout<<"freshen chosen"<<std::endl;
        }
        else {
            upname = "upgrade";
            mainvm.erase("upgrade");
            std::cout<<"upgrade chosen"<<std::endl;
        }
        for(const auto &i:mainvm) {
            if(i.first == "nodeps") {
                std::cout<<"nodeps chosen"<<std::endl;
            }
            else if(i.first == "force") {
                std::cout<<"force chosen"<<std::endl;
            }
            else if(i.first == "input") {
                std::vector<std::string> inputNames = i.second.as< std::vector<std::string> >();
                std::cout<<"input: ";
                for(const auto& iname:inputNames) {
                     std::cout<<iname<<" ";
                }
                std::cout<<std::endl;
            }
            else {
                std::cout<<"invalid "<<upname<<" option "<<i.first<<std::endl;
                std::cout<<removeOpts<<std::endl;
            }
    }
    }

    if(mainvm.count("query")) {
        std::cout<<mainOpts;
    }
    if(mainvm.count("sync")) {
        std::cout<<mainOpts;
    }

    if(mainvm.count("ps")) {
        std::cout<<mainOpts;
    }

    if(mainvm.count("vercmp")) {
        std::cout<<mainOpts;
    }
}

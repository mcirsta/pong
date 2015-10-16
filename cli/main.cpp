#define PACKAGE_VERSION 0.1
#define PM_VERSION      0.1

#include <iostream>

#include <boost/program_options.hpp>
namespace boost_po = boost::program_options;

struct mainOpts {
    char shortName;
    std::string longName;
    std::string initialVal;
    bool mainOpt;
    std::string description;
};

struct secondaryOpts {
    char shortName;
    std::string longName;
    std::string initialVal;
    bool mainOpt;
    std::string description;
};

std::vector<cmdLineStruct> cmdOpts = { {'h', "help",     "produce a help message"    },




    {'f', "force", "0", false, "force install, overwrite conflicting"}
                                     };

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

//std::pair<std::string, std::string> reg_foo(const std::string& s)
//{
//    size_t pos = s.find("-A");
//    if (pos != std::string::npos) {
//        std::cout<<s.substr(pos+2,s.size())<<std::endl;
//        return make_pair(std::string(), std::string());
//        //return make_pair(s.substr(5), std::string());
//    } else {
//        return make_pair(std::string(), std::string());
//    }
//}

int main (int argc, char *argv[]) 
{
    boost_po::options_description allOpts("All options");
    mainOpts.add_options()
            ("help,h",          "produce a help message")
            ("version,V",       "output the version number")
            ("add,A",           "add fpm file")
            ("remove,R",        "remove package")
            ("upgrade,U",       "upgrade fpm file")
            ("freshen,F",       "feshen fpm file")
            ("query,Q",         "query package")
            ("sync,S",          "sync")
            ("ps,P",            "ps")
            ("vercmp,Y",        "compare versions")
            ("input", boost_po::value< std::vector<std::string> >(), "input file")
            ("nodeps,d",        "skip dependency checks")
            ("noarch",          "install the package, even if it is for an other arch")
            ("cascade,c",       "remove packages and all packages that depend on them")
            ("dbonly,k",        "only remove database entry, do not remove files")
            ("nosave,n",        "remove configuration files as well")
            ("recursive,s",     "remove dependencies also (that won't break packages)")
            ("force,f",         "force install, overwrite conflicting")
            ;

    allOpts.add(mainOpts).add(addOpts).add(removeOpts).add(upgradeOpts).add(queryOpts).add(syncOpts).
            add(psOpts).add(vercmpOpts);

    boost_po::variables_map mainvm;

    //    try {

    //    boost_po::store(boost_po::parse_command_line(argc, argv, mainopts), mainvm);

    //    }
    //    catch(std::exception& e) {
    //        std::cout << e.what() << "\n";
    //    }

    boost_po::positional_options_description p;
    p.add("input", -1);

    boost_po::parsed_options parsedMain = boost_po::command_line_parser(argc, argv).options(allOpts).positional(p).run();

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

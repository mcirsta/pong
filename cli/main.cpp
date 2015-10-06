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

    boost_po::options_description addOpts("Add options");
    addOpts.add_options()
            ("nodeps,d",   "skip dependency checks")
            ("force,f",    "force install, overwrite conflicting files")
            ("noarch",     "install the package, even if it is for an other arch")
            ;
    boost_po::options_description remove("Remove options");
    remove.add_options()
    ("cascade,c",       "remove packages and all packages that depend on them")
    ("nodeps,d",        "skip dependency checks")
    ("dbonly,k",        "only remove database entry, do not remove files")
    ("nosave,n",        "remove configuration files as well")
    ("recursive,s",     "remove dependencies also (that won't break packages)")
    ;
    boost_po::options_description upgrade("Upgrade options");
    boost_po::options_description freshen("Freshen options");
    boost_po::options_description query("Query options");
    boost_po::options_description sync("Sync options");
    boost_po::options_description ps("ps options");
    boost_po::options_description vercmp("Vercmp options");

    boost_po::variables_map mainvm;

//    try {

//    boost_po::store(boost_po::parse_command_line(argc, argv, mainopts), mainvm);

//    }
//    catch(std::exception& e) {
//        std::cout << e.what() << "\n";
//    }

    boost_po::parsed_options parsedMain = boost_po::command_line_parser(argc, argv).options(mainopts).allow_unregistered().run();

    boost_po::store(parsedMain, mainvm);

    if(mainvm.count("help") || argc<=1) {
        std::cout<<mainopts;
        EXIT_SUCCESS;
    }

    if(mainvm.count("version")) {
        printCliVersion();
        EXIT_SUCCESS;
    }

    std::vector<std::string> remainingOpts = boost_po::collect_unrecognized(parsedMain.options, boost_po::include_positional);

    if(mainvm.count("add")) {
        std::cout<<"add chosen and rest to parse is "<<remainingOpts[0]<<std::endl;
        boost_po::variables_map addvm;
        addOpts.add(mainopts);
        parsedMain = boost_po::command_line_parser(remainingOpts).options(addOpts).allow_unregistered().run();
        boost_po::store(parsedMain, addvm);
        if(addvm.count("nodeps")) {
            std::cout<<"no deps chosen"<<std::endl;
        }
    }

    if(mainvm.count("remove")) {
        std::cout<<mainopts;
    }

    if(mainvm.count("upgrade")) {
        std::cout<<mainopts;
    }

    if(mainvm.count("freshen")) {
        std::cout<<mainopts;
    }

    if(mainvm.count("query")) {
        std::cout<<mainopts;
    }
    if(mainvm.count("sync")) {
        std::cout<<mainopts;
    }

    if(mainvm.count("ps")) {
        std::cout<<mainopts;
    }

    if(mainvm.count("vercmp")) {
        std::cout<<mainopts;
    }
}

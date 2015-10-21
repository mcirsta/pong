#define PACKAGE_VERSION 0.1
#define PM_VERSION      0.1

#include <iostream>

#include <boost/program_options.hpp>
namespace boost_po = boost::program_options;

std::map<std::string, const char*> pOpts {{"help",          "produce a help message"},
                                          {"version",       "output the version number"},
                                          {"add",           "add fpm file"},
                                          {"remove",        "remove package"},
                                          {"upgrade",       "upgrade fpm file"},
                                          {"freshen",       "feshen fpm file"},
                                          {"query",         "query package"},
                                          {"sync",          "sync"},
                                          {"ps",            "ps"},
                                          {"vercmp",        "compare versions"},
                                          {"nodeps",        "skip dependency checks"},
                                          {"force",         "force install, overwrite conflicting"},
                                          {"noarch",        "install the package, even if it is for an other arch"},
                                          {"input",         "input file"},
                                          {"help",          "produce a help message"},
                                          {"cascade",       "remove packages and all packages that depend on them"},
                                          {"dbonly",        "only remove database entry, do not remove files"},
                                          {"nosave",        "remove configuration files as well"},
                                          {"recursive",     "remove dependencies also (that won't break packages)"},
                                          {"freshen",       "only upgrade this package if it is already installed and at a lesser version"},
                                          {"changelog",     "view the changelog of a package"},
                                          {"nodeps",        "skip dependency checks"},
                                          {"orphans",       "list all packages that were installed as a dependency \
                                                               and are not required by any other packages"},
                                          {"fsck",          "check the integrity of packages' files"},
                                          {"groups",        "view all members of a package group"},
                                          {"info",          "view package information"},
                                          {"list",          "list the contents of the queried package"},
                                          {"foreign",       "list all packages that were not found in the sync db(s)"},
                                          {"owns",          "query the package that owns <file>"},
                                          {"file",          "pacman-g2 will query the package file [package] instead of \
                                                               looking in the database"},
                                          {"search",        "search locally-installed packages for matching strings"},
                                          {"test",          "search problems in the local database"},
                                          {"clean",         "remove old packages from cache directory (use -cc for all)"},
                                          {"dependsonly",   "install dependencies only"},
                                          {"groups",        "view all members of a package group"},
                                          {"print-uris",    "print out URIs for given packages and their dependencies"},
                                          {"search",        "search remote repositories for matching strings"},
                                          {"sysupgrade",    "upgrade all packages that are out of date"},
                                          {"downloadonly",  "download packages but do not install/upgrade anything"},
                                          {"refresh",       "download fresh package databases from the server"},
                                          {"ignore",        "<pkg>  ignore a package upgrade (can be used more than once)"},
                                          {"nointegrity",   "don't check the integrity of the packages using sha1"},
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

int main (int argc, char *argv[]) 
{
    boost_po::options_description mainOpts("Main options");
    mainOpts.add_options()
            ("help,h",          pOpts["help"])
            ("version,V",       pOpts["version"])
            ("add,A",           pOpts["add"])
            ("remove,R",        pOpts["remove"])
            ("upgrade,U",       pOpts["upgrade"])
            ("freshen,F",       pOpts["freshen"])
            ("query,Q",         pOpts["query"])
            ("sync,S",          pOpts["sync"])
            ("ps,P",            pOpts["ps"])
            ("vercmp,Y",        pOpts["vercmp"])
            ;
    boost_po::options_description addOpts("Add options");
    addOpts.add_options()
            ("nodeps,d",        pOpts["nodeps"])
            ("force,f",         pOpts["force"])
            ("noarch",          pOpts["noarch"])
            ("input", boost_po::value< std::vector<std::string> >(), pOpts["input"])
            ("help,h",          pOpts["help"])
            ;
    boost_po::options_description removeOpts("Remove options");
    removeOpts.add_options()
            ("cascade,c",       pOpts["cascade"])
            ("nodeps,d",        pOpts["nodeps"])
            ("dbonly,k",        pOpts["dbonly"])
            ("nosave,n",        pOpts["nosave"])
            ("recursive,s",     pOpts["recursive"])
            ("input", boost_po::value< std::vector<std::string> >(), pOpts["input"])
            ("help,h",          pOpts["help"])
            ;
    boost_po::options_description upgradeOpts("Upgrade options");
    upgradeOpts.add_options()
            ("nodeps,d",        pOpts["nodeps"])
            ("force,f",         pOpts["force"])
            ("freshen,e",       "only upgrade this package if it is already installed and at a lesser version")
            ("input", boost_po::value< std::vector<std::string> >(), pOpts["input"])
            ("help,h",          pOpts["help"])
            ;
    boost_po::options_description queryOpts("Query options");
    queryOpts.add_options()
            ("changelog,c",     pOpts["changelog"])
            ("nodeps,d",        pOpts["nodeps"])
            ("orphans,e",       pOpts["orphans"])
            ("fsck,f",          pOpts["fsck"])
            ("groups,g",        pOpts["groups"])
            ("info,i",          pOpts["info"])
            ("list,l",          pOpts["list"])
            ("foreign,m",       pOpts["foreign"])
            ("owns,o",          pOpts["owns"])
            ("file,p",          pOpts["file"])
            ("search,s",        pOpts["search"])
            ("test,t",          pOpts["test"])
            ;
    boost_po::options_description syncOpts("Sync options");
    syncOpts.add_options()
            ("clean,c",         pOpts["clean"])
            ("nodeps,d",        pOpts["nodeps"])
            ("dependsonly,e",   pOpts["dependsonly"])
            ("force,f",         pOpts["force"])
            ("groups,g",        pOpts["groups"])
            ("print-uris,p",    pOpts["print-uris"])
            ("search,s",        pOpts["search"])
            ("sysupgrade,u",    pOpts["sysupgrade"])
            ("downloadonly,w",  pOpts["downloadonly"])
            ("refresh,y",       pOpts["refresh"])
            ("ignore",          pOpts["ignore"])
            ("nointegrity",     pOpts["nointegrity"])
            ;
    boost_po::options_description vercmpOpts("Version compare options");

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

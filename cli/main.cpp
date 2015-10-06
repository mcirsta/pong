#include <iostream>

#include <boost/program_options.hpp>
namespace boost_po = boost::program_options;

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
            ("vercmp,Y",  "compare versions");
            ;

    boost_po::options_description add("Add options");
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
    }
}

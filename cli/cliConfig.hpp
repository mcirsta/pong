/*Comments*/
#ifndef CLICONFIG_H_
#define CLICONFIG_H_

#include <string>
#include <map>

#include <boost/program_options.hpp>
namespace boost_po = boost::program_options;

class CliConfig {
   static std::map<const std::string, const char*> pOpts;
   boost_po::options_description commonOpts {"Version compare options"};
   boost_po::options_description addRemoveOpts {"Install only options"};
   boost_po::options_description mainOpts {"Main options"};
   boost_po::options_description addOpts {"Add options"};
   boost_po::options_description removeOpts {"Remove options"};
   boost_po::options_description upgradeOpts {"Upgrade options"};
   boost_po::options_description queryOpts {"Query options"};
   boost_po::options_description syncOpts {"Sync options"};
   boost_po::options_description vercmpOpts {"Version compare options"};

   public:
           CliConfig();
           void getConfig(int argc, char *argv[]);
};

#endif

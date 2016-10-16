
#ifndef CLICONFIG_HPP
#define CLICONFIG_HPP

#include <boost/program_options.hpp>
#include <string>
#include <map>

namespace boost_po = boost::program_options;


enum class P_OP {    OP_UNKNOWN,
                     OP_VERSION,
                     OP_HELP,
                     OP_ADD,
                     OP_UPGRADE,
                     OP_FRESHEN,
                     OP_REMOVE,
                     OP_QUERY,
                     OP_SYNC,
                     OP_DATABASE,
                     OP_PS,
                     OP_VERCMP,
                };

bool isLibInitRequired(const P_OP& option);


class CliConfig {
    static std::map<const std::string, const char*> pOpts;
    int pargc;
    char **pargv;
    boost_po::options_description commonOpts {"Version compare options"};
    boost_po::options_description addRemoveOpts {"Install only options"};
    boost_po::options_description mainOpts {"Main options"};
    boost_po::options_description addOpts {"Add options"};
    boost_po::options_description removeOpts {"Remove options"};
    boost_po::options_description upgradeOpts {"Upgrade options"};
    boost_po::options_description queryOpts {"Query options"};
    boost_po::options_description syncOpts {"Sync options"};
    boost_po::options_description databaseOpts {"Database options"};
    boost_po::options_description vercmpOpts {"Version compare options"};
    inline void secondaryProcess(boost_po::options_description optsToAdd, boost_po::variables_map& secondaryVM);

public:
    CliConfig(int argc, char *argv[]);
    P_OP getConfig(boost::program_options::variables_map &secondaryVM);
};


#endif //CLICONFIG_HPP

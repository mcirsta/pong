
#include "cliConfig.hpp"
#include <iostream>


bool isLibInitRequired(const P_OP& option)
{
    return ((option != P_OP::OP_HELP) && (option != P_OP::OP_VERSION)
            && (option != P_OP::OP_UNKNOWN));
}

std::map<const std::string, const char*> CliConfig::pOpts =
{{"help",          "produce a help message"},
 {"version",       "output the version number"},
 {"add",           "add fpm file"},
 {"remove",        "remove package"},
 {"upgrade",       "upgrade fpm file"},
 {"freshen",       "feshen fpm file"},
 {"query",         "query package"},
 {"sync",          "sync"},
 {"database",      "database operations"},
 {"ps",            "ps"},
 {"vercmp",        "compare versions"},
 {"nodeps",        "skip dependency checks"},
 {"force",         "force install, overwrite conflicting"},
 {"noarch",        "install the package, even if it is for an other arch"},
 {"input",         "input file"},
 {"desc",          "produce a help message"},
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
   {"config",        "<path> set an alternate configuration file"},
   {"noconfirm",     "do not ask for anything confirmation"},
   {"ask",           "<number> pre-specify answers for questions (see manpage)"},
   {"regex",         "treat targets as regexs if no package found"},
   {"verbose",       "be verbose"},
   {"root",          "<path>   set an alternate installation root"},
   {"dbpath",        "<path> set an alternate database location"},
   {"arch",          "<arch name> set a different arch than native"},
   {"rebuilddeps",   "<pkg name> packages that have to be rebuilt ( have <,<=,=,> or >= on this package"},
   {"alldeps",       "<pkg name> all deps that this package has"},
   {"revdeps",       "<pkg name> packages that depend on this package"},
   {"directdeps",    "<pkg name> just the direct depends specified by this package"}
  };

/* !!! When adding new opt groups (boost_po::options_description) always add the common opts to them  */

  CliConfig::CliConfig( int argc, char *argv[] ) : pargc(argc) , pargv(argv) {
      commonOpts.add_options()
      ("config", boost_po::value<std::string>()->default_value(""), pOpts["config"])
      ("noconfirm",     pOpts["noconfirm"])
      ("ask",  boost_po::value<int>(), pOpts["ask"])
      ("regex",         pOpts["regex"])
      ("verbose,v",     pOpts["verbose"])
      ("root,r",   boost_po::value<std::string>()->default_value(""), pOpts["root"])
      ("dbpath,b", boost_po::value<std::string>()->default_value(""), pOpts["dbpath"])
      ("arch",  boost_po::value<std::string>()->default_value(""), pOpts["arch"])
      ("input", boost_po::value< std::vector<std::string>>(), pOpts["input"])
      ("desc,h",          pOpts["desc"])
      ;
      addRemoveOpts.add_options()
      ("noprogressbar", "do not show a progress bar when downloading files")
      ("noscriptlet",   "do not execute the install scriptlet if there is any")
      ;
      addRemoveOpts.add(commonOpts);

      mainOpts.add_options()
      ("help,H",          pOpts["help"])
      ("version,V",       pOpts["version"])
      ("add,A",           pOpts["add"])
      ("remove,R",        pOpts["remove"])
      ("upgrade,U",       pOpts["upgrade"])
      ("freshen,F",       pOpts["freshen"])
      ("query,Q",         pOpts["query"])
      ("sync,S",          pOpts["sync"])
      ("database,D",      pOpts["database"])
      ("ps,P",            pOpts["ps"])
      ("vercmp,Y",        pOpts["vercmp"])
      ;

      addOpts.add_options()
      ("nodeps,d",        pOpts["nodeps"])
      ("force,f",         pOpts["force"])
      ("noarch",          pOpts["noarch"])
      ;
      addOpts.add(addRemoveOpts);

      removeOpts.add_options()
      ("cascade,c",       pOpts["cascade"])
      ("nodeps,d",        pOpts["nodeps"])
      ("dbonly,k",        pOpts["dbonly"])
      ("nosave,n",        pOpts["nosave"])
      ("recursive,s",     pOpts["recursive"])
      ;
      removeOpts.add(addRemoveOpts);

      upgradeOpts.add_options()
      ("nodeps,d",        pOpts["nodeps"])
      ("force,f",         pOpts["force"])
      ("freshen,e",       "only upgrade this package if it is already installed and at a lesser version")
      ;
      upgradeOpts.add(addRemoveOpts);

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
      queryOpts.add(commonOpts);

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
      ("ignore", boost_po::value<std::string>(), pOpts["ignore"])
      ("nointegrity",     pOpts["nointegrity"])
      ;
      syncOpts.add(addRemoveOpts);

      databaseOpts.add_options()
      ("rebuilddeps",  boost_po::value<std::string>(),   pOpts["rebuilddeps"])
      ("alldeps",      boost_po::value<std::string>(),   pOpts["alldeps"])
      ("revdeps",      boost_po::value<std::string>(),   pOpts["revdeps"])
      ("directdeps",   boost_po::value<std::string>(),   pOpts["directdeps"])
      ;

      databaseOpts.add(commonOpts);

      vercmpOpts.add_options()
      ("input", boost_po::value< std::vector<std::string> >(), pOpts["input"])
      ("desc,h",          pOpts["desc"])
      ;
  }

  inline void CliConfig::secondaryProcess(boost_po::options_description optsToAdd, boost_po::variables_map& secondaryVM) {
      mainOpts.add(optsToAdd);
      boost_po::positional_options_description p;
      p.add("input", -1);
      try {
          boost_po::store( boost_po::command_line_parser(pargc, pargv).options(mainOpts).positional(p).run(), secondaryVM );
      }
      catch(std::exception& e) {
          std::cout << e.what() << std::endl;
      }
  }

  P_OP CliConfig::getConfig(boost_po::variables_map& secondaryVM) {
      boost_po::parsed_options parsedMain = boost_po::command_line_parser(pargc, pargv).options(mainOpts).allow_unregistered().run();

      boost::program_options::variables_map mainvm;
      boost_po::store(parsedMain, mainvm);

      if(mainvm.find("add") !=  mainvm.end()) {
          std::cout<<"add chosen"<<std::endl;
          secondaryProcess(addOpts ,secondaryVM);
          return P_OP::OP_ADD;
      }

      if(mainvm.find("remove") != mainvm.end()) {
          std::cout<<"remove chosen"<<std::endl;
          secondaryProcess(removeOpts ,secondaryVM);
          return P_OP::OP_REMOVE;
      }

      if(mainvm.find("upgrade") != mainvm.end()) {
          std::cout<<"upgrade chosen"<<std::endl;
          secondaryProcess(upgradeOpts ,secondaryVM);
          return P_OP::OP_UPGRADE;
      }

      if(mainvm.find("freshen") != mainvm.end()) {
          std::cout<<"freshen chosen"<<std::endl;
          secondaryProcess(upgradeOpts ,secondaryVM);
          return P_OP::OP_FRESHEN;
      }

      if(mainvm.find("query") != mainvm.end()) {
          std::cout<<"query chosen"<<std::endl;
          secondaryProcess(queryOpts ,secondaryVM);
          return P_OP::OP_QUERY;
      }
      if(mainvm.find("sync") != mainvm.end()) {
          std::cout<<"sync chosen"<<std::endl;
          secondaryProcess(syncOpts ,secondaryVM);
          return P_OP::OP_SYNC;
      }

      if(mainvm.find("database") != mainvm.end()) {
          std::cout<<"database chosen"<<std::endl;
          secondaryProcess(databaseOpts ,secondaryVM);
          return P_OP::OP_DATABASE;
      }

      if(mainvm.find("ps") != mainvm.end()) {
          std::cout<<"ps chosen"<<std::endl;
          //secondaryProcess(psOpts ,secondaryVM);
          return P_OP::OP_PS;
      }

      if(mainvm.count("vercmp")) {
          std::cout<<"vercmp chosen"<<std::endl;
          secondaryProcess(vercmpOpts ,secondaryVM);
          return P_OP::OP_VERCMP;
      }

      if(mainvm.count("help") || pargc<=1) {
          std::cout<<mainOpts;
          return P_OP::OP_HELP;
      }

      if(mainvm.find("version") != mainvm.end()) {
          return P_OP::OP_VERSION;
      }

      return P_OP::OP_HELP;
  }

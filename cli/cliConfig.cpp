#include "cliConfig.hpp"

std::map<const std::string, const char*> CliConfig::pOpts =
{{"help",          "produce a help message"},
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
   {"config",        "<path> set an alternate configuration file"},
   {"noconfirm",     "do not ask for anything confirmation"},
   {"ask",           "<number> pre-specify answers for questions (see manpage)"},
   {"regex",         "treat targets as regexs if no package found"},
   {"verbose",       "be verbose"},
   {"root",          "<path>   set an alternate installation root"},
   {"dbpath",        "<path> set an alternate database location"},
  };

  CliConfig::CliConfig() {
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
      addRemoveOpts.add_options()
      ("noprogressbar", "do not show a progress bar when downloading files")
      ("noscriptlet",   "do not execute the install scriptlet if there is any")
      ;
      addRemoveOpts.add(commonOpts);

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

      vercmpOpts.add_options()
      ("input", boost_po::value< std::vector<std::string> >(), pOpts["input"])
      ("help,h",          pOpts["help"])
      ;
  }

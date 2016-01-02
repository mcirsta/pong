/*Comments*/
#ifndef DISPATCHER_H_
#define DISPATCHER_H_

#include <boost/program_options.hpp>
#include "../lib/interface.hpp"


const char* databaseDispatcher(const  boost::program_options::variables_map &secondaryOpts);

#endif

#ifndef UTILS_H_
#define UTILS_H_

#include <cstdio>
#include <string>

bool checkXZ(FILE *fp);
bool extractXZ(FILE *sfp, std::string dest);

#endif

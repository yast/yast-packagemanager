#ifndef _PMDBHEADER_H
#define _PMDBHEADER_H

#include <y2pm/PMError.h>
#include <y2pm/PMInserter.h>

PMError read_package_list(PMInserter<PMPackagePtr>& dest, string file);

#endif

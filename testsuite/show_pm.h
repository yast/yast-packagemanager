/*
  show_pm.h

  cout << PMSolvable
  cout << PMObject
  cout << PMPackage

*/

#ifndef SHOW_PM_H
#define SHOW_PM_H

#include <string>
#include <y2pm/PMPackage.h>
#include <y2pm/PMPackagePtr.h>


std::string list2string (const std::list<std::string>& slist, const char *sep = " ");
void show_pmsolvable (PMSolvablePtr p);
void show_pmobject (PMObjectPtr p);
void show_pmpackage (PMPackagePtr p);

#endif //SHOW_PM_H


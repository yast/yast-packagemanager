/*
  show_PM.cc

  cout << PMPackage

*/

#include <list>
#include <string>
#include <y2pm/PMError.h>
#include <y2pm/PMSolvable.h>
#include <y2pm/PMSolvablePtr.h>
#include <y2pm/PMObject.h>
#include <y2pm/PMObjectPtr.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMPackagePtr.h>
#include <iostream>
#include <algorithm>

#include "show_pm.h"

using namespace std;

std::string list2string (const std::list<std::string>& slist, const char *sep)
{
   std::string ret;
   if (slist.empty())
	return ret;
   for (std::list<std::string>::const_iterator pos = slist.begin();
	pos != slist.end(); ++pos)
   {
	if (!ret.empty())
	    ret += sep;
	ret += *pos;
   }
   return ret;
}

void
show_pmsolvable (PMSolvablePtr p)
{
    cout << "-- Solvable --" << endl;
    cout << p->name() << "-" << p->version() << "-" << p->release() << "." << p->arch() << endl;
    cout << "Requires: " << list2string (PMSolvable::PkgRelList2StringList (p->requires())) << endl;
    cout << "PreRequires: " << list2string (PMSolvable::PkgRelList2StringList (p->prerequires())) << endl;
    cout << "Provides: " << list2string (PMSolvable::PkgRelList2StringList (p->provides())) << endl;
    cout << "Obsoletes: " << list2string (PMSolvable::PkgRelList2StringList (p->obsoletes())) << endl;
    cout << "Conflicts: " << list2string (PMSolvable::PkgRelList2StringList (p->conflicts())) << endl;
}

void
show_pmobject (PMObjectPtr p)
{
    show_pmsolvable (p);
    cout << "-- Object --" << endl;
#if 1
    cout << "Summary: " << p->summary() << endl;
    cout << "Description: " << list2string (p->description(), "\n") << endl;
    cout << "InsNotify: " << list2string (p->insnotify(), "\n") << endl;
    cout << "DelNotify: " << list2string (p->delnotify(), "\n") << endl;
    cout << "Size: " << p->size().asString() << endl;
#endif
}

void
show_pmpackage (PMPackagePtr p)
{
    p->startRetrieval();
    show_pmobject ((PMObjectPtr)p);

    cout << "Buildtime: " << p->buildtime() << endl;
    cout << "Buildhost: " << p->buildhost() << endl;
    cout << "Installtime: " << p->installtime() << endl;
    cout << "Distribution: " << p->distribution() << endl;
    cout << "Vendor: " << p->vendor() << endl;
    cout << "License: " << p->license() << endl;
    cout << "Packager: " << p->packager() << endl;
    cout << "Group: " << p->group() << endl;
    cout << "Changelog: " << list2string(p->changelog(), "\n") << endl;
    cout << "Url: " << p->url() << endl;
    cout << "OS: " << p->os() << endl;
    cout << "PreIn: " << list2string(p->prein(), "\n") << endl;
    cout << "PostIn: " << list2string(p->postin(), "\n") << endl;
    cout << "PreUn: " << list2string(p->preun(), "\n") << endl;
    cout << "PostUn: " << list2string(p->postun(), "\n") << endl;
    cout << "Source: " << p->sourcerpm() << endl;
    cout << "Archivesize: " << p->archivesize() << endl;
    cout << "Authors: " << list2string(p->authors(), ", ") << endl;
    cout << "Files: " << list2string(p->filenames(), "\n") << endl;
    cout << "Recommends: " << list2string(p->recommends()) << endl;
    cout << "Suggests: " << list2string(p->suggests()) << endl;
    cout << "Location: " << p->location() << endl;
    cout << "Keywords: " << list2string(p->keywords()) << endl;
    cout << "========" << endl;
    p->stopRetrieval();
    return;
}


void
show_pmselection (PMSelectionPtr s)
{
    s->startRetrieval();
    show_pmobject ((PMObjectPtr)s);

    cout << "Category: " << s->category () << endl;
    cout << "Visible: " << s->visible () << endl;
    cout << "Order: " << s->order() << endl;
    cout << "Suggests: " << list2string(s->suggests()) << endl;
    cout << "InsPacks: " << list2string(s->inspacks("")) << endl;
    cout << "DelPacks: " << list2string(s->delpacks("")) << endl;
    cout << "Archivesize: " << s->archivesize() << endl;
    cout << "========" << endl;
    s->stopRetrieval();
    return;
}


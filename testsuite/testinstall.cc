/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                     (C) 2002 SuSE AG |
\----------------------------------------------------------------------/

   File:       testinstall.cc
   Purpose:    test dependency solver
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

   !THIS CODE IS GPL AS READLINE IS USED!

/-*/


// readline
#include <cstdio>
#include <readline/readline.h>
#include <readline/history.h>

#include <unistd.h>

#include <y2util/timeclass.h>
#include <y2util/Y2SLog.h>
#include <Y2PM.h>
#include <y2pm/RpmDb.h>
#include <y2pm/PkgDep.h>
#include <y2pm/PMPackage.h>

#undef Y2SLOG
#define Y2SLOG "testinstall" 

using namespace std;

void install(vector<string>& argv);
void consistent(vector<string>& argv);
void help(vector<string>& argv);
void init(vector<string>& argv);
void query(vector<string>& argv);

struct Funcs {
    const char* name;
    void (*func)(vector<string>&);
    const char* helptext;
};

static struct Funcs func[] = {
    { "install", install, "install a package" },
    { "consistent", consistent, "check consistency" },
    { "init", init, "initialize packagemanager" },
    { "query", query, "query package" },
    { "help", help, "this screen" },
    { NULL, NULL }
};

static int verbose = 0;

void usage(char **argv) {
	cerr <<
"Do PHI dependency completion on a set of RPMS or the installed database.\n\n"
"Usage: " << argv[0] << " -v for increasing verbose output\n"
"	-h --help		print this help\n"
"	--from path-to-rpms	Specifies the path to the RPMs set.\n"
"				If not specified, use the installed set of RPMs\n"
"	<packagenames>		packagenames to resolve\n"
"		(if none specified, read from stdin)\n"
"\n\n"
"Output is a list of lines of the form:\n\n"
"	install <pkgname>		Install this package.\n"
"	remove <pkgname>		Remove this package.\n"
	    ;
	exit(1);
}


void query(vector<string>& argv)
{
    PMPackageManager& manager = Y2PM::packageManager();
    vector<string>::iterator it=argv.begin();
    ++it; // first one is function name itself
    for(;it!=argv.end();++it)
    {
	PMSelectablePtr sel = manager.getItem(*it);
	if(sel == NULL)
	{
	    cout << *it << " is not available" << endl;
	}
	else
	{
	    PMPackagePtr obj = static_cast<PMPackagePtr>(sel->installedObj());
	    for(PMSolvable::PMSolvableAttribute attr = PMSolvable::PMSLV_ATTR_BEGIN;
		attr < PMSolvable::PMSLV_NUM_ATTRIBUTES;
		attr = PMSolvable::PMSolvableAttribute(attr+1))
	    {
		cout
		    << obj->getAttributeName(attr)
		    << ": "
		    << obj->getAttributeValue(attr)
		    << endl;
	    }
	    for(PMObject::PMObjectAttribute attr = PMObject::PMOBJ_ATTR_BEGIN;
		attr < PMObject::PMOBJ_NUM_ATTRIBUTES;
		attr = PMObject::PMObjectAttribute(attr+1))
	    {
		cout
		    << obj->getAttributeName(attr)
		    << ": "
		    << obj->getAttributeValue(attr)
		    << endl;
	    }
	    for(PMPackage::PMPackageAttribute attr = PMPackage::PKG_ATTR_BEGIN;
		attr < PMPackage::PKG_NUM_ATTRIBUTES;
		attr = PMPackage::PMPackageAttribute(attr+1))
	    {
		cout
		    << obj->getAttributeName(attr)
		    << ": "
		    << obj->getAttributeValue(attr)
		    << endl;
	    }
	}
    }
}

void init(vector<string>& argv)
{
    TimeClass t;
    cout << "initializing packagemanager ... " << endl;

    t.startTimer();
    Y2PM::packageManager();
    t.stopTimer();
    
    cout << "done in " << t.getTimer() << " seconds" << endl;
    
}

void help(vector<string>& argv)
{
    for(unsigned i=0; func[i].name; i++)
    {
	cout << func[i].name << ": " << func[i].helptext << endl;
    }
}

// example
Alternatives::AltDefaultList alternative_default( PkgName name )
{
    Alternatives::AltDefaultList list;
    if(name==PkgName("spell"))
	list.push_front(PkgName("aspell"));
    else if(name==PkgName("libGL.so.1") || name==PkgName("libgl"))
	list.push_front(PkgName("mesasoft"));
    return list;
}

static PkgDep::WhatToDoWithUnresolvable unresolvable_callback(
    PkgDep* solver, const PkgRelation& rel, PMSolvablePtr& p)
{

    if(rel.name()->find("rpmlib(") != std::string::npos)
	return PkgDep::UNRES_IGNORE;
 /*   
    if(rel.name() == "/bin/bash" || rel.name() == "/bin/sh")
    {
	const PkgSet inst = solver->current_installed();
	p = inst.lookup(PkgName("bash"));
	return PkgDep::UNRES_TAKETHIS;
    }
    if(rel.name() == "/usr/bin/perl")
    {
	const PkgSet inst = solver->current_installed();
	p = inst.lookup(PkgName("perl"));
	return PkgDep::UNRES_TAKETHIS;
    }
*/ 
    if(rel.name()->operator[](0)=='/')
    {
	DBG << "ignoring file requirement " << rel.name() << endl;
	return PkgDep::UNRES_IGNORE;
    }

    return PkgDep::UNRES_FAIL;
}


PkgSet* getInstalled()
{
    PkgSet* set = new PkgSet();
    
    MIL << "initialize manager" << endl;
    PMPackageManager& manager = Y2PM::packageManager();
    MIL << "got " << manager.size() << endl;

    for(PMManager::PMSelectableVec::const_iterator it = manager.begin();
	it != manager.end(); ++it )
    {
	// create set of all packages
	set->add( (*it)->installedObj() );
    }

    return set;
}

/** not implemented yet */
PkgSet* getAvailable()
{
    return new PkgSet();
}

void install(vector<string>& argv)
{
    // build sets
    PkgSet empty;
    PkgSet candidates;
    
    PkgSet *installed = NULL;
    PkgSet *available = NULL;

    // swapped since available is not yet implemented
    installed = getAvailable();
    available = getInstalled();

    for (unsigned i=1; i < argv.size() ; i++) {
	if (available->lookup(PkgName(argv[i])) == NULL) {
		std::cerr << "package " << argv[i] << " is not available.\n";
		return;
	}
	candidates.add(available->lookup(PkgName(argv[i])));
    }
/*
    if (!oncmdline) {
	    char fupp[200];
	    while (fgets(fupp,sizeof(fupp),stdin)) {
		char *s;
		s = strchr(fupp,'\n'); if (s) *s = '\0';
		s = strchr(fupp,'\r'); if (s) *s = '\0';
		if (available->lookup(fupp)) 
		    candidates.add(available->lookup(fupp));
	    }
    }
*/
    // construct PkgDep object
//    PkgDep::set_default_alternatives_mode(PkgDep::AUTO_IF_NO_DEFAULT);
    PkgDep engine( *installed, *available, alternative_default );
    engine.set_unresolvable_callback(unresolvable_callback);

    // call upgrade
    PkgDep::ResultList good;
    PkgDep::ErrorResultList bad;
    PkgDep::NameList to_remove;

    int numinst=0,numrem=0,numbad=0;
    bool success = false;

    TimeClass t;
    t.startTimer();

    success = engine.install( candidates, good, bad);
    
    t.stopTimer();

    if (!success) {
	// if it failed, print problems
	cout << "*** Conflicts ***" << endl;
	for( PkgDep::ErrorResultList::const_iterator p = bad.begin();
	     p != bad.end(); ++p ) {
	    cerr << *p << endl;
	    numbad++;
	}
    }

    cout << "*** Packages to install ***" << endl;

    // otherwise, print what should be installed and what removed
    for(PkgDep::ResultList::const_iterator p=good.begin();p!=good.end();++p) {
	switch (verbose) {
	case 0: cout << "install " << p->name << endl;break;
	case 1: cout << "install " << p->name << "-" << p->edition << endl;break;
	default: cout << "install " << *p << endl;break;
	}
	numinst++;

    }
    cout << "*** Packages to remove ***" << endl;
    for(PkgDep::NameList::const_iterator q=to_remove.begin();q!=to_remove.end();++q) {
	switch (verbose) {
	case 0: cout << "remove " << *q << endl;break;
	default: cout << "remove " << *q << endl;break;
	}
	numrem++;
    } 
    cout << "***" << endl;
    cout << numbad << " bad, " << numinst << " to install, " << numrem << " to remove" << endl;
    cout << "Time consumed: " << t.getTimer() << endl;

    delete installed;
    delete available;
}

void consistent(vector<string>& argv)
{
    PkgSet *installed = NULL;
    PkgSet *available = NULL;

    installed = getInstalled();
    available = getAvailable();

    PkgDep engine( *installed, *available, alternative_default );
    engine.set_unresolvable_callback(unresolvable_callback);
    
    PkgDep::ErrorResultList bad;

    bool success = engine.consistent(bad);
    if(!success)
    {
	int numbad = 0;
	cout << " *** system inconsistent: " << endl << endl;
	for( PkgDep::ErrorResultList::const_iterator p = bad.begin();
	     p != bad.end(); ++p ) {
	    cerr << *p << endl;
	    numbad++;
	}
	cout << endl << " *** "<< numbad << " errors" << endl;
    }
    else
    {
	cout << "everything allright" << endl;
    }
}

int main( int argc, char *argv[] )
{
    char prompt[]="y2pm > ";

    char* buf = NULL;
    string inputstr;

    cout << "type help for help, ^D to exit" << endl << endl;

    buf = readline(prompt);
    while(buf)
    {
	vector<string> argv;
	unsigned i;
	if(RpmDb::tokenize(buf, ' ', argv) < 1)
	{
	    free(buf);
	    break;
	}
	for(i=0; func[i].name; i++)
	{
	    if(argv[0] == func[i].name)
		break;
	}
	if(func[i].func)
	{
	    func[i].func(argv);
	}
	else
	{
	    cout << "unknown function " << argv[0] << endl;
	}
	add_history(buf);
	free(buf);
	buf = readline(prompt);
    }

    return 0;
}

// vim:sw=4

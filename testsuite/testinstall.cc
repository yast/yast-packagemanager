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

#include <cstdlib> //atoi

// readline
#include <cstdio>
#include <readline/readline.h>
#include <readline/history.h>

#include <unistd.h>

#include <y2util/timeclass.h>
#include <y2util/Y2SLog.h>
#include <Y2PM.h>
#include <y2pm/InstallOrder.h>
#include <y2pm/RpmDb.h>
#include <y2pm/PkgDep.h>
#include <y2pm/PMPackage.h>

#include "show_pm.h"

#undef Y2SLOG
#define Y2SLOG "testinstall"

using namespace std;

static int _verbose = 0;

static string _instlog;

void install(vector<string>& argv);
void consistent(vector<string>& argv);
void help(vector<string>& argv);
void init(vector<string>& argv);
void query(vector<string>& argv);
void remove(vector<string>& argv);
void verbose(vector<string>& argv);
void debug(vector<string>& argv);
void rpminstall(vector<string>& argv);
void instlog(vector<string>& argv);

struct Funcs {
    const char* name;
    void (*func)(vector<string>&);
    const char* helptext;
};

static struct Funcs func[] = {
    { "install",    install,    "simulated install of a package" },
    { "rpminstall", rpminstall, "install rpm files" },
    { "consistent", consistent, "check consistency" },
    { "init",       init,       "initialize packagemanager" },
    { "query",      query,      "query packages" },
    { "remove",     remove,     "simulate remove packages" },
    { "help",       help,       "this screen" },
    { "verbose",    verbose,    "set verbosity level" },
    { "debug",      debug,      "switch on/off debug" },
    { "instlog",    instlog,    "set installation log file" },
    { NULL,         NULL,       NULL }
};

void usage(char **argv) {
	cout <<
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

void instlog(vector<string>& argv)
{
    if(argv.size()<2)
    {
	cout << "Usage: instlog <--clear|path to logfile>" << endl;
	return;
    }

    _instlog = argv[1];

    if(_instlog == "--clear")
    {
	_instlog.erase();
	cout << "disable install log";
    }
    else
    {
	cout << "set install log to " << _instlog;
    }

    bool ret = Y2PM::instTarget().setInstallationLogfile(_instlog);

    cout << (ret?" ok":" failed") << endl;
}

void verbose(vector<string>& argv)
{
    if(argv.size()<2)
    {
	cout << "need argument" << endl;
	return;
    }

    _verbose = atoi(argv[1].c_str());

    cout << "verbose level set to " << _verbose << endl;
}

void debug(vector<string>& argv)
{
    if(Y2SLog::dbg_enabled_bm)
    {
	cout << "debug disabled" << endl;
	Y2SLog::dbg_enabled_bm = false;
    }
    else
    {
	cout << "debug enabled" << endl;
	Y2SLog::dbg_enabled_bm = true;
    }
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
	    show_pmpackage (obj);
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

    if(rel.name() == "/bin/bash" || rel.name() == "/bin/sh")
    {
	const PkgSet set = solver->current_installed();
	p = set.lookup(PkgName("bash"));
	if(p==NULL)
	{
	    const PkgSet set = solver->current_available();
	    p = set.lookup(PkgName("bash"));
	}
	return PkgDep::UNRES_TAKETHIS;
    }
    if(rel.name() == "/usr/bin/perl")
    {
	const PkgSet set = solver->current_installed();
	p = set.lookup(PkgName("perl"));
	if(p==NULL)
	{
	    const PkgSet set = solver->current_available();
	    p = set.lookup(PkgName("perl"));
	}
	return PkgDep::UNRES_TAKETHIS;
    }
    if(rel.name()->operator[](0)=='/')
    {
	DBG << "ignoring file requirement " << rel.name() << endl;
	return PkgDep::UNRES_IGNORE;
    }

    return PkgDep::UNRES_FAIL;
}


static PMSolvable::PkgRelList_type& addprovidescallback(constPMSolvablePtr& ptr)
{
    PMSolvable::PkgRelList_type* list = new PMSolvable::PkgRelList_type();
    if(ptr->name() == "bash")
    {
	DBG << "add provides for bash" << endl;
	list->push_back(PkgRelation(PkgName("/bin/sh"), EQ, PkgEdition(PkgEdition::UNSPEC)));
	list->push_back(PkgRelation(PkgName("/bin/bash"), EQ, PkgEdition(PkgEdition::UNSPEC)));
    }
    else if(ptr->name() == "libpng-devel")
    {
	DBG << "add provides for" << ptr->name() << endl;
	list->push_back(PkgRelation(PkgName("/usr/include/png.h"), EQ, PkgEdition(PkgEdition::UNSPEC)));
    }

    return *list;
}

PkgSet* getInstalled()
{
    PkgSet* set = new PkgSet();
    set->setAdditionalProvidesCallback(addprovidescallback);

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
    PkgSet* set = new PkgSet();
    set->setAdditionalProvidesCallback(addprovidescallback);
    return set;
}

void install(vector<string>& argv)
{
    // build sets
    PkgSet empty;
    PkgSet candidates;
    candidates.setAdditionalProvidesCallback(addprovidescallback);

    PkgSet *installed = NULL;
    PkgSet *available = NULL;

    // swapped since available is not yet implemented
    installed = getAvailable();
    available = getInstalled();

    for (unsigned i=1; i < argv.size() ; i++) {
	if (available->lookup(PkgName(argv[i])) == NULL) {
		std::cout << "package " << argv[i] << " is not available.\n";
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
	    cout << *p << endl;
	    numbad++;
	}
    }

    cout << "*** Packages to install ***" << endl;

    // otherwise, print what should be installed and what removed
    for(PkgDep::ResultList::const_iterator p=good.begin();p!=good.end();++p) {
	switch (_verbose) {
	case 0: cout << "install " << p->name << endl;break;
	case 1: cout << "install " << p->name << "-" << p->edition << endl;break;
	default: cout << "install " << *p << endl;break;
	}
	numinst++;

    }
    cout << "*** Packages to remove ***" << endl;
    for(PkgDep::NameList::const_iterator q=to_remove.begin();q!=to_remove.end();++q) {
	switch (_verbose) {
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

    InstallOrder order(candidates);
    order.startrdfs();

    cout << "Installation order:" << endl;
    for(InstallOrder::SolvableList::const_iterator cit = order.getTopSorted().begin();
	cit != order.getTopSorted().end(); ++cit)
    {
	cout << (*cit)->name() << " ";
    }
    cout << endl;
}

void remove(vector<string>& argv)
{
//    PMPackageManager& manager = Y2PM::packageManager();
    vector<string>::iterator it=argv.begin();
    ++it; // first one is function name itself

    PkgDep::NameList list1;
    PkgDep::NameList list2;

    for(;it!=argv.end();++it)
    {
	list1.push_back(PkgName(*it));
	list2.push_back(PkgName(*it));
    }

    PkgSet *installed = NULL;
    PkgSet *available = NULL;

    installed = getInstalled();
    available = getAvailable();

    PkgDep engine( *installed, *available, alternative_default );
    engine.set_unresolvable_callback(unresolvable_callback);

    engine.remove(list1);

    cout << "Additionally removing ";
    for(PkgDep::NameList::iterator it = list1.begin(); it != list1.end(); ++it)
    {
	if(find(list2.begin(),list2.end(),*it) == list2.end())
	{
	    cout << *it << " ";
	}
    }
    cout << endl;
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
	    cout << *p << endl;
	    numbad++;
	}
	cout << endl << " *** "<< numbad << " errors" << endl;
    }
    else
    {
	cout << "everything allright" << endl;
    }
}

void progresscallback(int p, void* nix)
{
    cout << p << endl;
}
void rpminstall(vector<string>& argv)
{
    vector<string>::iterator it=argv.begin();
    list<string> pkgs;
    ++it; // first one is function name itself

    for(;it!=argv.end();++it)
    {
	pkgs.push_back(*it);
    }

    Y2PM::instTarget().setPackageInstallProgressCallback(progresscallback, NULL);

    if(Y2PM::instTarget().init("/", false) != InstTarget::Error::E_ok)
    {
	cout << "initialization failed" << endl;
    }
    else
	Y2PM::instTarget().installPackages(pkgs);
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
	if(RpmDb::tokenize(buf, ' ', 0, argv) < 1)
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

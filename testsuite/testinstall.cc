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

//getpw*
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

// readline
#include <cstdio>
#include <readline/readline.h>
#include <readline/history.h>

#include <y2util/timeclass.h>
#include <y2util/stringutil.h>
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
static int _maxremove = -1;
static bool _showtimes = false;
static bool _createbackups = false;

static string _instlog;
static string _rootdir = "/";

static bool _initialized = false;

static vector<string> nullvector;

static const char* statestr[] = { "= ", " -", " >", " +", "a-", "a>", "a+", " i", "  " };
    

//void installold(vector<string>& argv);
void install(vector<string>& argv);
void consistent(vector<string>& argv);
void help(vector<string>& argv);
void init(vector<string>& argv);
void show(vector<string>& argv);
void remove(vector<string>& argv);
void verbose(vector<string>& argv);
void debug(vector<string>& argv);
void rpminstall(vector<string>& argv);
void instlog(vector<string>& argv);
void setroot(vector<string>& argv);
void source(vector<string>& argv);
void deselect(vector<string>& argv);
void showpackage(vector<string>& argv);
//void setmaxremove(vector<string>& argv);
void solve(vector<string>& argv);
void showtimes(vector<string>& argv);
void createbackups(vector<string>& argv);
void rebuilddb(vector<string>& argv);
void du(vector<string>& argv);
void showselection(vector<string>& argv);
void installselection(vector<string>& argv);
void order(vector<string>& argv);

struct Funcs {
    const char* name;
    void (*func)(vector<string>&);
    bool need_init;
    const char* helptext;
};

static struct Funcs func[] = {
//    { "installold",    installold,    "simulated install of a package, direct PkgDep" },
    { "install",	install,	1,	"select packages for installation" },
    { "rpminstall",	rpminstall,	1,	"install rpm files" },
    { "consistent",	consistent,	1,	"check consistency" },
    { "init",		init,		1,	"initialize packagemanager" },
    { "show",		show,		1,	"show package info" },
    { "remove",		remove,		1,	"select package for removal" },
    { "help",		help,		0,	"this screen" },
    { "verbose",	verbose,	0,	"set verbosity level" },
    { "debug",		debug,		0,	"switch on/off debug" },
    { "instlog",	instlog,	1,	"set installation log file" },
    { "setroot",	setroot,	0,	"set root directory for operation" },
    { "source",		source,		1,	"scan media for inst sources" },
    { "deselect",	deselect,	1,	"deselect packages marked for installation" },
//    { "setmaxremove",	setmaxremove,	1,	"set maximum number of packages that will be removed on upgrade" },
    { "solve",		solve,		1,	"solve" },
    { "showtimes",	showtimes,	0,	"showtimes" },
    { "createbackups",	createbackups,	0,	"createbackups" },
    { "rebuilddb",	rebuilddb,	1,	"rebuild rpm db" },
    { "du",		du,		1,	"display disk space forecast" },
    
    { "showselection",	showselection,	1,
	"show state of selection (all if none specified. Not installed will be suppressed if verbose=0)" },
    { "showpackage",	showpackage,	1,
	"show state of package (all if none specified. Not installed will be suppressed if verbose=0)" },

    { "installselection",	installselection,	1,	"mark selection for installation" },
    
    { "order",		order,		1,	"compute installation order" },

    { NULL,		NULL,		0,	NULL }
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

void progresscallback(int p, void* nix)
{
    cout << p << "%" << endl;
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

void setroot(vector<string>& argv)
{
    if(_initialized)
    {
	cout << "target already initialized, can't change root" << endl;
	return;
    }

    if(argv.size()<2)
    {
	cout << "need argument" << endl;
	return;
    }

    _rootdir = argv[1].c_str();

    cout << "root dir set to " << _rootdir << endl;
}


void verbose(vector<string>& argv)
{
    if(argv.size()<2)
    {
	cout << "verbose level: " << _verbose << endl;
	return;
    }

    _verbose = atoi(argv[1].c_str());

    cout << "verbose level set to " << _verbose << endl;
}

void showtimes(vector<string>& argv)
{
    _showtimes = _showtimes?false:true;

    cout << "show times " << (_showtimes?"enabled":"disabled") << endl;
}

void createbackups(vector<string>& argv)
{
    _createbackups = _createbackups?false:true;
    if(_initialized)
	Y2PM::instTarget().createPackageBackups(_createbackups);

    cout << "create backups " << (_createbackups?"enabled":"disabled") << endl;
}

void du(vector<string>& argv)
{
    FSize total, used;
    cout << Y2PM::packageManager().SpaceDifference() << endl;
    Y2PM::instTarget().SpaceTotal(total,used);
    cout << total << " " << used << endl;
}

void rebuilddb(vector<string>& argv)
{
    cout << "rebuilding database ... " << endl;

    Y2PM::instTarget().setRebuildDBProgressCallback(progresscallback, NULL);
    PMError err = Y2PM::instTarget().bringIntoCleanState();
    if(err != PMError::E_ok)
    {
	cout << "failed: " << err << endl;
    }
    else
	cout << "done" << endl;
}

/*
void setmaxremove(vector<string>& argv)
{
    if(argv.size()<2)
    {
	cout << "current max remove: "
	    << (_maxremove<0?string("default"):stringutil::form("%d", _maxremove)) << endl;
	return;
    }
    int tmp = atoi(argv[1].c_str());
    if(tmp < 0)
    {
	cout  << "maxremove must be positive" << endl;
	return;
    }

    _maxremove = tmp;
    Y2PM::packageManager().setMaxRemoveThreshold(_maxremove);

    cout << "maxremove set to " << _maxremove << endl;
}
*/
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

void show(vector<string>& argv)
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
	    PMPackagePtr obj = static_cast<PMPackagePtr>(sel->theObject());
	    show_pmpackage (obj);
	}
    }
}

static InstSrcManager::ISrcIdList _isrclist;

void source(vector<string>& argv)
{
    if(argv.size()<2)
    {
	cout << "Usage: source <url|--enable nr|--forget>" << endl;
	return;
    }

    unsigned parampos = 1;
    string param = argv[parampos++];
    if(param == "--forget")
    {
	_isrclist.erase(_isrclist.begin(), _isrclist.end());
	return;
    }
    else if(param == "--enable" || param == "-e")
    {
	if (parampos >= argv.size())
	{
	    cout << "must specify number" << endl;
	    return;
	}
	while(parampos < argv.size())
	{
	    int num = atoi(argv[parampos++].c_str());
	    if(num < 0 || static_cast<unsigned>(num) >= _isrclist.size())
	    {
		cout << "invalid number" << endl;
		continue;
	    }

	    int count = 0;
	    for(InstSrcManager::ISrcIdList::iterator it = _isrclist.begin();
		it != _isrclist.end(); ++it, count++)
	    {
		if(count != num)
		    continue;
		    
		cout << "enabling source ... " << endl;
		PMError err = Y2PM::instSrcManager().enableSource(*it);
		if( err != PMError::E_ok)
		{
		    cout << "Error: " << err << endl;
		}
		else
		{
		    cout << "ok" << endl;
		}
	    }
	}
    }
    else
    {
	if(!_isrclist.empty())
	{
	    _isrclist.erase(_isrclist.begin(), _isrclist.end());
	}

	Url url(param);

	if(!url.isValid())
	{
	    cout << "invalid url" << endl;
	    return;
	}

	cout << "scanning " << url << endl;
	PMError err = Y2PM::instSrcManager().scanMedia(_isrclist, url);

	if(err != PMError::E_ok)
	{
	    cout << "failed to detect source on " << url << endl;
	    cout << err << endl;
	    return;
	}

	cout << "Detected media: " << endl;
	unsigned count = 0;
	for(InstSrcManager::ISrcIdList::iterator it = _isrclist.begin();
	    it != _isrclist.end(); ++it, count++)
	{
	    cout << count << ": " << *it << endl;
	}
    }
}

void init(vector<string>& argv)
{
    if(_initialized)
    {
	cout << "already initialized" << endl;
	return;
    }
    _initialized = true;

    cout << "initializing ... " << endl;

    Y2PM::packageManager();
    Y2PM::selectionManager();
    PMError dbstat = Y2PM::instTarget().init(_rootdir, true);
    if( dbstat != InstTargetError::E_ok )
    {
	cout << "error initializing target: " << dbstat << endl;
    }
    else
    {
	Y2PM::instTarget().createPackageBackups(_createbackups);
	Y2PM::packageManager().poolSetInstalled( Y2PM::instTarget().getPackages() );
	Y2PM::selectionManager().poolSetInstalled( Y2PM::instTarget().getSelections() );
    }
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

int printgoodlist(PkgDep::ResultList& good)
{
    int numinst = 0;
    cout << "*** Packages to install ***" << endl;

     // otherwise, print what should be installed and what removed
    for(PkgDep::ResultList::const_iterator p=good.begin();p!=good.end();++p)
    {
	switch (_verbose) {
	    case 0:
		if(p!=good.begin())
		    cout << " ";
		cout <<  p->name;
		break;
	    case 1:
		cout << "install " << p->name << "-" << p->edition << endl;
		break;
	    default:
		cout << "install " << *p << endl;
		break;
	}
	numinst++;

    }
    if(!_verbose) cout << endl;

    return numinst;
}

int printremovelist(PkgDep::SolvableList& to_remove)
{
    int numrem = 0;

    cout << "*** Packages to remove ***" << endl;
    for(PkgDep::SolvableList::const_iterator q=to_remove.begin();q!=to_remove.end();++q)
    {
	switch (_verbose)
	{
	    case 0:
		if(q!=to_remove.begin())
		    cout << " ";
		cout << (*q)->name();
		break;
	    case 1:
		cout << (*q)->name() << "-" << (*q)->edition() << endl;
		break;
	    default:
		cout << *q << endl;
		break;
	}
	numrem++;
    }
    if(!_verbose) cout << endl;

    return numrem;
}

int printbadlist(PkgDep::ErrorResultList& bad)
{
    int numbad = 0;
    
    if(bad.empty()) return 0;
    
    cout << "*** Conflicts ***" << endl;
    for( PkgDep::ErrorResultList::const_iterator p = bad.begin();
	 p != bad.end(); ++p ) {
	cout << *p << endl;
	numbad++;
    }
    return numbad;
}

static void install_internal(PMManager& manager, vector<string>& argv)
{
    for (unsigned i=1; i < argv.size() ; i++) {
	string pkg = stringutil::trim(argv[i]);

	if(pkg.empty()) continue;
	
	PMSelectablePtr selp = manager.getItem(pkg);
	if(!selp || !selp->has_candidate())
	{
	    std::cout << "package " << pkg << " is not available.\n";
	    continue;
	}
	PMSelectable::UI_Status s = PMSelectable::S_Install;

	if(selp->has_installed())
	{
	    s = PMSelectable::S_Update;
	}
	if(!selp->set_status(s))
	{
	    cout << stringutil::form("coult not mark %s for %s", pkg.c_str(),
		(s==PMSelectable::S_Install?"installation":"update")) << endl;
	}
    }
}

void install(vector<string>& argv)
{
    install_internal(Y2PM::packageManager(), argv);
}

void installselection(vector<string>& argv)
{
    install_internal(Y2PM::selectionManager(), argv);
}

void solve(vector<string>& argv)
{
    int numinst=0,numbad=0;
    bool success = false;

    PkgDep::ResultList good;
    PkgDep::ErrorResultList bad;

    success = Y2PM::packageManager().solveInstall(good, bad);


    numbad = printbadlist(bad);
    numinst = printgoodlist(good);

    cout << "***" << endl;
    cout << numbad << " bad, " << numinst << " to install" << endl;
}


void order(vector<string>& argv)
{
    PkgSet toinstall;
    for (PMManager::PMSelectableVec::iterator it = Y2PM::packageManager().begin();
	it != Y2PM::packageManager().end(); ++it )
    {
	if((*it)->to_install())
	{
	    toinstall.add((*it)->candidateObj());
	}
    }

    InstallOrder order(toinstall);
    order.startrdfs();

    cout << "Installation order:" << endl;
    for(InstallOrder::SolvableList::const_iterator cit = order.getTopSorted().begin();
	cit != order.getTopSorted().end(); ++cit)
    {
	cout << (*cit)->name() << " ";
    }
    cout << endl;

}
#if 0
void installold(vector<string>& argv)
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
#endif

void removephi(vector<string>& argv)
{
//    PMPackageManager& manager = Y2PM::packageManager();
    vector<string>::iterator it=argv.begin();
    ++it; // first one is function name itself

    PkgDep::SolvableList list1;
    PkgDep::SolvableList list2;

    PkgSet *installed = NULL;
    PkgSet *available = NULL;

    installed = getInstalled();
    available = getAvailable();

    for(;it!=argv.end();++it)
    {
	PMSolvablePtr p;
	if((p = installed->lookup(PkgName(*it))) != NULL)
	{
	    list1.push_back(p);
	    list2.push_back(p);
	}
    }


    PkgDep engine( *installed, *available, alternative_default );
    engine.set_unresolvable_callback(unresolvable_callback);

    engine.remove(list1);

    cout << "Additionally removing ";
    for(PkgDep::SolvableList::iterator it = list1.begin(); it != list1.end(); ++it)
    {
	if(find(list2.begin(),list2.end(),*it) == list2.end())
	{
	    cout << (*it)->name() << " ";
	}
    }
    cout << endl;
}


void consistent(vector<string>& argv)
{
    int numbad=0;
    bool success = false;

    PkgDep::ErrorResultList bad;

    success = Y2PM::packageManager().solveConsistent(bad);

    numbad = printbadlist(bad);

    cout << "***" << endl;
    cout << numbad << " bad" << endl;
}

/*
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
*/

void deselect(vector<string>& argv)
{
    for (unsigned i=1; i < argv.size() ; i++)
    {
	string pkg = stringutil::trim(argv[i]);

	if(pkg.empty()) continue;
	
	PMSelectablePtr selp = Y2PM::packageManager().getItem(pkg);
	if(!selp)
	{
	    std::cout << "package " << pkg << " is not available.\n";
	    continue;
	}

	selp->setNothingSelected();
    }
}

void rpminstall(vector<string>& argv)
{
    vector<string>::iterator it=argv.begin();
    list<Pathname> pkgs;
    ++it; // first one is function name itself

    for(;it!=argv.end();++it)
    {
	pkgs.push_back (Pathname (*it));
    }

    Y2PM::instTarget().setPackageInstallProgressCallback(progresscallback, NULL);

    if(Y2PM::instTarget().init(_rootdir, false) != InstTarget::Error::E_ok)
    {
	cout << "initialization failed" << endl;
    }
    else
	Y2PM::instTarget().installPackages(pkgs);
}
/*
void upgrade(vector<string>& argv)
{
    PkgDep::ResultList good;
    PkgDep::ErrorResultList bad;
    PkgDep::SolvableList to_remove;

    int numinst=0,numrem=0,numbad=0;
    bool success = false;

    for (unsigned i=1; i < argv.size() ; i++) {
	string pkg = stringutil::trim(argv[i]);

	if(pkg.empty()) continue;
	
	PMSelectablePtr selp = Y2PM::packageManager().getItem(pkg);
	if(!selp || !selp->has_candidate())
	{
	    std::cout << "package " << pkg << " is not available.\n";
	    continue;
	}
	PMSelectable::UI_Status s = PMSelectable::S_Install;

	if(selp->has_installed())
	{
	    s = PMSelectable::S_Update;
	}
	if(!selp->set_status(s))
	{
	    cout << stringutil::form("coult not mark %s for %s", pkg.c_str(),
		(s==PMSelectable::S_Install?"installation":"update")) << endl;
	}
    }

    success = Y2PM::packageManager().solveUpgrade(good, bad, to_remove);

    numbad = printbadlist(bad);
    numinst = printgoodlist(good);
    numrem = printremovelist(to_remove);

    cout << "***" << endl;
    cout << numbad << " bad, " << numinst << " to install, " << numrem << " to remove" << endl;
    if(!success)
    {
	cout << "*** upgrade failed, manual intervention required to solve conflicts ***" << endl;
    }
}
*/
static void showstate_internal(PMManager& manager, vector<string>& argv)
{

    PMManager::PMSelectableVec selectables;
    PMManager::PMSelectableVec::const_iterator begin, end;
    if(argv.size()>1)
    {
	for (unsigned i=1; i < argv.size() ; i++) {
	    string pkg = stringutil::trim(argv[i]);

	    if(pkg.empty()) continue;
	    
	    PMSelectablePtr selp = manager.getItem(pkg);
	    if(!selp)
	    {
		std::cout << "package " << pkg << " is not available.\n";
	    }
	    else
	    {
		selectables.insert(selp);
	    }
	}
	begin = selectables.begin();
	end = selectables.end();
    }
    else
    {
	begin = manager.begin();
	end = manager.end();
    }

    for(PMManager::PMSelectableVec::const_iterator cit = begin;
	cit != end; ++cit)
    {
	PMSelectable::UI_Status s = (*cit)->status();
	if(!_verbose && s == PMSelectable::S_NoInst)
	    continue;
	switch(s)
	{
	    case PMSelectable::S_Taboo:
	    case PMSelectable::S_Del:
	    case PMSelectable::S_Update:
	    case PMSelectable::S_Install:
	    case PMSelectable::S_AutoDel:
	    case PMSelectable::S_AutoUpdate:
	    case PMSelectable::S_AutoInstall:
	    case PMSelectable::S_KeepInstalled:
	    case PMSelectable::S_NoInst:
	    cout << statestr[s] << "   " << (*cit)->name();
	    if(_verbose > 1)
	    {
		if((*cit)->has_installed())
		{
		    cout << " (" << (*cit)->installedObj()->edition();
			if((*cit)->has_candidate())
			{
			    if((*cit)->to_install())
				cout << " -> ";
			    else
				cout << " / ";

			    cout << (*cit)->candidateObj()->edition();
			}
			cout << ")";
		}
	    }
	    cout << endl;
	}
    }
}

void showpackage(vector<string>& argv)
{
    showstate_internal(Y2PM::packageManager(),argv);
}

void showselection(vector<string>& argv)
{
    showstate_internal(Y2PM::selectionManager(),argv);
}

void remove(vector<string>& argv)
{
    for (unsigned i=1; i < argv.size() ; i++)
    {
	string pkg = stringutil::trim(argv[i]);

	if(pkg.empty()) continue;
	
	PMSelectablePtr selp = Y2PM::packageManager().getItem(pkg);
	if(!selp)
	{
	    std::cout << "package " << pkg << " is not available.\n";
	    continue;
	}
	
	if(!selp->set_status(PMSelectable::S_Del))
	{
	    cout << stringutil::form("coult not mark %s for deletion", pkg.c_str()) << endl;
	}
    }
}
/*
void solve(vector<string>& argv)
{
    PkgDep::ResultList good;
    PkgDep::ErrorResultList bad;
    PkgDep::SolvableList to_remove;

    int numinst=0,numrem=0,numbad=0;
    bool success = false;

//    success = Y2PM::packageManager().solveEverythingRight(good, bad, to_remove);

    cout << "doesnt work" << endl;

    numbad = printbadlist(bad);
    numinst = printgoodlist(good);
    numrem = printremovelist(to_remove);

    cout << "***" << endl;
    cout << numbad << " bad, " << numinst << " to install, " << numrem << " to remove" << endl;
    if(!success)
    {
	cout << "*** selection broken, manual intervention required to solve conflicts ***" << endl;
    }
}
*/
int main( int argc, char *argv[] )
{
    char prompt[]="y2pm > ";

    char* buf = NULL;
    string inputstr;
    string historyfile;

    cout << "type help for help, ^D to exit" << endl << endl;


    {
	struct passwd* pwd = getpwuid(getuid());
	if(pwd)
	{
	    char* home = pwd->pw_dir;
	    if(home)
	    {
		::using_history();
		historyfile=home;
		historyfile+="/";
		historyfile+=".y2pmshell_history";
		::read_history(historyfile.c_str());
	    }
	}
    }

    buf = readline(prompt);
    while(buf)
    {
	vector<string> cmds;

	inputstr = buf?buf:"";
	inputstr = stringutil::trim(inputstr);
	free(buf);
	buf = NULL;

	if(inputstr.empty()) goto readnext;

	if(RpmDb::tokenize(inputstr, ';', 0, cmds) < 1)
	{
	    cout << "invalid input" << endl;
	    goto readnext;
	}

	for(vector<string>::iterator vit = cmds.begin(); vit != cmds.end(); ++vit)
	{
	    vector<string> argv;
	    unsigned i;
	    string cmd = stringutil::trim(*vit);
	    if(cmd.empty())
		continue;
	    if(RpmDb::tokenize(cmd, ' ', 0, argv) < 1)
	    {
		cout << "invalid input" << endl;
		continue;
	    }
	    for(i=0; func[i].name; i++)
	    {
		if(argv[0] == func[i].name)
		    break;
	    }
	    if(func[i].func)
	    {
		TimeClass t;
		if(func[i].need_init && !_initialized && func[i].func != init)
		{
		    if(_showtimes) t.startTimer();
		    init(nullvector);
		    if(_showtimes) t.stopTimer();
		    if(_showtimes) cout << "time: " << t.getTimer() << endl;
		}

		if(_showtimes) t.startTimer();
		func[i].func(argv);
		if(_showtimes) t.stopTimer();
		if(_showtimes) cout << "time: " << t.getTimer() << endl;
	    }
	    else
	    {
		cout << "unknown function " << argv[0] << endl;
	    }
	}

	add_history(inputstr.c_str());
    readnext:
	buf = readline(prompt);
    }

    cout << endl;

    if(!historyfile.empty())
    {
	int ret = 0;
	if((ret = ::write_history(historyfile.c_str())))
	{
	    cout << "writing " << historyfile  << " failed: "<<
		strerror(ret) << endl;
	}
	history_truncate_file(historyfile.c_str(),500);
    }

    return 0;
}

// vim:sw=4

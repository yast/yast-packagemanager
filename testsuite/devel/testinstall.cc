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
#include <malloc.h>

// readline
#include <cstdio>
#include <readline/readline.h>
#include <readline/history.h>
#include <cctype> // get rid of readline defining isxdigit et al.

#include <y2util/timeclass.h>
#include <y2util/stringutil.h>
#include <y2util/Y2SLog.h>
#include <Y2PM.h>
#include <y2pm/InstallOrder.h>
#include <y2pm/RpmDb.h>
#include <y2pm/PkgDep.h>
#include <y2pm/PMPackage.h>
#include <y2pm/InstTarget.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PMSelectionManager.h>
#include <y2pm/InstSrcDescr.h>

#include <asm/msr.h>
#include <stdint.h>

#include "show_pm.h"

#undef Y2SLOG
#define Y2SLOG "testinstall"

using namespace std;

static bool _showtimes = false;
static bool _createbackups = false;

static string _instlog;
static string _rootdir = "/";

static bool _initialized = false;

static bool _keep_running = true;

static vector<string> nullvector;

static const char* statestr[] = { "@i", "--", " -", " >", " +", "a-", "a>", "a+", " i", "  " };

void install(vector<string>& argv);
void consistent(vector<string>& argv);
void help(vector<string>& argv);
void init(vector<string>& argv);
void autoenablesources(vector<string>& argv);
void show(vector<string>& argv);
void remove(vector<string>& argv);
void rpminstall(vector<string>& argv);
void instlog(vector<string>& argv);
void source(vector<string>& argv);
void showsources(vector<string>& argv);
void deselect(vector<string>& argv);
void showpackage(vector<string>& argv);
void solve(vector<string>& argv);
void createbackups(vector<string>& argv);
void rebuilddb(vector<string>& argv);
void du(vector<string>& argv);
void showselection(vector<string>& argv);
void setsel(vector<string>& argv);
void delsel(vector<string>& argv);
void solvesel(vector<string>& argv);
void setappl(vector<string>& argv);
void order(vector<string>& argv);
void upgrade(vector<string>& argv);
void commit(vector<string>& argv);
void mem(vector<string>& argv);
void varset(vector<string>& argv);
void varunset(vector<string>& argv);

void testset(vector<string>& argv);

void cdattach(vector<string>& argv);

struct Funcs {
    const char* name;
    void (*func)(vector<string>&);
    short flags;
    const char* helptext;
};

class Variable
{
    public:
	enum Type { t_string=0, t_bool, t_int };

	// return NULL on sucess, error message otherwise
	typedef const char* (*ValidateFunc)(const Variable& v);

    private:
	Type _type;
	union {
	    char* _strval;
	    bool _boolval;
	    long _intval;
	};

    public:
	bool _can_unset;
	ValidateFunc _valid;
	
    public:
	Variable();
	Variable(const char* value, bool can_unset = true, ValidateFunc valid = NULL);
	Variable( const Variable& );
	~Variable();
	bool isBool() const;
	bool isInt() const;
	bool isString() const;
	long getInt() const;
	bool getBool() const;
	const char* getString() const;
	bool canUnset() const;
	void dumpOn(ostream& os) const;
	const char* typestr() const;

	// return NULL on sucess, error message otherwise
	static const char* assign(Variable& v, const char* value);

	// does not call validate function
	Variable& operator=( const Variable& );

};

ostream& operator<<(ostream& os, const Variable& v)
{
    v.dumpOn(os);
    return os;
}

typedef map<string,Variable> VariableMap;
static VariableMap variables;
static map<string,string> vardesc;

static inline void printvar(ostream& os, string name, const Variable& v)
{
    os << name /*<< "[" << v.typestr() << "]" */ << " = " << v;
    if(vardesc.count(name))
	os << "  \t" << vardesc[name];
    os << endl;
}

void varset(vector<string>& argv)
{
    if(argv.size()<2)
    {
	for(VariableMap::iterator it = variables.begin();
		it != variables.end(); ++it)
	{
	    printvar(cout,it->first,it->second);
	}
	return;
    }
    else if(argv.size()<3)
    {
	if(!variables.count(argv[1]))
	{
	    cout << argv[1] << " not set" << endl;
	    return;
	}
	else
	{
	    Variable& v = variables[argv[1]];
	    printvar(cout,argv[1],v);
	}
    }
    else
    {
	const char* msg = Variable::assign(variables[argv[1]], argv[2].c_str());
	if(msg)
	    cout << "unable to set requested value, " << msg << endl;
    }
}

void varunset(vector<string>& argv)
{
    if(argv.size()<2)
    {
	cout << "unset <variable name>" << endl;
	return;
    }
    
    if(!variables.count(argv[1]))
    {
	    cout << argv[1] << " not set" << endl;
	    return;
    }

    if(!variables[argv[1]].canUnset())
    {
	cout << argv[1] << " is protected" << endl;
	return;
    }
	
    variables.erase(argv[1]);
}

Variable::Variable()
    : _type(t_bool), _boolval(false), _can_unset(true), _valid(NULL)
{
//    cout << __PRETTY_FUNCTION__ << ':' << __LINE__ << endl;
}

Variable::Variable(const Variable& v)
{
    _type = v._type;
    _valid = v._valid;
    _can_unset = v._can_unset;
    switch(_type)
    {
	case t_bool: _boolval = v._boolval; break;
	case t_int: _intval = v._intval; break;
	case t_string: _strval = strdup(v._strval); break;
    }
}

Variable& Variable::operator=( const Variable& v )
{
    _type = v._type;
    _valid = v._valid;
    _can_unset = v._can_unset;
    switch(_type)
    {
	case t_bool: _boolval = v._boolval; break;
	case t_int: _intval = v._intval; break;
	case t_string: _strval = strdup(v._strval); break;
    }

    return *this;
}

const char* Variable::assign(Variable& v, const char* value)
{
    Variable tmp(value,v._can_unset,v._valid);
    const char* msg = (v._valid?v._valid(tmp):NULL);
    if(msg)
	return msg;

    v=tmp;

    return NULL;
}

Variable::Variable(const char* value, bool can_unset, ValidateFunc valid)
{
    long val = 0;
    char* endptr;
    _can_unset = can_unset;
    _valid = valid;
    val = strtol(value,&endptr, 10);
    if(value != endptr)
    {
	if(val == 0 || val == 1)
	{
	    _type = t_bool;
	    _boolval = val;
	}
	else
	{
	    _type = t_int;
	    _intval = val;
	}
    }
    else if(!strcasecmp(value,"true") || !strcasecmp(value,"yes"))
    {
	_type = t_bool;
	_boolval = true;
    }
    else if(!strcasecmp(value,"false") || !strcasecmp(value,"no"))
    {
	_type = t_bool;
	_boolval = false;
    }
    else
    {
	_type = t_string;
	_strval = strdup(value);
    }
}

Variable::~Variable()
{
    if( _type == t_string && _strval )
    {
	free(_strval);
	_strval = NULL;
    }
}

void Variable::dumpOn(ostream& os) const
{
    switch(_type)
    {
	case t_bool: os << (_boolval?"true":"false") ; break;
	case t_int: os << _intval; break;
	case t_string: os << '"' << _strval << '"'; break;
    }
}

const char* Variable::typestr() const
{
    switch(_type)
    {
	case t_bool: return "bool";
	case t_int: return "int";
	case t_string: return "string";
    }
    return NULL;
}

bool Variable::isBool() const
{
    return ( _type == t_bool );
}

bool Variable::isInt() const
{
    return ( _type == t_int );
}

bool Variable::isString() const
{
    return ( _type == t_string );
}

long Variable::getInt() const
{
    switch(_type)
    {
	case t_bool:
	    return _boolval;
	case t_int:
	    return _intval;
	case t_string:
	    return 0;
    }
    return 0;
}

bool Variable::getBool() const
{
    switch(_type)
    {
	case t_bool:
	    return _boolval;
	case t_int:
	    return ( _intval != 0 );
	case t_string:
	    return 0;
    }
    return false;
}

const char* Variable::getString() const
{
    switch(_type)
    {
	case t_bool:
	    return (_boolval?"true":"false");
	case t_int:
	    return ""; //XXX
	case t_string:
	    return _strval;
    }
    return NULL;
}

bool Variable::canUnset() const
{
    return _can_unset;
}

static struct Funcs func[] = {
    // flags: 1 = need init, 2 = hidden, 4 = advanced
    { "install",	install,	1,	"select packages for installation" },
    { "rpminstall",	rpminstall,	3,	"install rpm files" },
    { "consistent",	consistent,	3,	"check consistency" },
    { "set",		varset,		0,	"set or show variable" },
    { "unset",		varunset,	0,	"unset variable" },
    { "init",		init,		1,	"initialize packagemanager" },
    { "show",		show,		1,	"show package info" },
    { "remove",		remove,		1,	"select package for removal" },
    { "instlog",	instlog,	3,	"set installation log file" },
    { "source",		source,		1,	"scan media for inst sources" },
    { "showsources",	showsources,	1,	"show known sources" },
    { "autoenablesources", autoenablesources,	1,	"enable all sources" },
    { "deselect",	deselect,	1,	"deselect packages marked for installation" },
    { "solve",		solve,		1,	"solve dependencies" },
    { "createbackups",	createbackups,	2,	"createbackups" },
    { "rebuilddb",	rebuilddb,	1,	"rebuild rpm db" },
    { "du",		du,		1,	"display disk space forecast" },
    { "selstate",	showselection,	5, 	"show state of selection (all if none specified. -a to show also not installed" },
    { "pkgstate",	showpackage,	1,	 "show state of package (all if none specified. -a to show also not installed" },
    { "setappl",	setappl,	5,	"set package to installed like a selection would do" },
    { "order",		order,		3,	"compute installation order" },
    { "upgrade",	upgrade,	1,	"compute upgrade" },
    { "commit",		commit,		1,	"commit changes to and actually perform installation" },
    { "setsel",		setsel,		5,	"mark selection for installation, need to call solvesel" },
    { "delsel",		delsel,		5,	"delete selection from installation, need to call solvesel" },
    { "solvesel",	solvesel,	5,	"solve selection dependencies" },
    { "cdattach",	cdattach,	2,	"cdattach" },
    { "mem",		mem,		0,	"memory statistics" },
    { "testset",	testset,	2,	"test memory consumption of PkgSet" },
    { "help",		help,		0,	"this screen" },

    { NULL,		NULL,		0,	NULL }
};

static ostream& operator<<( ostream& os, const struct mallinfo& i )
{
    os << "Memory from system: " << (i.arena >> 10) << "k, used: " << (i.uordblks >> 10) << "k";
    return os;
}

static int lastprogress = 0;
void progresscallback(int p, void* nix)
{
    if(p<0) p = 0;
    if(p>100) p = 100;

    if(p==0) lastprogress = 0;

    int num = (long)60*p/100;
    for(int i=0; i < num-lastprogress; i++)
    {
	cout << "%";
    }
    cout.flush();
    lastprogress = num;

    if(p == 100) cout << endl;
}

void providestartcallback(const std::string& name, const FSize& s, bool, void*)
{
    cout << stringutil::form("Downloading %s (%s)",name.c_str(), s.asString().c_str()) << endl;
}

void donecallback(PMError error, const std::string& reason, void*)
{
    if(error)
	cout << error << ": " << reason << endl;
    else
	cout << "ok" << endl;
}

bool pkgstartcallback(const std::string& name, const std::string& summary, const FSize& size, bool is_delete, void*)
{
    if(is_delete)
    {
	cout << stringutil::form("Deleting %s",name.c_str()) << endl;
    }
    else
    {
	cout << stringutil::form("Installing %s (%s) - %s ",name.c_str(),size.asString().c_str(),summary.c_str()) << endl;
    }
    return true;
}

static const char* setdebug(const Variable& v)
{
    if(!v.isBool()) return "must be bool";

    if(v.getBool())
    {
	cout << "debug enabled" << endl;
	Y2SLog::dbg_enabled_bm = true;
    }
    else
    {
	cout << "debug disabled" << endl;
	Y2SLog::dbg_enabled_bm = false;
    }

    return NULL;
}

static const char* setroot(const Variable& v)
{
    if(_initialized)
    {
	return "target already initialized, can't change root";
    }

    if(!v.isString()) return "must be String";

    PathInfo p(v.getString());
    if(!p.isDir()) return "not a directory";

    _rootdir = v.getString();

    cout << "root dir set to " << _rootdir << endl;

    return NULL;
}

static const char* timestat(const Variable& v)
{
    if(!v.isBool()) return "must be bool";

    _showtimes = v.getBool();

    cout << "time statistics " << (_showtimes?"enabled":"disabled") << endl;

    return NULL;
}


void init_variables()
{
    variables["debug"] = Variable("0",false,setdebug);
    vardesc["debug"] = "whether to enable debug messages";
    variables["verbose"] = Variable("0",false);
    vardesc["verbose"] = "certain commands display more info if >0, >1 etc.";
    variables["root"] = Variable("/",false,setroot);
    vardesc["root"] = "set root directory for operation";
    variables["autosource"] = Variable("1",false);
    vardesc["autosource"] = "automatically enable installation sources on init";
    variables["timestat"] = Variable("0",false,timestat);
    vardesc["timestat"] = "measure how long commands take";
    variables["quitoncommit"] = Variable("1",false);
    vardesc["quitoncommit"] = "quit after packages are commited";
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

void createbackups(vector<string>& argv)
{
    _createbackups = _createbackups?false:true;
    if(_initialized)
	Y2PM::instTarget().createPackageBackups(_createbackups);

    cout << "create backups " << (_createbackups?"enabled":"disabled") << endl;
}

void du(vector<string>& argv)
{
    cout << Y2PM::packageManager().updateDu() << endl;
}

void rebuilddb(vector<string>& argv)
{
    cout << "rebuilding database ... " << endl;

    PMError err = Y2PM::instTarget().bringIntoCleanState();
    if(err != PMError::E_ok)
    {
	cout << "failed: " << err << endl;
    }
    else
	cout << "done" << endl;
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

/********************/
static InstSrcManager::ISrcIdList _isrclist;

void showsources(vector<string>& argv)
{
    _isrclist.erase(_isrclist.begin(), _isrclist.end());
    Y2PM::instSrcManager().getSources(_isrclist);

    cout << "Known sources:" << endl;
    unsigned count = 0;
    for(InstSrcManager::ISrcIdList::iterator it = _isrclist.begin();
	it != _isrclist.end(); ++it, count++)
    {
	constInstSrcDescrPtr descr = (*it)->descr();
	cout << count << ": ";
	cout << descr->content_label() << " (" << descr->url() << ")" << endl;
    }
}

void autoenablesources(vector<string>& argv)
{
    InstSrcManager::ISrcIdList isrclist;

    cout << "read list of available packages ..." << endl;

    Y2PM::instSrcManager().getSources(isrclist);

    for(InstSrcManager::ISrcIdList::iterator it = isrclist.begin();
	it != isrclist.end(); ++it)
    {
	Y2PM::instSrcManager().enableSource(*it);
    }
}

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

    cout << "reading installed packages ..." << endl;

    Y2PM::packageManager();
    Y2PM::selectionManager();
    PMError dbstat = Y2PM::instTarget().init(_rootdir, true);
    if( dbstat != InstTargetError::E_ok )
    {
	cout << "error initializing target: " << dbstat << endl;
	_initialized = false;
	return;
    }


    Y2PM::instTarget().createPackageBackups(_createbackups);
    Y2PM::packageManager().poolSetInstalled( Y2PM::instTarget().getPackages() );
    Y2PM::selectionManager().poolSetInstalled( Y2PM::instTarget().getSelections() );

    Y2PM::noAutoInstSrcManager();

    Y2PM::setRebuildDBProgressCallback(progresscallback, NULL);
    Y2PM::setProvideStartCallback(providestartcallback, NULL);
    Y2PM::setProvideProgressCallback(progresscallback, NULL);
    Y2PM::setProvideDoneCallback(donecallback, NULL);
    Y2PM::setPackageStartCallback(pkgstartcallback, NULL);
    Y2PM::setPackageProgressCallback(progresscallback, NULL);
    Y2PM::setPackageDoneCallback(donecallback, NULL);

    if(variables["autosource"].getBool())
    {
	vector<string> v;
	autoenablesources(v);
    }
}

void help(vector<string>& argv)
{
    unsigned filtermask = 6;
    if(argv.size()>1)
    {
	if(argv[1]=="all")
	    filtermask = 0;
	else if(argv[1]=="hidden")
	    filtermask = 4;
	else if(argv[1]=="advanced")
	    filtermask = 2;
    }
    for(unsigned i=0; func[i].name; i++)
    {
	if(func[i].flags && func[i].flags&filtermask) continue;
	cout << func[i].name << "   " << func[i].helptext << endl;
    }
}

int printgoodlist(PkgDep::ResultList& good)
{
    int numinst = 0;
    int _verbose = variables["verbose"].getInt();
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
    int _verbose = variables["verbose"].getInt();

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

static void install_internal(PMManager& manager, vector<string>& argv, bool appl=false)
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

	bool ok;

	if(appl)
	    ok = selp->appl_set_install();
	else
	    ok = selp->user_set_install();

	if(!ok)
	{
	    cout << stringutil::form("coult not mark %s for installation/update", pkg.c_str());
	}
    }
}

void install(vector<string>& argv)
{
    install_internal(Y2PM::packageManager(), argv);
}

void setappl(vector<string>& argv)
{
    install_internal(Y2PM::packageManager(), argv, true);
}

void setsel(vector<string>& argv)
{
    install_internal(Y2PM::selectionManager(), argv);
}

void delsel(vector<string>& argv)
{
    for (unsigned i=1; i < argv.size() ; i++)
    {
	string sel = stringutil::trim(argv[i]);

	if(sel.empty()) continue;

	PMSelectablePtr selp = Y2PM::selectionManager().getItem(sel);
	if(!selp)
	{
	    std::cout << "selection " << sel << " is not available.\n";
	    continue;
	}

	selp->setNothingSelected();
    }
}


static bool solve_internal(PMManager& manager, vector<string>& argv)
{
    int numinst=0,numbad=0;
    bool success = false;
    bool filter = false;

    for(vector<string>::iterator it= argv.begin(); it != argv.end();++it)
    {
	if(*it == "-u")
	{
	    filter = true;
	    cout << "filtering" << endl;
	}
    }

    PkgDep::ResultList good;
    PkgDep::ErrorResultList bad;

    success = manager.solveInstall(good, bad, filter);


    numbad = printbadlist(bad);
    numinst = printgoodlist(good);

    cout << "***" << endl;
    cout << numbad << " bad, " << numinst << " to install" << endl;

    return success;
}

void solve(vector<string>& argv)
{
    solve_internal(Y2PM::packageManager(), argv);
}

void solvesel(vector<string>& argv)
{
    bool ok = solve_internal(Y2PM::selectionManager(), argv);
    if(ok)
    {
	cout << "solve ok, activation selection" << endl;
	Y2PM::selectionManager().activate(Y2PM::packageManager());
    }
}

void order(vector<string>& argv)
{
    PkgSet toinstall;
    PkgSet installed;
    for (PMManager::PMSelectableVec::iterator it = Y2PM::packageManager().begin();
	it != Y2PM::packageManager().end(); ++it )
    {
	if((*it)->has_installed())
	{
	    installed.add((*it)->installedObj());
	}
	if((*it)->to_install())
	{
	    toinstall.add((*it)->candidateObj());
	}
    }

    InstallOrder order(toinstall,installed);

    InstallOrder::SolvableList nowinstall;

    unsigned count=0;

    cout << "Installation order:" << endl;
    for(nowinstall = order.computeNextSet(); !nowinstall.empty(); nowinstall = order.computeNextSet())
    {
	count++;
	for(InstallOrder::SolvableList::const_iterator cit = nowinstall.begin();
	    cit != nowinstall.end(); ++cit)
	{
	    cout << (*cit)->name() << " ";
	}
	cout << endl;
	{
	    string filename=stringutil::form("/tmp/graph.run%d",count);
	    ofstream fs(filename.c_str());
	    if(fs)
	    {
		order.printAdj(fs,false);
		fs.close();
	    }
	    else
	    {
		cout << "unable to open " << filename << endl;
	    }
	}
	{
	    string filename=stringutil::form("/tmp/rgraph.run%d",count);
	    ofstream fs(filename.c_str());
	    if(fs)
	    {
		order.printAdj(fs,true);
		fs.close();
	    }
	    else
	    {
		cout << "unable to open " << filename << endl;
	    }
	}
	order.setInstalled(nowinstall);
    }

}

void upgrade(vector<string>& argv)
{
    PMUpdateStats stats;

    for(vector<string>::iterator it= argv.begin(); it != argv.end();++it)
    {
	if(*it == "-u")
	{
	    stats.delete_unmaintained = true;
	    cout << "delete unmaintained" << endl;
	}
    }

    Y2PM::packageManager().doUpdate(stats);
    cout << stats << endl;
}

void commit(vector<string>& argv)
{
    std::list<std::string> errors_r;
    std::list<std::string> remaining_r;
    std::list<std::string> srcremaining_r;

    Y2PM::commitPackages (0,errors_r, remaining_r, srcremaining_r);

    if(!remaining_r.empty())
    {
	cout << "failed packages:" << endl;
	for(list<string>::iterator it=remaining_r.begin(); it!=remaining_r.end();++it)
	{
	    cout << *it << endl;
	}
    }

    if(variables["quitoncommit"].getBool())
    {
	exit(0);
    }
    else
    {
	cout << endl << "System is in an undefined state now, please quit" << endl;
    }
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

    if(Y2PM::instTarget().init(_rootdir, false) != InstTarget::Error::E_ok)
    {
	cout << "initialization failed" << endl;
    }
    else
	Y2PM::instTarget().installPackages(pkgs);
}

static void showstate_internal(PMManager& manager, vector<string>& argv)
{
    bool nonone = true;
    bool showall = true;
    int _verbose = variables["verbose"].getInt();

    PMManager::PMSelectableVec selectables;
    PMManager::PMSelectableVec::const_iterator begin, end;
    if(argv.size()>1)
    {
	for (unsigned i=1; i < argv.size() ; i++) {
	    string pkg = stringutil::trim(argv[i]);

	    if(pkg.empty()) continue;

	    if(pkg == "-a")
	    {
		nonone = false;
		continue;
	    }

	    PMSelectablePtr selp = manager.getItem(pkg);
	    if(!selp)
	    {
		std::cout << "package " << pkg << " is not available.\n";
		showall = false;
	    }
	    else
	    {
		nonone = false;
		showall = false;
		selectables.insert(selp);
	    }
	}
	begin = selectables.begin();
	end = selectables.end();
    }

    if(showall)
    {
	begin = manager.begin();
	end = manager.end();
    }

    for(PMManager::PMSelectableVec::const_iterator cit = begin;
	cit != end; ++cit)
    {
	PMSelectable::UI_Status s = (*cit)->status();
	if(nonone && s == PMSelectable::S_NoInst)
	    continue;
	switch(s)
	{
	    case PMSelectable::S_Protected:
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
			    if((*cit)->candidateObj()->edition()
				    > (*cit)->installedObj()->edition())
			    {
				cout << '*';
			    }
			}
			cout << ')';
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
void mem(vector<string>& argv)
{
    cout << mallinfo() << endl;
}

void cdattach(vector<string>& argv)
{
    MediaAccessPtr media = new MediaAccess;

    Url mediaurl_r = (string("cd:///;") + argv[1]);


    PMError err;
    if ( (err = media->open( mediaurl_r, "/tmp/blub" )) )
    {
	ERR << "Failed to open " << mediaurl_r << " " << err << endl;
	return;
    }


    DBG << "open ok" << endl;
    bool repeat = true;
    bool notfirst =false;
    do
    {
	if(media->isAttached())
	{
	    DBG << "release medium" << endl;
	    cout << media->release() << endl;
	}
	DBG << "attach medium" << endl;
	if ( (err = media->attach(notfirst)) )
	{
	    ERR << "Failed to attach media: " << err << endl;
	    repeat = false;
	    return;
	}
	notfirst = true;
    } while (repeat);

    if(media->isAttached())
    {
	DBG << "release medium final" << endl;
	media->release();
    }
}

int main( int argc, char *argv[] )
{
    char prompt[]="y2pm > ";

    char* buf = NULL;
    string inputstr;
    string historyfile;

    init_variables();

    cout << "Welcome to the YaST2 Package Manager!" << endl;
    cout << "This tool is meant for debugging purpose only." << endl << endl;
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
    while(buf && _keep_running)
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

	for(vector<string>::iterator vit = cmds.begin();
		vit != cmds.end() && _keep_running; ++vit)
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
		uint64_t s,e;
		rdtscll(s);
		if((func[i].flags&1) && !_initialized && func[i].func != init)
		{
		    if(_showtimes) t.startTimer();
		    init(nullvector);
		    if(_showtimes) t.stopTimer();
		    if(_showtimes) cout << "time: " << t << endl;
		}

		if(_showtimes) t.startTimer();
		func[i].func(argv);
		rdtscll(e);
		if(_showtimes) cout << "cycles: " << e-s << endl;
		if(_showtimes) t.stopTimer();
		if(_showtimes) cout << "time: " << t << endl;
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

void testset(vector<string>& argv)
{
    int i = 0;
    cout << mallinfo() << endl;
    PkgSet* testset = new PkgSet;
    cout << mallinfo() << endl;

    if(!testset)
    {
	ERR << "out of memory" << endl;
	exit(EXIT_FAILURE);
    }

    PMManager::PMSelectableVec::const_iterator it, end;
    it = Y2PM::packageManager().begin();
    end = Y2PM::packageManager().end();
    for (;it!=end;++it)
    {
	PMSelectablePtr selp = *it;
	if(!selp)
	{
	    std::cerr << "invalid pointer" << endl;
	    continue;
	}

	PMSolvablePtr solp;
	if(selp->has_installed())
	{
	    solp = selp->installedObj();
	}
	else if(selp->has_candidate())
	{
	    solp = selp->candidateObj();
	}
	if(!solp)
	{
	    std::cout << "no solvable" << endl;
	    continue;
	}

	testset->add(solp);
	if(i<5)
	{
	    cout << mallinfo() << endl;
	}
	++i;
    }

    cout << mallinfo() << endl;

    delete testset;
    testset = NULL;

    cout << mallinfo() << endl;
}

// vim:sw=4

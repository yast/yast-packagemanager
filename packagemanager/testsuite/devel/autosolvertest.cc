#include <string>
#include <cstring>
#include <cctype>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <map>
#include <set>
#include <unistd.h>

#include <Y2PM.h>
#include <y2pm/PkgEdition.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMSelectable.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PMPackageDataProvider.h>

#include <y2pm/DbHeader.h>
#include <y2pm/PMDequeInserter.h>

#include "show_pm.h"

using namespace std;

/** alternatives handling */
static map<PkgName,Alternatives::AltDefaultList> altdefaults;

static Alternatives::AltDefaultList alternatives_callback( PkgName name )
{
    if(altdefaults.find(name) != altdefaults.end())
	return altdefaults[name];
    return Alternatives::AltDefaultList();
}


class Test {
    public:
	PkgDep::alternatives_mode altmode;
	bool just_solve : 1;
	list<string> cand_names;
    public:
	Test() : altmode(PkgDep::ASK_ALWAYS), just_solve(false) {};
};

typedef list<Test> TestList;
typedef TestList::iterator Test_iterator;

static void read_test_file( string filename, TestList& tests )
{
	ifstream is(filename.c_str());
	string word;
	char c;

	while(is) {
		Test tst;
		
		is >> word;
		if (word[0] == '#') {
			while( is && (is.get(c), c != '\n') )
				;
			continue;
		}
		if (!is)
			break;

		if (word == "JUST_SOLVE" ) {
			tst.just_solve = true;
			while( is && (is.get(c), c != '\n') )
				;
			tests.push_back( tst );
			continue;
		}

		// phi compat stuff
		if (word == "CLEAR_DEFAULTS" ) {
			altdefaults.clear();
			continue;
		}
		else if (word == "DEFINE_DEFAULTS") {
			Alternatives::AltDefaultList defs;
			while( is ) {
				is.get(c);
				if (c == '\n')
					break;
				is >> word;
				if(!word.empty())
				    defs.push_back(PkgName(word));
			}
			PkgName n = defs.front();
			defs.pop_front();
			altdefaults[n] = defs;
			continue;
		}
		if (word.substr(0,12) == "ALTHANDLING=") {
			word = word.substr(12);
			if(word == "ask-always") tst.altmode = PkgDep::ASK_ALWAYS;
			else if(word == "ask-no-default") tst.altmode = PkgDep::ASK_IF_NO_DEFAULT;
			else if(word == "auto-no-default") tst.altmode = PkgDep::AUTO_IF_NO_DEFAULT;
			else if(word == "auto-always") tst.altmode = PkgDep::AUTO_ALWAYS;

			is >> word;
		}

		do {
			tst.cand_names.push_back(word);
			is.get(c);
			if (c == '\n')
				break;
			is >> word;
		} while( is );

		tests.push_back( tst );
	}
}


static void setPkgStates(struct Test& t)
{
    Y2PM::packageManager().setNothingSelected();

    if(t.just_solve)
	return;

    for(list<string>::iterator it= t.cand_names.begin();
	it != t.cand_names.end(); ++it)
    {
	bool remove = false;
	bool protect = false;
	string pkg = *it;
	if(pkg.size() > 1)
	{
	    if(pkg[0] == '-')
	    {
		remove = true;
		pkg = pkg.substr(1);
	    }
	    else if(pkg[0] == '@')
	    {
		protect = true;
		pkg = pkg.substr(1);
	    }
	}
	PMSelectablePtr selp = Y2PM::packageManager().getItem(pkg);
	if(!selp)
	{
	    std::cout << "package " << pkg << " is not available.\n";
	    continue;
	}

	if(remove)
	{
	    if(!selp->user_set_delete())
	    {
		cerr << "could not mark " << pkg << " for removal" << endl;
	    }
	}
	else if(protect)
	{
	    if(!selp->user_set_taboo())
	    {
		cerr << "could not mark " << pkg << " for taboo/protected" << endl;
	    }
	}
	else
	{
	    if(!selp->user_set_install())
	    {
		cerr << "could not mark " << pkg << " for installation/update" << endl;
	    }
	}
    }
}


// sort results according to name and edition
class ResultSortCriterion
{
    public:
	bool operator()(const PkgDep::Result& l, const PkgDep::Result& r)
	{
	    int res = l.name->compare( r.name );
	    if ( res != 0 )
		return( res < 0 );
	    return( l.edition < r.edition );
	}
};

// sort alternatives by name
class AlternativesSortCriterion
{
    public:
	bool operator()(const PkgDep::Alternative& l, const PkgDep::Alternative& r)
	{
	    return (l.solvable->name() < r.solvable->name());
	}
};

static void dumpPkgStates(std::ostream& os)
{
    deque<string> install, remove;
    for(PMManager::PMSelectableVec::const_iterator it = Y2PM::packageManager().begin();
	it != Y2PM::packageManager().end(); ++it)
    {
	PMSelectablePtr sp = *it;
	if(sp->to_install())
	    install.push_back(sp->name());
	if(sp->to_delete())
	    remove.push_back(sp->name());
    }
    if(!install.empty())
    {
	os << "Install: ";
	sort(install.begin(), install.end());
	copy(install.begin(), install.end(), ostream_iterator<string>(os, ","));
	os << endl;
    }
    if(!remove.empty())
    {
	os << "Remove: ";
	sort(remove.begin(), remove.end());
	copy(remove.begin(), remove.end(), ostream_iterator<string>(os, ","));
	os << endl;
    }
}

int main(int argc, char* argv[])
{
    PMError err;

    if(argc<2)
    {
	cout << "need file" << endl;
	return 1;
    }

    string basename( argv[1] );
    string inst_file  = basename + ".inst";
    string avail_file = basename + ".avail";
    string test_file  = basename + ".t";
    string out_file   = basename + ".out";

    std::list<PMPackagePtr> pkgs;
    PMBackInserter<list<PMPackagePtr>,PMPackagePtr> li(pkgs);

    err = read_package_list(li, inst_file);
    if(err)
    {
	cout << err << endl;
	return 1;
    }
    Y2PM::packageManager().poolSetInstalled(pkgs);

    pkgs.clear();
    err = read_package_list(li, avail_file);
    if(err)
    {
	cout << err << endl;
	return 1;
    }
    Y2PM::packageManager().poolAddCandidates(pkgs);

    TestList tests;

    read_test_file(test_file, tests);

    ofstream of( out_file.c_str() );
    unsigned testno = 1;
		
    for( Test_iterator p = tests.begin(); p != tests.end(); ++p,++testno ){
	of << "--- Test " << testno << endl << endl;
	cout << "--------------- Test " << testno << " ---------------\n";
//	cout << "To install: " << p->cand_names << endl;
	
	PkgDep::ResultList good;
	PkgDep::ErrorResultList bad;

	setPkgStates(*p);

	PkgDep::set_default_alternatives_mode(p->altmode);

	// FIXME: alternatives_callback

	Y2PM::packageManager().solveInstall(good, bad, false);

	dumpPkgStates(of);
	of << endl;

	good.sort(ResultSortCriterion());
	bad.sort(ResultSortCriterion());

	for( PkgDep::ResultList::const_iterator p = good.begin();
	     p != good.end(); ++p ) {
	    of << *p << endl;
	}
	for( PkgDep::ErrorResultList::iterator p = bad.begin();
	     p != bad.end(); ++p ) {
	    p->alternatives.sort(AlternativesSortCriterion());
	    of << *p << endl;
	}
    }


    return 0;
}

// vim: sw=4

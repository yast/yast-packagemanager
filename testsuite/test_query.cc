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

   File:       test_query.cc
   Purpose:    test query interface
   Author:     Klaus Kaempf <kkaempf@suse.de>
   Maintainer: Klaus Kaempf <kkaempf@suse.de>

/-*/

#include <cstdlib> //atoi

// readline
#include <cstdio>

#include <unistd.h>

#include <y2util/Y2SLog.h>
#include <Y2PM.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/Query.h>
#include <y2pm/InstTarget.h>

#include "show_pm.h"

#undef Y2SLOG
#define Y2SLOG "test_query"

using namespace std;

int
main( int argc, char *argv[] )
{
    if (argc < 3)
    {
	cerr << "Usage:" << argv[0] << " <url> <query> [<query> ...]" << endl;
	return 1;
    }

    Y2PM y2pm;

    // init package manager and target

    y2pm.packageManager ();
    cout << "Initialized target" << endl;
    InstSrcManager& mgr = y2pm.instSrcManager();

    InstSrcManager::ISrcIdList nids;

    PMError err = mgr.scanMedia( nids, Url (argv[1]) );
    if ( nids.size() )
    {
	InstSrcManager::ISrcId source_id = *nids.begin();
	err = mgr.enableSource( source_id );
	if (err != PMError::E_ok)
	{
	    cerr << "Enable source failed: " << err.errstr() << endl;
	    return 1;
	}
	cout << "Source enabled" << endl;
    }
    else
    {
	cout << "No source found" << endl;
    }
    y2pm.instTarget().init("/", false);
    y2pm.packageManager().poolSetInstalled (y2pm.instTarget().getPackages () );

    Query query;
    int argp = 2;
    while (argp < argc)
    {
	cout << "Query: '" << argv[argp] << "'" << endl;
	err = query.queryPackage ((string)(argv[argp]));
	if (err != PMError::E_ok)
	{
	    cout << "Error: " << err.errstr() << endl;
	    cout << argv[argp] << endl;
	    int pos = query.failedPos();
	    while (pos-- > 0)
	    {
		cout << " ";
	    }
	    cout << "^" << endl;
	}
	else
	{
	    constPMPackagePtr package = query.nextPackage (y2pm.packageManager ());
	    if (!package)
	    {
		cout << "No match" << endl;
	    }
	    else
	    {
		while (package)
		{
		    cout <<  (const std::string &)(package->name())
			 << "-" << package->version()
			 << "-" << package->release()
			 << "." << (const std::string &)(package->arch()) << endl;
		    package = query.nextPackage (y2pm.packageManager ());
		}
	    }
	}
	argp++;
    }
    return 0;
}

// vim:sw=4

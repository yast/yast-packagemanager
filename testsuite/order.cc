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

   File:       order.cc
   Purpose:    order packages specified on command line
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/

#include <cstdlib>

#include <y2util/Y2SLog.h>
#include <y2util/ExternalProgram.h>
#include <y2pm/InstallOrder.h>
#include <y2pm/InstSrc.h>

// needed for sdl test
#include <y2pm/PMPackage.h>

#undef Y2SLOG
#define Y2SLOG "testinstall" 

using namespace std;

static char tmpdir[] = "/tmp/order.XXXXXX";

void removetmpdir(void)
{
    ExternalProgram prg(string("rm -rf ")+tmpdir);

    if(prg.close())
    {
	cerr << "removing " << tmpdir << " failed" << endl;
    }
}

int main( int argc, char *argv[] )
{
    if(argc<2)
    {
	cout << "Usage: " << argv[0] << " <dir>" << endl;
	return 1;
    }

    if(!mkdtemp(tmpdir))
    {
	cout << "mkdtemp: " << strerror(errno) << endl;
	return 1;
    }

    atexit(removetmpdir);

    InstSrcPtr nsrc;
    Pathname cache( tmpdir );
    cache += "/cache";

    string media_url = "dir:///";
    string product_dir = argv[1];

    PMError err = InstSrc::vconstruct( nsrc, cache, media_url, product_dir, InstSrc::T_UnitedLinux );

    if (err)
    {
	cerr << "Failed to construct InstSrc: " << err << endl;
	return 1;
    }
    if (!nsrc)
    {
	cerr << "Failed: No InstSrc" << endl;
	return 1;
    }

    cout << "parsing package info" << endl;
    err = nsrc->enableSource();

    if (err)
    {
	cerr << "Failed to enableSource: " << err << endl;
	return 1;
    }

    PkgSet candidates;

    const std::list<PMPackagePtr> *pacs = nsrc->getPackages();

    unsigned count = 0;
    for(std::list<PMPackagePtr>::const_iterator it = pacs->begin();
	it != pacs->end();
	++it, count++)
    {
//	if(string((*it)->name()).find("SDL") != string::npos)
	    candidates.add(*it);
    }

    InstallOrder order(candidates);
    cout << "computing installation order" << endl;
//    Y2SLog::dbg_enabled_bm = true;
    order.startrdfs();

    cout << "Deps:" << endl;
    cout << "------------------------" << endl;
    order.printAdj(cout);
    cout << "------------------------" << endl;

    cout << "rDeps:" << endl;
    cout << "------------------------" << endl;
    order.printAdj(cout,true);
    cout << "------------------------" << endl;

    cout << "Installation order (topsort):" << endl;
    cout << "------------------------" << endl;
    for(InstallOrder::SolvableList::const_iterator cit = order.getTopSorted().begin();
	cit != order.getTopSorted().end(); ++cit)
    {
	cout << (*cit)->name() << " ";
    }
    cout << endl;
    cout << "------------------------" << endl;
    cout << "Installation order (sets):" << endl;
    cout << "------------------------" << endl;
    InstallOrder::SolvableList pkgs = order.computeNextSet();
    int nr = 0;
    while(!pkgs.empty())
    {
	cout << nr++ << ": ";
	for(InstallOrder::SolvableList::const_iterator cit = pkgs.begin();
	    cit != pkgs.end(); ++cit)
	{
	    cout << (*cit)->name() << " ";
	    order.setInstalled(*cit);
	}

	cout << endl;

	pkgs = order.computeNextSet();
    }
    cout << endl;
    cout << "------------------------" << endl;
}

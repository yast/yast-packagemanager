/*
  test_selections.cc

  test program for InstSrcData->tryGetData()

*/

#include <list>
#include <string>
#include <vector>
#include <y2util/Pathname.h>
#include <Y2PM.h>
#include <y2pm/PMError.h>
#include <y2pm/PMSelection.h>
#include <y2pm/PMSelectionPtr.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcPtr.h>
#include <y2pm/InstSrcData.h>
#include <iostream>
#include <algorithm>

#include "show_pm.h"

#undef  Y2LOG
#define Y2LOG "PM_test_selections"

using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
	cerr << "[--url <media_url>]" << endl;
	cerr << "[--dir <product_dir>]" << endl;
	cerr << "[--short]		// short output, only cached values" << endl;
	cerr << "[--version <selection_version>]" << endl;
	cerr << "[--release <selection_release>]" << endl;
	cerr << "[--arch <selection_arch>]" << endl;
	cerr << "<command>: " << endl;
	cerr << "query" << endl;
	return 1;
    }

    typedef vector<string> strvec;
    typedef strvec::iterator strvecit;
    strvec args;
    unsigned argpos = 0;
    string media_url = "dir:///";
    string product_dir = "/";
    bool short_output = false;
    string selection_version = "";
    string selection_release = "";
    string selection_arch = "";

    for(int i = 1; i < argc; i++)
    {
	args.push_back(argv[i]);
    }
    
    unsigned argnum = args.size();

    string command = args[argpos++];

    if (command == "--url")
    {
	if (argpos >= argnum) return 1;
	media_url = args[argpos++];
	if (media_url.empty())
	    return 1;
	
	if (argnum > argpos)
	    command = args[argpos++];
	else
	    command = "";
    }
    if (command == "--dir")
    {
	if (argpos >= argnum) return 1;
	product_dir = args[argpos++];
	if (product_dir.empty())
	    return 1;

	if (argnum > argpos)
	    command = args[argpos++];
	else
	    command = "";
    }

    if (command == "--short")
    {
	short_output = true;

	if(argnum>argpos)
	    command = args[argpos++];
	else
	    command = "";
    }

    if (command == "--version")
    {
	if (argpos >= argnum) return 1;
	selection_version = args[argpos++];
	if (selection_version.empty())
	    return 1;

	if (argnum > argpos)
	    command = args[argpos++];
	else
	    command = "";
    }

    if (command == "--release")
    {
	if (argpos >= argnum) return 1;
	selection_release = args[argpos++];
	if (selection_release.empty())
	    return 1;

	if (argnum > argpos)
	    command = args[argpos++];
	else
	    command = "";
    }

    if (command == "--arch")
    {
	if (argpos >= argnum) return 1;
	selection_arch = args[argpos++];
	if (selection_arch.empty())
	    return 1;

	if (argnum > argpos)
	    command = args[argpos++];
	else
	    command = "";
    }

    Y2PM y2pm;

    y2pm.packageManager(false);		// start without target
#if 0
    InstSrcManager& mgr = y2pm.instSrcManager();
    InstSrcManager::ISrcIdList nids;

    PMError err = mgr.scanMedia( nids, url );
    if ( nids.size() )
    {
	InstSrcManager::ISrcId source_id = *nids.begin();
	err = mgr.enableSource( source_id );
    }
#endif
    InstSrcPtr nsrc;

    Pathname cache   ( "/tmp/tcache" ); // cachedir (must not exist)

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

    err = nsrc->enableSource();

    if (err)
    {
	cerr << "Failed to enableSource: " << err << endl;
	return 1;
    }

    if (command == "query")
    {

	for(; argpos < argnum; argpos++)
	{
	    cout << "querying " << args[argpos] << endl;
	    const std::list<PMSelectionPtr>& selections = InstData::findSelections (nsrc->data()->getSelections (), args[argpos], selection_version, selection_release, selection_arch);
	    if (selections.empty())
	    {
		cout << args[argpos] << " is not available" << endl;
	    }
	    else
	    {
		cout << selections.size()
		     << " matches for "
		     << args[argpos]
		     << (selection_version.empty()?"":("-"+selection_version))
		     << (selection_release.empty()?"":("-"+selection_release))
		     << (selection_arch.empty()?"":("."+selection_arch))
		     << " found" << endl;
		for (std::list<PMSelectionPtr>::const_iterator p_it = selections.begin();
			p_it != selections.end(); ++p_it)
		{
		    show_pmselection (*p_it);
		}
	    }
	}
    }
    else
    {
	cout << "unkown command " << command << endl;
    }
    return 0;
}

// vim:sw=4

/*
  test_packages.cc

  test program for InstSrcData->tryGetData()

*/

#include <list>
#include <string>
#include <vector>
#include <y2util/Pathname.h>
#include <y2pm/PMError.h>
#include <y2pm/PMSelection.h>
#include <y2pm/PMSelectionPtr.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcPtr.h>
#include <iostream>
#include <algorithm>

#undef  Y2LOG
#define Y2LOG "PM_test_selections"

using namespace std;

static void
show_selection (PMSelectionPtr p)
{
    cout << "-- Solvable --" << endl;
    for (PMSelection::PMSolvableAttribute attr
	= PMSelection::PMSolvableAttribute(PMSelection::PMSLV_ATTR_BEGIN);
	attr < PMSelection::PMSLV_NUM_ATTRIBUTES;
	attr = PMSelection::PMSolvableAttribute(attr+1))
    {
	cout
	<< p->getAttributeName(attr)
	<< ": "
	<< p->getAttributeValue((PMSelection::PMSelectionAttribute)attr)
	<< endl;
    }

    cout << "-- Object --" << endl;
    for (PMSelection::PMObjectAttribute attr
	= PMSelection::PMObjectAttribute(PMSelection::PMOBJ_ATTR_BEGIN);
	attr < PMSelection::PMOBJ_NUM_ATTRIBUTES;
	attr = PMSelection::PMObjectAttribute(attr+1))
    {
	cout
	<< p->getAttributeName(attr)
	<< ": "
	<< p->getAttributeValue(attr)
	<< endl;
    }

    cout << "-- Selection --" << endl;
	for (PMSelection::PMSelectionAttribute attr
	= PMSelection::PMSelectionAttribute(PMSelection::SEL_ATTR_BEGIN);
	    attr < PMSelection::PMSEL_NUM_ATTRIBUTES;
	attr = PMSelection::PMSelectionAttribute(attr+1))
    {
	cout
	<< p->getAttributeName(attr)
	<< ": "
	<< p->getAttributeValue(attr)
	<< endl;
    }

    return;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
	cerr << "[--url <media_url>]" << endl;
	cerr << "[--dir <product_dir>]" << endl;
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
	    const std::list<PMSelectionPtr> *sels = nsrc->getSelections ();
	    if (!sels
		|| sels->empty())
	    {
		cout << args[argpos] << " is not available" << endl;
	    }
	    else
	    {
		cout << sels->size()
		     << " matches for "
		     << args[argpos]
		     << (selection_version.empty()?"":("-"+selection_version))
		     << (selection_release.empty()?"":("-"+selection_release))
		     << (selection_arch.empty()?"":("."+selection_arch))
		     << " found" << endl;
		for (std::list<PMSelectionPtr>::const_iterator p_it = sels->begin();
			p_it != sels->end(); ++p_it)
		{
		    show_selection (*p_it);
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

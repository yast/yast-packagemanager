/*
  test_packages.cc

  test program for InstSrcData->tryGetDescr()

*/

#include <list>
#include <string>
#include <vector>
#include <y2util/Pathname.h>
#include <y2pm/PMError.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcPtr.h>
#include <iostream>
#include <algorithm>

#undef  Y2LOG
#define Y2LOG "PM_test_packages"

using namespace std;

class PMPkg_eq : public unary_function<PMPackagePtr,bool>
{
	string _name;
    public:
	explicit PMPkg_eq(const string& s) : _name(s) {}
	bool operator()(const PMPackagePtr& c) const
	    { return (c->name() == _name); }
};

void progresscallback(double p, void* nix)
{
    cout << p << endl;
}


static void
show_package (PMPackagePtr p)
{
    cout << "-- Solvable --" << endl;
    for (PMPackage::PMSolvableAttribute attr
	= PMPackage::PMSolvableAttribute(PMPackage::PMSLV_ATTR_BEGIN);
	attr < PMPackage::PMSLV_NUM_ATTRIBUTES;
	attr = PMPackage::PMSolvableAttribute(attr+1))
    {
	cout
	<< p->getAttributeName(attr)
	<< ": "
	<< p->getAttributeValue((PMPackage::PMPackageAttribute)attr)
	<< endl;
    }

    cout << "-- Object --" << endl;
    for (PMPackage::PMObjectAttribute attr
	= PMPackage::PMObjectAttribute(PMPackage::PMOBJ_ATTR_BEGIN);
	attr < PMPackage::PMOBJ_NUM_ATTRIBUTES;
	attr = PMPackage::PMObjectAttribute(attr+1))
    {
	cout
	<< p->getAttributeName(attr)
	<< ": "
	<< p->getAttributeValue(attr)
	<< endl;
    }

    cout << "-- Package --" << endl;
	for (PMPackage::PMPackageAttribute attr
	= PMPackage::PMPackageAttribute(PMPackage::PKG_ATTR_BEGIN);
	    attr < PMPackage::PMPKG_NUM_ATTRIBUTES;
	attr = PMPackage::PMPackageAttribute(attr+1))
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
	cerr << "[--version <package_version>]" << endl;
	cerr << "[--release <package_release>]" << endl;
	cerr << "[--arch <package_arch>]" << endl;
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
    string package_version = "";
    string package_release = "";
    string package_arch = "";

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
	package_version = args[argpos++];
	if (package_version.empty())
	    return 1;

	if (argnum > argpos)
	    command = args[argpos++];
	else
	    command = "";
    }

    if (command == "--release")
    {
	if (argpos >= argnum) return 1;
	package_release = args[argpos++];
	if (package_release.empty())
	    return 1;

	if (argnum > argpos)
	    command = args[argpos++];
	else
	    command = "";
    }

    if (command == "--arch")
    {
	if (argpos >= argnum) return 1;
	package_arch = args[argpos++];
	if (package_arch.empty())
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
	    const std::list<PMPackagePtr> *pacs = nsrc->findPackages (args[argpos], package_version, package_release, package_arch);
	    if (!pacs
		|| pacs->empty())
	    {
		cout << args[argpos] << " is not available" << endl;
	    }
	    else
	    {
		cout << pacs->size()
		     << " matches for "
		     << args[argpos]
		     << (package_version.empty()?"":("-"+package_version))
		     << (package_release.empty()?"":("-"+package_release))
		     << (package_arch.empty()?"":("."+package_arch))
		     << " found" << endl;
		for (std::list<PMPackagePtr>::const_iterator p_it = pacs->begin();
			p_it != pacs->end(); ++p_it)
		{
		    show_package (*p_it);
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

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

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
	cerr << "[--url <media_url>]" << endl;
	cerr << "[--dir <product_dir>]" << endl;
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

    InstSrcPtr nsrc;

    Pathname cache   ( "/tmp/tcache" ); // cachedir (must not exist)

    PMError err = InstSrc::vconstruct( nsrc, cache, media_url, product_dir, InstSrc::T_UnitedLinux );

    if (err)
    {
	cerr << "Failed: " << err << endl;
	return 1;
    }

    if (!nsrc)
    {
	cerr << "Failed: No InstSrc" << endl;
	return 1;
    }

    if (command == "query")
    {

	const std::list<PMPackagePtr> *pkglist = nsrc->getPackages ();
	for(;argpos<argnum;argpos++)
	{
	    cout << "querying " << args[argpos] << endl;
	    typedef std::list<PMPackagePtr>::const_iterator PkgLI;
	    PkgLI p = find_if (pkglist->begin (), pkglist->end(), PMPkg_eq(args[argpos]));
	    if(p == pkglist->end())
	    {
		cout << args[argpos] << " is not installed" << endl;
	    }
	    else
	    {
		cout << "-- Solvable --" << endl;
		for (PMPackage::PMSolvableAttribute attr
			= PMPackage::PMSolvableAttribute(PMPackage::PMSLV_ATTR_BEGIN);
		    attr < PMPackage::PMSLV_NUM_ATTRIBUTES;
		    attr = PMPackage::PMSolvableAttribute(attr+1))
		{
		    cout
			<< (*p)->getAttributeName(attr)
			<< ": "
			<< (*p)->getAttributeValue((PMPackage::PMPackageAttribute)attr)
			<< endl;
		}


		cout << "-- Object --" << endl;
		for (PMPackage::PMObjectAttribute attr
			= PMPackage::PMObjectAttribute(PMPackage::PMOBJ_ATTR_BEGIN);
		    attr < PMPackage::PMOBJ_NUM_ATTRIBUTES;
		    attr = PMPackage::PMObjectAttribute(attr+1))
		{
		    cout
			<< (*p)->getAttributeName(attr)
			<< ": "
			<< (*p)->getAttributeValue(attr)
			<< endl;
		}

		cout << "-- Package --" << endl;
	    	for (PMPackage::PMPackageAttribute attr
			= PMPackage::PMPackageAttribute(PMPackage::PKG_ATTR_BEGIN);
		    attr < PMPackage::PKG_NUM_ATTRIBUTES;
		    attr = PMPackage::PMPackageAttribute(attr+1))
		{
		    cout
			<< (*p)->getAttributeName(attr)
			<< ": "
			<< (*p)->getAttributeValue(attr)
			<< endl;
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

#include <list>
#include <string>
#include <vector>
#include <y2pm/RpmDb.h>
#include <y2pm/PMPackage.h>
#include <iostream>
#include <algorithm>

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
    if(argc<2)
    {
	cerr << "specify option" << endl;
	return 1;
    }

    typedef vector<string> strvec;
    typedef strvec::iterator strvecit;
    strvec args;
    unsigned argpos = 0;
    string root = "/";
    unsigned flags = 0;

    for(int i = 1; i < argc; i++)
    {
	args.push_back(argv[i]);
    }
    
    unsigned argnum = args.size();

    string command = args.at(argpos++);

    if(command == "--root")
    {
	if(argpos>=argnum) return 1;
	root = args.at(argpos++);
	if(root.empty())
	    return 1;
	
	if(argnum>argpos)
	    command = args.at(argpos++);
	else
	    command = "";
    }
    if(command == "--flags")
    {
	if(argpos>=argnum) return 1;
	string flagstr = args.at(argpos++);
	if(flagstr.empty())
	    return 1;

	flags = atoi(flagstr.c_str());
	
	if(argnum>argpos)
	    command = args.at(argpos++);
	else
	    command = "";
    }

    RpmDbPtr rpmdb = new RpmDb(root);
    RpmDb::DbStatus stat = RpmDb::RPMDB_OK;
    stat = rpmdb->initDatabase(true);
    if( ! (stat == RpmDb::RPMDB_OK || stat == RpmDb::RPMDB_NEW_CREATED))
    {
	cout << "error initializing rpmdb: " << stat << endl;
	return 1;
    }

    if(command == "query")
    {

	list<PMPackagePtr> pkglist;
	rpmdb->getPackages(pkglist);
	for(;argpos<argnum;argpos++)
	{
	    cout << "querying " << args.at(argpos) << endl;
	    typedef list<PMPackagePtr>::iterator PkgLI;
	    PkgLI p = find_if(pkglist.begin(),pkglist.end(),PMPkg_eq(args.at(argpos)));
	    if(p == pkglist.end())
	    {
		cout << args.at(argpos)<< " is not installed" << endl;
	    }
	    else
	    {
		for (PMPackage::PMSolvableAttribute attr
			= PMPackage::PMSolvableAttribute(PMPackage::PMSLV_ATTR_BEGIN);
		    attr < PMPackage::PMSLV_NUM_ATTRIBUTES;
		    attr = PMPackage::PMSolvableAttribute(attr+1))
		{
		    cout
			<< (*p)->getAttributeName(attr)
			<< ": "
			<< (*p)->getAttributeValue(attr)
			<< endl;
		}


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

		cout << (*p)->getAttributeName(PMPackage::ATTR_GROUP)
		    << ": "
		    << (*p)->getAttributeValue(PMPackage::ATTR_GROUP)
		    << endl;
		cout << (*p)->getAttributeName(PMPackage::ATTR_GROUP)
		    << ": "
		    << (*p)->getAttributeValue(PMPackage::ATTR_GROUP)
		    << endl;
		cout << (*p)->getAttributeName(PMPackage::ATTR_SUMMARY)
		    << ": "
		    << (*p)->getAttributeValue(PMPackage::ATTR_SUMMARY)
		    << endl;
	    }
	}
    }
    else if(command == "install")
    {
	rpmdb->setProgressCallback(progresscallback,NULL);
	for(;argpos<argnum;argpos++)
	{
	    bool success = rpmdb->installPackage(args[argpos],flags);
	    cout << "installation of " << args[argpos]
	    << (success?" succeeded":" failed") << endl;
	}
    }
    else if(command == "remove")
    {
	for(;argpos<argnum;argpos++)
	{
	    bool success = rpmdb->removePackage(args[argpos],flags);
	    cout << "removal of " << args[argpos]
	    << (success?" succeeded":" failed") << endl;
	}
    }else if(command == "check")
    {
	for(;argpos<argnum;argpos++)
	{
	    cout << "Checking: " << args[argpos] << endl; 
	    unsigned ret = rpmdb->checkPackage(args[argpos]);
	    cout << RpmDb::checkPackageResult2string(ret) << endl;
	}
    }
    else if(command == "checkversion")
    {
	string ver = args[argpos++];
	if(ver.empty())
	    return 1;
	for(;argpos<argnum;argpos++)
	{
	    cout << "Checking: " << args[argpos] << endl; 
	    unsigned ret = rpmdb->checkPackage(args[argpos],ver);
	    cout << RpmDb::checkPackageResult2string(ret) << endl;
	}
    }
    else if(command == "rebuilddb")
    {
	RpmDb::DbStatus stat = RpmDb::RPMDB_OK;
	stat = rpmdb->rebuildDatabase();
	if(stat != RpmDb::RPMDB_OK)
	{
	    return 1;
	}
    }
    else
    {
	cout << "unkown command " << command << endl;
    }
    return 0;
}

// vim:sw=4

#include <list>
#include <string>
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

void progresscallback(double p)
{
    cout << p << endl;
}

int main(int argc, char* argv[])
{
    if(argc<2)
    {
	cerr << "specify rpm package or -a" << endl;
	return 1;
    }

    string package = argv[1];

    RpmDbPtr rpmdb = new RpmDb("/");
    rpmdb->initDatabase();

    if(package == "-a")
    {
	list<PMPackagePtr> pkglist;
	rpmdb->getPackages(pkglist);
	for(int i = 2; i < argc; i++)
	{
	    typedef list<PMPackagePtr>::iterator PkgLI;
	    PkgLI p = find_if(pkglist.begin(),pkglist.end(),PMPkg_eq(argv[i]));
	    if(p == pkglist.end())
	    {
		cout << argv[i] << " is not installed" << endl;
	    }
	    else
	    {
		cout << "query of " << argv[i] << endl;
		for (unsigned attr = 0; attr < PMPackage::PKG_NUM_ATTRIBUTES; attr++)
		{
		    cout
			<< (*p)->getAttributeName(PMPackage::PMPackageAttribute(attr))
			<< ": "
			<< (*p)->getAttributeValue(PMPackage::PMPackageAttribute(attr))
			<< endl;
//		    cout << (*p)->getAttributeValue(PMPackage::PKG_LICENSE) << endl;
//		    cout << (*p)->getAttributeValue(PMPackage::PMOBJ_DESCRIPTION) << endl;
		}
	    }
	}
    }
    else if(package == "-i")
    {
	rpmdb->setProgressCallback(progresscallback);
	for(int i = 2; i < argc; i++)
	{
	    bool success = rpmdb->installPackage(argv[i],0);
	    cout << "installation of " << argv[i]
	    << (success?" succeeded":" failed") << endl;
	}
    }
    else
    {
	bool ret = rpmdb->checkPackage(package);
	cout << package << " is " << (ret?"ok":"corrupt") << endl;
    }

    return 0;
}

// vim:sw=4

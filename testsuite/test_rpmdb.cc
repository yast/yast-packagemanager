#include <list>
#include <string>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/RpmDb.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    if(argc<2)
    {
	cerr << "specify rpm package or -a" << endl;
	return 1;
    }

    string package = argv[1];

    RpmDb rpmdb("/");

    if(package == "-a")
    {
	rpmdb.initDatabase();
	list<PMPackagePtr> pkglist;
	rpmdb.getPackages(pkglist);
    }
    else
    {
	bool ret = rpmdb.checkPackage(package);
	cout << package << " is " << (ret?"ok":"corrupt") << endl;
    }

    return 0;
}

// vim:sw=4

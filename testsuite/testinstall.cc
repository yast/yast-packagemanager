#include <unistd.h>
#include <PkgDep.h>
#include <Package.h>
#include <PackageSource.h>
#include <PkgDb.h>
#include <Exception.h>
#include <timeclass.h>

void usage(char **argv) {
	cerr <<
"Do PHI dependency completion on a set of RPMS or the installed database.\n\n"
"Usage: " << argv[0] << " -v for increasing verbose output\n"
"	-h --help		print this help\n"
"	--from path-to-rpms	Specifies the path to the RPMs set.\n"
"				If not specified, use the installed set of RPMs\n"
"	<packagenames>		packagenames to resolve\n"
"		(if none specified, read from stdin)\n"
"\n\n"
"Output is a list of lines of the form:\n\n"
"	install <pkgname>		Install this package.\n"
"	remove <pkgname>		Remove this package.\n"
	    ;
	exit(1);
}

int main( int argc, char *argv[] )
{
    // add packages to the pool
    int verbose = 1;
//    PkgPool.add_installed_packages();

    // TODO: this has to be done by some dlopen magic
    MediaAccess::Ref fileaccess(new MediaAccess_File());
    PackageDataProvider::Ref source;
    try
    {
	source = new SuSEClassicDataProvider(Url("file:///mounts/dist/full/full-i386"));
	source->addAllPackages();
//	PackageDataProvider* inst = new SuSEClassicDataProvider(Url("file:///suse/lnussel/prog/phi/test"));
//	inst->addAllPackages();
    }
    catch(const PkgDbExcp& excp)
    {
	cerr << excp << endl;
	return 1;
    }
    catch(...)
    {
	cerr << "unexpected exception" << endl;
	return 2;
    }

    // build sets
    PkgSet installed; //( DTagListI0() );
    PkgSet empty;
    PkgSet candidates;
    PkgSet *available = NULL;

    available = new PkgSet();

/*
    packa = new Package();
    PkgRelation rel(PkgName("aaa_skel"),EQ,PkgEdition(PkgEdition::UNSPEC));
    PkgName *name = new PkgName("blubb");
    packa->setName(*name);
    packa->add_requires(rel);
    candidates.add(packa);
    delete name;
    name=NULL;
*/    
//    candidates.add(PkgPool.get(PkgName("kdebase3"),PkgEdition("3.0-5")));
//    candidates.add(available->lookup("dialog"));

    // create set of all packages
    for( PkgDb::const_iterator p = PkgPool.begin(); p != PkgPool.end(); ++p ) {
	    available->add( p->value );
    }
    PkgPool.attach_set( available );

    for (int i=1; i < argc ; i++) {
	if (!available->lookup(argv[i])) {
		std::cerr << "package " << argv[i] << " is not available.\n";
		return 1;
	}
	candidates.add(available->lookup(argv[i]));
    }
/*
    if (!oncmdline) {
	    char fupp[200];
	    while (fgets(fupp,sizeof(fupp),stdin)) {
		char *s;
		s = strchr(fupp,'\n'); if (s) *s = '\0';
		s = strchr(fupp,'\r'); if (s) *s = '\0';
		if (available->lookup(fupp)) 
		    candidates.add(available->lookup(fupp));
	    }
    }
*/
    // construct PkgDep object
    PkgDep engine( installed, *available );

    // call upgrade
    PkgDep::ResultList good;
    PkgDep::ErrorResultList bad;
    PkgDep::NameList to_remove;

    int numinst=0,numrem=0,numbad=0;
    bool success = false;

    TimeClass t;
    t.startTimer();

    try {
	success = engine.install( candidates, good, bad);
    }
    catch(const char* str)
    {
	cerr << str << endl;
	return 1;
    }
    catch(...)
    {
	cerr << "some error occured" << endl;
	return 1;
    }

    t.stopTimer();

    if (!success) {
	// if it failed, print problems
	cout << "*** Conflicts ***" << endl;
	for( PkgDep::ErrorResultList::const_iterator p = bad.begin();
	     p != bad.end(); ++p ) {
	    cerr << *p << endl;
	    numbad++;
	}
    }

    cout << "*** Packages to install ***" << endl;

    // otherwise, print what should be installed and what removed
    for(PkgDep::ResultList::const_iterator p=good.begin();p!=good.end();++p) {
	switch (verbose) {
	case 0: cout << "install " << p->name << endl;break;
	case 1: cout << "install " << p->name << "-" << p->edition << endl;break;
	default: cout << "install " << *p << endl;break;
	}
	numinst++;

    }
    cout << "*** Packages to remove ***" << endl;
    for(PkgDep::NameList::const_iterator q=to_remove.begin();q!=to_remove.end();++q) {
	switch (verbose) {
	case 0: cout << "remove " << (const char*)*q << endl;break;
	default: cout << "remove " << *q << endl;break;
	}
	numrem++;
    } 
    cout << "***" << endl;
    cout << numbad << " bad, " << numinst << " to install, " << numrem << " to remove" << endl;
    cout << "Time consumed: " << t.getTimer() << endl;

    return 0;
}

// vim:sw=4

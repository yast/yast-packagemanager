#include <unistd.h>
#include <y2util/timeclass.h>
#include <y2util/Y2SLog.h>
#include <Y2PM.h>
#include <y2pm/PkgDep.h>
#include <y2pm/PMPackage.h>

using namespace std;

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


// example
Alternatives::AltDefaultList alternative_default( PkgName name )
{
    Alternatives::AltDefaultList list;
    if(name==PkgName("spell"))
	list.push_front(PkgName("aspell"));
    else if(name==PkgName("libGL.so.1") || name==PkgName("libgl"))
	list.push_front(PkgName("mesasoft"));
    return list;
}

int main( int argc, char *argv[] )
{
    // add packages to the pool
    int verbose = 1;
//    PkgPool.add_installed_packages();


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


    PMPackageManager& manager = Y2PM::packageManager();

    INT << manager.size() << endl;

    for(PMManager::PMSelectableVec::const_iterator it = manager.begin();
	it != manager.end(); ++it )
    {
	// create set of all packages
	available->add( (*it)->installedObj() );
    }

    for (int i=1; i < argc ; i++) {
	if (available->lookup(PkgName(argv[i])) == NULL) {
		std::cerr << "package " << argv[i] << " is not available.\n";
		return 1;
	}
	candidates.add(available->lookup(PkgName(argv[i])));
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
//    PkgDep::set_default_alternatives_mode(PkgDep::AUTO_IF_NO_DEFAULT);
    PkgDep engine( installed, *available, alternative_default );

    // call upgrade
    PkgDep::ResultList good;
    PkgDep::ErrorResultList bad;
    PkgDep::NameList to_remove;

    int numinst=0,numrem=0,numbad=0;
    bool success = false;

    TimeClass t;
    t.startTimer();

    success = engine.install( candidates, good, bad);
    
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
	case 0: cout << "remove " << *q << endl;break;
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

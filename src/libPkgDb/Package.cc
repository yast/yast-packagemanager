#include <cstdio>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <y2util/hash.h>
#include <y2pm/Package.h>

using namespace std;

ostream& operator<<( ostream& os, const Package& pkg )
{
	pkg.print(os);
	return os;
}

void Package::print( ostream& os ) const
{
	os << "Files:\n";
	for( Package::FileList_const_iterator vi = _files.begin();
		 vi != _files.end(); ++vi )
		cout << " " << *vi << endl;
}

bool Package::is_from_provider( PackageDataProvider* provider )
{
	for( list<PackageDataProvider*>::const_iterator p = _dataproviders.begin();
		 p != _dataproviders.end(); ++p )
		if (*p == provider)
			return true;
	return false;
}

#if 0
// ---------------------------------------------------------------------------
//									 misc
// ---------------------------------------------------------------------------


bool Package::is_in_distrib( const DistTag& dist )
{
	for( DistTagList_const_iterator p = _in_distribs.begin();
		 p != _in_distribs.end(); ++p )
		if (*p == dist)
			return true;
	return false;
}

bool Package::is_in_distrib( const DistTagList& distl )
{
	for( DistTagList_const_iterator p = _in_distribs.begin();
		 p != _in_distribs.end(); ++p )
		for( DistTagList_const_iterator q = distl.begin();
			q != distl.end(); ++q )
			if (*p == *q)
				return true;
	return false;
}


// Local Variables:
// tab-width: 4
// End:
#endif

/*
bool Package::is_in_distrib( const DistTag& dist )
{
	for( DistTagList_const_iterator p = _in_distribs.begin();
		 p != _in_distribs.end(); ++p )
		if (*p == dist)
			return true;
	return false;
}

bool Package::is_in_distrib( const DistTagList& distl )
{
	for( DistTagList_const_iterator p = _in_distribs.begin();
		 p != _in_distribs.end(); ++p )
		for( DistTagList_const_iterator q = distl.begin();
			q != distl.end(); ++q )
			if (*p == *q)
				return true;
	return false;
}
*/


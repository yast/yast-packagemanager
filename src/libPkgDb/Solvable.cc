#include <iostream>
#include <y2pm/Solvable.h>

using namespace std;

Solvable::Solvable()
{
}

Solvable::Solvable(
	PkgName& name,
	PkgEdition& edition,
	PkgRelList_type& requires,
	PkgRelList_type& conflicts,
	PkgRelList_type& provides,
	PkgRelList_type& obsoletes
	)
{
	_name=name;
	_edition=edition;
	_requires=requires;
	_conflicts=conflicts;
	_provides=provides;
	_obsoletes=obsoletes;
}

Solvable::~Solvable()
{
}

ostream& operator<<( ostream& os, const Solvable& pkg )
{
	os << "Name: " << (const char *)pkg._name << endl;
	os << "Version: " << pkg._edition.version() << endl;
	if (pkg._edition.has_release())
		os << "Release: " << pkg._edition.release() << endl;
	if (pkg._edition.has_epoch())
		os << "Epoch: " << pkg._edition.epoch() << endl;

	if (pkg._requires.size())
		cout << "Requires: " << pkg._requires << endl;
	if (pkg._conflicts.size())
		cout << "Conflicts: " << pkg._conflicts << endl;
	if (pkg._provides.size())
		cout << "Provides: " << pkg._provides << endl;
	if (pkg._obsoletes.size())
		cout << "Obsoletes: " << pkg._obsoletes << endl;
	
//	os << "Size: " << pkg._size << endl;

#if 0
	for( unsigned i = 0; i < Package::N_STRS; ++i ) {
		if (!pkg.strings[i] || pkg.strings[i]->empty())
			continue;
		// we need a named object for the tag name here, because egcs-2.91.66
		// otherwise gets confused with the temporary string and a SIGSEGV is
		// the result.
		string tagn = Tag::tagname(Package::str_tags[i]);
		os << tagn << ": " << modify_newlines(*pkg.strings[i]) << endl;
	}
	for( unsigned i = 0; i < Package::N_BINS; ++i ) {
		if (!pkg.binfields[i] || !pkg.binfields[i]->contents)
			continue;
		// we need a named object for the tag name here, because egcs-2.91.66
		// otherwise gets confused with the temporary string and a SIGSEGV is
		// the result.
		string tagn = Tag::tagname(Package::bin_tags[i]);
		os << tagn << ": "<< hex << setfill('0');
		for( unsigned j = 0; j < pkg.binfields[i]->size; ++j ) {
			os << setw(2) << (unsigned)(pkg.binfields[i]->contents[j]);
		}
		os << dec << setfill(' ') << endl;
	}
#endif
	return os;
}

/*
ostream& operator<<( ostream& os, const DistTagList& dl )
{
	for( DistTagList_const_iterator q = dl.begin(); q != dl.end(); ++q ){
		if (q != dl.begin())
			cout << ", ";
		cout << (const char *)*q;
	}
	return os;
}
*/
ostream& operator<<( ostream& os, const Solvable::PkgRelList_type& rl )
{
	for( Solvable::PkgRelList_const_iterator q = rl.begin();
		 q != rl.end(); ++q ){
		if (q != rl.begin())
			cout << ", ";
		cout << *q;
	}
	return os;
}

const PkgRelation& Solvable::add_requires( PkgRelation& r)
{
	_requires.push_front(r);
	return *(_requires.begin());
}

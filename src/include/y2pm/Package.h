#ifndef _Package_h
#define _Package_h

#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <y2pm/Solvable.h>
#include <y2pm/PkgName.h>
#include <y2pm/PkgEdition.h>
//#include <DistTag.h>
#include <y2util/RefObject.h>
#include <y2pm/PackageSource.h>

class PackageDataProvider;

class Package : public Solvable
{
    public:
	typedef std::vector<std::string> FileList_type;
	typedef FileList_type::iterator FileList_iterator;
	typedef FileList_type::const_iterator FileList_const_iterator;

    private:
	std::list<PackageDataProvider*> _dataproviders;
	
	// file list
	FileList_type _files;

    public:
	Package() : Solvable() {};
	Package(
	    PkgName& name,
	    PkgEdition& edition,
	    PkgRelList_type& requires,
	    PkgRelList_type& conflicts,
	    PkgRelList_type& provides,
	    PkgRelList_type& obsoletes
	    ) : Solvable ( name, edition, requires, conflicts, provides, obsoletes) {};
	
	void print( std::ostream& os ) const;

	void addDataProvider( PackageDataProvider* provider)
	{
	    _dataproviders.push_front(provider);
	}
	bool is_from_provider( PackageDataProvider* provider );
	const FileList_type& files() const { return _files; }
//	const PackageSource* source() const { return _source; }

};

std::ostream& operator<<( std::ostream& os, const Package& pkg );
   	
#endif  /* _Package_h */


// Local Variables:
// tab-width: 4
// End:
// vim:sw=4

#ifndef _Package_h
#define _Package_h

#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <y2pm/PackageSource.h>
#include <y2pm/PkgName.h>
#include <y2pm/PkgEdition.h>
#include <y2pm/PkgRelation.h>
//#include <DistTag.h>
#include <y2util/RefObject.h>

class PkgRelation;
class PackageDataProvider;

// Package is the class that is stored in the package pool.
class Package {
  public:
	typedef std::list<PkgRelation> PkgRelList_type;
	typedef PkgRelList_type::iterator PkgRelList_iterator;
	typedef PkgRelList_type::const_iterator PkgRelList_const_iterator;

	typedef std::vector<std::string> FileList_type;
	typedef FileList_type::iterator FileList_iterator;
	typedef FileList_type::const_iterator FileList_const_iterator;
	
	typedef RefObject<Package> Ref;

	// iterates through all provided names: the package name itself and all
	// actually provided names (always const)
	friend class Provides_iterator;
	class Provides_iterator {
		const Package *pkg;
		PkgRelList_const_iterator iter;

	  public:
		Provides_iterator( const Package *p ) : pkg(p) {}
		Provides_iterator( PkgRelList_const_iterator i )
			: pkg(NULL), iter(i) {}

		PkgRelation operator* () const {
			return pkg ? PkgRelation(pkg->name(), EQ, pkg->edition()) : *iter;
		}
		
		Provides_iterator& operator++ () {
			if (pkg) {
				iter = pkg->provides_begin();
				pkg = NULL;
			}
			else
				++iter;
			return *this;
		}
		Provides_iterator operator++ (int) {
			Provides_iterator temp = *this;
			operator++();
			return temp;
		}
		
		bool operator== ( const Provides_iterator& i2 ) {
			return pkg == i2.pkg && iter == i2.iter;
		}
		bool operator!= ( const Provides_iterator& i2 ) {
			return !operator==(i2);
		}
	};
	typedef Provides_iterator Provides_const_iterator;
	
  private:

  	// name and edition
	PkgName _name;
	PkgEdition _edition;
	// list of distributions to which this package belongs
//	DistTagList _in_distribs;
	// file and offset from where the data came for reloading
//	PackageSource *_source;
	std::list<PackageDataProvider*> _dataproviders;
	// relations of the package
	//TODO
	PkgRelList_type _requires, _conflicts, _provides, _obsoletes;
	// file list
	FileList_type _files;

	Package( const Package& );
	Package operator= ( const Package& );
    public:
	Package();
	Package(
	    PkgName& name,
	    PkgEdition& edition,
	    PkgRelList_type& requires,
	    PkgRelList_type& conflicts,
	    PkgRelList_type& provides,
	    PkgRelList_type& obsoletes
	    );
	~Package();

	// print package data in ASCII summary format
	friend std::ostream& operator<<( std::ostream&, const Package& );

/*
	// add a DistTag to in_distribs
	void add_dist( const DistTag& tag ) {
		_in_distribs.push_front( tag );
	}
*/
	void addDataProvider( PackageDataProvider* provider)
	{
	    _dataproviders.push_front(provider);
	}
	// check if package has the DistTag 'tag' attached
//	bool is_in_distrib( const DistTag& dist );
//	bool is_in_distrib( const DistTagList& distl );
	bool is_from_provider( PackageDataProvider* provider );

	// add a provides:
	const PkgRelation& add_provides( PkgName name ) {
		_provides.push_front( PkgRelation( name, EQ, PkgEdition(PkgEdition::UNSPEC) ));
		return *(_provides.begin());
	}
	const PkgRelation& add_provides( const char *name ) {
		return add_provides( PkgName(name) ); }

	const PkgRelation& add_requires(PkgRelation& e);

	// iterator for stepping through all provided names (including the
	// auto-providing of the package name)
	Provides_iterator all_provides_begin() const {
		return Provides_iterator(this);
	}
	Provides_iterator all_provides_end() const {
		return Provides_iterator( provides_end() );
	}
	PkgRelation self_provides() const {
		return PkgRelation( _name, EQ, _edition );
	}
	
	// access methods for components
	const PkgName& name() const { return _name; }
	const PkgEdition& edition() const { return _edition; }
//	const DistTagList& in_distribs() const { return _in_distribs; }
//	const PackageSource* source() const { return _source; }
	const PkgRelList_type& requires() const { return _requires; }
	const PkgRelList_type& conflicts() const { return _conflicts; }
	const PkgRelList_type& provides() const { return _provides; }
	const PkgRelList_type& obsoletes() const { return _obsoletes; }
	const FileList_type& files() const { return _files; }

	void setName(PkgName& n) { _name = n; };

	// for convenience: directly return an iterator for relation lists
#define decl_PkgRelList_iterators(name)					\
	PkgRelList_iterator name##_begin() {				\
		return _##name.begin(); }						\
	PkgRelList_const_iterator name##_begin() const {	\
		return _##name.begin(); }						\
	PkgRelList_iterator name##_end() {					\
		return _##name.end(); }							\
	PkgRelList_const_iterator name##_end() const {		\
		return _##name.end(); }

	decl_PkgRelList_iterators(requires)
	decl_PkgRelList_iterators(conflicts)
	decl_PkgRelList_iterators(provides)
	decl_PkgRelList_iterators(obsoletes)
};

// some aux output operators
std::ostream& operator<<( std::ostream&, const Package::PkgRelList_type& );
//ostream& operator<<( ostream&, const DistTagList& );

    	
#endif  /* _Package_h */


// Local Variables:
// tab-width: 4
// End:
// vim:sw=4

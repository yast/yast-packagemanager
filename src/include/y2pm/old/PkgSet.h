#ifndef _PkgSet_h
#define _PkgSet_h

#include <list>
#include <y2util/hash.h>
#include <y2util/RefObject.h>
#include <y2pm/Solvable.h>
#include <y2pm/PkgRevRel.h>


class PkgSet {
  public:
	typedef hash<PkgName,const Solvable*> PkgList_type;
	typedef PkgList_type::iterator iterator;
	typedef PkgList_type::const_iterator const_iterator;

	typedef std::list<PkgRevRelation> RevRelList_type;
	typedef RevRelList_type::iterator RevRelList_iterator;
	typedef RevRelList_type::const_iterator RevRelList_const_iterator;

	typedef hash<PkgName,RevRelList_type> InvRel_type;
	typedef InvRel_type::iterator InvRel_iterator;
	typedef InvRel_type::const_iterator InvRel_const_iterator;

	typedef RefObject<PkgSet> Ref;

  private:
	// list of pointers to package contained in this set (the pointers go into
	// the pool)
	PkgList_type contents;
	// reverse package relations
	InvRel_type _required;
	InvRel_type _conflicted;
	InvRel_type _provided;
	InvRel_type _obsoleted;

  public:
	PkgSet() {}
	// make a set consisting of all packages with one of the given tags
//	PkgSet( const DistTagList& tags );

//	PkgSet( PackageDataProvider* provider );
	~PkgSet();
	// default copy constructor and assigment are ok
	//
	bool empty() { return contents.empty(); }

	// add a single package
	void add( const Solvable *pkg, bool force = false );
	// remove a package
	void remove( const Solvable *pkg );
	void remove( PkgName name ) {
		const Solvable *pkg = lookup(name);
		if (pkg)
			remove( pkg );
	}

	// notification from PkgPool that a pkg has a new provides
	void new_provides( const Solvable *pkg, const PkgRelation& prov );

	// look up a package by name
	const Solvable *lookup( PkgName name ) const {
		const HashElt<PkgName,const Solvable *> *p = contents.find(name);
		return p ? p->value : NULL;
	}
	const Solvable *operator[] ( PkgName name ) const { return lookup(name); }
	// package contained in this set?
	bool includes( const PkgName& name ) const {
		return contents.exists(name);
	}

	// iterator (steps through all contained packages)
	iterator begin() { return contents.begin(); }
	iterator end() { return contents.end(); }
	const_iterator begin() const { return contents.begin(); }
	const_iterator end() const { return contents.end(); }

	// member access methods
	const InvRel_type& required() const { return _required; }
	const InvRel_type& conflicted() const { return _conflicted; }
	const InvRel_type& provided() const { return _provided; }
	const InvRel_type& obsoleted() const { return _obsoleted; }

	// for convenience: directly return an iterator
#define decl_InvRel_iterators(name)					\
	InvRel_iterator name##_begin() {				\
		return _##name.begin(); }					\
	InvRel_const_iterator name##_begin() const {	\
		return _##name.begin(); }					\
	InvRel_iterator name##_end() {					\
		return _##name.end(); }						\
	InvRel_const_iterator name##_end() const {		\
		return _##name.end(); }

	decl_InvRel_iterators(required)
	decl_InvRel_iterators(conflicted)
	decl_InvRel_iterators(provided)
	decl_InvRel_iterators(obsoleted)
};

#endif  /* _PkgSet_h */


// Local Variables:
// tab-width: 4
// End:

#ifndef _PkgSet_h
#define _PkgSet_h

#include <list>
#include <map>
#include <y2util/hash.h>
#include <y2pm/PMSolvablePtr.h>
#include <y2pm/PkgRevRel.h>

inline size_t hashfun( const PkgName & ustr_r ) {
    return hashfun( (const Ustring &)ustr_r );
}

/** A PkgSet represents an unordered collection of packages. This
may be e.g. a set of currently installed packages, to be installed packages, or
ones that are available for installation. A package name must be unique in a
PkgSet, i.e. it is not possible to include more than one version of a package

<p>

Additionally to compounding packages, has one more important feature for
dependency checking: It maintains reverse indices of all package relations.
I.e., you can ask the set what packages (e.g.) require or provide a certain
name. These reverse indices are the core of an efficient dependency and
consistency checking.
 * */

class PkgSet {
  public:
	typedef std::map<PkgName,PMSolvablePtr> PkgList_type;
	typedef PkgList_type::iterator iterator;
	typedef PkgList_type::const_iterator const_iterator;

	typedef std::list<PkgRevRelation> RevRelList_type;
	typedef RevRelList_type::iterator RevRelList_iterator;
	typedef RevRelList_type::const_iterator RevRelList_const_iterator;

	typedef std::map<PkgName,RevRelList_type> InvRel_type;
	typedef InvRel_type::iterator InvRel_iterator;
	typedef InvRel_type::const_iterator InvRel_const_iterator;

	typedef PMSolvable::PkgRelList_type& (*getAdditionalProvides_callback)( constPMSolvablePtr& ptr );

	typedef std::list<constPMSolvablePtr> SolvableList;

  private:
	// list of pointers to package contained in this set (the pointers go into
	// the pool)
	PkgList_type contents;
	// reverse package relations
	InvRel_type _required;
	InvRel_type _conflicted;
	InvRel_type _provided;
	InvRel_type _obsoleted;

	getAdditionalProvides_callback _additionalprovides_callback;

  public:
	// default copy constructor and assigment are ok

	PkgSet();

	~PkgSet();

	bool empty() const;

	unsigned size() const;

	/** Adds a package to the set. If force is false, the package is added
	 * only if no equal or newer edition is already member of the set.
	 * This is handy most of the time, but can be overridden by passing
	 * true as second parameter. Then pkg is always added, replacing a
	 * previous member of the set with the same name.  */
	void add( PMSolvablePtr pkg, bool force = false );
	
	/** remove a package
	 *
	 * Note: it does not matter if pkg is actually the pointer stored. The
	 * item to be deleted is identified by name.
	 * */
	void remove( PMSolvablePtr pkg );

	/** remove a package by name */
	void remove( PkgName name );

	/* notification from PkgPool that a pkg has a new provides
	 <b>obsolete</b>
	 */
	void new_provides( PMSolvablePtr pkg, const PkgRelation& prov );

	/** look up a package by name
	 *
	 * @return NULL if package is not in set
	 * */
	PMSolvablePtr lookup( const PkgName& name ) const;
	
	PMSolvablePtr operator[] ( const PkgName& name ) const { return lookup(name); }
	/**
	 * package contained in this set?
	 * */
	bool includes( const PkgName& name ) const;

	// iterator (steps through all contained packages)
	iterator begin() { return contents.begin(); }
	iterator end() { return contents.end(); }
	const_iterator begin() const { return contents.begin(); }
	const_iterator end() const { return contents.end(); }

	/**
	 * The required, conflicted, provided and obsoleted methods return the
	 * reverse indices for packages relations. The returned value is a hash
	 * that can be indexed by a PkgName. For example, to learn the names of
	 * all packages that require foo:
	 * <code>
	 * <pre>
	 * PkgSet pset;
	 * // ...
	 * const PkgSet::RevRelList_type& requires_foo = getRevRelforPkg(pset.required(),PkgName("foo"));
	 * for( PkgSet::RevRelList_const_iterator p = requires_foo.begin();
	 *    p != requires_foo.end(); ++p ) {
	 * cout << *p->pkg()->name << endl;
	 * }
	 * </pre>
	 * </code>
	 * */

	const InvRel_type& required() const { return _required; }
	const InvRel_type& conflicted() const { return _conflicted; }
	const InvRel_type& provided() const { return _provided; }
	const InvRel_type& obsoleted() const { return _obsoleted; }

	static const RevRelList_type& getRevRelforPkg(const InvRel_type&, const PkgName&);

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

	/**
	 * set callback function which is called when Packages are added to this set
	 * */
	void setAdditionalProvidesCallback( getAdditionalProvides_callback callback )
	{
	    _additionalprovides_callback = callback;
	}

	/**
	 * return current callback
	 * */
	getAdditionalProvides_callback AdditionalProvidesCallback() const
	{
	    return _additionalprovides_callback;
	}

	std::ostream& dumpOn(std::ostream& os);
};

#endif  /* _PkgSet_h */


// Local Variables:
// tab-width: 4
// End:

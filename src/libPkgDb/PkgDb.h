#ifndef _PkgDb_h
#define _PkgDb_h

// disabled overrides as they are dbheader dependant -- ln

#include <hash.h>
#include <PkgName.h>
#include <Package.h>
#include <PackageSource.h>

size_t hashfun( const PkgNameEd& );
class PkgSet;

class PkgDb {
  public:
	typedef hash<PkgNameEd,Package*> PkgPool_type;
	typedef PkgPool_type::iterator iterator;
	typedef PkgPool_type::const_iterator const_iterator;
/*
	typedef hash<PkgNameEd,DbHeader*> PkgOverride_type;
	typedef PkgOverride_type::iterator PkgOverride_iterator;
	typedef PkgOverride_type::const_iterator PkgOverride_const_iterator;
*/
	typedef list<const char *> ReqFiles_type;
	typedef ReqFiles_type::iterator ReqFiles_iterator;
	typedef ReqFiles_type::const_iterator ReqFiles_const_iterator;

	typedef list<PkgName> AltDefaultList;
	typedef AltDefaultList::iterator AltDefaultList_iterator;
	typedef AltDefaultList::const_iterator AltDefaultList_const_iterator;
	typedef hash<PkgName,AltDefaultList> AltDefault_type;
	
  private:
	// the package pool itself
	PkgPool_type Pool;
	// global override list
//	PkgOverride_type Overrides;
	// list of required files
	ReqFiles_type StaticRequiredFiles; // from /usr/lib/PkgDb/required-files
	ReqFiles_type AddRequiredFiles;    // additions by imports
	// hash of alternative defaults
	AltDefault_type alt_defaults;
	
	// list of PkgSet using the pool (for new_provides notifications)
	list<PkgSet*> attached_sets;

	// constructor helpers
	void read_override_list();
	void read_static_reqfiles();
	void read_alt_defaults();
	
	// required files list helpers
	void recheck_pool_for_ReqFile( const char *rf );
	void check_new_ReqFiles( Package *pkg, const ReqFiles_type& RF );
	void check_one_ReqFile( Package *pkg, const char *rf );
	void notify_sets_of_new_provides( const Package *pkg,
									  const PkgRelation& new_p );
	
  public:

	PkgDb();
	// default copy constructor and assigment are ok

	// feed packages into the pool
//	void add_source( const char *path, DistTag tag = NONE_Tag );
//	void add_installed_packages() { add_source( "", INSTALLED_Tag ); }
	
	// add a single package if it isn't already present; apply overrides if
	// ones exist
	void add_package( Package *pkg, PackageDataProvider*);
	
	// return package for specific index (name+edition)
	const Package* get( const PkgNameEd& ne ) {
		if (!Pool.exists(ne))
			return NULL;
		return Pool[ne];
	}
	const Package* get( const PkgName& n, const PkgEdition& e ) {
		PkgNameEd ne( n, e );
		if (!Pool.exists(ne))
			return NULL;
		return Pool[ne];
	}
	const Package* operator[] ( const PkgNameEd& ne ) { return get(ne); }

	// iterator (steps through all contained packages)
	iterator begin() { return Pool.begin(); }
	iterator end() { return Pool.end(); }

	// give read-only access to list of required files
	const ReqFiles_type& static_required_files() const
		{ return StaticRequiredFiles; }
/*
	// look up an override
	const DbHeader* get_override( PkgNameEd ne ) {
		if (Overrides.exists( ne ))
			return Overrides[ne];
		else
			return NULL;
	}
	const DbHeader* get_override( PkgName n, PkgEdition e ) {
		return get_override( PkgNameEd( n, e ));
	}
*/
	// look up an alternative default
	AltDefaultList alternative_default( PkgName name ) {
		if (alt_defaults.exists(name))
			return alt_defaults[name];
		else
			return AltDefaultList();
	}

	// maintain alternative defaults list
	void clear_alternative_defaults() { alt_defaults = AltDefault_type(); }
	void add_alternative_defaults( const PkgName& name,
								   const AltDefaultList& defaults ) {
		alt_defaults[name] = defaults;
	}
	
	// attach and detach PkgSets
	void attach_set( PkgSet *set ) {
		attached_sets.push_back( set );
	}
	void detach_set( PkgSet *set ) {
		attached_sets.remove( set );
	}
};

extern PkgDb PkgPool;

#endif  /* _PkgDb_h */

// Local Variables:
// tab-width: 4
// End:

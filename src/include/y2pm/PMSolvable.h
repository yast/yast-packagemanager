/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       PMSolvable.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMSolvable_h
#define PMSolvable_h

#include <iosfwd>
#include <list>

#include <y2pm/PMSolvablePtr.h>

#include <y2pm/PkgName.h>
#include <y2pm/PkgEdition.h>
#include <y2pm/PkgArch.h>
#include <y2pm/PkgRelation.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSolvable
/**
 *
 * @short Contains everything needed for dependency solving.
 **/
class PMSolvable : virtual public Rep {
  REP_BODY(PMSolvable);

  public:

    typedef std::list<PkgRelation>          PkgRelList_type;
    typedef PkgRelList_type::iterator       PkgRelList_iterator;
    typedef PkgRelList_type::const_iterator PkgRelList_const_iterator;

  public:

    ///////////////////////////////////////////////////////////////////
    //
    //	CLASS NAME : Provides_iterator
    /**
     *
     **/
    class Provides_iterator {

      private:

	constPMSolvablePtr        pkg;
	PkgRelList_const_iterator iter;

      public:

	Provides_iterator( constPMSolvablePtr p )
	  : pkg( p )
	{}

	Provides_iterator( PkgRelList_const_iterator i )
	  : iter( i )
	{}

	PkgRelation operator* () const {
	      return (pkg!= NULL) ? PkgRelation(pkg->name(), EQ, pkg->edition()) : *iter;
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

	Provides_iterator  operator++ (int) {
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

  protected:

    // name, edition, and architecture
    PkgName _name;
    PkgEdition _edition;
    PkgArch _arch;

    // relations of the package
    PkgRelList_type _requires, _conflicts, _provides, _obsoletes;

  public:

    /**
     * constructor
     * @param PkgName name
     * @param PkgEdition edition
     * @param PkgArch arch
     */
    PMSolvable( const PkgName& name,
		const PkgEdition& edition,
		const PkgArch& arch);

    virtual ~PMSolvable();

  public:

    /**
     * print package data in ASCII summary format
     */
    virtual std::ostream & dumpOn( std::ostream & str ) const;

    /**
     * add a provides: by PkgName
     */
    const PkgRelation& addProvides(const PkgName name ) {
      _provides.push_front( PkgRelation( name, EQ, PkgEdition(PkgEdition::UNSPEC) ));
      return *(_provides.begin());
    }

    /**
     * add a provides: by const char *
     */
    const PkgRelation& addProvides( const char *name ) {
      return addProvides( PkgName(name) );
    }

    /**
     * set provides list
     */
    const PkgRelList_type& setProvides(const PkgRelList_type& provides)
    {
      _provides = provides;
      return _provides;
    }

    /**
     * set requires list
     */
    const PkgRelList_type& setRequires(const PkgRelList_type& requires)
    {
      _requires = requires;
      return _requires;
    }

    /**
     * add prerequires flag for requires
     *
     * @param prerequires list of relations in requires that have to be marked
     * as prerequires
     *
     * @return reference to requires
     */
    const PkgRelList_type& addPreRequires(PkgRelList_type& prerequires);

    /**
     * set obsoletes list
     */
    const PkgRelList_type& setObsoletes(const PkgRelList_type& obsoletes)
    {
      _obsoletes = obsoletes;
      return _obsoletes;
    }

    /**
     * set conflicts list
     */
    const PkgRelList_type& setConflicts(const PkgRelList_type& conflicts)
    {
      _conflicts = conflicts;
      return _conflicts;
    }

    /**
     * add a requires relation to the front
     */
    const PkgRelation& addRequires(const PkgRelation& r) {
      _requires.push_front(r);
      return *(_requires.begin());
    }

    /**
     * iterator for stepping through all provided names (including the
     * auto-providing of the package name)
     */
    Provides_iterator all_provides_begin() const {
      return Provides_iterator(constPMSolvablePtr(this));
    }
    Provides_iterator all_provides_end() const {
      return Provides_iterator( provides_end() );
    }
    PkgRelation self_provides() const {
      return PkgRelation( _name, EQ, _edition );
    }

    /**
     * check if Solvable provides a relation
     *
     * @param rel Relation to check for
     *
     * @return true if this packages provides relation, false otherwise
     * */
    bool doesProvide(const PkgRelation& rel) const;

    /**
     * access methods for components
     */
    const PkgName& name() const { return _name; }
    const PkgEdition& edition() const { return _edition; }
    const PkgArch& arch() const { return _arch; }

    /**
     * helper functions for edition
     */
    const std::string & version() const { return _edition.version(); }
    const std::string & release() const { return _edition.release(); }

    /**
     * Returns "name-version-release" string
     **/
    std::string nameEd() const { return (const std::string &)_name + '-' + _edition.asString(); }

    /**
     * Returns "name-version-release.arch" string
     **/
    std::string nameEdArch() const { return nameEd() + '.' + (const std::string &)_arch; }

    /**
     * access methods for dependencies
     * use PkgRelList2StringList to convert them to list<string>
     */
    const PkgRelList_type& requires() const { return _requires; }
    PkgRelList_type prerequires() const;
    const PkgRelList_type& conflicts() const { return _conflicts; }
    const PkgRelList_type& provides() const { return _provides; }
    const PkgRelList_type& obsoletes() const { return _obsoletes; }


    static std::list<std::string> PkgRelList2StringList ( const PkgRelList_type & rellist_r );
    static PkgRelList_type StringList2PkgRelList (const std::list<std::string>& relationlist);

    // for convenience: directly return an iterator for relation lists
#define decl_PkgRelList_iterators(name)					\
	PkgRelList_iterator name##_begin() {				\
		return _##name.begin(); }				\
	PkgRelList_const_iterator name##_begin() const {		\
		return _##name.begin(); }				\
	PkgRelList_iterator name##_end() {				\
		return _##name.end(); }					\
	PkgRelList_const_iterator name##_end() const {			\
		return _##name.end(); }

    decl_PkgRelList_iterators(requires)
    decl_PkgRelList_iterators(conflicts)
    decl_PkgRelList_iterators(provides)
    decl_PkgRelList_iterators(obsoletes)
};

///////////////////////////////////////////////////////////////////

extern std::ostream & operator<<( std::ostream&, const PMSolvable::PkgRelList_type & );

///////////////////////////////////////////////////////////////////

#endif // PMSolvable_h


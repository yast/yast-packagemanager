/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                     (C) 2002 SuSE AG |
\----------------------------------------------------------------------/

   File:       PkgDep.h
   Purpose:    All functions for solving dependences
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

   copied and modified from phi
/-*/

#ifndef _PkgDep_h
#define _PkgDep_h

#include <iostream>
#include <list>
#include <deque>

#include <y2util/hash.h>

#include <y2pm/PkgName.h>
#include <y2pm/PkgEdition.h>
#include <y2pm/PkgRevRel.h>
#include <y2pm/PkgSet.h>
#include <y2pm/Alternatives.h>

/**
 * the solver
 * */
class PkgDep {

	// ------------------------------ types ------------------------------

  public:
	enum alternative_kind {
		SIMPLE, REQUIRES_MORE, CONFLICT
	};
	enum alternatives_mode {
		ASK_ALWAYS, ASK_IF_NO_DEFAULT, AUTO_IF_NO_DEFAULT, AUTO_ALWAYS
	};

	/** RelInfo, Alternative, and NeededEditionRange are for returning results */
	struct RelInfo {

		enum Kind {
			REQUIREMENT = 0, CONFLICT, OBSOLETION
		};

		/**
		 * name of package causing the relation
		 * */
		PkgName name;
		/**
		 * copy of the relation (if rel.name() is different
		 * from name, then name provides rel.name)
		 * */
		PkgRelation rel;

		/**
		 * Tells you what this relation means
		 * */
		
		Kind kind;

		/**
		 * PMSolvablePtr if applicable.
		 * */
		PMSolvablePtr solvable;

		RelInfo( PMSolvablePtr s, PkgRelation r, Kind k = REQUIREMENT )
			: name(s->name()), rel(r),
			kind(k), solvable(s)
			{}
		RelInfo( PkgName n, PkgRelation r, Kind k = REQUIREMENT, PMSolvablePtr s = NULL )
			: name(n), rel(r), kind(k), solvable(s)
			{}
		RelInfo( PkgRevRelation r, Kind k = REQUIREMENT )
			: name(r.pkg()->name()), rel(r.relation()),
			kind(k), solvable(r.pkg())
			{}
	};

	struct Alternative {
		PMSolvablePtr solvable;
		alternative_kind kind;

		Alternative( PMSolvablePtr p, alternative_kind k )
			: solvable(p), kind(k) {}
	};

	typedef Alternatives::AltDefaultList (*AlternativesCallback)( PkgName name );

	/**
	 * For unresolvable packages, the edition requirements are merged in this
	 * structure and returned. If either 'greater' or 'less' are UNSPEC,
	 * there's no requirement in this direction. If both are UNSPEC, any
	 * edition will satisfy the requirements. Otherwise, a satisfying edition
	 * must be greater than 'greater' and less than 'less'.
	 * The {le,gr}_incl fields are true if the ranges are meant inclusive at
	 * the resp. border.
	 * 'impossible' is true if greater > less, i.e. there are contradicting
	 * requirements.
	 * */
	struct NeededEditionRange {
		PkgEdition greater;
		PkgEdition less;
		bool less_incl : 1;
		bool greater_incl : 1;

		NeededEditionRange()
			: greater(PkgEdition::UNSPEC), less(PkgEdition::UNSPEC),
			  less_incl(false), greater_incl(false) {}
		bool allows_any() const {
			return greater.is_unspecified() && less.is_unspecified();
		}
		bool impossible() const {
			return( (less < greater) ||
					(less == greater && (!less_incl || !greater_incl)) );
		}
		void merge( const PkgRelation& rel );
	};

	typedef std::list<PkgName> NameList;
	typedef NameList::iterator NameList_iterator;
	typedef NameList::const_iterator NameList_const_iterator;

	typedef std::list<RelInfo> RelInfoList;
	typedef RelInfoList::iterator RelInfoList_iterator;
	typedef RelInfoList::const_iterator RelInfoList_const_iterator;

	typedef std::list<PMSolvablePtr> SolvableList;

	struct Notes;

	/** This is the basic return type for packages that could be
	 * processed without problems. It contains further
	 * information about the package
	 * */
	struct Result
	{
		/** name, copy of solvable->name if solvable != 0 */
		PkgName name;
		/** edition, copy solvable->edition if solvable != 0 */
		PkgEdition edition;

		/**
		 * pointer to affected solvable, may be NULL if there is none
		 * */
		PMSolvablePtr solvable;

		/**
		 * A list of packages that require the package
		 * described by the Result. Please note that a
		 * referer is remembered only if no installed
		 * package would have satisfied the requirement,
		 * too.
		 * */
		RelInfoList referers;
		/**
		 * This is the installed version of the package if
		 * the installed version is less. If the package
		 * isn't installed yet, the value is the unspecified
		 * version.
		 * */
		PkgEdition is_upgrade_from;
		/**
		 * This is the installed version of the package if
		 * the installed ver-sion is greater. If the package
		 * isn't installed yet, the value is the unspecified
		 * version.
		 * */
		PkgEdition is_downgrade_from;
		/**
		 * true if this package was passed in the input list (i.e.,
		 * hasn't been added by PkgDep)
		 * */
		bool from_input_list : 1;
		/**
		 * This flag is set if the package has been upgraded
		 * to resolve a conflict with a new package that
		 * would have existed otherwise.
		 * */
		bool upgrade_to_remove_conflict : 1;
		/**
		 * this package has been installed to avoid a broken
		 * requirement by another upgrade/obsoletion
		 * */
		bool install_to_avoid_break : 1;

		/**
		 * construct Result.
		 * is_upgrade_from and is_downgrade_from will be calculated
		 * */
		Result(const PkgDep& pkgdep, PMSolvablePtr pkg);

		/** construct basic Result */
		Result(const PkgDep& pkgdep, const PkgName& name);
		void add_notes( const Notes& notes );
	};

	/**
	 * The ErrorResult structure is derived from Result and thus
	 * also contains its fields. It additionally carries
	 * information about errors that happened
	 * */
	struct ErrorResult : public Result {
		/**
		 * true if this pkg is required but not available,
		 * not_avail_range gives details
		 * */
		bool not_available;
		/**
		 * This field gives details about the editions that
		 * would be needed to satisfy the requirements. For
		 * interpretation, see the description of
		 * NeededEditionRange above. It s meaningful only if
		 * not_available is set. Otherwise,
		 * not_avail_range.allows_any() is true.
		 * */
		NeededEditionRange not_avail_range;
		/**
		 * This lists the requirements of the package that
		 * could not be resolved. The RelInfos can never be
		 * conflicts. For each missing package, there s a
		 * separate entry in the error result list with
		 * not_available set.
		 * */
		RelInfoList unresolvable;
		/**
		 * If this list is not empty, it means that the
		 * package is a virtual package and there is more
		 * than one real package that provides it, and that
		 * the alternative has not been solved
		 * automatically. The Alternatives give the name of
		 * each possible alternative and its kind (simple,
		 * requires more, generates a conflict).
		 * */
		std::list<Alternative> alternatives;

		/**
		 * This field lists all kinds of conflicts of the
		 * package with installed packages or other packages
		 * to be installed. The RelInfos can be requirements
		 * conflicts or obsoletions. An entry can be generated if:
		 *
		 * <ul>
		 * 
		 * <li>if  an already installed package conflicts
		 * with the new one</li>
		 *
		 * <li>if the new package conflicts with an
		 * installed one</li>
		 *
		 * <li>if a previously satisfied requirement of an
		 * installed package is broken by installing
		 * (probably upgrading) this package (in this case
		 * kind in the RelInfo is CONFLICT)</li>
		 *
		 * <li>if an already installed package obsoletes
		 * this one (kind = OBSOLETION, together with
		 * state_change_not_possible<li>
		 *
		 * <li>if this package obsoletes an installed one
		 * (kind = OBSOLETION, together with
		 * state_change_not_possible<li>
		 * 
		 * </ul>
		 * */

		RelInfoList conflicts_with;
		/**
		 * This is an auxiliary field to conflicts_with. The
		 * conflicts disappear if all the packages listed
		 * would be removed. This list is generated by a
		 * rather simple algorithm and does not have any
		 * knowledge about the installation. If something
		 * conflicts with libc, expect that the list
		 * contains nearly all installed packages :-)
		 * */
		SolvableList remove_to_solve_conflict;

		ErrorResult(const PkgDep& pkgdep, PMSolvablePtr pkg)
			: Result(pkgdep,pkg), not_available(false), state_change_not_possible(false) {}
		ErrorResult(const PkgDep& pkgdep, const PkgName& name)
			: Result(pkgdep,name), not_available(false), state_change_not_possible(false) {}
		ErrorResult(const Result& res)
			: Result(res), not_available(false), state_change_not_possible(false) {}

		void add_unresolvable( PMSolvablePtr s, const PkgRelation& rel );
		void add_conflict( const PkgRevRelation& rrel,
						   const PkgDep& dep,
						   PMSolvablePtr to_remove,
						   PMSolvablePtr assume_instd,
						   RelInfo::Kind kind = RelInfo::CONFLICT );
		/**
		 * This Result has a problem with s and its relation
		 * rel. To solve the conflict, to_remove must be
		 * removed/not installed.
		 * */
		void add_conflict( PMSolvablePtr s, const PkgRelation& rel,
						   const PkgDep& dep,
						   PMSolvablePtr to_remove,
						   PMSolvablePtr assume_instd,
						   RelInfo::Kind kind = RelInfo::CONFLICT );
		void add_alternative( PMSolvablePtr p, alternative_kind k );
		void add_notes( const Notes& notes );

		/**
		 * package should be set to auto install, but the
		 * user's decission prevented that
		 * */
		bool state_change_not_possible;
	};

	friend class Result;
	friend class ErrorResult;
	typedef std::list<Result> ResultList;
	typedef std::list<ErrorResult> ErrorResultList;
	
	enum WhatToDoWithUnresolvable {
	    UNRES_IGNORE = 0, // just ignore this dependence
	    UNRES_TAKETHIS,   // use suggested PMSolvable (has to be returned/filled by function)
	    UNRES_FAIL        // Relation is indeed unresolvable
	};
	
	/**
	 * function to call when an unresolvable dependence is found
	 * <br> yes, the non const PkgDep* is ugly
	 *
	 * @param solver pointer to current solver
	 * @param rel unresolvable dependence
	 * @param PMSolvablePtr PMSolvable which should be assumed to provide this dependence
	 *
	 * @return tells caller what to do with dependence. If return value is
	 * UNRES_TAKETHIS, this function has to point ptr to something useful
	 * */
	typedef WhatToDoWithUnresolvable(*DealWithUnresolvable_callback)(
	    PkgDep* solver, const PkgRelation& rel, PMSolvablePtr& ptr);
	//FIXME make PkgDep* const, requires lots of constPMSolvablePtr everywhere

	DealWithUnresolvable_callback _unresolvable_callback;

  private:

	typedef std::list<PkgRevRelation> RevRelList;
	typedef RevRelList::iterator RevRelList_iterator;
	typedef RevRelList::const_iterator RevRelList_const_iterator;

	enum search_result { NONE, ONE, MULTI };

	struct IRelInfo {
		PMSolvablePtr pkg;
		PkgRelation rel;

		IRelInfo( PMSolvablePtr p, PkgRelation r ) : pkg(p), rel(r) {}
	};

	typedef std::list<IRelInfo> IRelInfoList;
	typedef IRelInfoList::iterator IRelInfoList_iterator;
	typedef IRelInfoList::const_iterator IRelInfoList_const_iterator;

	/** Notes keeps some additional information to candidates */
	struct Notes {
		bool from_input : 1;
		bool upgrade_to_solve_conflict : 1;
		bool install_to_avoid_break : 1;
		bool not_available : 1;
		IRelInfoList referers;
		NeededEditionRange not_avail_range;

		Notes() : from_input(false), upgrade_to_solve_conflict(false),
			install_to_avoid_break(false),
			not_available(false)
			{}
	};

	typedef hash<PkgName,Notes> Notes_type;
	typedef Notes_type::iterator Notes_iterator;
	typedef Notes_type::const_iterator Notes_const_iterator;

	struct AltInfo {
		PMSolvablePtr pkg;
		PkgRelation req;
		RevRelList providers;
		ErrorResult result;
		
		AltInfo( PMSolvablePtr p, const PkgRelation& r, const RevRelList& l,
				 const ErrorResult& rs )
			: pkg(p), req(r), providers(l), result(rs) {}
	};

	typedef std::deque<AltInfo> AltInfoList;
	typedef AltInfoList::iterator AltInfo_iterator;
	typedef AltInfoList::const_iterator AltInfo_const_iterator;
	
	// ---------------------------- static vars ----------------------------
//	static const PkgSet *default_avail;

	// initialisation in utils.cc
	static alternatives_mode default_alternatives_mode;
	static unsigned default_max_remove;

	// --------------------------- instance vars ---------------------------
	alternatives_mode alt_mode;
	PkgSet installed;
	const PkgSet& available;

	AlternativesCallback _alternatives_callback;

	// --------------------- used during an install run --------------------
	PkgSet vinstalled;
	PkgSet *candidates;
	Notes_type notes;
	std::deque<PMSolvablePtr > to_check;
	AltInfoList alts_to_check;
	noval_hash<PkgName> alts_handled;
	ErrorResultList* i_obsoleted; // obsolete packages
	ResultList *good;
	ErrorResultList *bad;

	bool _install_installed;

	// -------------------------- private methods --------------------------
	// install.cc
	void add_package( PMSolvablePtr cand );
	search_result search_for_provider( const PkgRelation& req,
									   PMSolvablePtr referer,
									   ErrorResult *res );
	bool check_for_broken_reqs( PMSolvablePtr oldpkg, PMSolvablePtr newpkg,
								ErrorResult &res );
	bool req_ok_after_upgrade( const PkgRelation& rel, PMSolvablePtr oldpkg,
							   PMSolvablePtr newpkg );
	// alternatives.cc
	void handle_alternative( const AltInfo& alt_info );
	// consistency.cc
	bool pkg_consistent( PMSolvablePtr pkg, ErrorResult *err );
	// remove.cc
	void virtual_remove_package( PMSolvablePtr pkg, SolvableList& to_remove,
								 PMSolvablePtr assume_instd = NULL ) const;
	// utils.cc
	bool also_provided_by_installed( const PkgRelation& rel );
	PMSolvablePtr upgrade_solves_conflict( PMSolvablePtr pkg,
											const PkgRelation& confl );
	PMSolvablePtr try_upgrade_conflictor( PMSolvablePtr pkg,
										   const PkgRelation& provides );
	PMSolvablePtr try_upgrade_conflicted( PMSolvablePtr pkg,
										   const PkgRelation& confl );
	PMSolvablePtr try_upgrade_requirerer( PMSolvablePtr pkg,
										   PMSolvablePtr oldpkg,
										   PMSolvablePtr newpkg );
	PMSolvablePtr available_upgrade( PMSolvablePtr pkg );
	void do_upgrade_for_conflict( PMSolvablePtr upgrade );
	bool has_conflict_with( const PkgRelation& confl, PMSolvablePtr pkg );
	void add_referer( const PkgName& name, PMSolvablePtr referer,
					  const PkgRelation& rel );
	void add_referer( PMSolvablePtr pkg, PMSolvablePtr referer,
					  const PkgRelation& rel ) {
		add_referer( pkg->name(), referer, rel );
	}
	void add_not_available( PMSolvablePtr referer, const PkgRelation& rel );

	// return empty list
	static Alternatives::AltDefaultList default_alternatives_callback( PkgName name )
	{
		return Alternatives::AltDefaultList();
	}

	/**
	 * default unresolvable callback, ignores deps that name begins with rpmlib(
	 * */
	static WhatToDoWithUnresolvable default_unresolvable_callback(
	    PkgDep* solver, const PkgRelation& rel, PMSolvablePtr& p);
	
public:
	PkgDep( PkgSet& instd, const PkgSet& avail,
			AlternativesCallback alternatives_callback = default_alternatives_callback,
			alternatives_mode m = default_alternatives_mode )
		: alt_mode(m), installed(instd), available(avail),
		_alternatives_callback(alternatives_callback)
		{
		    _unresolvable_callback = default_unresolvable_callback;
		}

//	PkgDep(  const PkgSet& instd, alternatives_mode m=default_alternatives_mode)
//		: alt_mode(m), installed(instd), available(*default_avail), _pool(pool) {}
/*	PkgDep( alternatives_mode m = default_alternatives_mode )
		: alt_mode(m), installed(PkgSet(DTagListI0())),
		  available(*default_avail) {}
*/
	/**
	 * install packages (if good result, add candidates to installed set)
	 * */
	bool install( PkgSet& candidates,
			  ResultList& good,
			  ErrorResultList& bad,
			  ErrorResultList& out_obsoleted,
			  bool commit_to_installed = true);
	/** remove a list of packages; the 'pkgs' list will be extended by all
	 * packages that have to be removed, too, to make the installed set
	 * consistent again */
	void remove( SolvableList& pkgs );
	/** check consistency of current installed set */
	bool consistent( ErrorResultList& failures );
	/**
	 * @param candidates which packages to consider for upgrade
	 * @param out_good good list
	 * @param out_bad bad list
	 * @param to_remove which packages will be removed
	 * @param all if true candidates will be extended by
	 * packages with newer versions available
	 * @param none if true, candidates will not be used at all
	 * @param max_remove maximum number of packages that will
	 * automatically be removed
	 * 
	 * DONT USE!
	 * */
	/*
	bool upgrade(	PkgSet&candidates, ResultList& out_good,
			ErrorResultList& out_bad, SolvableList& to_remove,
			bool all = false, bool none = false,
			unsigned max_remove = default_max_remove );
	*/

	/**
	 * just like install but also try to fix inconsistent
	 * installed packages
	 * */
	bool solvesystemnoauto(
		PkgSet &candidates,
		ResultList& out_good,
		ErrorResultList& out_bad,
		ErrorResultList& out_obsoleted);


	/** return current installed set for inspection */
	const PkgSet& current_installed() { return installed; }
	
	/** return current available set */
	const PkgSet& current_available() { return available; }

	// set defaults
/*
	static void set_default_available( const PkgSet& av ) {
		default_avail = &av;
	}
*/
	void set_alternatives_mode(alternatives_mode mode)
	{
		alt_mode = mode;
	}
	
	void set_alternatives_callback(Alternatives::AltDefaultList (*alternatives_callback)( PkgName name ))
	{
		_alternatives_callback = alternatives_callback;
	}

	/**
	 * set function to call when an unresolvable dependence is found
	 *
	 * @see DealWithUnresolvable_callback
	 * */
	void set_unresolvable_callback(DealWithUnresolvable_callback callback)
	{
	    _unresolvable_callback = callback;
	}

	/**
	 * whether to try to solve already installed Packages if
	 * they are in the candidates list for installation
	 * */
	void install_installed(bool yes)
	    { _install_installed = yes; }

	static void set_default_alternatives_mode( alternatives_mode m ) {
		default_alternatives_mode = m;
	}

	static void set_default_max_remove( unsigned mr ) {
		default_max_remove = mr;
	}

    public: // static members

	/** recoursive remove package pkg from PkgSet set and extend
	 * to_remove with all removed packages */
	static void remove_package( PkgSet *set, PMSolvablePtr pkg,
						 SolvableList& to_remove);

	/**
	 * count number of packages providing req in set
	 * */
	static unsigned count_providers_for( const PkgSet* set, const PkgRelation& req );
};

// output.cc
std::ostream& operator<<( std::ostream& os, const PkgDep::Result& res );
std::ostream& operator<<( std::ostream& os, const PkgDep::ErrorResult& res );
std::ostream& operator<<( std::ostream& os, const PkgDep::RelInfoList& rl );
std::ostream& operator<<( std::ostream& os, const std::list<PkgDep::Alternative>& al );
std::ostream& operator<<( std::ostream& os, const PkgDep::NameList& nl );
std::ostream& operator<<( std::ostream& os, const PkgDep::NeededEditionRange& range );

#endif  /* _PkgDep_h */

// Local Variables:
// tab-width: 4
// End:

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

// solver
class PkgDep {

	// ------------------------------ types ------------------------------

  public:
	enum alternative_kind {
		SIMPLE, REQUIRES_MORE, CONFLICT
	};
	enum alternatives_mode {
		ASK_ALWAYS, ASK_IF_NO_DEFAULT, AUTO_IF_NO_DEFAULT, AUTO_ALWAYS
	};

	// RelInfo, Alternative, and NeededEditionRange are for returning results
	struct RelInfo {
		// name of package causing the relation
		PkgName name;
		// copy of the relation
		// (if rel.name() is different from name, then name provides
		// rel.name)
		PkgRelation rel;
		// true if the relation is a conflict, false if it's a requires
		bool is_conflict;

		RelInfo( PkgName n, PkgRelation r, bool is_c = false )
			: name(n), rel(r), is_conflict(is_c) {}
		RelInfo( PkgRevRelation r, bool is_c = false )
			: name(r.pkg()->name()), rel(r.relation()), is_conflict(is_c) {}
	};

	struct Alternative {
		PkgName name;
		alternative_kind kind;

		Alternative( const PkgName& n, alternative_kind k )
			: name(n), kind(k) {}
	};

	// For unresolvable packages, the edition requirements are merged in this
	// structure and returned. If either 'greater' or 'less' are UNSPEC,
	// there's no requirement in this direction. If both are UNSPEC, any
	// edition will satisfy the requirements. Otherwise, a satisfying edition
	// must be greater than 'greater' and less than 'less'.
	// The {le,gr}_incl fields are true if the ranges are meant inclusive at
	// the resp. border.
	// 'impossible' is true if greater > less, i.e. there are contradicting
	// requirements.
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

	struct Notes;
	struct Result {
		PkgName name;
		PkgEdition edition;

		// list of packages that require this pkg (i.e., why it was added or
		// can't be omitted from the input list)
		RelInfoList referers;
		// UNSPEC means is no upgrade; otherwise: currently installed version
		PkgEdition is_upgrade_from;
		// UNSPEC means is no downgrade; otherwise: currently installed version
		PkgEdition is_downgrade_from;
		// true if this package was passed in the input list (i.e., hasn't been
		// added by PkgDep)
		bool from_input_list : 1;
		// this package has been upgraded to resolve a conflict (otherwise, the
		// update wouldn't be necessary)
		bool upgrade_to_remove_conflict : 1;
		// this package has been installed to avoid a broken requirement by
		// another upgrade/obsoletion
		bool install_to_avoid_break : 1;

		Result(const PkgDep& pkgdep, PMSolvablePtr pkg);
		Result(const PkgDep& pkgdep, const PkgName& name);
		void add_notes( const Notes& notes );
	};

	struct ErrorResult : public Result {
		// true if this pkg is required but not available, not_avail_range
		// gives details
		bool not_available;
		NeededEditionRange not_avail_range;
		// true if not all requirements could be satisfied
		RelInfoList unresolvable;
		//
		std::list<Alternative> alternatives;
		//
		RelInfoList conflicts_with;
		//
		NameList remove_to_solve_conflict;

		ErrorResult(const PkgDep& pkgdep, PMSolvablePtr pkg)
			: Result(pkgdep,pkg), not_available(false) {}
		ErrorResult(const PkgDep& pkgdep, const PkgName& name)
			: Result(pkgdep,name), not_available(false) {}
		ErrorResult(const Result& res)
			: Result(res), not_available(false) {}

		void add_unresolvable( PkgName n, const PkgRelation& rel );
		void add_conflict( const PkgRevRelation& rrel,
						   const PkgDep& dep,
						   PMSolvablePtr to_remove,
						   PMSolvablePtr assume_instd,
						   bool is_conflict = true );
		void add_conflict( PkgName n, const PkgRelation& rel,
						   const PkgDep& dep,
						   PMSolvablePtr to_remove,
						   PMSolvablePtr assume_instd,
						   bool is_conflict = true );
		void add_alternative( PkgName n, alternative_kind k );
		void add_notes( const Notes& notes );
	};

	friend class Result;
	friend class ErrorResult;
	typedef std::list<Result> ResultList;
	typedef std::list<ErrorResult> ErrorResultList;

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

	// Notes keeps some additional information to candidates
	struct Notes {
		bool from_input : 1;
		bool upgrade_to_solve_conflict : 1;
		bool install_to_avoid_break : 1;
		bool not_available : 1;
		IRelInfoList referers;
		NeededEditionRange not_avail_range;

		Notes() : from_input(false), upgrade_to_solve_conflict(false),
			install_to_avoid_break(false), not_available(false) {}
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

	Alternatives::AltDefaultList (*_alternatives_callback)( PkgName name );

	// --------------------- used during an install run --------------------
	PkgSet vinstalled;
	PkgSet *candidates;
	Notes_type notes;
	std::deque<PMSolvablePtr > to_check;
	AltInfoList alts_to_check;
	noval_hash<PkgName> alts_handled;
	NameList i_obsoleted;
	ResultList *good;
	ErrorResultList *bad;

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
	void virtual_remove_package( PMSolvablePtr pkg, NameList& to_remove,
								 PMSolvablePtr assume_instd = NULL ) const;
	void remove_package( PkgSet *set, PMSolvablePtr pkg,
						 NameList& to_remove) const;
	// utils.cc
	bool also_provided_by_installed( const PkgRelation& rel );
	unsigned count_providers_for( const PkgSet* set,
								  const PkgRelation& req ) const;
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
	
public:
	PkgDep( PkgSet& instd, const PkgSet& avail,
			Alternatives::AltDefaultList (*alternatives_callback)( PkgName name ) = default_alternatives_callback,
			alternatives_mode m = default_alternatives_mode )
		: alt_mode(m), installed(instd), available(avail), _alternatives_callback(alternatives_callback) {}

//	PkgDep(  const PkgSet& instd, alternatives_mode m=default_alternatives_mode)
//		: alt_mode(m), installed(instd), available(*default_avail), _pool(pool) {}
/*	PkgDep( alternatives_mode m = default_alternatives_mode )
		: alt_mode(m), installed(PkgSet(DTagListI0())),
		  available(*default_avail) {}
*/
	// install packages (if good result, add candidates to installed set)
	// does not throw -- ln
	bool install( PkgSet& candidates,
				  ResultList& good, ErrorResultList& bad,
				  bool commit_to_installed = true );
	// remove a list of packages; the 'pkgs' list will be extended by all
	// packages that have to be removed, too, to make the installed set
	// consistent again
	void remove( NameList& pkgs );
	// check consistency of current installed set
	bool consistent( ErrorResultList& failures );
	bool upgrade(	PkgSet&candidates, ResultList& out_good,
			ErrorResultList& out_bad, NameList& to_remove,
			unsigned max_remove = default_max_remove );

	// return current installed set for inspection
	const PkgSet& current_installed() { return installed; }

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

	static void set_default_alternatives_mode( alternatives_mode m ) {
		default_alternatives_mode = m;
	}

	static void set_default_max_remove( unsigned mr ) {
		default_max_remove = mr;
	}
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

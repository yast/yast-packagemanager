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

   File:       CheckSetDeps.h

   Author:     Ludwig Nussel <lnussel@suse.de
   Maintainer: Ludwig Nussel <lnussel@suse.de

/-*/

#ifndef PMCheckSetDeps_h
#define PMCheckSetDeps_h

#include <y2pm/PkgSet.h>

/** \brief check whether requirements in a PkgSet are fulfilled
 *
 * Can be used as a functor
 * */
class CheckSetDeps
{
    public:
	/** foo => ( libbar.so.1, baz-devel = 4.2 ) */
	typedef std::map<PMSolvablePtr, PMSolvable::PkgRelList_type> BrokenMap;

    private:
	PkgSet& _c;
	BrokenMap& _b;

	bool _track_packages;
	bool _track_relations;

	void* dummy1;
	void* dummy2;

    public:
	/** \brief Constructor
	 * @param c the set to check
	 * @param b location where to store broken deps
	 * */
	CheckSetDeps(PkgSet& c, BrokenMap& b);

	/** \brief set whether to track broken packages at all */
	void setTrackPackages(bool y);

	/** \brief set whether to track broken relations at all */
	void setTrackRelations(bool y);

	/** \brief check package dependencies
	 *
	 * checks the requires of one package against the provides of the set.
	 * Tracks broken dependencies in the map specified at construction time
	 *
	 * @param p the package to check
	 * @returns true if all dependencies are satisfied, false otherwise
	 * */
	bool operator()(PMSolvablePtr p);

	/** \brief check all packages in the set
	 * @returns true if all dependencies are satisfied, false otherwise
	 * */ 
	bool checkAll();
};

#endif

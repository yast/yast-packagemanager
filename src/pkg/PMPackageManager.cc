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

   File:       PMPackageManager.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/Pathname.h>

#include <y2pm/PMPackageManager.h>

#include <Y2PM.h>
#include <y2pm/InstTarget.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/InstallOrder.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::PMPackageManager
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMPackageManager::PMPackageManager()
{
  _rpmGroupsTree = new YRpmGroupsTree();

#if 0
  _rpmGroupsTree->addFallbackRpmGroups();
#endif
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::~PMPackageManager
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMPackageManager::~PMPackageManager()
{
  if ( _rpmGroupsTree )
    delete _rpmGroupsTree;

  MIL << "PMPackageManager::~PMPackageManager()" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::assertObjectType
//	METHOD TYPE : PMObjectPtr
//
//	DESCRIPTION :
//
PMObjectPtr PMPackageManager::assertObjectType( const PMObjectPtr & object_r ) const
{
  PMPackagePtr p( object_r );
  if ( object_r && !p ) {
    ERR << "Object is not a Package: " << object_r << endl;
  }
  return p;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::prePSI
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMPackageManager::prePSI()
{

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::postPSI
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMPackageManager::postPSI()
{
#warning must improve taboo handling in pre/postPSI

  for ( PMSelectableVec::iterator it = begin(); it != end(); ++it ) {
    const PMSelectablePtr & sel( *it );

    if ( sel->has_installed() && !PMPackagePtr( sel->installedObj() )->vendor().isSuSE() ) {
      sel->user_set_taboo();
      MIL << "Protect non SuSE package " << sel->installedObj() << endl;
    }
  }
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const PMPackageManager & obj )
{
  str << "PMPackageManager" << endl;
  return str;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::getPackagesToInsDel
//	METHOD TYPE : void
//
//	DESCRIPTION : go through all packages and check for install/delete/installsource
//		status. Sort package according to prereq and return these
//		packages via dellist_r, instlist_r, srclist_r
//
void PMPackageManager::getPackagesToInsDel( std::list<PMPackagePtr> & dellist_r,
					    std::list<PMPackagePtr> & instlist_r,
					    std::list<PMPackagePtr> & srclist_r,
					    InstSrcManager::ISrcIdList sourcerank_r )
{
    dellist_r.clear();
    instlist_r.clear();
    srclist_r.clear();

    DBG << "getPackagesToInsDel..." << endl;

    for ( PMSelectableVec::iterator it = begin(); it != end(); ++it )
    {
	const PMSelectablePtr & sel( *it );

	if ( !sel )
	{
	    INT << "NULL SELECTABLE" << endl;
	    continue;
	}

	if (sel->to_install())
	{
	    if ( sel->candidateObj() )
	    {
		// unordered list for backup
		instlist_r.push_back( sel->candidateObj() );
	    }
	    else
	    {
		INT << "NULL candidate to install" << endl;
	    }
	}
	else if (sel->to_delete())
	{
	    if ( sel->installedObj() )
	    {
	      if ( sel->by_auto() ) {
		DBG << "Ignore auto_delete (should be obsoleted): " << sel << endl;
	      } else {
		dellist_r.push_back( sel->installedObj() );
	      }
	    }
	    else
	    {
		INT << "NULL installed to delete" << endl;
	    }
	}

	if (sel->source_install())
	{
	    if ( sel->candidateObj() )
	    {
		// unordered list for backup
		srclist_r.push_back( sel->candidateObj() );
	    }
	    else
	    {
		INT << "NULL candidate to source install" << endl;
	    }
	}
    }

    DBG << "num packages: delete " << dellist_r.size() << ", install " << instlist_r.size() << endl;

    ///////////////////////////////////////////////////////////////////
    // sort installed list...
    ///////////////////////////////////////////////////////////////////
    if ( instlist_r.empty() )
      return;

    ///////////////////////////////////////////////////////////////////
    // Get desired sequence InstSrc'es to install from.
    ///////////////////////////////////////////////////////////////////
    if ( sourcerank_r.empty() ) {
      // if not provided use InstSrcManager's default
      Y2PM::instSrcManager().getSources( sourcerank_r, /*enabled_only*/true );
    }
    typedef map<unsigned,unsigned> RankPriority;
    RankPriority rankPriority;
    {
      // map InstSrc rank to install priority
      unsigned prio = 0;
      for ( InstSrcManager::ISrcIdList::const_iterator it = sourcerank_r.begin(); it != sourcerank_r.end(); ++it, ++prio ) {
	rankPriority[(*it)->descr()->default_rank()] = prio;
      }
    }

    ///////////////////////////////////////////////////////////////////
    // Compute install order
    ///////////////////////////////////////////////////////////////////
    // backup list for debug purpose.
    // You can as well build the set, clear the list and rebuild it in install order.
    std::list<PMPackagePtr> instbackup_r;
    instbackup_r.swap( instlist_r );

    PkgSet iset; // for install order
    PkgSet installed; // dummy, empty, should contain already installed
    for ( list<PMPackagePtr>::const_iterator pkgIt = instbackup_r.begin(); pkgIt != instbackup_r.end(); ++pkgIt ) {
      iset.add( *pkgIt );
    }
    InstallOrder order( iset, installed );
    // start recursive depth-first-search
    order.startrdfs();

    ///////////////////////////////////////////////////////////////////
    // build install list in install order
    ///////////////////////////////////////////////////////////////////
    list<PMPackagePtr> best_list;
    unsigned best_prio     = 0;
    unsigned best_medianum = 0;

    list<PMPackagePtr> last_list;
    unsigned last_prio     = 0;
    unsigned last_medianum = 0;

    for ( InstallOrder::SolvableList pkgs = order.computeNextSet(); ! pkgs.empty(); pkgs = order.computeNextSet() ) {
      ///////////////////////////////////////////////////////////////////
      // pkgs contains all packages we could install now. Pick all packages
      // from current media, or best media if none for current.
      ///////////////////////////////////////////////////////////////////

      best_list.clear();
      last_list.clear();

      for ( InstallOrder::SolvableList::const_iterator cit = pkgs.begin(); cit != pkgs.end(); ++cit ) {
	PMPackagePtr cpkg = PMPackagePtr::cast_away_const( *cit );
	if ( !cpkg ) {
	  INT << "SORT returned NULL Package" << endl;
	  continue;
	}

	if ( rankPriority[cpkg->instSrcRank()] == last_prio
	     && cpkg->medianr() == last_medianum ) {
	  // prefer packages on current media.
	  last_list.push_back( cpkg );
	  continue;
	}

	if ( last_list.empty() ) {
	  // check for best media as long as there are no packages for current media.

	  if ( ! best_list.empty() ) {

	    if ( rankPriority[cpkg->instSrcRank()] < best_prio ) {
	      best_list.clear(); // new best
	    } else if ( rankPriority[cpkg->instSrcRank()] == best_prio ) {
	      if ( cpkg->medianr() < best_medianum ) {
		best_list.clear(); // new best
	      } else if ( cpkg->medianr() == best_medianum ) {
		best_list.push_back( cpkg ); // same as best -> add
		continue;
	      } else {
		continue; // worse
	      }
	    } else {
	      continue; // worse
	    }
	  }

	  if ( best_list.empty() ) {
	    // first package or new best
	    best_list.push_back( cpkg );
	    best_prio     = rankPriority[cpkg->instSrcRank()];
	    best_medianum = cpkg->medianr();
	    continue;
	  }
	}

      } // for all packages in current set

      ///////////////////////////////////////////////////////////////////
      // remove packages picked from install order and append them to
      // install list.
      ///////////////////////////////////////////////////////////////////
      list<PMPackagePtr> & take_list( last_list.empty() ? best_list : last_list );
      if ( last_list.empty() ) {
	I__ << "SET NEW" << endl;
	last_prio     = best_prio;
	last_medianum = best_medianum;
      } else {
	I__ << "SET CONTINUE" << endl;
      }

      for ( list<PMPackagePtr>::iterator it = take_list.begin(); it != take_list.end(); ++it ) {
	order.setInstalled( *it );
	I__ << "SET isrc " << (*it)->instSrcRank() << " | no " << (*it)->medianr() << " -> " << (*it)->nameEdArch() << endl;
      }
      instlist_r.splice( instlist_r.end(), take_list );

    } // for all sets computed


    if ( instbackup_r.size() != instlist_r.size() )
    {
	INT << "Lost packages in InstallOrder sort." << endl;
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::getDu
//	METHOD TYPE : const PkgDuMaster &
//
//	DESCRIPTION :
//
const PkgDuMaster & PMPackageManager::getDu()
{
  if ( _du_master.mountpoints().empty() ) {
     setMountPoints( Y2PM::instTarget().getMountPoints() );
  }
  return _du_master;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::updateDu
//	METHOD TYPE : const PkgDuMaster &
//
//	DESCRIPTION :
//
const PkgDuMaster & PMPackageManager::updateDu()
{
  getDu(); // tries to initialize an empty mountpoint set from InstTarget.

  if ( _du_master.resetStats() ) {
    // There's at least one mountpoint
    for ( PMSelectableVec::iterator it = begin(); it != end(); ++it ) {
      const constPMSelectablePtr & sel( *it );

      if ( ! sel->to_modify() )
	continue; // package unchanged

      if ( sel->to_install() ) {

	PMPackagePtr( sel->candidateObj() )->du_add( _du_master );
	if ( sel->has_installed() )
	  PMPackagePtr( sel->installedObj() )->du_sub( _du_master );

      } else { // to delete

	PMPackagePtr( sel->installedObj() )->du_sub( _du_master );
      }

    }
  } else {
    WAR << "Unable to get mountpoint info" << endl;
  }

  return _du_master;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::countDuInstalled
//	METHOD TYPE : PkgDuMaster &
//
//	DESCRIPTION :
//
PkgDuMaster & PMPackageManager::countDuInstalled( PkgDuMaster & dudata ) const
{
  if ( dudata.resetStats() ) {
    // There's at least one mountpoint
    for ( PMSelectableVec::iterator it = begin(); it != end(); ++it ) {
      const constPMSelectablePtr & sel( *it );
      if ( sel->has_installed() )
	PMPackagePtr( sel->installedObj() )->du_add( dudata );
    }
  }
  return dudata;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::countDuCandidates
//	METHOD TYPE : PkgDuMaster &
//
//	DESCRIPTION :
//
PkgDuMaster & PMPackageManager::countDuCandidates( PkgDuMaster & dudata ) const
{
  if ( dudata.resetStats() ) {
    // There's at least one mountpoint
    for ( PMSelectableVec::iterator it = begin(); it != end(); ++it ) {
      const constPMSelectablePtr & sel( *it );
      if ( sel->has_candidate() )
	PMPackagePtr( sel->candidateObj() )->du_add( dudata );
    }
  }
  return dudata;
}
///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::countDuSelected
//	METHOD TYPE : PkgDuMaster &
//
//	DESCRIPTION :
//
PkgDuMaster & PMPackageManager::countDuSelected( PkgDuMaster & dudata ) const
{
  if ( dudata.resetStats() ) {
    // There's at least one mountpoint
    for ( PMSelectableVec::iterator it = begin(); it != end(); ++it ) {
      const constPMSelectablePtr & sel( *it );
      if ( sel->to_install() )
	PMPackagePtr( sel->candidateObj() )->du_add( dudata );
    }
  }
  return dudata;
}

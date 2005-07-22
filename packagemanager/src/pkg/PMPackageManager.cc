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
#include <y2pm/PMPackageManagerCallbacks.h>

#include <Y2PM.h>
#include <y2pm/InstTarget.h>
#include <y2pm/InstSrcManager.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/InstallOrder.h>
#include <y2pm/PMVendorAttr.h>

using namespace std;
using namespace PMPackageManagerCallbacks;

template<typename _Ct, class Compare>
ostream & operator<<( ostream & str, const set<_Ct, Compare> & obj ) {
  str << "[" << obj.size() << "]{";
  for ( typename set<_Ct, Compare>::const_iterator it = obj.begin(); it != obj.end(); ++it ) {
    if ( it == obj.begin() )
      str << endl;
    str << "  " << *it << endl;
  }
  return str << '}';
}
///////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////


/******************************************************************
**
**
**	FUNCTION NAME : strip_obsoleted_to_delete
**	FUNCTION TYPE : void
**
** strip packages to_delete which get obsoleted by
** to_install (i.e. delay deletion in case the
** obsoleting package likes to save whatever...
*/
void strip_obsoleted_to_delete( list<PMPackagePtr> & deleteList_r,
				const list<PMPackagePtr> & instlist_r )
{
  if ( deleteList_r.size() == 0 || instlist_r.size() == 0 )
    return; // ---> nothing to do

  // build obsoletes from instlist_r
  list<PkgRelation> obsoletes;
  for ( list<PMPackagePtr>::const_iterator it = instlist_r.begin();
	it != instlist_r.end(); ++it ) {
    obsoletes.insert( obsoletes.end(),
		      (*it)->obsoletes_begin(), (*it)->obsoletes_end() );
  }
  if ( obsoletes.size() == 0 )
    return; // ---> nothing to do

  // match them... ;(
  list<PMPackagePtr> undelayed;
  // forall applDelete Packages...
  for ( list<PMPackagePtr>::iterator it = deleteList_r.begin();
	it != deleteList_r.end(); ++it ) {
    PMPackagePtr ipkg( *it );
    bool delayPkg = false;
    // ...check whether an obsolets....
    for ( list<PkgRelation>::iterator obs = obsoletes.begin();
	  ! delayPkg && obs != obsoletes.end(); ++obs ) {
      // ...matches anything provided by the package?
      for ( PMSolvable::PkgRelList_const_iterator prov = ipkg->provides_begin();
	    prov != ipkg->provides_end(); ++prov ) {
	if ( obs->matches( *prov ) ) {
	  // if so, delay package deletion
	  DBG << "Ignore appl_delete (should be obsoleted): " << ipkg << endl;
	  delayPkg = true;
	  break;
	}
      }
    }
    if ( ! delayPkg ) {
      undelayed.push_back( ipkg );
    }
  }
  // Puhh...
  deleteList_r.swap( undelayed );
}

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
  for ( PMSelectableVec::iterator it = begin(); it != end(); ++it ) {
    const PMSelectablePtr & sel( *it );

    if ( sel->has_installed() ) {
      Vendor v( PMPackagePtr(sel->installedObj())->vendor() );
      if ( PMVendorAttr::autoProtect( v ) ) {
	sel->user_set_taboo();
	MIL << "Protect vendor '" << v << "': " << sel->installedObj() << endl;
      }
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
					    std::list<PMPackagePtr> & srclist_r )
{
    dellist_r.clear();
    instlist_r.clear();
    srclist_r.clear();

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

    MIL << "PackagesToInsDel: delete " << dellist_r.size()
      << ", install " << instlist_r.size()
	<< ", srcinstall " << srclist_r.size() << endl;

    ///////////////////////////////////////////////////////////////////
    //
    // strip packages to_delete which get obsoleted by
    // to_install (i.e. delay deletion in case the
    // obsoleting package likes to save whatever...
    //
    ///////////////////////////////////////////////////////////////////
    strip_obsoleted_to_delete( dellist_r, instlist_r );

    if ( dellist_r.size() ) {
      ///////////////////////////////////////////////////////////////////
      //
      // sort delete list...
      //
      ///////////////////////////////////////////////////////////////////
      PkgSet dset;  // for delete order
      PkgSet dummy; // dummy, empty, should contain already installed
      for ( list<PMPackagePtr>::const_iterator pkgIt = dellist_r.begin();
	    pkgIt != dellist_r.end(); ++pkgIt ) {
	dset.add( *pkgIt );
      }

      InstallOrder order( dset, dummy ); // sort according top prereq
      order.init();
      const InstallOrder::SolvableList & dsorted( order.getTopSorted() );

      dellist_r.clear();
      for ( InstallOrder::SolvableList::const_reverse_iterator cit = dsorted.rbegin();
	    cit != dsorted.rend(); ++cit ) {
	PMPackagePtr cpkg = PMPackagePtr::cast_away_const( *cit );
	if ( !cpkg ) {
	  INT << "SORT returned NULL Package" << endl;
	  continue;
	}
	dellist_r.push_back( cpkg );
      }
    }

    ///////////////////////////////////////////////////////////////////
    //
    // sort installed list...
    //
    ///////////////////////////////////////////////////////////////////
    if ( instlist_r.empty() ) {
      return;
    }

    ///////////////////////////////////////////////////////////////////
    // Get desired order of InstSrc'es to install from.
    ///////////////////////////////////////////////////////////////////
    typedef map<unsigned,unsigned> RankPriority;
    RankPriority rankPriority;
    {
      InstSrcManager::ISrcIdList sourcerank( Y2PM::instSrcManager().instOrderSources() );
      // map InstSrc rank to install priority
      unsigned prio = 0;
      for ( InstSrcManager::ISrcIdList::const_iterator it = sourcerank.begin();
	    it != sourcerank.end(); ++it, ++prio ) {
	rankPriority[(*it)->descr()->default_rank()] = prio;
      }
    }

    ///////////////////////////////////////////////////////////////////
    // Compute install order according to packages prereq.
    // Try to group packages with respect to the desired install order
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
    FSize srcSizes;
    // There's at least one mountpoint
    for ( PMSelectableVec::iterator it = begin(); it != end(); ++it ) {
      const constPMSelectablePtr & sel( *it );

      if ( sel->source_install() ) {
	srcSizes += PMPackagePtr( sel->candidateObj() )->sourcesize();
      }

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
    _du_master.addSrcPkgs( srcSizes );
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

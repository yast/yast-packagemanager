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
#include <y2pm/InstallOrder.h>

#include <Y2PM.h>

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
//	METHOD NAME : PMPackageManager::anythingByUser
//	METHOD TYPE : bool
//
//	DESCRIPTION :are there currently any "by_user" selectables ?
//
bool
PMPackageManager::anythingByUser(void)
{
    for ( PMSelectableVec::iterator it = begin(); it != end(); ++it )
    {
	if ((*it)->by_user())
	    return true;
    }
    return false;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::anythingToDelete
//	METHOD TYPE : void
//
//	DESCRIPTION :are there currently any selectables to delete ?
//
bool
PMPackageManager::anythingToDelete (void)
{
    for ( PMSelectableVec::iterator it = begin(); it != end(); ++it )
    {
	if ((*it)->to_delete())
	    return true;
    }
    return false;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::anythingToInstall
//	METHOD TYPE : void
//
//	DESCRIPTION :are there currently any selectables to install ?
//
bool
PMPackageManager::anythingToInstall (void)
{
    for ( PMSelectableVec::iterator it = begin(); it != end(); ++it )
    {
	if ((*it)->to_install())
	    return true;
    }
    return false;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::getPackagesToInsDel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMPackageManager::getPackagesToInsDel( std::list<PMPackagePtr> & dellist_r,
					    std::list<PMPackagePtr> & instlist_r )
{
    dellist_r.clear();
    instlist_r.clear();

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
		dellist_r.push_back( sel->installedObj() );
	    }
	    else
	    {
		INT << "NULL installed to delete" << endl;
	    }
	}
    }

    DBG << "num packages: delete " << dellist_r.size() << ", install " << instlist_r.size() << endl;

    ///////////////////////////////////////////////////////////////////
    // sort installed list.
    ///////////////////////////////////////////////////////////////////

    // backup list
    std::list<PMPackagePtr> instbackup_r;
    instbackup_r.swap( instlist_r );

    if ( instlist_r.size() )
    {
	// oops, swap() is supposed to empty it's argument
	INT << "DONT swap lists" << endl;
	instlist_r.clear();
    }

#warning check source priority and media count

    // loop over medias
    for (unsigned int cdnum = 1; cdnum < 10; ++cdnum)
    {
	PkgSet iset; // for install order

	// loop over packages, adding matchin medianr to set
	for (std::list<PMPackagePtr>::const_iterator pkgIt = instbackup_r.begin();
	     pkgIt != instbackup_r.end(); ++pkgIt)
	{
	    if ((*pkgIt)->medianr() == cdnum)
	    {
		iset.add (*pkgIt);
	    }
	}

	// matching medianr found -> solve
	if (!iset.empty())
	{
	    InstallOrder order( iset );

	    // start recursive depth-first-search
	    order.startrdfs();

	   for (InstallOrder::SolvableList pkgs = order.computeNextSet();
		!pkgs.empty(); pkgs = order.computeNextSet() )
	   {

		for (InstallOrder::SolvableList::const_iterator cit = pkgs.begin();
		     cit != pkgs.end(); ++cit )
		{
		    PMPackagePtr cpkg = PMPackagePtr::cast_away_const( *cit );

		    if ( !cpkg )
		    {
			INT << "SORT returned NULL Package" << endl;
			continue;
		    }

		    instlist_r.push_back( cpkg );
		    order.setInstalled( *cit );
		}
	    }
	} // ! iset.empty
    } // cdnum loop

    if ( instbackup_r .size() != instlist_r.size() )
    {
	INT << "Lost packages in InstallOrder sort." << endl;
    }

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
  bool count_mp = _du_master.resetStats();

  if ( ! count_mp ) {
    DBG << "No MountPoints set; count totals only" << endl;
  }

  // currently installed packages do not have du info ;(
  //
  // new install: add candidate
  // replace:     ---
  // delete:      sub candiadte (if one)
  for ( PMSelectableVec::iterator it = begin(); it != end(); ++it ) {
    const constPMSelectablePtr & sel( *it );
    if ( sel->to_modify() ) {

      if ( sel->to_install() ) {

	_du_master.add( sel->candidateObj()->size() );
	if ( sel->has_installed() ) {
	  _du_master.sub( sel->installedObj()->size() );
	  // replace:     ---
	} else {
	  // new install: add candidate
	  if ( count_mp )
	    PMPackagePtr( sel->candidateObj() )->du_add( _du_master );
	}

      } else {

	// to delete
	_du_master.sub( sel->installedObj()->size() );
	// delete:      sub candiadte (if one)
	if ( count_mp && sel->has_candidate() ) {
	  PMPackagePtr( sel->candidateObj() )->du_sub( _du_master );
	}

      }

    }
  }

  DBG << _du_master << endl;
  return _du_master;
}


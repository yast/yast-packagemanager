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
#include <y2pm/PMPackageManager.h>
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
//	DESCRIPTION :
//
void PMPackageManager::getPackagesToInsDel( std::list<PMPackagePtr> & dellist_r,
					    std::list<PMPackagePtr> & instlist_r )
{
  dellist_r.clear();
  instlist_r.clear();

  DBG << "getPackagesToInsDel..." << endl;

  PkgSet iset; // for install order

  for ( PMSelectableVec::iterator it = first(); it != last(); ++it ) {
    PMSelectablePtr & sel( *it );

    if ( !sel ) {
      INT << "NULL SELECTABLE" << endl;
      continue;
    }

    switch( sel->status() ) {

    case PMSelectable::S_Install:
    case PMSelectable::S_Update:
    case PMSelectable::S_Auto:
      if ( sel->candidateObj() ) {
	iset.add( sel->candidateObj() );
	instlist_r.push_back( sel->candidateObj() );
      } else
	INT << "NULL candidate to install" << endl;
      break;

    case PMSelectable::S_Del:
      if ( sel->installedObj() ) {
	dellist_r.push_back( sel->installedObj() );
      } else
	INT << "NULL installed to delete" << endl;
      break;

    case PMSelectable::S_NoInst:
    case PMSelectable::S_KeepInstalled:
    case PMSelectable::S_Taboo:
      // nothing to do.
      break;
    }
  }
  DBG << "num packages: delete " << dellist_r.size() << ", install " << instlist_r.size() << endl;

  ///////////////////////////////////////////////////////////////////
  // sort installed list.
  ///////////////////////////////////////////////////////////////////

  std::list<PMPackagePtr> instbackup_r;
  instbackup_r.swap( instlist_r );
  if ( instlist_r.size() ) {
    INT << "DONT swap lists" << endl;
    instlist_r.clear();
  }

  InstallOrder order( iset );
  order.startrdfs();

  for( InstallOrder::SolvableList pkgs = order.computeNextSet();
       ! pkgs.empty(); pkgs = order.computeNextSet() ) {

    for( InstallOrder::SolvableList::const_iterator cit = pkgs.begin();
	 cit != pkgs.end(); ++cit ) {
      PMPackagePtr cpkg = PMPackagePtr::cast_away_const( *cit );
      if ( !cpkg ) {
	INT << "SORT returned NULL Package" << endl;
	continue;
      }
#warning MUST check for CD and media
      instlist_r.push_back( cpkg );
      order.setInstalled( *cit );
    }
  }

  if ( instbackup_r .size() != instlist_r.size() ) {
    INT << "Lost packages in InstallOrder sort." << endl;
  }

}


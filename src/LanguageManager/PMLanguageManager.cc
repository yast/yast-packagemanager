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

   File:       PMLanguageManager.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>
#include <functional>
#include <algorithm>

#include <y2util/Y2SLog.h>

#include <Y2PM.h>
#include <y2pm/PMLanguageManager.h>
#include <y2pm/PMLanguageManagerCallbacks.h>
#include <y2pm/PMSelectionManager.h>

using namespace std;
using namespace PMLanguageManagerCallbacks;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguageManager::PMLanguageManager
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMLanguageManager::PMLanguageManager()
{
  MIL << "PMLanguageManager::PMLanguageManager()" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguageManager::~PMLanguageManager
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMLanguageManager::~PMLanguageManager()
{
  MIL << "PMLanguageManager::~PMLanguageManager()" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguageManager::assertObjectType
//	METHOD TYPE : PMObjectPtr
//
//	DESCRIPTION :
//
PMObjectPtr PMLanguageManager::assertObjectType( const PMObjectPtr & object_r ) const
{
  PMLanguagePtr p( object_r );
  if ( object_r && !p ) {
    ERR << "Object is not a Language: " << object_r << endl;
  }
  return p;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguageManager::prePSI
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMLanguageManager::prePSI()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguageManager::postPSI
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMLanguageManager::postPSI()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguageManager::selectableNotify
//	METHOD TYPE : void
//
void PMLanguageManager::selectableNotify( constPMSelectablePtr item_r,
					  SelState old_r, SelState new_r )
{
  if ( PMSelectable::fate( old_r ) != PMSelectable::fate( new_r ) ) {
    LangCode langCode( PMLanguagePtr( item_r->theObject() )->langCode() );
    PMError err;

    if ( item_r->is_onSystem() ) {
      err = Y2PM::addRequestedLocales( langCode );
    } else {
      err = Y2PM::delRequestedLocales( langCode );
    }
    if ( err ) {
      WAR << err << ": " << item_r->name() << old_r << " -> " << new_r << endl;
    }
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguageManager::rescan
//	METHOD TYPE : void
//
void PMLanguageManager::rescan()
{
  Objects new_available;   // new locales
  Objects still_available; // already known locales
  PM::LocaleSet foundLocales;

  // derive available locales from installed/available selections
  for ( PMSelectableVec::const_iterator it = Y2PM::selectionManager().begin();
	it != Y2PM::selectionManager().end(); ++it ) {

    PMSelectionPtr sel = (*it)->installedObj();
    if ( sel ) {
      PM::LocaleSet clocales( sel->supportedLocales() );
      foundLocales.insert( clocales.begin(), clocales.end() );
    }

    sel = (*it)->candidateObj();
    if ( sel ) {
      PM::LocaleSet clocales( sel->supportedLocales() );
      foundLocales.insert( clocales.begin(), clocales.end() );
    }
  }

  for ( PM::LocaleSet::const_iterator it = foundLocales.begin(); it != foundLocales.end(); ++it ) {
    // Don't mind creating NULL entires in _availableObjs.
    // They are deleted afterwards.
    if ( ! _availableObjs[*it] ) {
      new_available[*it] = new PMLanguage( *it );
    } else {
      still_available[*it] = _availableObjs[*it];
      _availableObjs[*it] = 0;
    }
  }

  // delete NULL entries
  for ( Objects::iterator it = _availableObjs.begin(); it != _availableObjs.end(); /*++in loop*/) {
    if ( it->second ) {
      ++it;
    } else {
      _availableObjs.erase( it++ );
    }
  }

  // adjust pool
  if ( new_available.size() )
    poolAddCandidates( new_available );
  if ( _availableObjs.size() )
  poolRemoveCandidates( _availableObjs );

  // adjust _availableObjs
  _availableObjs.swap( still_available );
  _availableObjs.insert( new_available.begin(), new_available.end() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguageManager::setComittedLocales
//	METHOD TYPE : void
//
void PMLanguageManager::setComittedLocales( PM::LocaleSet locales_r )
{
  _installedObjs.clear();
  for ( PM::LocaleSet::const_iterator it = locales_r.begin(); it != locales_r.end(); ++it ) {
    _installedObjs[*it] = new PMLanguage( *it );
  }
  poolSetInstalled( _installedObjs );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguageManager::getLangPackages
//	METHOD TYPE : PMLanguageManager::PkgSelectables
//
PMLanguageManager::PkgSelectables PMLanguageManager::getLangPackagesFor( LangCode langCode_r ) const
{
  PkgSelectables packages;

  for ( PMSelectableVec::const_iterator sit = Y2PM::selectionManager().begin();
	sit != Y2PM::selectionManager().end(); ++sit ) {

    PMSelectionPtr sel = (*sit)->installedObj();
    if ( sel ) {
      set<PMSelectablePtr> cpkg( sel->pureInspacks_ptrs( langCode_r ) );
      packages.insert( cpkg.begin(), cpkg.end() );
    }

    sel = (*sit)->candidateObj();
    if ( sel ) {
      set<PMSelectablePtr> cpkg( sel->pureInspacks_ptrs( langCode_r ) );
      packages.insert( cpkg.begin(), cpkg.end() );
    }
  }

  return packages;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const PMLanguageManager & obj )
{
  str << "PMLanguageManager" << endl;
  return str;
}

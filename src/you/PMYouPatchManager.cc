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

   File:       PMYouPatchManager.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>
#include <fstream>

#include <y2util/Y2SLog.h>

#include <y2pm/PMYouPatchManager.h>
#include <y2pm/InstYou.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatchManager::PMYouPatchManager
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMYouPatchManager::PMYouPatchManager()
{
  _instYou = new InstYou;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatchManager::~PMYouPatchManager
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMYouPatchManager::~PMYouPatchManager()
{
  delete _instYou;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatchManager::assertObjectType
//	METHOD TYPE : PMObjectPtr
//
//	DESCRIPTION :
//
PMObjectPtr PMYouPatchManager::assertObjectType( const PMObjectPtr & object_r ) const
{
  PMYouPatchPtr p( object_r );
  if ( object_r && !p ) {
    ERR << "Object is not a YouPatch: " << object_r << endl;
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
ostream & operator<<( ostream & str, const PMYouPatchManager & obj )
{
  str << "PMYouPatchManager" << endl;
  return str;
}

/******************************************************************
**
**
**	FUNCTION NAME : instYou
**	FUNCTION TYPE : InstYou
**
**	DESCRIPTION :
*/
InstYou &PMYouPatchManager::instYou()
{
  return *_instYou;
}

void PMYouPatchManager::updatePackageStates()
{
  instYou().updatePackageStates();
}

bool PMYouPatchManager::updatesAvailable()
{
  PMSelectableVec::const_iterator it;
  for( it = begin(); it != end(); ++it ) {
    if ( (*it)->to_install() ) return true;
  }
  
  return false;
}

bool PMYouPatchManager::securityUpdatesAvailable()
{
  PMSelectableVec::const_iterator it;
  for( it = begin(); it != end(); ++it ) {
    if ( (*it)->to_install() ) {
      PMYouPatchPtr patch = (*it)->theObject();
      if ( !patch ) {
        INT << "patch has no object" << endl;
      } else {
        if ( patch->kind() == PMYouPatch::kind_security ) return true;
      }
    }
  }
  
  return false;
}

Pathname PMYouPatchManager::settingsFile() const
{
  return "/var/lib/YaST2/you/settings";
}

FSize PMYouPatchManager::totalDownloadSize()
{
  return instYou().totalDownloadSize();
}

list<string> PMYouPatchManager::rawPatchInfo( const PMYouPatchPtr &patch )
{
  list<string> text;

  if ( patch ) {
    ifstream in( patch->localFile().asString().c_str() );
    if ( in.fail() ) {
      ERR << "Unable to load '" << patch->localFile() << "'" << endl;
    } else {
      string line;
      while( getline( in, line ) ) {
        text.push_back( line );
      }
    }
  }

  return text;
}
